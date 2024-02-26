#include "pch.h"
#pragma unmanaged

#include "crashrpt.h"


LPTOP_LEVEL_EXCEPTION_FILTER g_clr_crash_handler;

PEXCEPTION_POINTERS g_last_exception_pointers;

// This is a workaround for the CLR crash handler, which can skip the default forge crash handler.
bool g_exception_handled_by_clr = false;

LONG __stdcall ForgeExceptionHandler(PEXCEPTION_POINTERS exception_pointers) {
    if (g_last_exception_pointers != nullptr) {
        // This indicates, that something went wrong and the crash handler was called twice, probably due to a problem in the crl crash handler.
        goto skip_clr_crash_handler;
    }

    g_last_exception_pointers = exception_pointers;

    // Check whether the exception is a managed exception:
    auto coreCLR = GetModuleHandle(L"coreclr.dll");
    bool isManagedException = false;
    if (coreCLR != nullptr) {
        const DWORD coreclr_base = reinterpret_cast<DWORD>(coreCLR);
        const int coreclr_exception_record_at = 4;
        const auto exception_record = exception_pointers->ExceptionRecord;
        const auto exception_code = exception_record->ExceptionCode;

        // See https://github.com/dotnet/runtime/blob/main/src/coreclr/utilcode/ex.cpp#L1313
        if (exception_code == 0xE0434352 && exception_record->ExceptionInformation[coreclr_exception_record_at] == coreclr_base) {
            isManagedException = true;

            crAddProperty(L"Managed", L"True");
        }else {
            crAddProperty(L"Managed", L"False");
        }
    }


#if DEBUG
    {
        std::string message = "Exception not caught.\n\nAttach debugger and press retry to debug, or press cancel to stop execution. Press ignore to invoke the default error handler";
        if (isManagedException) {
            message = "Managed " + message;
        }

        const int choice = MessageBoxA(nullptr, message.c_str(), "Forge -  Error", MB_ABORTRETRYIGNORE);
        if (choice == IDRETRY) {
            return EXCEPTION_CONTINUE_SEARCH;
        } else if (choice == IDABORT) {
            ExitProcess(0);
        }
    }
#endif

    if (isManagedException) {
        if (!g_clr_crash_handler) {
#if DEBUG
            // This should never happen, but if it does, we should report it.
            MessageBoxW(nullptr, L"CLR Exception Handler not set, but received a managed exception", L"Forge - Error", 0);
#endif
            // If the CLR crash handler is not set, we should not handle the exception with the clr.
            goto skip_clr_crash_handler;
        }

        if (g_clr_crash_handler(exception_pointers) == EXCEPTION_CONTINUE_EXECUTION || g_exception_handled_by_clr)
            return EXCEPTION_CONTINUE_EXECUTION;
    }

    skip_clr_crash_handler:

    // TODO: check whether the exception is a pure S4 exception

    CR_EXCEPTION_INFO ei{};
    ei.cb = sizeof(CR_EXCEPTION_INFO);
    ei.exctype = CR_SEH_EXCEPTION;
    ei.code = 42;
    ei.pexcptrs = exception_pointers;

    crGenerateErrorReport(&ei);

    ExitProcess(0); // It is better to stop the process here or else corrupted data may incomprehensibly crash it later.
    return EXCEPTION_CONTINUE_SEARCH;
}

void GuardedGameTick() {
    DWORD S4_Main = reinterpret_cast<DWORD>(GetModuleHandle(nullptr));
    DWORD retpos = S4_Main + 0x5cba9;

    __try {
        __asm {
            jmp retpos
        }
    } __except (ForgeExceptionHandler(GetExceptionInformation())) { }
}


void _declspec(naked) _GuardedGameTick() {
    __asm {
        // Previous call at this position...
        call ShowWindow
    }

    // Separate function to allow for the __try SEH block
    GuardedGameTick();
}

