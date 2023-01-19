#include "pch.h"
#include "ManagedLoader.h"
#include <filesystem>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tlhelp32.h>

#pragma managed(push, off)
S4API ModAPI;

static void CleanUp() {
    if (nullptr != ModAPI) {
        ModAPI->Release();
        ModAPI = nullptr;
    }
};

static bool Init() {
    ModAPI = S4ApiCreate(); // get an interface to the mod api
    if (ModAPI == nullptr) return false;

    auto h = CreateThread(nullptr, 0, static_cast<LPTHREAD_START_ROUTINE>(&InitPlugins), nullptr, 0, nullptr);

    return true;
}

HANDLE GetParentProcess() {
    HANDLE Snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    PROCESSENTRY32 ProcessEntry = {};
    ProcessEntry.dwSize = sizeof(PROCESSENTRY32);

    if (Process32First(Snapshot, &ProcessEntry)) {
        DWORD CurrentProcessId = GetCurrentProcessId();

        do {
            if (ProcessEntry.th32ProcessID == CurrentProcessId)
                break;
        } while (Process32Next(Snapshot, &ProcessEntry));
    }

    CloseHandle(Snapshot);

    return OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, ProcessEntry.th32ParentProcessID);
}

extern "C" BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved) {
    //MessageBoxA(nullptr, "NetModAPI DEBUG", "NetModAPI", 0);
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH: {

        if (GetAsyncKeyState(VK_F2)) {
            return TRUE;
        }
        
        auto parent_handle = GetParentProcess();
        wchar_t process_name[1024] = { 0 };
        DWORD   process_name_size = 1024;
        QueryFullProcessImageNameW(parent_handle, 0, process_name, &process_name_size);
        std::filesystem::path process_file_path(process_name);

        const std::string parentName = process_file_path.filename().string();
        
        // abort if we're not called from Ubisoft Launcher or Internal Call... (Prevents loading in first call)
        if (_stricmp(parentName.c_str(), "UbisoftGameLauncher.exe") != 0 &&
            _stricmp(parentName.c_str(), "UbisoftGameLauncher64.exe") != 0 &&
            _stricmp(parentName.c_str(), "S4_Main.exe") != 0) {
            return TRUE;
        }


#ifndef PUBLIC
        if (GetAsyncKeyState('Q')) {
            MessageBoxA(nullptr, "NetModAPI DEBUG", "NetModAPI", 0);
        }

        AllocConsole();
        freopen("CONOUT$", "w", stdout);
        freopen("CONOUT$", "w", stderr);
#endif // !PUBLIC

        if (!Init()) break;
        break;
    }
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    default:
        break;
    case DLL_PROCESS_DETACH:
        CleanUp();
#ifndef TEST
        //ExitProcess(-1);
#endif
        break;
    }
    return TRUE;
}

#pragma managed(pop)