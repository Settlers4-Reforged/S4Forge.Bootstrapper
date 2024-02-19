#include "pch.h"
#pragma unmanaged

#include "crashrpt.h"


LONG __stdcall UnhandledExceptionCallback(_EXCEPTION_POINTERS* exceptionInfo) {
    static bool wasCalled = false;

#if DEBUG
    int ret = MessageBoxA(nullptr, "Exception not caught. Attach debugger and press retry to debug, or press cancel to stop execution. Press ignore to invoke the default error handler", "Forge -  Error", MB_ABORTRETRYIGNORE);
    if (ret == IDRETRY) {
        return EXCEPTION_CONTINUE_SEARCH;
    } else if (ret == IDABORT) {
        ExitProcess(0);
    }
#endif

    if(wasCalled)
        return EXCEPTION_CONTINUE_SEARCH;

    wasCalled = true;
    //TODO: Add custom callback when exception is sent


    CR_EXCEPTION_INFO ei = {};
    ei.cb = sizeof(CR_EXCEPTION_INFO);
    ei.exctype = CR_SEH_EXCEPTION;
    ei.code = 42;
    ei.pexcptrs = exceptionInfo;

    crGenerateErrorReport(&ei);

    ExitProcess(0); // It is better to stop the process here or else corrupted data may incomprehensibly crash it later.
    return EXCEPTION_CONTINUE_SEARCH;
}

std::wstring ExePath() {
    TCHAR buffer[MAX_PATH] = {0};
    GetModuleFileName(NULL, buffer, MAX_PATH);
    std::wstring::size_type pos = std::wstring(buffer).find_last_of(L"\\/");
    return std::wstring(buffer).substr(0, pos);
}

bool CrashHandling::InstallCrashHandler() {
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
        info.dwFlags |= CR_INST_AUTO_THREAD_HANDLERS;
        info.dwFlags |= CR_INST_SHOW_ADDITIONAL_INFO_FIELDS;
        // Restart the app on crash 
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

        SetUnhandledExceptionFilter(UnhandledExceptionCallback);
    } else {
        MessageBoxW(nullptr, L"Failed to load CrashRpt", L"Forge - CrashRpt", 0);
        return false;
    }

    return true;
}

#pragma managed
using namespace System::Runtime::InteropServices;

void CrashHandling::CrashReporter::ReportCrash(System::String^ message) {}

bool CrashHandling::CrashReporter::AddPropertyToCrashReport(System::String^ name, System::String^ value) {
    System::IntPtr psz_prop_name = Marshal::StringToHGlobalUni(name);
    System::IntPtr psz_prop_value = Marshal::StringToHGlobalUni(value);
    if(const int result = crAddProperty(static_cast<wchar_t*>(psz_prop_name.ToPointer()), static_cast<wchar_t*>(psz_prop_value.ToPointer())); result != 0) {
        Marshal::FreeHGlobal(psz_prop_name);
        Marshal::FreeHGlobal(psz_prop_value);
        return false;
    }

    Marshal::FreeHGlobal(psz_prop_name);
    Marshal::FreeHGlobal(psz_prop_value);
    return true;
}

bool CrashHandling::CrashReporter::AddFileToCrashReport(System::String^ file) {
    System::IntPtr psz_file = Marshal::StringToHGlobalUni(file);
    if(const int result = crAddFile2(static_cast<wchar_t*>(psz_file.ToPointer()), nullptr, nullptr, CR_AF_MAKE_FILE_COPY | CR_AF_MISSING_FILE_OK); result != 0) {
        Marshal::FreeHGlobal(psz_file);
        return false;
    }

    Marshal::FreeHGlobal(psz_file);
    return true;
}
