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

static NetModAPI::Logger::Logger() {
    auto config = gcnew Config::LoggingConfiguration();

    // Targets where to log to: File and Console
    FileTarget^ logFile = gcnew FileTarget("logfile");
    logFile->FileName = Layouts::Layout::FromString("plugins/s4-netmodapi.txt");
    logFile->DeleteOldFileOnStartup = true;
    ColoredConsoleTarget^ logConsole = gcnew ColoredConsoleTarget("logconsole");
    logConsole->DetectConsoleAvailable = true;

    // Rules for mapping loggers to targets
    config->AddRule(LogLevel::Debug, LogLevel::Fatal, logConsole, "*");
	config->AddRule(LogLevel::Debug, LogLevel::Fatal, logFile, "*");

    // Apply config           
    LogManager::Configuration = config;
    log = LogManager::GetLogger("global");
    log->Info("Started logging");
}

void NetModAPI::Logger::LogInfo(String^ msg) {
    log->Info(msg);
}

void NetModAPI::Logger::LogDebug(String^ msg) {
    log->Debug(msg);
}

void NetModAPI::Logger::LogError(String^ msg, Exception^ exception) {
    Debugger::Break();
    if (log != nullptr)
    {
        log->Error(exception, msg);
    }
}

#pragma warning(pop)