LONG __stdcall CorruptedStateExceptionHandler(PEXCEPTION_POINTERS exception_pointers) {
    DWORD dwExceptionCode = exception_pointers->ExceptionRecord->ExceptionCode;

    switch (dwExceptionCode) {
    case STATUS_ACCESS_VIOLATION:
    case STATUS_STACK_OVERFLOW:
    case EXCEPTION_ILLEGAL_INSTRUCTION:
    case EXCEPTION_IN_PAGE_ERROR:
    case EXCEPTION_INVALID_DISPOSITION:
    case EXCEPTION_NONCONTINUABLE_EXCEPTION:
    case EXCEPTION_PRIV_INSTRUCTION:
    case STATUS_UNWIND_CONSOLIDATE:
#ifdef DEBUG
        MessageBox(nullptr, L"Encountered a corrupted state exception", L"Forge - Error", 0);
#endif
        // Forward to the forge handler
        return ForgeExceptionHandler(exception_pointers);
        // TODO: maybe check if stuck in a loop and exit the process
    default:
        return EXCEPTION_CONTINUE_SEARCH;
    }
}

std::wstring ExePath() {
    TCHAR buffer[MAX_PATH] = { 0 };
    GetModuleFileName(nullptr, buffer, MAX_PATH);
    std::wstring::size_type pos = std::wstring(buffer).find_last_of(L"\\/");
    return std::wstring(buffer).substr(0, pos);
}

bool CrashHandling::InstallCrashHandler() {
    // Patch the game loop to catch any straggling exceptions
    DWORD S4_Main = reinterpret_cast<DWORD>(GetModuleHandle(nullptr));
    hlib::JmpPatch patch = hlib::JmpPatch(S4_Main + 0x5cba3, reinterpret_cast<DWORD>(&_GuardedGameTick));
    patch.patch();

    // This is a workaround for the CLR crash handler in P/Invoke scenarios, which *will* skip the windows SEH Handler.
    // It is only intended to catch corrupted state exceptions, which are not caught by the CLR crash handler.
    // Any other exceptions should be handled by the default SEH flow.
    // THIS GETS CALLED FOR EVERY EXCEPTION - HANDLE WITH CARE
    AddVectoredExceptionHandler(1, CorruptedStateExceptionHandler);

    const std::wstring crashRptPath = ExePath() + L"\\plugins\\Forge\\CrashRpt\\";
    const std::wstring libPath = crashRptPath;
    const std::wstring langFile = libPath + L"\\crashrpt_lang.ini";

    const std::wstring uploadEndpoint = L"http://crash.settlers4-hd.com/crashfix/index.php/crashReport/uploadExternal";

    const HMODULE crashRpt = LoadLibrary((crashRptPath + L"CrashRpt1403.dll").c_str());
    if(crashRpt != nullptr) {
        // Define CrashRpt configuration parameters
        CR_INSTALL_INFO info{};
        info.cb = sizeof(CR_INSTALL_INFO);
        info.pszAppName = (L"Settlers4-Forge");
        info.pszAppVersion = FORGE_VERSION_STRING;
        info.pszUrl = uploadEndpoint.c_str();
        info.uPriorities[CR_HTTP] = 1;  // First try send report over HTTP 
        info.uPriorities[CR_SMTP] = CR_NEGATIVE_PRIORITY;  // SKIP
        info.uPriorities[CR_SMAPI] = CR_NEGATIVE_PRIORITY; // SKIP

        info.pszCrashSenderPath = libPath.c_str();
        info.pszDebugHelpDLL = libPath.c_str();
        info.pszLangFilePath = langFile.c_str();

        // Install all available exception handlers
        info.dwFlags |= CR_INST_ALL_POSSIBLE_HANDLERS;
        info.dwFlags &= ~CR_INST_SEH_EXCEPTION_HANDLER; // Disable the SEH, because of the CoreCLR crash handler
        info.dwFlags |= CR_INST_AUTO_THREAD_HANDLERS;
        info.dwFlags |= CR_INST_SHOW_ADDITIONAL_INFO_FIELDS;

        // Define the Privacy Policy URL 
        info.pszPrivacyPolicyURL = (L"https://settlers4-hd.com/privacypolicy.html");

        // Install crash reporting
        const int nResult = crInstall(&info);
        if(nResult != 0) {
#ifdef DEBUG
            // Something goes wrong. Get error message.
            TCHAR szErrorMsg[512] = L"";
            crGetLastErrorMsg(szErrorMsg, 512);
            MessageBoxW(nullptr, szErrorMsg, L"Forge - CrashRpt", 0);
#endif
            return false;
        }

        g_clr_crash_handler = SetUnhandledExceptionFilter(ForgeExceptionHandler);
    } else {
        MessageBoxW(nullptr, L"Failed to load CrashRpt", L"Forge - CrashRpt", 0);
        return false;
    }

    return true;
}

