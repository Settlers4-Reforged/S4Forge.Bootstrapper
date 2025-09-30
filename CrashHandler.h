#pragma once

namespace CrashHandling {
    LONG __stdcall ForgeExceptionHandler(PEXCEPTION_POINTERS exception_pointers);

    bool InstallCrashHandler();
    bool DisableCrashHandler();

    ref class CrashRptDebugReporter : Forge::Logging::IDebugService {
    internal:
        static CrashRptDebugReporter^ Instance = gcnew CrashRptDebugReporter();
    public:
        virtual void ReportException(Forge::Logging::DebugReportSource source, System::String^ message, System::Exception^ exception, bool fatal);
        virtual void ReportGeneric(Forge::Logging::DebugReportSource source, System::String^ message);
        virtual void Log(System::String^ origin, System::String^ message);
        virtual bool AddPropertyToReport(System::String^ name, System::String^ value);
        virtual bool AddFileToReport(System::String^ file);
        virtual void SetGpuInfo(System::String^ vendor, System::String^ gpuName, System::String^ renderer);

        virtual System::String^ GetFullStacktrace(int skip, bool skipToFirstManaged);
    };

    ref class DummyDebugReporter : Forge::Logging::IDebugService {
    public:
        virtual void ReportException(Forge::Logging::DebugReportSource source, System::String^ message, System::Exception^ exception, bool fatal) {
            // Do nothing
        }
        virtual void ReportGeneric(Forge::Logging::DebugReportSource source, System::String^ message) {
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

        virtual System::String^ GetFullStacktrace(int skip, bool skipToFirstManaged) {
            return "";
        }
    };
}
