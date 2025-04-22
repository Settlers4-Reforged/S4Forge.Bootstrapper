#pragma once
#include "DirectX.h"
#include "d3d9.h"

#pragma make_public(RECT)
#pragma make_public(IDirect3DTexture9)

namespace Microsoft::DirectX::DirectDraw {

    public ref struct SurfaceDesc {
    public:
        DWORD dwSize;
        DWORD dwFlags;
        DWORD dwHeight;
        DWORD dwWidth;
        LONG lPitch;
        DWORD dwBackBufferCount;
        DWORD dwMipMapCount;
        DWORD dwZBufferBitDepth;
        DWORD dwAlphaBitDepth;
        DWORD dwReserved;
        LPVOID lpSurface;
        //DDSCAPS2 ddsCaps;
        DWORD dwTextureStage;
    };

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

        property IDirect3DSurface9* NativeSurface {
            IDirect3DSurface9* get() {
                return reinterpret_cast<IDirect3DSurface9*>(reinterpret_cast<int*>(m_surface)[43]);
            }
        }

        property IDirect3DDevice9* Device {
            IDirect3DDevice9* get() {
                IDirect3DDevice9* device;
                NativeSurface->GetDevice(&device);
                return device;
            }
        }

        property IDirect3D9* Direct3D {
            IDirect3D9* get() {
                IDirect3D9* d3d;
                Device->GetDirect3D(&d3d);
                return d3d;
            }
        }

        void SetAsDeviceRenderTarget() {
            IDirect3DDevice9* device;
            NativeSurface->GetDevice(&device);
            device->SetRenderTarget(0, NativeSurface);
        }

        // ResetD3D and PrepareD3D are used for SDL compatibility rendering

        IDirect3DVertexShader9* defaultVertexShader;
        IDirect3DPixelShader9* defaultPixelShader;
        DWORD defaultMinFilter, defaultMagFilter;
        void ResetD3D() {
            D3DPERF_BeginEvent(0xff00ff, L"Cleanup SDL");

            IDirect3DDevice9* device = Device;

            // Not resetting will prevent any rendering
            IDirect3DDevice9_SetVertexShader(device, defaultVertexShader);
            IDirect3DDevice9_SetPixelShader(device, defaultPixelShader);

            //TODO: find a way to actually have linear+ filtering as it can drastically improve the look of the game
            // Not resetting will lead to "smearing" in the terrain engine
            IDirect3DDevice9_SetSamplerState(device, 0, D3DSAMP_MINFILTER, defaultMinFilter);
            // Not resetting will break the color keying for e.g. the chat
            IDirect3DDevice9_SetSamplerState(device, 0, D3DSAMP_MAGFILTER, defaultMinFilter);

            D3DPERF_EndEvent();

            D3DPERF_EndEvent(); //SDL RENDER
        }

        IDirect3DSurface9 *mainRenderTarget;
        void RestoreMainRenderTarget() {
            IDirect3DDevice9* device = Device;
            IDirect3DDevice9_SetRenderTarget(device, 0, mainRenderTarget);
        }

