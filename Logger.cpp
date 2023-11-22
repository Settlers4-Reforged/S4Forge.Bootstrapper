#include "pch.h"
#include "Logger.h"

#undef GetCurrentDirectory
#pragma warning(push)
// NLog imports from netstandard - we support it, but the compiler still complains
// Make it shut up:
#pragma warning(disable: 4691)


using namespace System::Diagnostics;
using namespace NLog;
using namespace NLog::Targets;

FileTarget^ NetModAPI::Logger::CreateFileLogger(String^ log_path, String^ log_suffix) {
    FileTarget^ log_file = gcnew FileTarget("logfile");
    log_file->FileName = Layouts::Layout::FromString(log_path + "latest" + log_suffix + ".txt");
    log_file->ArchiveOldFileOnStartup = true;
    log_file->Footer = Layouts::Layout::FromString("End of log");
    log_file->ArchiveFileName = Layouts::Layout::FromString(log_path + "log" + log_suffix + ".{#}.txt");
    log_file->MaxArchiveFiles = 5;
    log_file->CreateDirs = true;
    log_file->AutoFlush = true;

    return log_file;
}

static NetModAPI::Logger::Logger() {
    auto config = gcnew Config::LoggingConfiguration();

    String^ log_path = "plugins/Forge/Logs/";

    // Targets where to log to: File and Console
    FileTarget^ log_file = CreateFileLogger(log_path, "");
    FileTarget^ error_log_file = CreateFileLogger(log_path, "-error");
    ColoredConsoleTarget^      logConsole = gcnew ColoredConsoleTarget("logconsole");
    logConsole->DetectConsoleAvailable = true;

    // Rules for mapping loggers to targets
    config->AddRule(LogLevel::Debug, LogLevel::Fatal, logConsole, "*");
    config->AddRule(LogLevel::Debug, LogLevel::Fatal, log_file, "*");
    config->AddRule(LogLevel::Debug, LogLevel::Fatal, error_log_file, "error");

    // Apply config           
    LogManager::Configuration = config;
    log = LogManager::GetLogger("global");
    log->Info("Started logging");

    error_log = LogManager::GetLogger("error");
    error_log->Info("Started Error logging");
}

void NetModAPI::Logger::LogInfo(String^ msg) {
    log->Info(msg);
}

void NetModAPI::Logger::LogDebug(String^ msg) {
    log->Debug(msg);
}

void NetModAPI::Logger::LogWarn(String^ msg) {
    log->Warn(msg);
}

void NetModAPI::Logger::LogError(String^ msg, Exception^ exception) {
    if (error_log != nullptr)
    {
        error_log->Error(exception, msg);
    }
    Debugger::Break();
}

#pragma warning(pop)
