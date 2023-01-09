#include "pch.h"
#include "ManagedLoader.h"

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

extern "C" BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved) {
    //MessageBoxA(nullptr, "NetModAPI DEBUG", "NetModAPI", 0);
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH: {

        if (GetAsyncKeyState(VK_F2)) {
            return TRUE;
        }

#ifndef PUBLIC
        if (GetAsyncKeyState('Q')) {
            MessageBoxA(nullptr, "NetModAPI DEBUG", "NetModAPI", 0);
        }
#endif // !PUBLIC

        if (!Init()) break;
        break;
    }
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
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