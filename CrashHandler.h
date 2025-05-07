#pragma once
#include "Helper.h"

namespace CrashHandling {
    LONG __stdcall ForgeExceptionHandler(PEXCEPTION_POINTERS exception_pointers);

    bool InstallCrashHandler();
    bool DisableCrashHandler();


    public value struct DebugReportSource {
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

    public interface class IDebugReporter {
        void ReportException(DebugReportSource source, System::String^ message, System::Exception^ exception, bool fatal);
        void ReportGeneric(DebugReportSource source, System::String^ message);
        void Log(System::String^ origin, System::String^ message);
        bool AddPropertyToReport(System::String^ name, System::String^ value);
        bool AddFileToReport(System::String^ file);

        void SetGpuInfo(System::String^ vendor, System::String^ gpuName, System::String^ renderer);
    };

    ref class CrashRptDebugReporter : IDebugReporter {
    public:
        virtual void ReportException(DebugReportSource source, System::String^ message, System::Exception^ exception, bool fatal);
        virtual void ReportGeneric(DebugReportSource source, System::String^ message);
        virtual void Log(System::String^ origin, System::String^ message);
        virtual bool AddPropertyToReport(System::String^ name, System::String^ value);
        virtual bool AddFileToReport(System::String^ file);
        virtual void SetGpuInfo(System::String^ vendor, System::String^ gpuName, System::String^ renderer);
    };

    ref class DummyDebugReporter : IDebugReporter {
    public:
        virtual void ReportException(DebugReportSource source, System::String^ message, System::Exception^ exception, bool fatal) {
            // Do nothing
        }
        virtual void ReportGeneric(DebugReportSource source, System::String^ message) {
            // Do nothing
        }
        virtual void Log(System::String^ origin, System::String^ message) {
            // Do nothing
        }
        virtual bool AddPropertyToReport(System::String^ name, System::String^ value) {
            return false;
        }
        virtual bool AddFileToReport(System::String^ file) {
            return false;
        }
        virtual void SetGpuInfo(System::String^ vendor, System::String^ gpuName, System::String^ renderer) {
            // Do nothing
        }
    };


    public ref class DebugService abstract sealed {
    private:
        static IDebugReporter^ instance;
    public:
        static System::String^ GetFullStacktrace(int skip, bool skipToFirstManaged) {
            return GetStackTrace(nullptr, skip, skipToFirstManaged);
        }

        static IDebugReporter^ GetReporter() {
            if (instance == nullptr) {
#ifdef TEST
                instance = gcnew DummyDebugReporter();
#else
                instance = gcnew CrashRptDebugReporter();
#endif
            }

            return instance;
        }
    };

}
