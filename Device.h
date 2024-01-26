#pragma once

namespace Microsoft::DirectX::DirectDraw {
    public ref class Device {
    private:
        IDirectDraw* _device;

    public:

        Device(System::IntPtr^ device) {
            _device = static_cast<IDirectDraw*>(device->ToPointer());
        }

        Device(IDirectDraw* device) {
            _device = device;
        }

        ~Device() {
            _device->Release();
        }

        property IDirectDraw* Native {
            IDirectDraw* get() {
                return _device;
            }
        }

        HRESULT(Initialize)(GUID FAR* a) {
            return _device->Initialize(a);
        }

        HRESULT(Compact)() {
            return _device->Compact();
        }
        HRESULT(CreateClipper)(long a, LPDIRECTDRAWCLIPPER FAR* b, IUnknown FAR* c) {
            return _device->CreateClipper(a, b, c);
        }
        HRESULT(CreatePalette)(long a, LPPALETTEENTRY b, LPDIRECTDRAWPALETTE FAR* c, IUnknown FAR* d) {
            return _device->CreatePalette(a, b, c, d);
        }
        HRESULT(CreateSurface)(LPDDSURFACEDESC a, LPDIRECTDRAWSURFACE FAR* b, IUnknown FAR* c) {
            return _device->CreateSurface(a, b, c);
        }
        HRESULT(DuplicateSurface)(LPDIRECTDRAWSURFACE a, LPDIRECTDRAWSURFACE FAR* b) {
            return _device->DuplicateSurface(a, b);
        }
        HRESULT(EnumDisplayModes)(long a, LPDDSURFACEDESC b, LPVOID c, LPDDENUMMODESCALLBACK d) {
            return _device->EnumDisplayModes(a, b, c, d);
        }
        HRESULT(EnumSurfaces)(long a, LPDDSURFACEDESC b, LPVOID c, LPDDENUMSURFACESCALLBACK d) {
            return _device->EnumSurfaces(a, b, c, d);
        }
        HRESULT(FlipToGDISurface)() {
            return _device->FlipToGDISurface();
        }
        HRESULT(SetCooperativeLevel)(HWND a, long b) {
            return _device->SetCooperativeLevel(a, b);
        }

        HRESULT(SetDisplayMode)(long a, long b, long c) {
            return _device->SetDisplayMode(a, b, c);
        }
        HRESULT(GetDisplayMode)(LPDDSURFACEDESC a) {
            return _device->GetDisplayMode(a);
        }
        HRESULT(RestoreDisplayMode)() {
            return _device->RestoreDisplayMode();
        }

        HRESULT(WaitForVerticalBlank)(long a, HANDLE b) {
            return _device->WaitForVerticalBlank(a, b);
        }

        HRESULT(GetCaps)(LPDDCAPS a, LPDDCAPS b) {
            return _device->GetCaps(a, b);
        }
        HRESULT(GetFourCCCodes)(LPDWORD a, LPDWORD b) {
            return _device->GetFourCCCodes(a, b);
        }
        HRESULT(GetGDISurface)(LPDIRECTDRAWSURFACE FAR* a) {
            return _device->GetGDISurface(a);
        }
        HRESULT(GetMonitorFrequency)(LPDWORD a) {
            return _device->GetMonitorFrequency(a);
        }
        HRESULT(GetScanLine)(LPDWORD a) {
            return _device->GetScanLine(a);
        }
        HRESULT(GetVerticalBlankStatus)(LPBOOL a) {
            return _device->GetVerticalBlankStatus(a);
        }
    };
}