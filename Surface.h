#pragma once
#include "DirectX.h"


namespace Microsoft::DirectX::DirectDraw {
    public ref class Surface : IDisposable {
    private:
        IDirectDrawSurface7* m_surface;

        HBRUSH m_brush;
        int fillStyle;
        int fillColor;
    public:
        Surface(System::IntPtr^ surface) {
            m_surface = static_cast<IDirectDrawSurface7*>(surface->ToPointer());
        }

        Surface(IDirectDrawSurface7* surface) {
            m_surface = surface;
        }

        ~Surface() {
            DeleteObject(m_brush);
        }

        property IDirectDrawSurface7* Native {
            IDirectDrawSurface7* get() {
                return m_surface;
            }
        }

        HRESULT AddAttachedSurface(LPDIRECTDRAWSURFACE7 a) {
            return m_surface->AddAttachedSurface(a);
        }

        property LPDDCOLORKEY ColorKey {
            LPDDCOLORKEY get() {
                LPDDCOLORKEY colorKey;
                HRESULT_CHECK(m_surface->GetColorKey(0, colorKey));
                return colorKey;
            }
            void set(LPDDCOLORKEY value) {
                HRESULT_CHECK(m_surface->SetColorKey(0, value));
            }
        }

        property LPDIRECTDRAWPALETTE Palette {
            LPDIRECTDRAWPALETTE get() {
                LPDIRECTDRAWPALETTE palette;
                HRESULT_CHECK(m_surface->GetPalette(&palette));
                return palette;
            }
            void set(LPDIRECTDRAWPALETTE value) {
                HRESULT_CHECK(m_surface->SetPalette(value));
            }
        }

        property LPDDPIXELFORMAT PixelFormat {
            LPDDPIXELFORMAT get() {
                LPDDPIXELFORMAT pixelFormat = new DDPIXELFORMAT();
                HRESULT_CHECK(m_surface->GetPixelFormat(pixelFormat));
                return pixelFormat;
            }
        }

        property LPDDSURFACEDESC2 SurfaceDesc {
            LPDDSURFACEDESC2 get() {
                LPDDSURFACEDESC2 surfaceDesc = new DDSURFACEDESC2();
                HRESULT_CHECK(m_surface->GetSurfaceDesc(surfaceDesc));
                return surfaceDesc;
            }
        }

        property long OverlayPositionX {
            long get() {
                long x, y;
                HRESULT_CHECK(m_surface->GetOverlayPosition(&x, &y));
                return x;
            }
            void set(long value) {
                long x, y;
                HRESULT_CHECK(m_surface->GetOverlayPosition(&x, &y));
                HRESULT_CHECK(m_surface->SetOverlayPosition(value, y));
            }
        }

        property long OverlayPositionY {
            long get() {
                long x, y;
                HRESULT_CHECK(m_surface->GetOverlayPosition(&x, &y));
                return y;
            }
            void set(long value) {
                long x, y;
                HRESULT_CHECK(m_surface->GetOverlayPosition(&x, &y));
                HRESULT_CHECK(m_surface->SetOverlayPosition(x, value));
            }
        }

        property int FillStyle {
            int get() {
                return fillStyle;
            }
            void set(int value) {
                fillStyle = value;
            }
        }

        property int FillColor {
            int get() {
                return fillColor;
            }
            void set(int value) {
                fillColor = value;
            }
        }

        void DrawBox(Numerics::Vector4 rect) {
            HDC hdc;

            HRESULT_CHECK(m_surface->GetDC(&hdc));

            const RECT r = ToRECT(rect);
            FillRect(hdc, &r, m_brush);

            HRESULT_CHECK(m_surface->ReleaseDC(hdc));
        }

        void AddOverlayDirtyRect(LPRECT a) {
            HRESULT_CHECK(m_surface->AddOverlayDirtyRect(a));
        }

        void Blt(LPRECT a, LPDIRECTDRAWSURFACE7 b, LPRECT c, DWORD d, LPDDBLTFX e) {
            HRESULT_CHECK(m_surface->Blt(a, b, c, d, e));
        }

        void BltBatch(LPDDBLTBATCH a, DWORD b, DWORD c) {
            HRESULT_CHECK(m_surface->BltBatch(a, b, c));
        }

        void BltFast(DWORD a, DWORD b, LPDIRECTDRAWSURFACE7 c, LPRECT d, DWORD e) {
            HRESULT_CHECK(m_surface->BltFast(a, b, c, d, e));
        }

        void DeleteAttachedSurface(DWORD a, LPDIRECTDRAWSURFACE7 b) {
            HRESULT_CHECK(m_surface->DeleteAttachedSurface(a, b));
        }

        void EnumAttachedSurfaces(LPVOID a, LPDDENUMSURFACESCALLBACK7 b) {
            HRESULT_CHECK(m_surface->EnumAttachedSurfaces(a, b));
        }

        void EnumOverlayZOrders(DWORD a, LPVOID b, LPDDENUMSURFACESCALLBACK7 c) {
            HRESULT_CHECK(m_surface->EnumOverlayZOrders(a, b, c));
        }

        void Flip(LPDIRECTDRAWSURFACE7 a, DWORD b) {
            HRESULT_CHECK(m_surface->Flip(a, b));
        }

        void GetAttachedSurface(LPDDSCAPS2 a, LPDIRECTDRAWSURFACE7* b) {
            HRESULT_CHECK(m_surface->GetAttachedSurface(a, b));
        }

        void GetBltStatus(DWORD a) {
            HRESULT_CHECK(m_surface->GetBltStatus(a));
        }

        void GetCaps(LPDDSCAPS2 a) {
            HRESULT_CHECK(m_surface->GetCaps(a));
        }

        void GetDC(HDC* a) {
            HRESULT_CHECK(m_surface->GetDC(a));
        }

        void GetFlipStatus(DWORD a) {
            HRESULT_CHECK(m_surface->GetFlipStatus(a));
        }

        void Initialize(LPDIRECTDRAW a, LPDDSURFACEDESC2 b) {
            HRESULT_CHECK(m_surface->Initialize(a, b));
        }

        void IsLost() {
            HRESULT_CHECK(m_surface->IsLost());
        }

        void Lock(LPRECT a, LPDDSURFACEDESC2 b, DWORD c, HANDLE d) {
            HRESULT_CHECK(m_surface->Lock(a, b, c, d));
        }

        void ReleaseDC(HDC a) {
            HRESULT_CHECK(m_surface->ReleaseDC(a));
        }

        void Restore() {
            HRESULT_CHECK(m_surface->Restore());
        }

        void Unlock(LPRECT a) {
            HRESULT_CHECK(m_surface->Unlock(a));
        }

        void UpdateOverlay(LPRECT a, LPDIRECTDRAWSURFACE7 b, LPRECT c, DWORD d, LPDDOVERLAYFX e) {
            HRESULT_CHECK(m_surface->UpdateOverlay(a, b, c, d, e));
        }

        void UpdateOverlayDisplay(DWORD a) {
            HRESULT_CHECK(m_surface->UpdateOverlayDisplay(a));
        }

        void UpdateOverlayZOrder(DWORD a, LPDIRECTDRAWSURFACE7 b) {
            HRESULT_CHECK(m_surface->UpdateOverlayZOrder(a, b));
        }
    };
}
