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
