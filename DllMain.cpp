#include "pch.h"
#include "ManagedLoader.h"

#pragma managed(push, off)

#include <filesystem>
#define WIN32_LEAN_AND_MEAN
#include <iostream>
#include <windows.h>
#include <tlhelp32.h>
#include "Logger.h"

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
    // Ubisoft didn't load, so skip Forge...
    if (!ubisoft_ready) {
        return;
    }

#ifndef PUBLIC
    // Conditionally disable Forge if F2 is pressed
    if (GetAsyncKeyState(VK_F2)) {
        return;
    }

    AllocConsole();
    freopen("CONOUT$", "w", stdout);
    freopen("CONOUT$", "w", stderr);
    InitializeLogging();

#endif // !PUBLIC

    __try {
        // Load Mod API
        ModAPI = S4ApiCreate();
        if (ModAPI == nullptr) {
            throw new std::exception("Failed to initialize the mod api");
        }

        InitPlugins(nullptr);
    } __except (CrashHandling::ForgeExceptionHandler(GetExceptionInformation())) {}
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
    /* NOTE(WizzardMaker):
     * Do not use any reference of std::string or any other managed type
     * This will incur a load time cost, especially during debugging
     * It will also cause a loader lock for Test/Non S4 run environments
     */

    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
    {

        const HANDLE parent_handle = GetParentProcess();
        wchar_t      process_name[1024] = { 0 };
        DWORD        process_name_size = 1024;
        QueryFullProcessImageNameW(parent_handle, 0, process_name, &process_name_size);
        CloseHandle(parent_handle);
        wchar_t process_file_path[MAX_PATH];
        wcscpy_s(process_file_path, process_name);

        wchar_t* parent_name_w = wcsrchr(process_file_path, L'\\');
        if (parent_name_w != nullptr) {
            parent_name_w++;
        } else {
            parent_name_w = process_file_path;
        }

        char parent_name[MAX_PATH];
        wcstombs(parent_name, parent_name_w, MAX_PATH);

        // abort if we're not called from Ubisoft Launcher or Internal Call... (Prevents loading in first call)
        if (_stricmp(parent_name, "UbisoftGameLauncher.exe") != 0 &&
            _stricmp(parent_name, "UbisoftGameLauncher64.exe") != 0 &&
            _stricmp(parent_name, "S4_Main.exe") != 0) {
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
