#pragma once

namespace CrashHandling {
    bool InstallCrashHandler();


    value struct CrashReportSource {
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

    interface class ICrashReporter {
        void ReportCrash(CrashReportSource source, System::String^ message);
        bool AddPropertyToCrashReport(System::String^ name, System::String^ value);
        bool AddFileToCrashReport(System::String^ file);
    };

    ref class CrashReporter : ICrashReporter {
    public:
        virtual void ReportCrash(CrashReportSource source, System::String^ message);
        virtual bool AddPropertyToCrashReport(System::String^ name, System::String^ value);
        virtual bool AddFileToCrashReport(System::String^ file);
    };


    ref class CrashReporterService abstract sealed {
    private:
        static ICrashReporter^ instance;
    public:
        static ICrashReporter^ GetCrashReporter() {
            if (instance == nullptr)
                instance = gcnew CrashReporter();

            return instance;
        }
    };

}
