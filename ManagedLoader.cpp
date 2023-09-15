#include "pch.h"
#include "ManagedIncludes.h"
#include "ManagedLoader.h"

DWORD __stdcall InitPlugins(void* param) {
    NetModAPI::NetModAPI::LoadForge();
    return 0;
}