#include "pch.h"
#include "Logger.h"

#undef GetCurrentDirectory

using namespace System::Diagnostics;

static NetModAPI::Logger::Logger() {
    auto config = gcnew NLog::Config::LoggingConfiguration();

    // Targets where to log to: File and Console
    auto logfile = gcnew NLog::Targets::FileTarget("logfile");
    logfile->FileName = NLog::Layouts::Layout::FromString("plugins/s4-netmodapi.txt");
    logfile->DeleteOldFileOnStartup = true;
    auto logconsole = gcnew NLog::Targets::ColoredConsoleTarget("logconsole");

    // Rules for mapping loggers to targets            
    config->AddRule(NLog::LogLevel::Info, NLog::LogLevel::Fatal, logconsole, "*");
    config->AddRule(NLog::LogLevel::Debug, NLog::LogLevel::Fatal, logfile, "*");

    // Apply config           
    NLog::LogManager::Configuration = config;
    log = NLog::LogManager::GetLogger("global");
    log->Info("Started logging");
}

void NetModAPI::Logger::Log(String^ msg) {
    log->Info(msg);
}

void NetModAPI::Logger::LogError(String^ msg, Exception^ exception) {
    Debugger::Break();
    if (log != nullptr)
    {
        log->Error(msg, exception);
    }
}
