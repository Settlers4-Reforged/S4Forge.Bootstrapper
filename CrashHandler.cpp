#include "pch.h"
#include "Logger.h"
#include "MinHook.h"
#include "Lambda2Delegate.h"
#include <vcclr.h>

#include "Helper.h"
#include "Resource.h"

#pragma managed

using namespace System::Runtime::InteropServices;
using namespace System;
using namespace Sentry;
using namespace Forge::Logging;

LPTOP_LEVEL_EXCEPTION_FILTER g_clr_crash_handler;

PEXCEPTION_POINTERS g_last_exception_pointers;

// This is a workaround for the CLR crash handler, which can skip the default forge crash handler.
bool g_exception_handled_by_clr = false;

bool g_exception_handler_active = true;

#pragma unmanaged

void __cdecl GuardedGameTick() {
    DWORD S4_Main = reinterpret_cast<DWORD>(GetModuleHandle(nullptr));
    DWORD retpos = S4_Main + 0x5cba9;

    __try {
        __asm {
            jmp retpos
        }
    } __except (CrashHandling::ForgeExceptionHandler(GetExceptionInformation())) {}

    return;
}


void _declspec(naked) _GuardedGameTick() {
    __asm {
        // Previous call at this position...
        call ShowWindow
    }

    // Separate function to allow for the __try SEH block
    GuardedGameTick();
}

#pragma managed

bool CrashHandling::InstallCrashHandler() {
    DWORD S4_Main = reinterpret_cast<DWORD>(GetModuleHandle(nullptr));

    g_clr_crash_handler = UnhandledExceptionFilter;

    // Remove the installation code for the original exception handler
    hlib::NopPatch s4_exception_handler = hlib::NopPatch(S4_Main + 0x5C855, 5);
    s4_exception_handler.patch();
    s4_exception_handler = hlib::NopPatch(S4_Main + 0x951EF6, 5);
    s4_exception_handler.patch();

    // Patch the game loop to catch any straggling exceptions
    hlib::JmpPatch(S4_Main + 0x5cba3, reinterpret_cast<DWORD>(&_GuardedGameTick)).patch();

    // Fix a std::wstring::destroy_wstring crash, when deleting a string on the exit of the game
    hlib::NopPatch(S4_Main + 0x5ccbc, 5).patch();

    SentryOptions^ options = gcnew SentryOptions();
    options->Dsn = "https://a5d9124fef748e98c437e552e17f77bc@o4503905572225024.ingest.us.sentry.io/4506394193494016";
    options->Environment = (gcnew String(FILE_VERSION_STRING))->ToLower();
    options->AutoSessionTracking = true;
    options->AttachStacktrace = true;
    options->StackTraceMode = StackTraceMode::Enhanced;
    options->IsGlobalModeEnabled = true;
    options->TracesSampleRate = 0.1;
    options->CacheDirectoryPath = "plugins/Forge/CrashRpt/";
    Func<SentryEvent^, SentryHint^, SentryEvent^>^ beforeSend = Lambda2Delegate<>() = [](SentryEvent^ ev, SentryHint^ hint)->SentryEvent^ {
        auto params = Text::Json::JsonWriterOptions();
        params.Indented = true;

        auto fileStream = gcnew IO::FileStream("plugins/Forge/CrashRpt/report.json", IO::FileMode::Create);

        Text::Json::Utf8JsonWriter^ writer = gcnew Text::Json::Utf8JsonWriter(fileStream, params);
        ev->WriteTo(writer, nullptr);
        writer->Flush();
        fileStream->Close();
        return ev;
        };
    options->SetBeforeSend(beforeSend);

    SentrySdk::Init(options);

    return true;
}

bool CrashHandling::DisableCrashHandler() {
    g_exception_handler_active = false;

    return true;
}

void OpenFeedbackDialog(String^ eventId) {
    try {
        String^ reporterPath = "plugins/Forge/CrashRpt/ForgeCrashReporter.exe";
        if (!IO::File::Exists(reporterPath)) return;


        Diagnostics::Process^ process = gcnew Diagnostics::Process();
        process->StartInfo->FileName = Environment::CurrentDirectory + "/" + reporterPath;
        process->StartInfo->WorkingDirectory = Environment::CurrentDirectory + "/plugins/Forge/CrashRpt/";
        process->StartInfo->ArgumentList->Add("report");
        process->StartInfo->ArgumentList->Add(Environment::CurrentDirectory + "/plugins/Forge/CrashRpt/report.json");
        process->Start();
        process->WaitForExit(60/*s*/ * 1000/*ms*/);
    } finally {}
}

void ReportException(Exception^ exception, bool fatal) {
    g_exception_handled_by_clr = true;

    auto id = SentrySdk::CaptureException(exception);
    OpenFeedbackDialog(id.ToString());

    if (fatal) {
        CrashHandling::DisableCrashHandler();

        SentrySdk::Close();

        TerminateProcess(GetCurrentProcess(), 1);
    }
}


void CrashHandling::CrashRptDebugReporter::Log(String^ origin, String^ message) {
    SentrySdk::AddBreadcrumb(message, origin, "debug", nullptr, BreadcrumbLevel::Info);
}

void AddTagToSentry(String^ name, String^ value) {
    gcroot nameRoot = name;
    gcroot valueRoot = value;

    SentrySdk::ConfigureScope(Lambda2Delegate<>() = [nameRoot, valueRoot](Scope^ scope) {
        scope->SetTag(nameRoot, valueRoot);
        });
}

bool CrashHandling::CrashRptDebugReporter::AddPropertyToReport(String^ name, String^ value) {
    AddTagToSentry(name, value);
    return true;
}


