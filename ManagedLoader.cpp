#include "pch.h"
#include "ManagedIncludes.h"
#include "ManagedLoader.h"

DWORD __stdcall InitPlugins(void* param) {
    NetModAPI::NetModAPI::LoadAllPlugins();
    return 0;
}