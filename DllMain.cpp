#include "pch.h"
#include "ManagedLoader.h"

#pragma managed(push, off)

#include <filesystem>
#define WIN32_LEAN_AND_MEAN
#include <iostream>
#include <windows.h>
#include <tlhelp32.h>

S4API ModAPI;

static void CleanUp() {
    CrashHandling::DisableCrashHandler();

    if (nullptr != ModAPI) {
        ModAPI->Release();
        ModAPI = nullptr;
    }
};

bool ubisoft_ready = false;

extern "C" __declspec(dllexport) void InitAsi() {
    if (!ubisoft_ready) {
        return;
    }

    __try {
        InitPlugins(nullptr);
    } __except (CrashHandling::ForgeExceptionHandler(GetExceptionInformation())) {}
}

static bool Init() {
    CrashHandling::InstallCrashHandler();

    ModAPI = S4ApiCreate(); // get an interface to the mod api
    if (ModAPI == nullptr) return false;

    return true;
}

HANDLE GetParentProcess() {
    const HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    PROCESSENTRY32 processEntry = {};
    processEntry.dwSize = sizeof(PROCESSENTRY32);

    if (Process32First(snapshot, &processEntry)) {
        const DWORD currentProcessId = GetCurrentProcessId();

        do {
            if (processEntry.th32ProcessID == currentProcessId)
                break;
        } while (Process32Next(snapshot, &processEntry));
    }

    CloseHandle(snapshot);

    return OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, processEntry.th32ParentProcessID);
}
extern "C" BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    //MessageBoxA(nullptr, "NetModAPI DEBUG", "NetModAPI", 0);
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
    {
#ifndef TEST
        const HANDLE parent_handle = GetParentProcess();
        wchar_t      process_name[1024] = { 0 };
        DWORD        process_name_size = 1024;
        QueryFullProcessImageNameW(parent_handle, 0, process_name, &process_name_size);
        const std::filesystem::path process_file_path(process_name);

        const std::string parent_name = process_file_path.filename().string();

        // abort if we're not called from Ubisoft Launcher or Internal Call... (Prevents loading in first call)
        if (_stricmp(parent_name.c_str(), "UbisoftGameLauncher.exe") != 0 &&
            _stricmp(parent_name.c_str(), "UbisoftGameLauncher64.exe") != 0 &&
            _stricmp(parent_name.c_str(), "S4_Main.exe") != 0) {
            ubisoft_ready = false;
#ifdef PUBLIC
            return TRUE;
#else
            //Instead try to load a crack, we are probably debugging...
            LoadLibraryA("LumaPlayFiles\\UbiAPI.dll");
            LoadLibraryA("LumaPlayFiles\\LumaPlay_x86.dll");
#endif 
        }

        ubisoft_ready = true;

#ifndef PUBLIC
        if (GetAsyncKeyState(VK_F2)) {
            return TRUE;
        }

        if (GetAsyncKeyState('Q')) {
            MessageBoxA(nullptr, "NetModAPI DEBUG", "NetModAPI", 0);
        }

        AllocConsole();
        freopen("CONOUT$", "w", stdout);
        freopen("CONOUT$", "w", stderr);
#endif // !PUBLIC

        if (!Init()) break;
#endif
        break;
    }
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    default:
        break;
    case DLL_PROCESS_DETACH:
        CleanUp();
        break;
    }
    return TRUE;
}

#pragma managed(pop)