void AddAttachmentToSentry(String^ file, bool isDump = false) {
    gcroot fileRoot = file;

    SentrySdk::ConfigureScope(Lambda2Delegate<>() = [fileRoot, isDump](Scope^ scope) {
        String^ contentType = "text/plain";
        AttachmentType type = AttachmentType::Default;

        if (IO::Path::GetExtension(fileRoot) != ".txt") {
            contentType = "application/octet-stream";
        }

        if (isDump) {
            contentType = "application/octet-stream";
            type = AttachmentType::Minidump;
        }



        scope->AddAttachment(fileRoot, type, contentType);
        });
}

bool CrashHandling::CrashRptDebugReporter::AddFileToReport(String^ file) {
    AddAttachmentToSentry(file);
    return true;
}

void SentrySetGpuInfo(String^ vendor, String^ gpuName, String^ renderer) {
    gcroot vendorRoot = vendor;
    gcroot gpuNameRoot = gpuName;
    gcroot rendererRoot = renderer;

    SentrySdk::ConfigureScope(Lambda2Delegate<>() = [vendorRoot, gpuNameRoot, rendererRoot](Scope^ scope) {
        scope->Contexts->Gpu->VendorName = vendorRoot;
        scope->Contexts->Gpu->Name = gpuNameRoot;
        scope->Contexts->Gpu->ApiType = rendererRoot;
        });
}
void CrashHandling::CrashRptDebugReporter::SetGpuInfo(System::String^ vendor, System::String^ gpuName, System::String^ renderer) {
    SentrySetGpuInfo(vendor, gpuName, renderer);
}

System::String^ CrashHandling::CrashRptDebugReporter::GetFullStacktrace(int skip, bool skipToFirstManaged) {
    return GetStackTrace(nullptr, skip, skipToFirstManaged);
}

void HandleNativeException(PEXCEPTION_POINTERS exception_pointers, bool isManaged) {
    MINIDUMP_EXCEPTION_INFORMATION minidump_exception_information = MINIDUMP_EXCEPTION_INFORMATION();
    minidump_exception_information.ThreadId = GetCurrentThreadId();
    minidump_exception_information.ExceptionPointers = exception_pointers;
    minidump_exception_information.ClientPointers = FALSE;

    const wchar_t* dumpFilePath = L"plugins/Forge/CrashRpt/ForgeCrash.dmp";
    HANDLE         dumpFileHandle = CreateFileW(dumpFilePath, GENERIC_WRITE, FILE_SHARE_WRITE, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), dumpFileHandle, MiniDumpNormal, (exception_pointers == nullptr && !isManaged) ? nullptr : &minidump_exception_information, nullptr, nullptr);
    CloseHandle(dumpFileHandle);

    AddAttachmentToSentry(gcnew String(dumpFilePath), exception_pointers != nullptr);

    SentrySdk::AddBreadcrumb(GetStackTrace(exception_pointers, 0, false), "Native Stack Trace", "error", nullptr, BreadcrumbLevel::Error);
}

void CrashHandling::CrashRptDebugReporter::ReportException(DebugReportSource source, String^ message, Exception^ exception, bool fatal) {
    AddPropertyToReport("ReportType", "Crash");
    HandleNativeException(g_last_exception_pointers, true);
    ::ReportException(exception, fatal);
}

void CrashHandling::CrashRptDebugReporter::ReportGeneric(DebugReportSource source, String^ message) {
    //TODO: add feedback
}

LONG __stdcall CrashHandling::ForgeExceptionHandler(PEXCEPTION_POINTERS exception_pointers) {
    if (!g_exception_handler_active) {
#ifdef DEBUG
        MessageBox(nullptr, L"Crash handler was called, while being deactivated. Exception will be ignored.", L"Forge - Error", 0);
#endif
        return EXCEPTION_CONTINUE_SEARCH;
    }

    if (g_last_exception_pointers != nullptr) {
        // This indicates, that something went wrong and the crash handler was called twice, probably due to a problem in the crl crash handler.
        MessageBoxW(nullptr, L"Crash handler was called twice", L"Forge - Error", 0);
        goto skip_clr_crash_handler;
    }


    g_last_exception_pointers = exception_pointers;

    {
        // Check whether the exception is a managed exception:
        auto coreCLR = GetModuleHandle(L"coreclr.dll");
        bool isManagedException = false;
        if (coreCLR != nullptr) {
            const DWORD coreclr_base = reinterpret_cast<DWORD>(coreCLR);
            const int coreclr_exception_record_at = 4;
            const auto exception_record = exception_pointers->ExceptionRecord;
            const auto exception_code = exception_record->ExceptionCode;

            // Module not found exception, those can be handled by the clr, but some are first caught by this function here, so we continue the search.
            if (exception_code == 0xC06D007E) {
                return EXCEPTION_CONTINUE_SEARCH;
            }

            // See https://github.com/dotnet/runtime/blob/main/src/coreclr/utilcode/ex.cpp#L1313
            if (exception_code == 0xE0434352 && exception_record->ExceptionInformation[coreclr_exception_record_at] == coreclr_base) {
                isManagedException = true;
            }
        }

        HandleNativeException(exception_pointers, isManagedException);


#if DEBUG
        {
            const wchar_t* message = L"Exception not caught.\n\nAttach debugger and press retry to debug, or press cancel to stop execution. Press ignore to invoke the default error handler";
            if (isManagedException) {
                message = L"Managed Exception not caught.\n\nAttach debugger and press retry to debug, or press cancel to stop execution. Press ignore to invoke the default error handler";
            }

            const int choice = MessageBox(nullptr, message, L"Forge -  Error", MB_ABORTRETRYIGNORE);
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
    }


skip_clr_crash_handler:
    ReportException(gcnew SEHException("Unhandled unmanaged exception was thrown"), true);
    return EXCEPTION_CONTINUE_SEARCH;
}
