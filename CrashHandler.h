#pragma once
#include <crashrpt.h>

namespace CrashHandling {
    LONG __stdcall ForgeExceptionHandler(PEXCEPTION_POINTERS exception_pointers);

    bool InstallCrashHandler();


    value struct DebugReportSource {
        // The source name (often your plugin name), as in the project registered with CrashRpt
        System::String^ application;
        // The current source version
        System::String^ version;

        // A custom target URL for the crash report upload, only applicable if non default
        System::String^ targetUrl;
        // A custom privacy policy URL, only applicable if non default
        System::String^ privacyPolicyUrl;

        // The path to the language file for the crash report, only applicable if non default
        System::String^ langFilePath;
    };

    interface class IDebugReporter {
        void ReportCrash(DebugReportSource source, System::String^ message);
        void ReportGeneric(DebugReportSource source, System::String^ message);
        bool AddPropertyToReport(System::String^ name, System::String^ value);
        bool AddFileToReport(System::String^ file);
        bool AddScreenshotToReport(HWND hwnd);
    };

    ref class CrashRptDebugReporter : IDebugReporter {
    public:
        virtual void ReportCrash(DebugReportSource source, System::String^ message);
        virtual void ReportGeneric(DebugReportSource source, System::String^ message);
        virtual bool AddPropertyToReport(System::String^ name, System::String^ value);
        virtual bool AddFileToReport(System::String^ file);
        virtual bool AddScreenshotToReport(HWND hwnd);
    private:
        void SendReport(DebugReportSource source, System::String^ message, CR_EXCEPTION_INFO exception_info);
    };


    public ref class DebugService abstract sealed {
    private:
        static IDebugReporter^ instance;
    public:
        static IDebugReporter^ GetReporter() {
            if (instance == nullptr)
                instance = gcnew CrashRptDebugReporter();

            return instance;
        }
    };

}
