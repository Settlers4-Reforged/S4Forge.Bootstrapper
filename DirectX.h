#pragma once

#define HRESULT_CHECK(hr) \
    if (int code = hr; FAILED(code)) { \
        throw gcnew System::Exception("Operation '"#hr"' failed with a non zero status code: " + code.ToString());\
    }

inline RECT ToRECT(System::Numerics::Vector4 rect) {
    return RECT{static_cast<LONG>(rect.X), static_cast<LONG>(rect.Y), static_cast<LONG>(rect.Z), static_cast<LONG>(rect.W)};
}

#include "Device.h"
#include "Surface.h"

public ref class DXDebugHelper {
public:
    static void BeginEvent(unsigned int color, System::String^ name) {
        using namespace System::Runtime::InteropServices;

        wchar_t* wcsName = (wchar_t*)Marshal::StringToHGlobalUni(name).ToPointer();
        D3DPERF_BeginEvent(color, wcsName);
        Marshal::FreeHGlobal((System::IntPtr)wcsName);
    }
    static void SetMarker(unsigned int color, System::String^ name) {
        using namespace System::Runtime::InteropServices;

        wchar_t* wcsName = (wchar_t*)Marshal::StringToHGlobalUni(name).ToPointer();
        D3DPERF_SetMarker(color, wcsName);
        Marshal::FreeHGlobal((System::IntPtr)wcsName);
    }
    static void EndEvent(void) {
        D3DPERF_EndEvent();
    }
};