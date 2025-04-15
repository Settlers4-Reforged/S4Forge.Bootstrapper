#include "pch.h"
#include "Patching.h"


void NetModAPI::Patch::Apply() {
    this->m_handle->patch();
}

void NetModAPI::Patch::Revert() {
    this->m_handle->unpatch();
}


NetModAPI::Patch::Patch(hlib::AbstractPatch* handle) {
    this->m_handle = handle;
}

NetModAPI::Patch::Patch(int address, array<unsigned char>^ patch) {
    const void* patchNative = System::Runtime::InteropServices::Marshal::UnsafeAddrOfPinnedArrayElement(patch, 0).ToPointer();
    this->m_handle = new hlib::Patch(address, patchNative, patch->Length);
}

NetModAPI::Patch::Patch(int address, array<unsigned char>^ patch, array<unsigned char>^ expectedOrigin) {
    const void* patchNative = System::Runtime::InteropServices::Marshal::UnsafeAddrOfPinnedArrayElement(patch, 0).ToPointer();
    const void* expectedOriginNative = System::Runtime::InteropServices::Marshal::UnsafeAddrOfPinnedArrayElement(expectedOrigin, 0).ToPointer();
    this->m_handle = new hlib::Patch(address, patchNative, expectedOriginNative, patch->Length);
}

NetModAPI::Patch::Patch(int address, System::Int32 patch) {
    this->m_handle = new hlib::Patch(address, static_cast<DWORD>(patch));
}

NetModAPI::Patch::Patch(int address, System::Int32 patch, System::Int32 expectedOrig) {
    this->m_handle = new hlib::Patch(address, static_cast<DWORD>(patch), expectedOrig);
}

NetModAPI::Patch::Patch(int address, unsigned char patch) {
    this->m_handle = new hlib::Patch(address, patch);
}

NetModAPI::Patch::Patch(int address, unsigned char patch, unsigned char expected) {
    this->m_handle = new hlib::Patch(address, patch, expected);
}

NetModAPI::JmpPatch::JmpPatch(System::IntPtr address, System::IntPtr jumpTargetAddress, int nops) : Patch(new hlib::JmpPatch(address.ToInt32(), jumpTargetAddress.ToInt32(), nops)) {}

NetModAPI::JmpPatch::JmpPatch(System::IntPtr address, System::IntPtr jumpTargetAddress, array<unsigned char>^ expectedOrigin, int nops) {
    if(expectedOrigin->Length > 4) throw gcnew System::ArgumentException("expectedOrigin cannot be longer than 4");

    if(expectedOrigin->Length < 4) {
        array<unsigned char>^ tmp = gcnew array<unsigned char>(4);
        expectedOrigin->CopyTo(tmp, 0);
        expectedOrigin = tmp;
    }
    const hlib::Patch::BYTE5 expectedOriginNative{expectedOrigin[0], expectedOrigin[1], expectedOrigin[2], expectedOrigin[3]};

    this->m_handle = new hlib::JmpPatch(address.ToInt32(), jumpTargetAddress.ToInt32(), &expectedOriginNative, nops);
}

NetModAPI::CallPatch::CallPatch(System::IntPtr address, System::IntPtr callTargetAddress, int nops) {
    this->m_handle = new hlib::CallPatch(address.ToInt32(), callTargetAddress.ToInt32(), nops);
}

NetModAPI::CallPatch::CallPatch(System::IntPtr address, System::IntPtr callTargetAddress, array<unsigned char>^ expectedOrigin, int nops) {
    if(expectedOrigin->Length > 4) throw gcnew System::ArgumentException("expectedOrigin cannot be longer than 4");

    if(expectedOrigin->Length < 4) {
        array<unsigned char>^ tmp = gcnew array<unsigned char>(4);
        expectedOrigin->CopyTo(tmp, 0);
        expectedOrigin = tmp;
    }
    const hlib::Patch::BYTE5 expectedOriginNative{expectedOrigin[0], expectedOrigin[1], expectedOrigin[2], expectedOrigin[3]};

    this->m_handle = new hlib::CallPatch(address.ToInt32(), callTargetAddress.ToInt32(), &expectedOriginNative, nops);
}

NetModAPI::NopPatch::NopPatch(System::IntPtr address, int nops) {
    this->m_handle = new hlib::NopPatch(address.ToInt32(), nops);
}

NetModAPI::NopPatch::NopPatch(System::IntPtr address, array<unsigned char>^ expectedOrigin, int nops) {
    if (expectedOrigin->Length > 4) throw gcnew System::ArgumentException("expectedOrigin cannot be longer than 4");

    if (expectedOrigin->Length < 4) {
        array<unsigned char>^ tmp = gcnew array<unsigned char>(4);
        expectedOrigin->CopyTo(tmp, 0);
        expectedOrigin = tmp;
    }

    const hlib::Patch::BYTE5 expectedOriginNative{ expectedOrigin[0], expectedOrigin[1], expectedOrigin[2], expectedOrigin[3] };

    this->m_handle = new hlib::NopPatch(address.ToInt32(), &expectedOriginNative, nops);
}