#pragma managed
using namespace System::Runtime::InteropServices;

void CrashHandling::CrashReporter::ReportCrash(CrashReportSource source, System::String^ message) {
    g_exception_handled_by_clr = true;

    System::IntPtr ^psz_app_name = %Marshal::StringToHGlobalUni(source.application);
    System::IntPtr ^psz_app_version = %Marshal::StringToHGlobalUni(source.version);
    System::IntPtr ^psz_privacy_policy_url = %Marshal::StringToHGlobalUni(source.privacyPolicyUrl);
    System::IntPtr ^psz_lang_file_path = %Marshal::StringToHGlobalUni(source.langFilePath);
    System::IntPtr ^psz_target_url = %Marshal::StringToHGlobalUni(source.targetUrl);

    const PEXCEPTION_POINTERS exception_pointers = g_last_exception_pointers;
    if (exception_pointers == nullptr) {
        throw gcnew System::InvalidOperationException("This function can only be called in the context of a crash handler! No previous exception was recorded...");
    }

    CR_EXCEPTION_INFO ei{};
    ei.cb = sizeof(CR_EXCEPTION_INFO);
    ei.exctype = CR_SEH_EXCEPTION;
    ei.code = exception_pointers->ExceptionRecord->ExceptionCode;
    ei.pexcptrs = exception_pointers;

    //TODO: Modify CrashRpt to allow custom app name and version after installation

    AddPropertyToCrashReport("Message", message);

    crGenerateErrorReport(&ei);

    Marshal::FreeHGlobal(*psz_app_name);
    Marshal::FreeHGlobal(*psz_app_version);
    Marshal::FreeHGlobal(*psz_privacy_policy_url);
    Marshal::FreeHGlobal(*psz_lang_file_path);
    Marshal::FreeHGlobal(*psz_target_url);
}

bool CrashHandling::CrashReporter::AddPropertyToCrashReport(System::String^ name, System::String^ value) {
    System::IntPtr ^psz_prop_name = %Marshal::StringToHGlobalUni(name);
    System::IntPtr ^psz_prop_value = %Marshal::StringToHGlobalUni(value);
    if(const int result = crAddProperty(static_cast<wchar_t*>(psz_prop_name->ToPointer()), static_cast<wchar_t*>(psz_prop_value->ToPointer())); result != 0) {
        Marshal::FreeHGlobal(*psz_prop_name);
        Marshal::FreeHGlobal(*psz_prop_value);
        return false;
    }

    Marshal::FreeHGlobal(*psz_prop_name);
    Marshal::FreeHGlobal(*psz_prop_value);
    return true;
}

bool CrashHandling::CrashReporter::AddFileToCrashReport(System::String^ file) {
    System::IntPtr ^psz_file = %Marshal::StringToHGlobalUni(file);
    if(const int result = crAddFile2(static_cast<wchar_t*>(psz_file->ToPointer()), nullptr, nullptr, CR_AF_MAKE_FILE_COPY | CR_AF_MISSING_FILE_OK); result != 0) {
        Marshal::FreeHGlobal(*psz_file);
        return false;
    }

    Marshal::FreeHGlobal(*psz_file);
    return true;
}