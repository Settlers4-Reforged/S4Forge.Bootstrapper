#pragma once

namespace CrashHandling {
    bool InstallCrashHandler();

    interface class ICrashReporter {
        void ReportCrash(System::String^ message);
        bool AddPropertyToCrashReport(System::String^ name, System::String^ value);
        bool AddFileToCrashReport(System::String^ file);
    };

    ref class CrashReporter : ICrashReporter {
    public:
        virtual void ReportCrash(System::String^ message);
        virtual bool AddPropertyToCrashReport(System::String^ name, System::String^ value);
        virtual bool AddFileToCrashReport(System::String^ file);
    };
}
