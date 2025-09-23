#include "pch.h"
#include "Logger.h"
#include <vector>
#include <sstream>

typedef void(__stdcall* LogCallbackType)(int, const wchar_t*, const wchar_t*, const wchar_t*);
std::vector<LogCallbackType> logCallbacks;

void __stdcall LogMessage(int loglevel, const wchar_t* source, const wchar_t* category, const wchar_t* message) {
    if (logCallbacks.size() != 0) {
        for (auto callback : logCallbacks) {
            callback(loglevel, source, category, message);
        }
    }

    std::wstringstream logline;

    switch (loglevel) {
    case LogLevelDebug:
        logline << L"[DEBUG] ";
        break;
    case LogLevelInfo:
        logline << L"[INFO ] ";
        break;
    case LogLevelWarning:
        logline << L"[WARN ] ";
        break;
    case LogLevelError:
        logline << L"[ERROR] ";
        break;
    default:
        logline << L"[UNKWN] ";
        break;
    }

    logline << L"[" << source << L"][" << category << L"] " << message;
    printf("%ls\n", logline.str().c_str());
}

void __stdcall AddLogCallback(void(__stdcall* callback)(int, const wchar_t*, const wchar_t*, const wchar_t*)) {
    logCallbacks.push_back(callback);
}

void InitializeLogging() {

}
