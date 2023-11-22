#pragma once

using namespace System;

namespace NetModAPI {
    ref class Logger abstract sealed {
        static NLog::ILogger^ log = nullptr;
        static NLog::ILogger^ error_log = nullptr;

        static NLog::Targets::FileTarget^ CreateFileLogger(String^ log_path, String^ log_suffix);

    public:
        static Logger();

        static void LogInfo(String^ msg);
        static void LogDebug(String^ msg);
        static void LogWarn(String^ msg);
        static void LogError(String^ msg, Exception^ exception);
    };
}

