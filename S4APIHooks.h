#pragma once
#pragma unmanaged

#include <functional>
#include <vector>

extern int callbackIndex;

template <class func>
struct Callback {
    int                 id;
    std::function<func> callbackFunction;

    Callback(int id, std::function<func> callback) {
        this->id = id;
        this->callbackFunction = callback;
    }
};

template <class func>
static Callback<func>* CreateCallback(std::function<func>* callback) {
    Callback<func>* c = new Callback<func>(++callbackIndex, *callback);
    return c;
}

/** Callback types **/
typedef HRESULT (S4FRAMECALLBACK)(LPDIRECTDRAWSURFACE7 lpSurface, INT32 iPillarboxWidth, LPVOID lpReserved);
typedef HRESULT (S4MAPINITCALLBACK)(LPVOID lpReserved0, LPVOID lpReserved1);
typedef HRESULT (S4MOUSECALLBACK)(DWORD dwMouseButton, INT iX, INT iY, DWORD dwMsgId, HWND hwnd, LPCS4UIELEMENT lpUiElement);
typedef HRESULT (S4SETTLERSENDCALLBACK)(DWORD dwPosition, S4_MOVEMENT_ENUM dwCommand, LPVOID lpReserved);
typedef HRESULT (S4TICKCALLBACK)(DWORD dwTick, BOOL bHasEvent, BOOL bIsDelayed);
typedef HRESULT (S4LUAOPENCALLBACK)(VOID);
typedef BOOL (S4BLTCALLBACK)(LPS4BLTPARAMS params, BOOL discard);
typedef BOOL (S4GUIBLTCALLBACK)(LPS4GUIBLTPARAMS params, BOOL discard);
typedef HRESULT (S4ENTITYCALLBACK)(WORD entity, S4_ENTITY_CAUSE cause); // called when an entity is spawned or destructed // todo: implement me
typedef HRESULT (S4GUIDRAWCALLBACK)(LPS4GUIDRAWBLTPARAMS entity, BOOL discard);
typedef HRESULT (S4GUICLEARCALLBACK)(LPS4GUICLEARPARAMS entity, BOOL discard);
typedef LRESULT (S4WNDPROC)(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

#define DEFINE_CALLBACK_LIST(type, name) extern std::vector<Callback<type>*> name##List;


DEFINE_CALLBACK_LIST(S4FRAMECALLBACK, S4FrameCallback)
DEFINE_CALLBACK_LIST(S4FRAMECALLBACK, S4UIFrameCallback)
DEFINE_CALLBACK_LIST(S4MAPINITCALLBACK, S4MapInitCallback)
DEFINE_CALLBACK_LIST(S4MOUSECALLBACK, S4MouseCallback)
DEFINE_CALLBACK_LIST(S4SETTLERSENDCALLBACK, S4SettlerSendCallback)
DEFINE_CALLBACK_LIST(S4TICKCALLBACK, S4TickCallback)
DEFINE_CALLBACK_LIST(S4LUAOPENCALLBACK, S4LuaOpenCallback)
DEFINE_CALLBACK_LIST(S4BLTCALLBACK, S4BltCallback)
DEFINE_CALLBACK_LIST(S4GUIBLTCALLBACK, S4GUIBltCallback)
DEFINE_CALLBACK_LIST(S4ENTITYCALLBACK, S4EntityCallback)
DEFINE_CALLBACK_LIST(S4GUIDRAWCALLBACK, S4GUIDrawCallback)
DEFINE_CALLBACK_LIST(S4GUICLEARCALLBACK, S4GUIClearCallback)
DEFINE_CALLBACK_LIST(S4WNDPROC, S4WndProc)

template <class func, class... Rest>
static HRESULT HandleCallback(std::vector<Callback<func>*> functions, Rest... rest) {
    for(auto call : functions) {
        const auto result = call->callbackFunction(rest...);
        if(result != 0) {
            return result;
        }
    }

    return 0;
}

template <class func>
static BOOL RemoveCallback(int index, std::vector<Callback<func>*> list) {
    for(auto callback : list) {
        if(callback->id == index)
            return TRUE;
    }

    return FALSE;
}

#define CONDITIONAL(r) if(r) return;

static void RemoveAnyCallback(int index) {
    CONDITIONAL(RemoveCallback(index,S4FrameCallbackList));
    CONDITIONAL(RemoveCallback(index,S4UIFrameCallbackList));
    CONDITIONAL(RemoveCallback(index,S4MapInitCallbackList));
    CONDITIONAL(RemoveCallback(index,S4MouseCallbackList));
    CONDITIONAL(RemoveCallback(index,S4SettlerSendCallbackList));
    CONDITIONAL(RemoveCallback(index,S4TickCallbackList));
    CONDITIONAL(RemoveCallback(index,S4LuaOpenCallbackList));
    CONDITIONAL(RemoveCallback(index,S4BltCallbackList));
    CONDITIONAL(RemoveCallback(index,S4GUIBltCallbackList));
    CONDITIONAL(RemoveCallback(index,S4EntityCallbackList));
    CONDITIONAL(RemoveCallback(index,S4GUIDrawCallbackList));
    CONDITIONAL(RemoveCallback(index,S4GUIClearCallbackList));
    CONDITIONAL(RemoveCallback(index,S4WndProcList));
}


static HRESULT __stdcall CS4FrameCallback(LPDIRECTDRAWSURFACE7 lpSurface, INT32 iPillarboxWidth, LPVOID lpReserved) {
    return HandleCallback(S4FrameCallbackList, lpSurface, iPillarboxWidth, lpReserved);
}

static HRESULT __stdcall CS4UIFrameCallback(LPDIRECTDRAWSURFACE7 lpSurface, INT32 iPillarboxWidth, LPVOID lpReserved) {
    return HandleCallback(S4UIFrameCallbackList, lpSurface, iPillarboxWidth, lpReserved);
}

static HRESULT __stdcall CS4MapInitCallback(LPVOID lpReserved0, LPVOID lpReserved1) {
    return HandleCallback(S4MapInitCallbackList, lpReserved0, lpReserved1);
}

static HRESULT __stdcall CS4MouseCallback(DWORD dwMouseButton, INT iX, INT iY, DWORD dwMsgId, HWND hwnd, LPCS4UIELEMENT lpUiElement) {
    return HandleCallback(S4MouseCallbackList, dwMouseButton, iX, iY, dwMsgId, hwnd, lpUiElement);
}

static HRESULT __stdcall CS4SettlerSendCallback(DWORD dwPosition, S4_MOVEMENT_ENUM dwCommand, LPVOID lpReserved) {
    return HandleCallback(S4SettlerSendCallbackList, dwPosition, dwCommand, lpReserved);
}

static HRESULT __stdcall CS4TickCallback(DWORD dwTick, BOOL bHasEvent, BOOL bIsDelayed) {
    return HandleCallback(S4TickCallbackList, dwTick, bHasEvent, bIsDelayed);
}

static HRESULT __stdcall CS4LuaOpenCallback(VOID) {
    return HandleCallback(S4LuaOpenCallbackList);
}

static BOOL __stdcall CS4BltCallback(LPS4BLTPARAMS params, BOOL discard) {
    return FAILED(HandleCallback(S4BltCallbackList, params, discard));
}

static BOOL __stdcall CS4GUIBltCallback(LPS4GUIBLTPARAMS params, BOOL discard) {
    return FAILED(HandleCallback(S4GUIBltCallbackList, params, discard));
}

static HRESULT __stdcall CS4EntityCallback(WORD entity, S4_ENTITY_CAUSE cause) {
    return HandleCallback(S4EntityCallbackList, entity, cause);
}

static HRESULT __stdcall CS4GUIDrawCallback(LPS4GUIDRAWBLTPARAMS entity, BOOL discard) {
    return HandleCallback(S4GUIDrawCallbackList, entity, discard);
}

static HRESULT __stdcall CS4GUIClearCallback(LPS4GUICLEARPARAMS entity, BOOL discard) {
    return HandleCallback(S4GUIClearCallbackList, entity, discard);
}


extern LONG_PTR oldWndProc;

static LRESULT CALLBACK HookedWndProcedure(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    HandleCallback(S4WndProcList, hwnd, uMsg, wParam, lParam);

    return CallWindowProc((WNDPROC)oldWndProc, hwnd, uMsg, wParam, lParam);
}


#pragma managed