        void PrepareD3D() {
            IDirect3DDevice9* device = Device;

            D3DPERF_SetMarker(0xffff00, L"Start SDL Render");

            D3DPERF_BeginEvent(0xff00ff, L"SDL Render");
            D3DPERF_BeginEvent(0xff00ff, L"Prepare SDL");

            // Fetch previous state:
            DWORD _defaultMinFilter, _defaultMagFilter;
            IDirect3DDevice9_GetSamplerState(device, 0, D3DSAMP_MINFILTER, &_defaultMinFilter);
            IDirect3DDevice9_GetSamplerState(device, 0, D3DSAMP_MAGFILTER, &_defaultMagFilter);
            IDirect3DDevice9_SetSamplerState(device, 0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
            IDirect3DDevice9_SetSamplerState(device, 0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
            defaultMinFilter = _defaultMinFilter;
            defaultMagFilter = _defaultMagFilter;

            IDirect3DPixelShader9* _defaultPixelShader;
            IDirect3DDevice9_GetPixelShader(device, &_defaultPixelShader);
            defaultPixelShader = _defaultPixelShader;

            // Reset state to a SDL compatible state
            IDirect3DDevice9_SetPixelShader(device, NULL);
            IDirect3DDevice9_SetTexture(device, 0, NULL);
            IDirect3DDevice9_SetTexture(device, 1, NULL);
            IDirect3DDevice9_SetTexture(device, 2, NULL);
            IDirect3DDevice9_SetFVF(device, D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1);

            IDirect3DVertexShader9* _defaultVertexShader;
            IDirect3DDevice9_GetVertexShader(device, &_defaultVertexShader);
            defaultVertexShader = _defaultVertexShader;
            IDirect3DDevice9_SetVertexShader(device, NULL);
            IDirect3DDevice9_SetRenderState(device, D3DRS_ZENABLE, D3DZB_FALSE);
            IDirect3DDevice9_SetRenderState(device, D3DRS_CULLMODE, D3DCULL_NONE);
            IDirect3DDevice9_SetRenderState(device, D3DRS_LIGHTING, FALSE);

            // Enable color modulation by diffuse color
            IDirect3DDevice9_SetTextureStageState(device, 0, D3DTSS_COLOROP,
                D3DTOP_MODULATE);
            IDirect3DDevice9_SetTextureStageState(device, 0, D3DTSS_COLORARG1,
                D3DTA_TEXTURE);
            IDirect3DDevice9_SetTextureStageState(device, 0, D3DTSS_COLORARG2,
                D3DTA_DIFFUSE);

            // Enable alpha modulation by diffuse alpha
            IDirect3DDevice9_SetTextureStageState(device, 0, D3DTSS_ALPHAOP,
                D3DTOP_MODULATE);
            IDirect3DDevice9_SetTextureStageState(device, 0, D3DTSS_ALPHAARG1,
                D3DTA_TEXTURE);
            IDirect3DDevice9_SetTextureStageState(device, 0, D3DTSS_ALPHAARG2,
                D3DTA_DIFFUSE);

            // Disable second texture stage, since we're done
            IDirect3DDevice9_SetTextureStageState(device, 1, D3DTSS_COLOROP,
                D3DTOP_DISABLE);
            IDirect3DDevice9_SetTextureStageState(device, 1, D3DTSS_ALPHAOP,
                D3DTOP_DISABLE);

            // Enable separate alpha blend function, if possible
            IDirect3DDevice9_SetRenderState(device, D3DRS_SEPARATEALPHABLENDENABLE, TRUE);

            // Set an identity world and view matrix
            D3DMATRIX matrix{};
            matrix.m[0][0] = 1.0f;
            matrix.m[1][1] = 1.0f;
            matrix.m[2][2] = 1.0f;
            matrix.m[3][3] = 1.0f;
            IDirect3DDevice9_SetTransform(device, D3DTS_WORLD, &matrix);
            IDirect3DDevice9_SetTransform(device, D3DTS_VIEW, &matrix);
            IDirect3DDevice9_SetTransform(device, D3DTS_PROJECTION, &matrix);
            IDirect3DDevice9_SetTransform(device, D3DTS_TEXTURE0, &matrix); // Texture matrix for Tooltips change in game, this reverts it to identity

            D3DVIEWPORT9 viewport = {};
            IDirect3DDevice9_GetViewport(device, &viewport);

            IDirect3DSurface9 *renderTarget;
            IDirect3DDevice9_GetRenderTarget(device, 0, &renderTarget);
            this->mainRenderTarget = renderTarget;

            D3DPERF_EndEvent();
        }

        void BeginDeviceScene() {
            IDirect3DDevice9* device;
            NativeSurface->GetDevice(&device);
            HRESULT result = device->BeginScene();
            if (result != D3D_OK) {
                throw gcnew System::Exception("EndScene failed");
            }
        }

        void EndDeviceScene() {
            IDirect3DDevice9* device;
            NativeSurface->GetDevice(&device);
            HRESULT result = device->EndScene();
            if (result != D3D_OK) {
                throw gcnew System::Exception("EndScene failed");
            }
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
                LPDDCOLORKEY colorKey{};
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

        Microsoft::DirectX::DirectDraw::SurfaceDesc^ Lock(RECT* a, DWORD flags, HANDLE d) {
            DDSURFACEDESC2 desc;
            ZeroMemory(&desc, sizeof(desc));
            desc.dwSize = sizeof(desc);

            HRESULT_CHECK(m_surface->Lock(a, &desc, flags, d));

            Microsoft::DirectX::DirectDraw::SurfaceDesc^ surfaceDesc = gcnew Microsoft::DirectX::DirectDraw::SurfaceDesc();
            surfaceDesc->dwSize = desc.dwSize;
            surfaceDesc->dwFlags = desc.dwFlags;
            surfaceDesc->dwHeight = desc.dwHeight;
            surfaceDesc->dwWidth = desc.dwWidth;
            surfaceDesc->lPitch = desc.lPitch;
            surfaceDesc->lpSurface = desc.lpSurface;
            //TODO: maybe fill in other data

            return surfaceDesc;
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
