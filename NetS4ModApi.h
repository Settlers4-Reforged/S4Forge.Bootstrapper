#pragma once

#include "S4APIHooks.h"
#include "S4Hooks.h"

#pragma make_public(IDirectDrawSurface7)

using namespace System;
using namespace Runtime::InteropServices;
using namespace Collections::Generic;

namespace NetModAPI {
    delegate HRESULT S4FrameCallback(LPDIRECTDRAWSURFACE7 lpSurface, INT32 iPillarboxWidth, LPVOID lpReserved);
    delegate HRESULT S4MapInitCallback(LPVOID lpReserved0, LPVOID lpReserved1);
    delegate HRESULT S4MouseCallback(DWORD dwMouseButton, INT iX, INT iY, DWORD dwMsgId, HWND hwnd, LPCS4UIELEMENT lpUiElement);
    delegate HRESULT S4SettlerSendCallback(DWORD dwPosition, S4_MOVEMENT_ENUM dwCommand, LPVOID lpReserved);
    delegate HRESULT S4TickCallback(DWORD dwTick, BOOL bHasEvent, BOOL bIsDelayed);
    delegate HRESULT S4LuaOpenCallback();
    delegate BOOL S4BltCallback(LPS4BLTPARAMS params, BOOL discard);
    delegate BOOL S4GuiBltCallback(LPS4GUIBLTPARAMS params, BOOL discard);
    delegate HRESULT S4EntityCallback(WORD entity, S4_ENTITY_CAUSE cause); // called when an entity is spawned or destructed // todo: implement me
    delegate HRESULT S4GuiDrawCallback(LPS4GUIDRAWBLTPARAMS entity, BOOL discard);
    delegate HRESULT S4GuiClearCallback(LPS4GUICLEARPARAMS entity, BOOL discard);
    delegate LRESULT S4WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    interface class INetS4ModApi {
        /** Hooks/Observers **/
        void RemoveListener(S4HOOK hook);
        S4HOOK  AddFrameListener(S4FrameCallback^ clbk);
        S4HOOK  AddUIFrameListener(S4FrameCallback^ clbk, S4_GUI_ENUM gui);
        S4HOOK  AddMapInitListener(S4MapInitCallback^ clbk);
        S4HOOK  AddMouseListener(S4MouseCallback^ clbk);
        S4HOOK  AddSettlerSendListener(S4SettlerSendCallback^ clbk);
        S4HOOK  AddTickListener(S4TickCallback^ clbk);
        S4HOOK  AddLuaOpenListener(S4LuaOpenCallback^ clbk);
        S4HOOK  AddBltListener(S4BltCallback^ clbk);
        S4HOOK  AddEntityListener(S4EntityCallback^ clbk);
        S4HOOK  AddGuiBltListener(S4GuiBltCallback^ clbk);
        S4HOOK  AddGuiClearListener(S4GuiClearCallback^ clbk);
        S4HOOK  AddGuiElementBltListener(S4GuiDrawCallback^ clbk);

        /** WndProc **/
        S4HOOK  AddWndProc(S4WndProc^ clbk);

        /** Event Engine **/
        byte PostToMessageQueue(void* eventEngine, void* event);
        void* CreateS4Event(void* _this, unsigned int a2, unsigned int a3, unsigned int a4, char a5);
        void* CreateS4UIEvent(void* _this, unsigned int a2, unsigned int a3, unsigned int a4, char a5, unsigned int a6, __int16 a7);

        /** Sound **/
        void PlaySound(int id);
        int GetMusicVolume();
        void SetMusicVolume(int volume);
        int GetMusicEnabled();
        void ToggleMusic(bool enabled);

        int GetSoundVolume();
        void SetSoundVolume(int volume);
        int GetSoundEnabled();
        void ToggleSound(bool enabled);

        /** ISettlers4Api methods **/
        LPVOID  GetDebugData(LPVOID a, LPVOID b);
        DWORD  GetLastError();

        /** Misc helper functions **/
        HRESULT GetMD5OfModule(HMODULE module, LPSTR out, SIZE_T sz);
        BOOL  IsEdition(S4_EDITION_ENUM edition);
        HWND  GetHwnd();

        /** Settlers 4 functions **/
        HRESULT GetHoveringUiElement(LPS4UIELEMENT element);
        BOOL  GetEntitiesCount(WORD* counter);

        BOOL  IsCurrentlyOnScreen(S4_GUI_ENUM gui);
        S4_ENTITY_ENUM  EntityGetClass(WORD entity);
        BOOL  GetEntities(DWORD* entities, size_t size);
        BOOL  EntityGetPosition(WORD entity, LPINT x, LPINT y);
        BOOL  EntityGetDirection(WORD entity, LPDWORD dir);
        BOOL  EntityGetHealth(WORD entity, LPDWORD health);
        BOOL  EntityGetMaxHealth(WORD entity, LPDWORD maxHealth);
        BOOL  EntityGetTribe(WORD entity, S4_TRIBE_ENUM* tribe);
        BOOL  EntityTreeGetType(WORD entity, S4_TREE_ENUM* tree);
        BOOL  EntityPileGetType(WORD entity, S4_GOOD_ENUM* good);
        BOOL  EntityDecoGetType(WORD entity, LPDWORD deco);
        BOOL  EntityPileGetAmount(WORD entity, LPDWORD amount);
        BOOL  EntitySettlerGetType(WORD entity, S4_SETTLER_ENUM* settler);
        BOOL  EntityVehicleGetType(WORD entity, S4_VEHICLE_ENUM* vehicle);
        BOOL  EntityAnimalGetType(WORD entity, S4_ANIMAL_ENUM* animal);
        BOOL  EntityBuildingGetType(WORD entity, S4_BUILDING_ENUM* building);
        BOOL  EntityStoneGetLevel(WORD entity, LPDWORD level);
        BOOL  EntityGetRole(WORD entity, LPVOID* role);
        BOOL  EntityGetRoleClass(WORD entity, S4_ROLE_ENUM* role);
        BOOL  EntityGetOwner(WORD entity, LPDWORD player);

        BOOL  ClearSelection();
        BOOL  GetSelection(PWORD out, SIZE_T outlen, PSIZE_T selectionCount);
        BOOL  RemoveSelection(PWORD settlers, SIZE_T settlerslen, PSIZE_T removedCount);
        DWORD  SetMaxSelection(DWORD maxSelection);
        DWORD  GetMaxSelection();
        BOOL  StartBuildingPlacement(S4_BUILDING_ENUM building);
        DWORD  GetGameTime();

        /** Landscape functions **/
        DWORD  LandscapeGetHeight(INT x, INT y);
        S4_GROUND_ENUM  LandscapeGetType(INT x, INT y);
        BOOL  LandscapeIsPond(INT x, INT y);
        BOOL  LandscapeIsDarkLand(INT x, INT y);
        BOOL  LandscapeIsDarkLandBorder(INT x, INT y);
        DWORD  LandscapeGetFogOfWar(INT x, INT y);
        BOOL  LandscapeIsFoundingStone(INT x, INT y);
        S4_RESOURCE_ENUM  LandscapeGetResource(INT x, INT y);
        BOOL  LandscapeSetResource(INT x, INT y, S4_RESOURCE_ENUM res);
        DWORD  LandscapeGetObject(INT x, INT y);
        DWORD  LandscapeGetOwner(INT x, INT y);
        DWORD  LandscapeGetEcoSector(INT x, INT y);
        BOOL  LandscapeIsOccupied(INT x, INT y);

        /** Settlers 4 NetEvents functions **/
        BOOL  SendWarriors(INT x, INT y, S4_MOVEMENT_ENUM mode, PWORD warriors, SIZE_T countOfWarriors, DWORD player);
        BOOL  BuildBuilding(S4_BUILDING_ENUM buildingType, INT x, INT y, DWORD player);
        BOOL  CrushBuilding(DWORD building, DWORD player);
        BOOL  ToggleBuildingPriority(DWORD building, DWORD player);
        BOOL  ToggleBuildingHalt(DWORD building, DWORD player);
        BOOL  SetBuildingWorkarea(DWORD building, INT x, INT y, DWORD player);
        BOOL  SetBuildingProduction(DWORD building, S4_GOOD_ENUM good, INT amount, DWORD player);
        BOOL  SetBuildingProductionPercentMode(DWORD building, BOOL enable, DWORD player);
        BOOL  SetBuildingProductionPercentage(DWORD building, BYTE swords, BYTE bows, BYTE armors, BYTE racespecialweapons, DWORD player);
        BOOL  CastSpell(DWORD priest, DWORD spell, INT x, INT y, DWORD player);
        BOOL  GarrisonWarriors(DWORD building, DWORD player);
        BOOL  UnGarrisonWarriors(DWORD building, INT column, BOOL bowman, DWORD player);
        BOOL  ChangeGoodDistribution(S4_GOOD_ENUM good, S4_BUILDING_ENUM building, INT percent, DWORD ecosector, DWORD player);
        BOOL  ChangeGoodPriority(S4_GOOD_ENUM good, INT offset, DWORD ecosector, DWORD player);
        BOOL  RecruitWarriors(DWORD building, S4_SETTLER_ENUM unit, INT amount, DWORD player);
        BOOL  RecruitVehicle(DWORD building, S4_VEHICLE_ENUM unit, INT amount, DWORD player);
        BOOL  SetTradingRoute(DWORD sourceBuilding, DWORD destinationBuilding, DWORD player);
        BOOL  TradeGood(DWORD buidling, S4_GOOD_ENUM good, INT amount, DWORD player);
        BOOL  StoreGood(DWORD buidling, S4_GOOD_ENUM good, BOOL enable, DWORD player);

        /** UI Rendering **/
        S4CUSTOMUI  ShowMessageBox(LPCWSTR title, LPCWSTR message, INT x, INT y, INT w, INT h, DWORD flags);
        S4CUSTOMUI  CreateCustomUiElement(LPCS4CUSTOMUIELEMENT custom_ui);
        BOOL  DestroyCustomUiElement(S4CUSTOMUI custom_ui);
        BOOL  HideCustomUiElement(S4CUSTOMUI custom_ui);
        BOOL  ShowCustomUiElement(S4CUSTOMUI custom_ui);

        /** S4 Scripting **/
        DWORD  GetLocalPlayer();
        DWORD  BuildingsAdd(S4_BUILDING_ENUM building, INT x, INT y, DWORD player);
        DWORD  BuildingsAmount(S4_BUILDING_ENUM building, DWORD status, DWORD player);
        BOOL  BuildingsCrush(DWORD building);
        BOOL  BuildingsDelete(DWORD building, DWORD mode);
        BOOL  BuildingsExistsBuildingInArea(S4_BUILDING_ENUM building, INT x, INT y, INT r, DWORD status, DWORD player);
        DWORD  BuildingsGetFirstBuilding(S4_BUILDING_ENUM building, DWORD player);
        DWORD  BuildingsGetInhabitantAmount(DWORD building, DWORD player);
        DWORD  BuildingsGetTarget(DWORD building);
        BOOL  BuildingsIsSelected(S4_BUILDING_ENUM building);
        DWORD  DarkTribeAddManakopter(INT x, INT y, DWORD player);
        BOOL  DarkTribeFlyTo(INT x, INT y);
        BOOL  AIActivate(DWORD player, BOOL activate);
        BOOL  MakeDark(INT x, INT y);
        BOOL  MakeGreen(INT x, INT y);
        DWORD  EffectsAdd(DWORD effect, DWORD sound, INT x, INT y, DWORD delay);
        DWORD  ArePlayerAreasConnected(INT x1, INT y1, DWORD player2, INT x2, INT y2, DWORD player);
        BOOL  GameDefaultGameEndCheck();
        BOOL  DisableLastNPlayersInStatistic(DWORD n);
        BOOL  EnableLandExploredCheck();
        BOOL  FindAnyUnit(INT x, INT y, INT r, DWORD player);
        DWORD  GetAmountOfTreesInArea(INT x, INT y, INT r);
        DWORD  GetDifficulty();
        DWORD  GetNumberOfSquaresWithDarkLand(DWORD player);
        DWORD  GetOffenceFightingStrength(DWORD player);
        BOOL  HasPlayerLost(DWORD player);
        BOOL  IsAlmostAllLandExplored(DWORD player);
        BOOL  IsAreaDarkLand(INT x, INT y, INT r);
        BOOL  IsAreaGreen(INT x, INT y, INT r);
        BOOL  IsAreaOwned(INT x, INT y, INT r, DWORD player);
        DWORD  GetNumberOfPlayers();
        S4_TRIBE_ENUM  GetPlayerTribe(DWORD player);
        BOOL  ResetFogging();
        BOOL  SetAlliesDontRevealFog(BOOL enable);
        BOOL  SetFightingStrength(DWORD strength, DWORD player);
        BOOL  ShowClock(DWORD seconds);
        DWORD  Time();
        DWORD  GoodsAddPileEx(S4_GOOD_ENUM good, DWORD amount, INT x, INT y);
        DWORD  GoodsAmount(S4_GOOD_ENUM good, DWORD player);
        DWORD  GoodsDelete(S4_GOOD_ENUM good, INT x, INT y, INT r);
        DWORD  GoodsGetAmountInArea(S4_GOOD_ENUM good, INT x, INT y, INT r, DWORD player);
        DWORD  MagicCastSpell(S4_TRIBE_ENUM tribe, S4_SPELL_ENUM spell, INT x, INT y, DWORD player);
        BOOL  MagicIncreaseMana(INT amount, DWORD player);
        DWORD  MapAddDecoObject(DWORD object, INT x, INT y);
        BOOL  MapDeleteDecoObject(INT x, INT y, INT r);
        DWORD  MapSize();
        BOOL  MapPointIsOnScreen(INT x, INT y);
        BOOL  MapSetScreenPos(INT x, INT y);
        BOOL  SettlersAdd(S4_SETTLER_ENUM settler, INT amount, INT x, INT y, DWORD player);
        BOOL  SettlersAddToFerry(DWORD ferry, S4_SETTLER_ENUM settler, INT amount);
        DWORD  SettlersAmount(S4_SETTLER_ENUM settler, DWORD player);
        DWORD  SettlersAmountInArea(S4_SETTLER_ENUM settler, INT x, INT y, INT r, DWORD player);
        BOOL  SettlersIsSelected(S4_SETTLER_ENUM settler, INT amount);
        BOOL  SettlersKillSelectableSettlers(S4_SETTLER_ENUM settler, INT x, INT y, INT r, BOOL animation, DWORD player);
        DWORD  SettlersProductionAmount(S4_SETTLER_ENUM settler);
        BOOL  SettlersSetHealthInArea(S4_SETTLER_ENUM settler, INT x, INT y, INT r, DWORD health, DWORD player);
        DWORD  StatisticBuildingsCaptured(DWORD player);
        DWORD  StatisticGoodsProduced(DWORD player);
        DWORD  StatisticLandOwnedByPlayer(DWORD player);
        DWORD  StatisticManaCollected(DWORD player);
        DWORD  StatisticMushroomFarmsDestroyed(DWORD player);
        DWORD  StatisticServantsFreed(DWORD player);
        DWORD  StatisticSpellsCast(DWORD player);
        DWORD  StatisticUnitsDestroyed(DWORD player);
        BOOL  ClearMarker();
        BOOL  DeleteWorldCursor();
        BOOL  PressButton(DWORD dialog, DWORD control);
        BOOL  RevealWorldMap(BOOL enable);
        BOOL  SelectNextBuilding(S4_BUILDING_ENUM building);
        BOOL  SetMarker(INT x, INT y);
        BOOL  SetWorldCursor(INT x, INT y);
        BOOL  SetZoom(INT zoom);
        DWORD  VehiclesAdd(S4_VEHICLE_ENUM vehicle, DWORD direction, DWORD ammo, DWORD commands, INT x, INT y, DWORD player);
        BOOL  VehiclesAddWheelerToFerry(DWORD ferry, S4_VEHICLE_ENUM vehicle);
        DWORD  VehiclesAmount(S4_VEHICLE_ENUM vehicle, DWORD player);
        DWORD  VehiclesAmountInArea(S4_VEHICLE_ENUM vehicle, INT x, INT y, INT r, DWORD player);
        DWORD  VehiclesGetFerryCargoInArea(INT x, INT y, INT r, DWORD player);
        DWORD  VehiclesGetHealth(INT x, INT y);
        BOOL  VehiclesIsSelected(S4_VEHICLE_ENUM vehicle, INT amount);
        BOOL  VehiclesKill(S4_VEHICLE_ENUM vehicle, INT x, INT y, INT r, DWORD player);
        BOOL  SetGround(INT x, INT y, INT r, DWORD ground);
        BOOL  ShowTextMessage(LPCSTR message, DWORD icon, DWORD reserved);
        BOOL  SoundPlay(S4_SOUND_ENUM sound, LPVOID reserved, LPVOID reserved2);
    };

