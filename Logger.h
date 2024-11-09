#pragma once

using namespace System;

namespace NetModAPI {
    ref class Logger abstract sealed {
        static String^ log_path = Environment::CurrentDirectory->TrimEnd('/', '\\') +  "/plugins/Forge/Logs/";

        static NLog::ILogger^ log = nullptr;
        static NLog::ILogger^ error_log = nullptr;

        static NLog::Config::LoggingConfiguration^ config = nullptr;

        static Collections::Generic::Dictionary<String^, NLog::ILogger^>^ loggers = gcnew Collections::Generic::Dictionary<String^, NLog::ILogger^>();
        static Collections::Generic::Dictionary<String^, NLog::ILogger^>^ error_loggers = gcnew Collections::Generic::Dictionary<String^, NLog::ILogger^>();
        static NLog::ILogger^ GetLogger(String^ name);
        static NLog::ILogger^ GetErrorLogger(String^ name);

        static NLog::Targets::FileTarget^ CreateFileLogger(String^ log_path, String^ log_suffix);

    public:
        static Logger();

        static void AddLogger(String^ name);

        static void LogInfo(String^ msg, [System::Runtime::InteropServices::Optional]String^ logger);
        static void LogDebug(String^ msg, [System::Runtime::InteropServices::Optional]String^ logger);
        static void LogWarn(String^ msg, [System::Runtime::InteropServices::Optional]String^ logger);
        static void LogError(String^ msg, Exception^ exception, [System::Runtime::InteropServices::Optional]String^ logger);
    };
}
