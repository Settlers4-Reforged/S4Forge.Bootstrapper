#pragma once

extern "C" {
    void __declspec(dllexport) __stdcall LogMessage(int loglevel, const wchar_t* source, const wchar_t* category, const wchar_t* message);
    void __declspec(dllexport) __stdcall AddLogCallback(void(__stdcall* callback)(int, const wchar_t*, const wchar_t*, const wchar_t*));
}

void InitializeLogging();

#define LogLevelDebug 1
#define LogLevelInfo 2
#define LogLevelWarning 3
#define LogLevelError 4