    ref class NetS4ModApi : INetS4ModApi {
    private:
        static S4API s4;

    public:
        NetS4ModApi() {}

        static NetS4ModApi() {
            s4 = ModAPI;
            s4->AddFrameListener(&CS4FrameCallback);
            s4->AddUIFrameListener(&CS4UIFrameCallback, S4_GUI_UNKNOWN);
            s4->AddMapInitListener(&CS4MapInitCallback);
            s4->AddSettlerSendListener(&CS4SettlerSendCallback);
            s4->AddMouseListener(&CS4MouseCallback);
            s4->AddTickListener(&CS4TickCallback);
            s4->AddLuaOpenListener(&CS4LuaOpenCallback);
            s4->AddBltListener(&CS4BltCallback);
            s4->AddEntityListener(&CS4EntityCallback);
            s4->AddGuiBltListener(&CS4GUIBltCallback);
            s4->AddGuiElementBltListener(&CS4GUIDrawCallback);
            s4->AddGuiClearListener(&CS4GUIClearCallback);

            S4Hook::LoadS4Hooks();
        }

#define CALLBACK_ADD(name, list, type) GCHandle::Alloc(name);std::function<type> fun = static_cast<LP##type>(Marshal::GetFunctionPointerForDelegate(name).ToPointer());auto callback = CreateCallback<type>(&fun); list##.emplace_back(callback); return callback->id

        /** Hooks/Observers **/
        virtual void RemoveListener(S4HOOK hook) {
            RemoveAnyCallback(hook);
        }
        virtual S4HOOK  AddFrameListener(S4FrameCallback^ clbk) {
            CALLBACK_ADD(clbk, S4FrameCallbackList, S4FRAMECALLBACK);
        }
        virtual S4HOOK  AddUIFrameListener(S4FrameCallback^ clbk, S4_GUI_ENUM gui) {
            CALLBACK_ADD(clbk, S4UIFrameCallbackList, S4FRAMECALLBACK);
        }
        virtual S4HOOK  AddMapInitListener(S4MapInitCallback^ clbk) {
            CALLBACK_ADD(clbk, S4MapInitCallbackList, S4MAPINITCALLBACK);
        }
        virtual S4HOOK  AddMouseListener(S4MouseCallback^ clbk) {
            CALLBACK_ADD(clbk, S4MouseCallbackList, S4MOUSECALLBACK);
        }
        virtual S4HOOK  AddSettlerSendListener(S4SettlerSendCallback^ clbk) {
            CALLBACK_ADD(clbk, S4SettlerSendCallbackList, S4SETTLERSENDCALLBACK);
        }
        virtual S4HOOK  AddTickListener(S4TickCallback^ clbk) {
            CALLBACK_ADD(clbk, S4TickCallbackList, S4TICKCALLBACK);
        }
        virtual S4HOOK  AddLuaOpenListener(S4LuaOpenCallback^ clbk) {
            CALLBACK_ADD(clbk, S4LuaOpenCallbackList, S4LUAOPENCALLBACK);
        }
        virtual S4HOOK  AddBltListener(S4BltCallback^ clbk) {
            CALLBACK_ADD(clbk, S4BltCallbackList, S4BLTCALLBACK);
        }
        virtual S4HOOK  AddEntityListener(S4EntityCallback^ clbk) {
            CALLBACK_ADD(clbk, S4EntityCallbackList, S4ENTITYCALLBACK);
        }
        virtual S4HOOK  AddGuiBltListener(S4GuiBltCallback^ clbk) {
            CALLBACK_ADD(clbk, S4GUIBltCallbackList, S4GUIBLTCALLBACK);
        }
        virtual S4HOOK  AddGuiClearListener(S4GuiClearCallback^ clbk) {
            CALLBACK_ADD(clbk, S4GUIClearCallbackList, S4GUICLEARCALLBACK);
        }
        virtual S4HOOK  AddGuiElementBltListener(S4GuiDrawCallback^ clbk) {
            CALLBACK_ADD(clbk, S4GUIDrawCallbackList, S4GUIDRAWCALLBACK);
        }

        /** WndProc **/
        virtual S4HOOK  AddWndProc(S4WndProc^ clbk) {
            if (oldWndProc == 0) {
                HWND h_wnd = s4->GetHwnd();
                if (h_wnd == nullptr)
                    MessageBox(nullptr, L"Failed to get window handle", L"NetModAPI", 0);
                oldWndProc = SetWindowLongPtr(h_wnd, GWLP_WNDPROC, (LONG_PTR)HookedWndProcedure);
            }

            GCHandle::Alloc(clbk);
            std::function<S4WNDPROC> fun = static_cast<WNDPROC>(Marshal::GetFunctionPointerForDelegate(clbk).ToPointer());
            auto callback = CreateCallback<S4WNDPROC>(&fun);
            S4WndProcList.emplace_back(callback);
            return callback->id;
        }

        /** Event Engine **/
        virtual byte PostToMessageQueue(void* eventEngine, void* event) { return S4Hook::PostToMessageQueue(eventEngine, event); }
        virtual void* CreateS4Event(void* _this, unsigned int a2, unsigned int a3, unsigned int a4, char a5) { return S4Hook::CreateS4Event(_this, a2, a3, a4, a5); }
        virtual void* CreateS4UIEvent(void* _this, unsigned int a2, unsigned int a3, unsigned int a4, char a5, unsigned int a6, __int16 a7) { return S4Hook::CreateS4UIEvent(_this, a2, a3, a4, a5, a6, a7); }

        /** Sound **/
        virtual void PlaySound(int id) { S4Hook::PlaySound(id); }
        virtual int GetMusicVolume() { return S4Hook::GetMusicVolume(); }
        virtual void SetMusicVolume(int volume) { S4Hook::SetMusicVolume(volume); }
        virtual int GetMusicEnabled() { return S4Hook::GetMusicEnabled(); }
        virtual void ToggleMusic(bool enabled) { S4Hook::ToggleMusic(enabled); }

        virtual int GetSoundVolume() { return S4Hook::GetSoundVolume(); }
        virtual void SetSoundVolume(int volume) { S4Hook::SetSoundVolume(volume); }
        virtual int GetSoundEnabled() { return S4Hook::GetSoundEnabled(); }
        virtual void ToggleSound(bool enabled) { S4Hook::ToggleSound(enabled); }

        /** ISettlers4Api methods **/
        virtual LPVOID  GetDebugData(LPVOID a, LPVOID b) { return s4->GetDebugData(a, b); }
        virtual DWORD  GetLastError() { return s4->GetLastError(); }

        /** Misc helper functions **/
        virtual HRESULT GetMD5OfModule(HMODULE module, LPSTR out, SIZE_T sz) { return s4->GetMD5OfModule(module, out, sz); };
        virtual BOOL  IsEdition(S4_EDITION_ENUM edition) { return s4->IsEdition(edition); }
        virtual HWND  GetHwnd() { return s4->GetHwnd(); }

        /** Settlers 4 functions **/
        virtual HRESULT GetHoveringUiElement(LPS4UIELEMENT element) { return s4->GetHoveringUiElement(element); };
        virtual BOOL  GetEntitiesCount(WORD* counter) { return s4->GetEntitiesCount(counter); }

        virtual BOOL  IsCurrentlyOnScreen(S4_GUI_ENUM gui) { return s4->IsCurrentlyOnScreen(gui); }
        virtual S4_ENTITY_ENUM  EntityGetClass(WORD entity) { return s4->EntityGetClass(entity); }
        virtual BOOL  GetEntities(DWORD* entities, size_t size) { return s4->GetEntities(entities, size); };
        virtual BOOL  EntityGetPosition(WORD entity, LPINT x, LPINT y) { return s4->EntityGetPosition(entity, x, y); }
        virtual BOOL  EntityGetDirection(WORD entity, LPDWORD dir) { return s4->EntitygGetDirection(entity, dir); }
        virtual BOOL  EntityGetHealth(WORD entity, LPDWORD health) { return s4->EntityGetHealth(entity, health); }
        virtual BOOL  EntityGetMaxHealth(WORD entity, LPDWORD maxHealth) { return s4->EntityGetMaxHealth(entity, maxHealth); }
        virtual BOOL  EntityGetTribe(WORD entity, S4_TRIBE_ENUM* tribe) { return s4->EntityGetTribe(entity, tribe); }
        virtual BOOL  EntityTreeGetType(WORD entity, S4_TREE_ENUM* tree) { return s4->EntityTreeGetType(entity, tree); }
        virtual BOOL  EntityPileGetType(WORD entity, S4_GOOD_ENUM* good) { return s4->EntityPileGetType(entity, good); }
        virtual BOOL  EntityDecoGetType(WORD entity, LPDWORD deco) { return s4->EntityDecoGetType(entity, deco); }
        virtual BOOL  EntityPileGetAmount(WORD entity, LPDWORD amount) { return s4->EntityPileGetAmount(entity, amount); }
        virtual BOOL  EntitySettlerGetType(WORD entity, S4_SETTLER_ENUM* settler) { return s4->EntitySettlerGetType(entity, settler); }
        virtual BOOL  EntityVehicleGetType(WORD entity, S4_VEHICLE_ENUM* vehicle) { return s4->EntityVehicleGetType(entity, vehicle); }
        virtual BOOL  EntityAnimalGetType(WORD entity, S4_ANIMAL_ENUM* animal) { return s4->EntityAnimalGetType(entity, animal); }
        virtual BOOL  EntityBuildingGetType(WORD entity, S4_BUILDING_ENUM* building) { return s4->EntityBuildingGetType(entity, building); }
        virtual BOOL  EntityStoneGetLevel(WORD entity, LPDWORD level) { return s4->EntityStoneGetLevel(entity, level); }
        virtual BOOL  EntityGetRole(WORD entity, LPVOID* role) { return s4->EntityGetRole(entity, role); }
        virtual BOOL  EntityGetRoleClass(WORD entity, S4_ROLE_ENUM* role) { return s4->EntitygGetRoleClass(entity, role); }
        virtual BOOL  EntityGetOwner(WORD entity, LPDWORD player) { return s4->EntitygGetOwner(entity, player); }

        virtual BOOL  ClearSelection() { return s4->ClearSelection(); }  // defined in CS4Selection.cpp
        virtual BOOL  GetSelection(PWORD out, SIZE_T outlen, PSIZE_T selectionCount) { return s4->GetSelection(out, outlen, selectionCount); }  // defined in CS4Selection.cpp
        virtual BOOL  RemoveSelection(PWORD settlers, SIZE_T settlerslen, PSIZE_T removedCount) { return s4->RemoveSelection(settlers, settlerslen, removedCount); }  // defined in CS4Selection.cpp
        virtual DWORD  SetMaxSelection(DWORD maxSelection) { return s4->SetMaxSelection(maxSelection); }  // defined in CS4Selection.cpp
        virtual DWORD  GetMaxSelection() { return s4->GetMaxSelection(); }  // defined in CS4Selection.cpp
        virtual BOOL  StartBuildingPlacement(S4_BUILDING_ENUM building) { return s4->StartBuildingPlacement(building); }
        virtual DWORD  GetGameTime() { return s4->GetGameTime(); }

        /** Landscape functions **/
        virtual DWORD  LandscapeGetHeight(INT x, INT y) { return s4->LandscapeGetHeight(x, y); }  // defined in CS4Landscape.cpp
        virtual S4_GROUND_ENUM  LandscapeGetType(INT x, INT y) { return s4->LandscapeGetType(x, y); }  // defined in CS4Landscape.cpp
        virtual BOOL  LandscapeIsPond(INT x, INT y) { return s4->LandscapeIsPond(x, y); }  // defined in CS4Landscape.cpp
        virtual BOOL  LandscapeIsDarkLand(INT x, INT y) { return s4->LandscapeIsDarkLand(x, y); }  // defined in CS4Landscape.cpp
        virtual BOOL  LandscapeIsDarkLandBorder(INT x, INT y) { return s4->LandscapeIsDarkLandBorder(x, y); }  // defined in CS4Landscape.cpp
        virtual DWORD  LandscapeGetFogOfWar(INT x, INT y) { return s4->LandscapeGetFogOfWar(x, y); }  // defined in CS4Landscape.cpp
        virtual BOOL  LandscapeIsFoundingStone(INT x, INT y) { return s4->LandscapeIsFoundingStone(x, y); }  // defined in CS4Landscape.cpp
        virtual S4_RESOURCE_ENUM  LandscapeGetResource(INT x, INT y) { return s4->LandscapeGetResource(x, y); }  // defined in CS4Landscape.cpp
        virtual BOOL  LandscapeSetResource(INT x, INT y, S4_RESOURCE_ENUM res) { return s4->LandscapeSetResource(x, y, res); }  // defined in CS4Landscape.cpp
        virtual DWORD  LandscapeGetObject(INT x, INT y) { return s4->LandscapeGetObject(x, y); }  // defined in CS4Landscape.cpp
        virtual DWORD  LandscapeGetOwner(INT x, INT y) { return s4->LandscapeGetOwner(x, y); }  // defined in CS4Landscape.cpp
        virtual DWORD  LandscapeGetEcoSector(INT x, INT y) { return s4->LandscapeGetEcoSector(x, y); }; // defined in CS4Landscape.cpp	
        virtual BOOL  LandscapeIsOccupied(INT x, INT y) { return s4->LandscapeIsOccupied(x, y); }; // defined in CS4Landscape.cpp

        /** Settlers 4 NetEvents functions **/
        virtual BOOL  SendWarriors(INT x, INT y, S4_MOVEMENT_ENUM mode, PWORD warriors, SIZE_T countOfWarriors, DWORD player) { return s4->SendWarriors(x, y, mode, warriors, countOfWarriors, player); }
        virtual BOOL  BuildBuilding(S4_BUILDING_ENUM buildingType, INT x, INT y, DWORD player) { return s4->BuildBuilding(buildingType, x, y, player); }  // defined in CS4Build.cpp
        virtual BOOL  CrushBuilding(DWORD building, DWORD player) { return s4->CrushBuilding(building, player); }  // defined in CS4Build.cpp
        virtual BOOL  ToggleBuildingPriority(DWORD building, DWORD player) { return s4->ToggleBuildingPriority(building, player); }  // defined in CS4Build.cpp
        virtual BOOL  ToggleBuildingHalt(DWORD building, DWORD player) { return s4->ToggleBuildingHalt(building, player); }  // defined in CS4Build.cpp
        virtual BOOL  SetBuildingWorkarea(DWORD building, INT x, INT y, DWORD player) { return s4->SetBuildingWorkarea(building, x, y, player); }  // defined in CS4Build.cpp
        virtual BOOL  SetBuildingProduction(DWORD building, S4_GOOD_ENUM good, INT amount, DWORD player) { return s4->SetBuildingProduction(building, good, amount, player); }  // defined in CS4Build.cpp
        virtual BOOL  SetBuildingProductionPercentMode(DWORD building, BOOL enable, DWORD player) { return s4->SetBuildingProductionPercentMode(building, enable, player); }  // defined in CS4Build.cpp
        virtual BOOL  SetBuildingProductionPercentage(DWORD building, BYTE swords, BYTE bows, BYTE armors, BYTE racespecialweapons, DWORD player) { return s4->SetBuildingProductionPercentage(building, swords, bows, armors, racespecialweapons, player); }  // defined in CS4Build.cpp
        virtual BOOL  CastSpell(DWORD priest, DWORD spell, INT x, INT y, DWORD player) { return s4->CastSpell(priest, spell, x, y, player); }  // defined in CS4Casting.cpp
        virtual BOOL  GarrisonWarriors(DWORD building, DWORD player) { return s4->GarrisonWarriors(building, player); }  // defined in CS4GarrisonWarriors.cpp
        virtual BOOL  UnGarrisonWarriors(DWORD building, INT column, BOOL bowman, DWORD player) { return s4->UnGarrisonWarriors(building, column, bowman, player); }  // defined in CS4GarrisonWarriors.cpp
        virtual BOOL  ChangeGoodDistribution(S4_GOOD_ENUM good, S4_BUILDING_ENUM building, INT percent, DWORD ecosector, DWORD player) { return s4->ChangeGoodDistribution(good, building, percent, ecosector, player); }  // defined in CS4GoodDistribution.cpp
        virtual BOOL  ChangeGoodPriority(S4_GOOD_ENUM good, INT offset, DWORD ecosector, DWORD player) { return s4->ChangeGoodPriority(good, offset, ecosector, player); }  // defined in CS4GoodDistribution.cpp
        virtual BOOL  RecruitWarriors(DWORD building, S4_SETTLER_ENUM unit, INT amount, DWORD player) { return s4->RecruitWarriors(building, unit, amount, player); }  // defined in CS4Recruit.cpp
        virtual BOOL  RecruitVehicle(DWORD building, S4_VEHICLE_ENUM unit, INT amount, DWORD player) { return s4->RecruitVehicle(building, unit, amount, player); }  // defined in CS4Recruit.cpp
        virtual BOOL  SetTradingRoute(DWORD sourceBuilding, DWORD destinationBuilding, DWORD player) { return s4->SetTradingRoute(sourceBuilding, destinationBuilding, player); }  // defined in CS4Trading.cpp
        virtual BOOL  TradeGood(DWORD buidling, S4_GOOD_ENUM good, INT amount, DWORD player) { return s4->TradeGood(buidling, good, amount, player); }  // defined in CS4Trading.cpp
        virtual BOOL  StoreGood(DWORD buidling, S4_GOOD_ENUM good, BOOL enable, DWORD player) { return s4->StoreGood(buidling, good, enable, player); }  // defined in CS4Trading.cpp

        /** UI Rendering **/
        virtual S4CUSTOMUI  ShowMessageBox(LPCWSTR title, LPCWSTR message, INT x, INT y, INT w, INT h, DWORD flags) { return s4->ShowMessageBox(title, message, x, y, w, h, flags); }
        virtual S4CUSTOMUI  CreateCustomUiElement(LPCS4CUSTOMUIELEMENT custom_ui) { return s4->CreateCustomUiElement(custom_ui); }
        virtual BOOL  DestroyCustomUiElement(S4CUSTOMUI custom_ui) { return s4->DestroyCustomUiElement(custom_ui); }
        virtual BOOL  HideCustomUiElement(S4CUSTOMUI custom_ui) { return s4->HideCustomUiElement(custom_ui); }
        virtual BOOL  ShowCustomUiElement(S4CUSTOMUI custom_ui) { return s4->ShowCustomUiElement(custom_ui); }

        /** S4 Scripting **/
        virtual DWORD  GetLocalPlayer() { return s4->GetLocalPlayer(); }  // defined in CS4Scripting.cpp
        virtual DWORD  BuildingsAdd(S4_BUILDING_ENUM building, INT x, INT y, DWORD player) { return s4->BuildingsAdd(building, x, y, player); }  // defined in CS4Scripting.cpp
        virtual DWORD  BuildingsAmount(S4_BUILDING_ENUM building, DWORD status, DWORD player) { return s4->BuildingsAmount(building, status, player); }  // defined in CS4Scripting.cpp
        virtual BOOL  BuildingsCrush(DWORD building) { return s4->BuildingsCrush(building); }  // defined in CS4Scripting.cpp
        virtual BOOL  BuildingsDelete(DWORD building, DWORD mode) { return s4->BuildingsDelete(building, mode); }  // defined in CS4Scripting.cpp
        virtual BOOL  BuildingsExistsBuildingInArea(S4_BUILDING_ENUM building, INT x, INT y, INT r, DWORD status, DWORD player) { return s4->BuildingsExistsBuildingInArea(building, x, y, r, status, player); }  // defined in CS4Scripting.cpp
        virtual DWORD  BuildingsGetFirstBuilding(S4_BUILDING_ENUM building, DWORD player) { return s4->BuildingsGetFirstBuilding(building, player); }  // defined in CS4Scripting.cpp
        virtual DWORD  BuildingsGetInhabitantAmount(DWORD building, DWORD player) { return s4->BuildingsGetInhabitantAmount(building, player); }  // defined in CS4Scripting.cpp
        virtual DWORD  BuildingsGetTarget(DWORD building) { return s4->BuildingsGetTarget(building); }  // defined in CS4Scripting.cpp
        virtual BOOL  BuildingsIsSelected(S4_BUILDING_ENUM building) { return s4->BuildingsIsSelected(building); }  // defined in CS4Scripting.cpp
        virtual DWORD  DarkTribeAddManakopter(INT x, INT y, DWORD player) { return s4->DarkTribeAddManakopter(x, y, player); }  // defined in CS4Scripting.cpp
        virtual BOOL  DarkTribeFlyTo(INT x, INT y) { return s4->DarkTribeFlyTo(x, y); }  // defined in CS4Scripting.cpp
        virtual BOOL  AIActivate(DWORD player, BOOL activate) { return s4->AIActivate(player, activate); }  // defined in CS4Scripting.cpp
        virtual BOOL  MakeDark(INT x, INT y) { return s4->MakeDark(x, y); }  // defined in CS4Scripting.cpp
        virtual BOOL  MakeGreen(INT x, INT y) { return s4->MakeGreen(x, y); }  // defined in CS4Scripting.cpp
        virtual DWORD  EffectsAdd(DWORD effect, DWORD sound, INT x, INT y, DWORD delay) { return s4->EffectsAdd(effect, sound, x, y, delay); }  // defined in CS4Scripting.cpp
        virtual DWORD  ArePlayerAreasConnected(INT x1, INT y1, DWORD player2, INT x2, INT y2, DWORD player) { return s4->ArePlayerAreasConnected(x1, y1, player2, x2, y2, player); }  // defined in CS4Scripting.cpp
        virtual BOOL  GameDefaultGameEndCheck() { return s4->GameDefaultGameEndCheck(); }  // defined in CS4Scripting.cpp
        virtual BOOL  DisableLastNPlayersInStatistic(DWORD n) { return s4->DisableLastNPlayersInStatistic(n); }  // defined in CS4Scripting.cpp
        virtual BOOL  EnableLandExploredCheck() { return s4->EnableLandExploredCheck(); }  // defined in CS4Scripting.cpp
        virtual BOOL  FindAnyUnit(INT x, INT y, INT r, DWORD player) { return s4->FindAnyUnit(x, y, r, player); }  // defined in CS4Scripting.cpp
        virtual DWORD  GetAmountOfTreesInArea(INT x, INT y, INT r) { return s4->GetAmountOfTreesInArea(x, y, r); }  // defined in CS4Scripting.cpp
        virtual DWORD  GetDifficulty() { return s4->GetDifficulty(); }  // defined in CS4Scripting.cpp
        virtual DWORD  GetNumberOfSquaresWithDarkLand(DWORD player) { return s4->GetNumberOfSquaresWithDarkLand(player); }  // defined in CS4Scripting.cpp
        virtual DWORD  GetOffenceFightingStrength(DWORD player) { return s4->GetOffenceFightingStrength(player); }  // defined in CS4Scripting.cpp
        virtual BOOL  HasPlayerLost(DWORD player) { return s4->HasPlayerLost(player); }  // defined in CS4Scripting.cpp
        virtual BOOL  IsAlmostAllLandExplored(DWORD player) { return s4->IsAlmostAllLandExplored(player); }  // defined in CS4Scripting.cpp
        virtual BOOL  IsAreaDarkLand(INT x, INT y, INT r) { return s4->IsAreaDarkLand(x, y, r); }  // defined in CS4Scripting.cpp
        virtual BOOL  IsAreaGreen(INT x, INT y, INT r) { return s4->IsAreaGreen(x, y, r); }  // defined in CS4Scripting.cpp
        virtual BOOL  IsAreaOwned(INT x, INT y, INT r, DWORD player) { return s4->IsAreaOwned(x, y, r, player); }  // defined in CS4Scripting.cpp
        virtual DWORD  GetNumberOfPlayers() { return s4->GetNumberOfPlayers(); }  // defined in CS4Scripting.cpp
        virtual S4_TRIBE_ENUM  GetPlayerTribe(DWORD player) { return s4->GetPlayerTribe(player); }  // defined in CS4Scripting.cpp
        virtual BOOL  ResetFogging() { return s4->ResetFogging(); }  // defined in CS4Scripting.cpp
        virtual BOOL  SetAlliesDontRevealFog(BOOL enable) { return s4->SetAlliesDontRevealFog(enable); }  // defined in CS4Scripting.cpp
        virtual BOOL  SetFightingStrength(DWORD strength, DWORD player) { return s4->SetFightingStrength(strength, player); }  // defined in CS4Scripting.cpp
        virtual BOOL  ShowClock(DWORD seconds) { return s4->ShowClock(seconds); }  // defined in CS4Scripting.cpp
        virtual DWORD  Time() { return s4->Time(); }  // defined in CS4Scripting.cpp
        virtual DWORD  GoodsAddPileEx(S4_GOOD_ENUM good, DWORD amount, INT x, INT y) { return s4->GoodsAddPileEx(good, amount, x, y); }  // defined in CS4Scripting.cpp
        virtual DWORD  GoodsAmount(S4_GOOD_ENUM good, DWORD player) { return s4->GoodsAmount(good, player); }  // defined in CS4Scripting.cpp
        virtual DWORD  GoodsDelete(S4_GOOD_ENUM good, INT x, INT y, INT r) { return s4->GoodsDelete(good, x, y, r); }  // defined in CS4Scripting.cpp
        virtual DWORD  GoodsGetAmountInArea(S4_GOOD_ENUM good, INT x, INT y, INT r, DWORD player) { return s4->GoodsGetAmountInArea(good, x, y, r, player); }  // defined in CS4Scripting.cpp
        virtual DWORD  MagicCastSpell(S4_TRIBE_ENUM tribe, S4_SPELL_ENUM spell, INT x, INT y, DWORD player) { return s4->MagicCastSpell(tribe, spell, x, y, player); }  // defined in CS4Scripting.cpp
        virtual BOOL  MagicIncreaseMana(INT amount, DWORD player) { return s4->MagicIncreaseMana(amount, player); }  // defined in CS4Scripting.cpp
        virtual DWORD  MapAddDecoObject(DWORD object, INT x, INT y) { return s4->MapAddDecoObject(object, x, y); }  // defined in CS4Scripting.cpp
        virtual BOOL  MapDeleteDecoObject(INT x, INT y, INT r) { return s4->MapDeleteDecoObject(x, y, r); }  // defined in CS4Scripting.cpp
        virtual DWORD  MapSize() { return s4->MapSize(); }  // defined in CS4Scripting.cpp
        virtual BOOL  MapPointIsOnScreen(INT x, INT y) { return s4->MapPointIsOnScreen(x, y); }  // defined in CS4Scripting.cpp
        virtual BOOL  MapSetScreenPos(INT x, INT y) { return s4->MapSetScreenPos(x, y); }  // defined in CS4Scripting.cpp
        virtual BOOL  SettlersAdd(S4_SETTLER_ENUM settler, INT amount, INT x, INT y, DWORD player) { return s4->SettlersAdd(settler, amount, x, y, player); }  // defined in CS4Scripting.cpp
        virtual BOOL  SettlersAddToFerry(DWORD ferry, S4_SETTLER_ENUM settler, INT amount) { return s4->SettlersAddToFerry(ferry, settler, amount); }  // defined in CS4Scripting.cpp
        virtual DWORD  SettlersAmount(S4_SETTLER_ENUM settler, DWORD player) { return s4->SettlersAmount(settler, player); }  // defined in CS4Scripting.cpp
        virtual DWORD  SettlersAmountInArea(S4_SETTLER_ENUM settler, INT x, INT y, INT r, DWORD player) { return s4->SettlersAmountInArea(settler, x, y, r, player); }  // defined in CS4Scripting.cpp
        virtual BOOL  SettlersIsSelected(S4_SETTLER_ENUM settler, INT amount) { return s4->SettlersIsSelected(settler, amount); }  // defined in CS4Scripting.cpp
        virtual BOOL  SettlersKillSelectableSettlers(S4_SETTLER_ENUM settler, INT x, INT y, INT r, BOOL animation, DWORD player) { return s4->SettlersKillSelectableSettlers(settler, x, y, r, animation, player); }  // defined in CS4Scripting.cpp
        virtual DWORD  SettlersProductionAmount(S4_SETTLER_ENUM settler) { return s4->SettlersProductionAmount(settler); }  // defined in CS4Scripting.cpp
        virtual BOOL  SettlersSetHealthInArea(S4_SETTLER_ENUM settler, INT x, INT y, INT r, DWORD health, DWORD player) { return s4->SettlersSetHealthInArea(settler, x, y, r, health, player); }  // defined in CS4Scripting.cpp
        virtual DWORD  StatisticBuildingsCaptured(DWORD player) { return s4->StatisticBuildingsCaptured(player); }  // defined in CS4Scripting.cpp
        virtual DWORD  StatisticGoodsProduced(DWORD player) { return s4->StatisticGoodsProduced(player); }  // defined in CS4Scripting.cpp
        virtual DWORD  StatisticLandOwnedByPlayer(DWORD player) { return s4->StatisticLandOwnedByPlayer(player); }  // defined in CS4Scripting.cpp
        virtual DWORD  StatisticManaCollected(DWORD player) { return s4->StatisticManaCollected(player); }  // defined in CS4Scripting.cpp
        virtual DWORD  StatisticMushroomFarmsDestroyed(DWORD player) { return s4->StatisticMushroomFarmsDestroyed(player); }  // defined in CS4Scripting.cpp
        virtual DWORD  StatisticServantsFreed(DWORD player) { return s4->StatisticServantsFreed(player); }  // defined in CS4Scripting.cpp
        virtual DWORD  StatisticSpellsCast(DWORD player) { return s4->StatisticSpellsCast(player); }  // defined in CS4Scripting.cpp
        virtual DWORD  StatisticUnitsDestroyed(DWORD player) { return s4->StatisticUnitsDestroyed(player); }  // defined in CS4Scripting.cpp
        virtual BOOL  ClearMarker() { return s4->ClearMarker(); }  // defined in CS4Scripting.cpp
        virtual BOOL  DeleteWorldCursor() { return s4->DeleteWorldCursor(); }  // defined in CS4Scripting.cpp
        virtual BOOL  PressButton(DWORD dialog, DWORD control) { return s4->PressButton(dialog, control); }  // defined in CS4Scripting.cpp
        virtual BOOL  RevealWorldMap(BOOL enable) { return s4->RevealWorldMap(enable); }  // defined in CS4Scripting.cpp
        virtual BOOL  SelectNextBuilding(S4_BUILDING_ENUM building) { return s4->SelectNextBuilding(building); }  // defined in CS4Scripting.cpp
        virtual BOOL  SetMarker(INT x, INT y) { return s4->SetMarker(x, y); }  // defined in CS4Scripting.cpp
        virtual BOOL  SetWorldCursor(INT x, INT y) { return s4->SetWorldCursor(x, y); }  // defined in CS4Scripting.cpp
        virtual BOOL  SetZoom(INT zoom) { return s4->SetZoom(zoom); }  // defined in CS4Scripting.cpp
        virtual DWORD  VehiclesAdd(S4_VEHICLE_ENUM vehicle, DWORD direction, DWORD ammo, DWORD commands, INT x, INT y, DWORD player) { return s4->VehiclesAdd(vehicle, direction, ammo, commands, x, y, player); }  // defined in CS4Scripting.cpp
        virtual BOOL  VehiclesAddWheelerToFerry(DWORD ferry, S4_VEHICLE_ENUM vehicle) { return s4->VehiclesAddWheelerToFerry(ferry, vehicle); }  // defined in CS4Scripting.cpp
        virtual DWORD  VehiclesAmount(S4_VEHICLE_ENUM vehicle, DWORD player) { return s4->VehiclesAmount(vehicle, player); }  // defined in CS4Scripting.cpp
        virtual DWORD  VehiclesAmountInArea(S4_VEHICLE_ENUM vehicle, INT x, INT y, INT r, DWORD player) { return s4->VehiclesAmountInArea(vehicle, x, y, r, player); }  // defined in CS4Scripting.cpp
        virtual DWORD  VehiclesGetFerryCargoInArea(INT x, INT y, INT r, DWORD player) { return s4->VehiclesGetFerryCargoInArea(x, y, r, player); }  // defined in CS4Scripting.cpp
        virtual DWORD  VehiclesGetHealth(INT x, INT y) { return s4->VehiclesGetHealth(x, y); }  // defined in CS4Scripting.cpp
        virtual BOOL  VehiclesIsSelected(S4_VEHICLE_ENUM vehicle, INT amount) { return s4->VehiclesIsSelected(vehicle, amount); }  // defined in CS4Scripting.cpp
        virtual BOOL  VehiclesKill(S4_VEHICLE_ENUM vehicle, INT x, INT y, INT r, DWORD player) { return s4->VehiclesKill(vehicle, x, y, r, player); }  // defined in CS4Scripting.cpp
        virtual BOOL  SetGround(INT x, INT y, INT r, DWORD ground) { return s4->SetGround(x, y, r, ground); }  // defined in CS4Scripting.cpp
        virtual BOOL  ShowTextMessage(LPCSTR message, DWORD icon, DWORD reserved) { return s4->ShowTextMessage(message, icon, reserved); }  // defined in CS4Scripting.cpp
        virtual BOOL  SoundPlay(S4_SOUND_ENUM sound, LPVOID reserved, LPVOID reserved2) { return s4->SoundPlay(sound, reserved, reserved2); }  // defined in CS4Scripting.cpp
    };

}
