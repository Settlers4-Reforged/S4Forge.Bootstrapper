#pragma once

#include "S4APIHooks.h"
#include "S4Hooks.h"
#include <vcclr.h>

#pragma make_public(IDirectDrawSurface7)

using namespace System;
using namespace Runtime::InteropServices;
using namespace Collections::Generic;
using namespace Forge::Native;

namespace NetModAPI {
    ref class NetS4ModApi : IS4ModApi {
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
        virtual void RemoveListener(unsigned int hook) {
            RemoveAnyCallback(hook);
        }
        virtual unsigned int AddFrameListener(S4FrameCallback^ clbk) {
            CALLBACK_ADD(clbk, S4FrameCallbackList, S4FRAMECALLBACK);
        }
        virtual unsigned int AddUIFrameListener(S4FrameCallback^ clbk, int gui) {
            CALLBACK_ADD(clbk, S4UIFrameCallbackList, S4FRAMECALLBACK);
        }
        virtual unsigned int AddMapInitListener(S4MapInitCallback^ clbk) {
            CALLBACK_ADD(clbk, S4MapInitCallbackList, S4MAPINITCALLBACK);
        }
        virtual unsigned int AddMouseListener(S4MouseCallback^ clbk) {
            CALLBACK_ADD(clbk, S4MouseCallbackList, S4MOUSECALLBACK);
        }
        virtual unsigned int AddSettlerSendListener(S4SettlerSendCallback^ clbk) {
            CALLBACK_ADD(clbk, S4SettlerSendCallbackList, S4SETTLERSENDCALLBACK);
        }
        virtual unsigned int AddTickListener(S4TickCallback^ clbk) {
            CALLBACK_ADD(clbk, S4TickCallbackList, S4TICKCALLBACK);
        }
        virtual unsigned int AddLuaOpenListener(S4LuaOpenCallback^ clbk) {
            CALLBACK_ADD(clbk, S4LuaOpenCallbackList, S4LUAOPENCALLBACK);
        }
        virtual unsigned int AddBltListener(S4BltCallback^ clbk) {
            CALLBACK_ADD(clbk, S4BltCallbackList, S4BLTCALLBACK);
        }
        virtual unsigned int AddEntityListener(S4EntityCallback^ clbk) {
            CALLBACK_ADD(clbk, S4EntityCallbackList, S4ENTITYCALLBACK);
        }
        virtual unsigned int AddGuiBltListener(S4GuiBltCallback^ clbk) {
            CALLBACK_ADD(clbk, S4GUIBltCallbackList, S4GUIBLTCALLBACK);
        }
        virtual unsigned int AddGuiClearListener(S4GuiClearCallback^ clbk) {
            CALLBACK_ADD(clbk, S4GUIClearCallbackList, S4GUICLEARCALLBACK);
        }
        virtual unsigned int AddGuiElementBltListener(S4GuiDrawCallback^ clbk) {
            CALLBACK_ADD(clbk, S4GUIDrawCallbackList, S4GUIDRAWCALLBACK);
        }

        /** WndProc **/
        virtual unsigned int AddWndProc(S4WndProc^ clbk) {
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
        virtual byte PostToMessageQueue(void* eventEngine, Forge::Game::Events::Native::CEvent* event) { return S4Hook::PostToMessageQueue(eventEngine, event); }
        virtual Forge::Game::Events::Native::CEvent* CreateS4Event(Forge::Game::Events::Native::CEvent* _this, unsigned int a2, unsigned int a3, unsigned int a4, signed char a5) {
            return static_cast<Forge::Game::Events::Native::CEvent*>(S4Hook::CreateS4Event(_this, a2, a3, a4, a5));
        }
        virtual Forge::Game::Events::Native::CEvent* CreateS4UIEvent(Forge::Game::Events::Native::CEvent* _this, unsigned int a2, unsigned int a3, unsigned int a4, unsigned char a5, unsigned int a6, __int16 a7) { return static_cast<Forge::Game::Events::Native::CEvent*>(S4Hook::CreateS4UIEvent(_this, a2, a3, a4, a5, a6, a7)); }

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
        virtual void* GetDebugData(void* a, void* b) { return s4->GetDebugData(a, b); }
        virtual unsigned int  GetLastError() { return s4->GetLastError(); }

        /** Misc helper functions **/
        virtual int GetMD5OfModule(void* module, System::String^ out, unsigned int size) { return 0; };
        virtual bool  IsEdition(S4Edition edition) { return s4->IsEdition(static_cast<S4_EDITION_ENUM>(edition)); }
        virtual void* GetHwnd() { return s4->GetHwnd(); }

        /** Settlers 4 functions **/
        virtual int GetHoveringUiElement(IntPtr element) { return s4->GetHoveringUiElement(static_cast<LPS4UIELEMENT>(element.ToPointer())); };
        virtual bool  GetEntitiesCount(unsigned short% counter) {
            pin_ptr<unsigned short> ptr = &counter;
            return s4->GetEntitiesCount(ptr);
        }

        virtual bool  IsCurrentlyOnScreen(int gui) { return s4->IsCurrentlyOnScreen((S4_GUI_ENUM)gui); }
        virtual EntityClass  EntityGetClass(unsigned short entity) { return (EntityClass)s4->EntityGetClass(entity); }
        virtual bool  GetEntities(array<unsigned int>^ entities, unsigned int size) { 
            pin_ptr<unsigned int> pin = &entities[0];
            return s4->GetEntities(reinterpret_cast<LPDWORD>(pin), size);
        };
        virtual bool  EntityGetPosition(unsigned short entity, int %x, int %y) { 
            pin_ptr<int> xPin, yPin;
            xPin = &x;
            yPin = &y;
            return s4->EntityGetPosition(entity, xPin, yPin);
        }
        virtual bool  EntityGetDirection(unsigned short entity, unsigned int% dir) {
            pin_ptr<unsigned int> dirPin = &dir;
            return s4->EntitygGetDirection(entity, reinterpret_cast<LPDWORD>(dirPin));
        }
        virtual bool  EntityGetHealth(unsigned short entity, unsigned int% health) {
            pin_ptr<unsigned int> healthPin = &health;
            return s4->EntityGetHealth(entity, reinterpret_cast<LPDWORD>(healthPin));
        }
        virtual bool  EntityGetMaxHealth(unsigned short entity, unsigned int% maxHealth) {
            pin_ptr<unsigned int> pin = &maxHealth;
            return s4->EntityGetMaxHealth(entity, reinterpret_cast<LPDWORD>(pin));
        }
        virtual bool  EntityGetTribe(unsigned short entity, Tribe% tribe) {
            pin_ptr<Tribe> pin = &tribe;
            return s4->EntityGetTribe(entity, reinterpret_cast<S4_TRIBE_ENUM*>(pin));
        }
        virtual bool  EntityTreeGetType(unsigned short entity, TreeType% tree) {
            pin_ptr<TreeType> pin = &tree;
            return s4->EntityTreeGetType(entity, reinterpret_cast<S4_TREE_ENUM*>(pin));
        }
        virtual bool  EntityPileGetType(unsigned short entity, GoodType% good) {
            pin_ptr<GoodType> pin = &good;
            return s4->EntityPileGetType(entity, reinterpret_cast<S4_GOOD_ENUM*>(pin));
        }
        virtual bool  EntityDecoGetType(unsigned short entity, unsigned int% deco) {
            pin_ptr<unsigned int> pin = &deco;
            return s4->EntityDecoGetType(entity, reinterpret_cast<LPDWORD>(pin));
        }
        virtual bool  EntityPileGetAmount(unsigned short entity, unsigned int% amount) {
            pin_ptr<unsigned int> pin = &amount;
            return s4->EntityPileGetAmount(entity, reinterpret_cast<LPDWORD>(pin));
        }
        virtual bool  EntitySettlerGetType(unsigned short entity, SettlerType% settler) {
            pin_ptr<SettlerType> pin = &settler;
            return s4->EntitySettlerGetType(entity, reinterpret_cast<S4_SETTLER_ENUM*>(pin));
        }
        virtual bool  EntityVehicleGetType(unsigned short entity, VehicleType% vehicle) {
            pin_ptr<VehicleType> pin = &vehicle;
            return s4->EntityVehicleGetType(entity, reinterpret_cast<S4_VEHICLE_ENUM*> (pin));
        }
        virtual bool  EntityAnimalGetType(unsigned short entity, AnimalType% animal) {
            pin_ptr<AnimalType> pin = &animal;
            return s4->EntityAnimalGetType(entity, reinterpret_cast<S4_ANIMAL_ENUM*> (pin));
        }
        virtual bool  EntityBuildingGetType(unsigned short entity, BuildingType% building) {
            pin_ptr<BuildingType> pin = &building;
            return s4->EntityBuildingGetType(entity, reinterpret_cast<S4_BUILDING_ENUM*>(pin));
        }
        virtual bool  EntityStoneGetLevel(unsigned short entity, unsigned int% level) {
            pin_ptr<unsigned int> pin = &level;
            return s4->EntityStoneGetLevel(entity, reinterpret_cast<LPDWORD>(pin));
        }
        virtual bool  EntityGetRole(unsigned short entity, void**% role) { return s4->EntityGetRole(entity, role); }
        virtual bool  EntityGetRoleClass(unsigned short entity, EntityRole% role) {
            pin_ptr<EntityRole> pin = &role;
            return s4->EntitygGetRoleClass(entity, reinterpret_cast<S4_ROLE_ENUM*>(pin));
        }
        virtual bool  EntityGetOwner(unsigned short entity, unsigned int% player) {
            pin_ptr<unsigned int> pin = &player;
            return s4->EntitygGetOwner(entity, reinterpret_cast<LPDWORD>(pin));
        }

        virtual bool  ClearSelection() { return s4->ClearSelection(); }  // defined in CS4Selection.cpp
        virtual bool  GetSelection(array<unsigned short>^ out, unsigned int outlen, unsigned int% selectionCount) {
            pin_ptr<unsigned short> outPin = &out[0];
            pin_ptr<unsigned int> selectionCountPin = &selectionCount;
            return s4->GetSelection(outPin, outlen, reinterpret_cast<unsigned long*>(selectionCountPin));
        }  // defined in CS4Selection.cpp
        virtual bool  RemoveSelection(array<unsigned short>^ remove, unsigned int settlerslen, unsigned int% removedCount) {
            pin_ptr<unsigned short> removePin = &remove[0];
            pin_ptr<unsigned int> removedCountPin = &removedCount;
            return s4->RemoveSelection(removePin, settlerslen, reinterpret_cast<unsigned long*>(removedCountPin));
        }  // defined in CS4Selection.cpp
        virtual unsigned int  SetMaxSelection(unsigned int maxSelection) { return s4->SetMaxSelection(maxSelection); }  // defined in CS4Selection.cpp
        virtual unsigned int  GetMaxSelection() { return s4->GetMaxSelection(); }  // defined in CS4Selection.cpp
        virtual bool  StartBuildingPlacement(BuildingType building) { return s4->StartBuildingPlacement(static_cast<S4_BUILDING_ENUM>(building)); }
        virtual unsigned int  GetGameTime() { return s4->GetGameTime(); }

        /** Landscape functions **/
        virtual unsigned int  LandscapeGetHeight(INT x, INT y) { return s4->LandscapeGetHeight(x, y); }  // defined in CS4Landscape.cpp
        virtual GroundType LandscapeGetType(INT x, INT y) { return static_cast<GroundType>(s4->LandscapeGetType(x, y)); }  // defined in CS4Landscape.cpp
        virtual bool  LandscapeIsPond(INT x, INT y) { return s4->LandscapeIsPond(x, y); }  // defined in CS4Landscape.cpp
        virtual bool  LandscapeIsDarkLand(INT x, INT y) { return s4->LandscapeIsDarkLand(x, y); }  // defined in CS4Landscape.cpp
        virtual bool  LandscapeIsDarkLandBorder(INT x, INT y) { return s4->LandscapeIsDarkLandBorder(x, y); }  // defined in CS4Landscape.cpp
        virtual unsigned int  LandscapeGetFogOfWar(INT x, INT y) { return s4->LandscapeGetFogOfWar(x, y); }  // defined in CS4Landscape.cpp
        virtual bool  LandscapeIsFoundingStone(INT x, INT y) { return s4->LandscapeIsFoundingStone(x, y); }  // defined in CS4Landscape.cpp
        virtual ResourceType LandscapeGetResource(INT x, INT y) { return static_cast<ResourceType>(s4->LandscapeGetResource(x, y)); }  // defined in CS4Landscape.cpp
        virtual bool  LandscapeSetResource(INT x, INT y, ResourceType res) { return s4->LandscapeSetResource(x, y, static_cast<S4_RESOURCE_ENUM>(res)); }  // defined in CS4Landscape.cpp
        virtual unsigned int  LandscapeGetObject(INT x, INT y) { return s4->LandscapeGetObject(x, y); }  // defined in CS4Landscape.cpp
        virtual unsigned int  LandscapeGetOwner(INT x, INT y) { return s4->LandscapeGetOwner(x, y); }  // defined in CS4Landscape.cpp
        virtual unsigned int  LandscapeGetEcoSector(INT x, INT y) { return s4->LandscapeGetEcoSector(x, y); }; // defined in CS4Landscape.cpp	
        virtual bool  LandscapeIsOccupied(INT x, INT y) { return s4->LandscapeIsOccupied(x, y); }; // defined in CS4Landscape.cpp

        /** Settlers 4 NetEvents functions **/
        virtual bool  SendWarriors(INT x, INT y, MovementType mode, array<unsigned short>^ warriors, unsigned int countOfWarriors, unsigned int player) {
            pin_ptr<unsigned short> pin = &warriors[0];
            return s4->SendWarriors(x, y, static_cast<S4_MOVEMENT_ENUM>(mode), pin, countOfWarriors, player);
        }
        virtual bool  BuildBuilding(BuildingType buildingType, INT x, INT y, unsigned int player) { return s4->BuildBuilding(static_cast<S4_BUILDING_ENUM>(buildingType), x, y, player); }  // defined in CS4Build.cpp
        virtual bool  CrushBuilding(unsigned int building, unsigned int player) { return s4->CrushBuilding(building, player); }  // defined in CS4Build.cpp
        virtual bool  ToggleBuildingPriority(unsigned int building, unsigned int player) { return s4->ToggleBuildingPriority(building, player); }  // defined in CS4Build.cpp
        virtual bool  ToggleBuildingHalt(unsigned int building, unsigned int player) { return s4->ToggleBuildingHalt(building, player); }  // defined in CS4Build.cpp
        virtual bool  SetBuildingWorkarea(unsigned int building, INT x, INT y, unsigned int player) { return s4->SetBuildingWorkarea(building, x, y, player); }  // defined in CS4Build.cpp
        virtual bool  SetBuildingProduction(unsigned int building, GoodType good, INT amount, unsigned int player) { return s4->SetBuildingProduction(building, static_cast<S4_GOOD_ENUM>(good), amount, player); }  // defined in CS4Build.cpp
        virtual bool  SetBuildingProductionPercentMode(unsigned int building, bool enable, unsigned int player) { return s4->SetBuildingProductionPercentMode(building, enable, player); }  // defined in CS4Build.cpp
        virtual bool  SetBuildingProductionPercentage(unsigned int building, BYTE swords, BYTE bows, BYTE armors, BYTE racespecialweapons, unsigned int player) { return s4->SetBuildingProductionPercentage(building, swords, bows, armors, racespecialweapons, player); }  // defined in CS4Build.cpp
        virtual bool  CastSpell(unsigned int priest, unsigned int spell, INT x, INT y, unsigned int player) { return s4->CastSpell(priest, spell, x, y, player); }  // defined in CS4Casting.cpp
        virtual bool  GarrisonWarriors(unsigned int building, unsigned int player) { return s4->GarrisonWarriors(building, player); }  // defined in CS4GarrisonWarriors.cpp
        virtual bool  UnGarrisonWarriors(unsigned int building, INT column, bool bowman, unsigned int player) { return s4->UnGarrisonWarriors(building, column, bowman, player); }  // defined in CS4GarrisonWarriors.cpp
        virtual bool  ChangeGoodDistribution(GoodType good, BuildingType building, INT percent, unsigned int ecosector, unsigned int player) { return s4->ChangeGoodDistribution(static_cast<S4_GOOD_ENUM>(good), static_cast<S4_BUILDING_ENUM>(building), percent, ecosector, player); }  // defined in CS4GoodDistribution.cpp
        virtual bool  ChangeGoodPriority(GoodType good, INT offset, unsigned int ecosector, unsigned int player) { return s4->ChangeGoodPriority(static_cast<S4_GOOD_ENUM>(good), offset, ecosector, player); }  // defined in CS4GoodDistribution.cpp
        virtual bool  RecruitWarriors(unsigned int building, SettlerType unit, INT amount, unsigned int player) { return s4->RecruitWarriors(building, static_cast<S4_SETTLER_ENUM>(unit), amount, player); }  // defined in CS4Recruit.cpp
        virtual bool  RecruitVehicle(unsigned int building, VehicleType unit, INT amount, unsigned int player) { return s4->RecruitVehicle(building, static_cast<S4_VEHICLE_ENUM>(unit), amount, player); }  // defined in CS4Recruit.cpp
        virtual bool  SetTradingRoute(unsigned int sourceBuilding, unsigned int destinationBuilding, unsigned int player) { return s4->SetTradingRoute(sourceBuilding, destinationBuilding, player); }  // defined in CS4Trading.cpp
        virtual bool  TradeGood(unsigned int buidling, GoodType good, INT amount, unsigned int player) { return s4->TradeGood(buidling, static_cast<S4_GOOD_ENUM>(good), amount, player); }  // defined in CS4Trading.cpp
        virtual bool  StoreGood(unsigned int buidling, GoodType good, bool enable, unsigned int player) { return s4->StoreGood(buidling, static_cast<S4_GOOD_ENUM>(good), enable, player); }  // defined in CS4Trading.cpp


        /** S4 Scripting **/
        virtual unsigned int  GetLocalPlayer() { return s4->GetLocalPlayer(); }  // defined in CS4Scripting.cpp
        virtual unsigned int  BuildingsAdd(BuildingType building, INT x, INT y, unsigned int player) { return s4->BuildingsAdd(static_cast<S4_BUILDING_ENUM>(building), x, y, player); }  // defined in CS4Scripting.cpp
        virtual unsigned int  BuildingsAmount(BuildingType building, unsigned int status, unsigned int player) { return s4->BuildingsAmount(static_cast<S4_BUILDING_ENUM>(building), status, player); }  // defined in CS4Scripting.cpp
        virtual bool  BuildingsCrush(unsigned int building) { return s4->BuildingsCrush(building); }  // defined in CS4Scripting.cpp
        virtual bool  BuildingsDelete(unsigned int building, unsigned int mode) { return s4->BuildingsDelete(building, mode); }  // defined in CS4Scripting.cpp
        virtual bool  BuildingsExistsBuildingInArea(BuildingType building, INT x, INT y, INT r, unsigned int status, unsigned int player) { return s4->BuildingsExistsBuildingInArea(static_cast<S4_BUILDING_ENUM>(building), x, y, r, status, player); }  // defined in CS4Scripting.cpp
        virtual unsigned int  BuildingsGetFirstBuilding(BuildingType building, unsigned int player) { return s4->BuildingsGetFirstBuilding(static_cast<S4_BUILDING_ENUM>(building), player); }  // defined in CS4Scripting.cpp
        virtual unsigned int  BuildingsGetInhabitantAmount(unsigned int building, unsigned int player) { return s4->BuildingsGetInhabitantAmount(building, player); }  // defined in CS4Scripting.cpp
        virtual unsigned int  BuildingsGetTarget(unsigned int building) { return s4->BuildingsGetTarget(building); }  // defined in CS4Scripting.cpp
        virtual bool  BuildingsIsSelected(BuildingType building) { return s4->BuildingsIsSelected(static_cast<S4_BUILDING_ENUM>(building)); }  // defined in CS4Scripting.cpp
        virtual unsigned int  DarkTribeAddManakopter(INT x, INT y, unsigned int player) { return s4->DarkTribeAddManakopter(x, y, player); }  // defined in CS4Scripting.cpp
        virtual bool  DarkTribeFlyTo(INT x, INT y) { return s4->DarkTribeFlyTo(x, y); }  // defined in CS4Scripting.cpp
        virtual bool  AIActivate(unsigned int player, bool activate) { return s4->AIActivate(player, activate); }  // defined in CS4Scripting.cpp
        virtual bool  MakeDark(INT x, INT y) { return s4->MakeDark(x, y); }  // defined in CS4Scripting.cpp
        virtual bool  MakeGreen(INT x, INT y) { return s4->MakeGreen(x, y); }  // defined in CS4Scripting.cpp
        virtual unsigned int  EffectsAdd(unsigned int effect, unsigned int sound, INT x, INT y, unsigned int delay) { return s4->EffectsAdd(effect, sound, x, y, delay); }  // defined in CS4Scripting.cpp
        virtual unsigned int  ArePlayerAreasConnected(INT x1, INT y1, unsigned int player2, INT x2, INT y2, unsigned int player) { return s4->ArePlayerAreasConnected(x1, y1, player2, x2, y2, player); }  // defined in CS4Scripting.cpp
        virtual bool  GameDefaultGameEndCheck() { return s4->GameDefaultGameEndCheck(); }  // defined in CS4Scripting.cpp
        virtual bool  DisableLastNPlayersInStatistic(unsigned int n) { return s4->DisableLastNPlayersInStatistic(n); }  // defined in CS4Scripting.cpp
        virtual bool  EnableLandExploredCheck() { return s4->EnableLandExploredCheck(); }  // defined in CS4Scripting.cpp
        virtual bool  FindAnyUnit(INT x, INT y, INT r, unsigned int player) { return s4->FindAnyUnit(x, y, r, player); }  // defined in CS4Scripting.cpp
        virtual unsigned int  GetAmountOfTreesInArea(INT x, INT y, INT r) { return s4->GetAmountOfTreesInArea(x, y, r); }  // defined in CS4Scripting.cpp
        virtual unsigned int  GetDifficulty() { return s4->GetDifficulty(); }  // defined in CS4Scripting.cpp
        virtual unsigned int  GetNumberOfSquaresWithDarkLand(unsigned int player) { return s4->GetNumberOfSquaresWithDarkLand(player); }  // defined in CS4Scripting.cpp
        virtual unsigned int  GetOffenceFightingStrength(unsigned int player) { return s4->GetOffenceFightingStrength(player); }  // defined in CS4Scripting.cpp
        virtual bool  HasPlayerLost(unsigned int player) { return s4->HasPlayerLost(player); }  // defined in CS4Scripting.cpp
        virtual bool  IsAlmostAllLandExplored(unsigned int player) { return s4->IsAlmostAllLandExplored(player); }  // defined in CS4Scripting.cpp
        virtual bool  IsAreaDarkLand(INT x, INT y, INT r) { return s4->IsAreaDarkLand(x, y, r); }  // defined in CS4Scripting.cpp
        virtual bool  IsAreaGreen(INT x, INT y, INT r) { return s4->IsAreaGreen(x, y, r); }  // defined in CS4Scripting.cpp
        virtual bool  IsAreaOwned(INT x, INT y, INT r, unsigned int player) { return s4->IsAreaOwned(x, y, r, player); }  // defined in CS4Scripting.cpp
        virtual unsigned int  GetNumberOfPlayers() { return s4->GetNumberOfPlayers(); }  // defined in CS4Scripting.cpp
        virtual Tribe  GetPlayerTribe(unsigned int player) { return static_cast<Tribe>(s4->GetPlayerTribe(player)); }  // defined in CS4Scripting.cpp
        virtual bool  ResetFogging() { return s4->ResetFogging(); }  // defined in CS4Scripting.cpp
        virtual bool  SetAlliesDontRevealFog(bool enable) { return s4->SetAlliesDontRevealFog(enable); }  // defined in CS4Scripting.cpp
        virtual bool  SetFightingStrength(unsigned int strength, unsigned int player) { return s4->SetFightingStrength(strength, player); }  // defined in CS4Scripting.cpp
        virtual bool  ShowClock(unsigned int seconds) { return s4->ShowClock(seconds); }  // defined in CS4Scripting.cpp
        virtual unsigned int  Time() { return s4->Time(); }  // defined in CS4Scripting.cpp
        virtual unsigned int  GoodsAddPileEx(GoodType good, unsigned int amount, INT x, INT y) { return s4->GoodsAddPileEx(static_cast<S4_GOOD_ENUM>(good), amount, x, y); }  // defined in CS4Scripting.cpp
        virtual unsigned int  GoodsAmount(GoodType good, unsigned int player) { return s4->GoodsAmount(static_cast<S4_GOOD_ENUM>(good), player); }  // defined in CS4Scripting.cpp
        virtual unsigned int  GoodsDelete(GoodType good, INT x, INT y, INT r) { return s4->GoodsDelete(static_cast<S4_GOOD_ENUM>(good), x, y, r); }  // defined in CS4Scripting.cpp
        virtual unsigned int  GoodsGetAmountInArea(GoodType good, INT x, INT y, INT r, unsigned int player) { return s4->GoodsGetAmountInArea(static_cast<S4_GOOD_ENUM>(good), x, y, r, player); }  // defined in CS4Scripting.cpp
        virtual unsigned int  MagicCastSpell(Tribe tribe, SpellType spell, INT x, INT y, unsigned int player) { return s4->MagicCastSpell(static_cast<S4_TRIBE_ENUM>(tribe), static_cast<S4_SPELL_ENUM>(spell), x, y, player); }  // defined in CS4Scripting.cpp
        virtual bool  MagicIncreaseMana(INT amount, unsigned int player) { return s4->MagicIncreaseMana(amount, player); }  // defined in CS4Scripting.cpp
        virtual unsigned int  MapAddDecoObject(unsigned int object, INT x, INT y) { return s4->MapAddDecoObject(object, x, y); }  // defined in CS4Scripting.cpp
        virtual bool  MapDeleteDecoObject(INT x, INT y, INT r) { return s4->MapDeleteDecoObject(x, y, r); }  // defined in CS4Scripting.cpp
        virtual unsigned int  MapSize() { return s4->MapSize(); }  // defined in CS4Scripting.cpp
        virtual bool  MapPointIsOnScreen(INT x, INT y) { return s4->MapPointIsOnScreen(x, y); }  // defined in CS4Scripting.cpp
        virtual bool  MapSetScreenPos(INT x, INT y) { return s4->MapSetScreenPos(x, y); }  // defined in CS4Scripting.cpp
        virtual bool  SettlersAdd(SettlerType settler, INT amount, INT x, INT y, unsigned int player) { return s4->SettlersAdd(static_cast<S4_SETTLER_ENUM>(settler), amount, x, y, player); }  // defined in CS4Scripting.cpp
        virtual bool  SettlersAddToFerry(unsigned int ferry, SettlerType settler, INT amount) { return s4->SettlersAddToFerry(ferry, static_cast<S4_SETTLER_ENUM>(settler), amount); }  // defined in CS4Scripting.cpp
        virtual unsigned int  SettlersAmount(SettlerType settler, unsigned int player) { return s4->SettlersAmount(static_cast<S4_SETTLER_ENUM>(settler), player); }  // defined in CS4Scripting.cpp
        virtual unsigned int  SettlersAmountInArea(SettlerType settler, INT x, INT y, INT r, unsigned int player) { return s4->SettlersAmountInArea(static_cast<S4_SETTLER_ENUM>(settler), x, y, r, player); }  // defined in CS4Scripting.cpp
        virtual bool  SettlersIsSelected(SettlerType settler, INT amount) { return s4->SettlersIsSelected(static_cast<S4_SETTLER_ENUM>(settler), amount); }  // defined in CS4Scripting.cpp
        virtual bool  SettlersKillSelectableSettlers(SettlerType settler, INT x, INT y, INT r, bool animation, unsigned int player) { return s4->SettlersKillSelectableSettlers(static_cast<S4_SETTLER_ENUM>(settler), x, y, r, animation, player); }  // defined in CS4Scripting.cpp
        virtual unsigned int  SettlersProductionAmount(SettlerType settler) { return s4->SettlersProductionAmount(static_cast<S4_SETTLER_ENUM>(settler)); }  // defined in CS4Scripting.cpp
        virtual bool  SettlersSetHealthInArea(SettlerType settler, INT x, INT y, INT r, unsigned int health, unsigned int player) { return s4->SettlersSetHealthInArea(static_cast<S4_SETTLER_ENUM>(settler), x, y, r, health, player); }  // defined in CS4Scripting.cpp
        virtual unsigned int  StatisticBuildingsCaptured(unsigned int player) { return s4->StatisticBuildingsCaptured(player); }  // defined in CS4Scripting.cpp
        virtual unsigned int  StatisticGoodsProduced(unsigned int player) { return s4->StatisticGoodsProduced(player); }  // defined in CS4Scripting.cpp
        virtual unsigned int  StatisticLandOwnedByPlayer(unsigned int player) { return s4->StatisticLandOwnedByPlayer(player); }  // defined in CS4Scripting.cpp
        virtual unsigned int  StatisticManaCollected(unsigned int player) { return s4->StatisticManaCollected(player); }  // defined in CS4Scripting.cpp
        virtual unsigned int  StatisticMushroomFarmsDestroyed(unsigned int player) { return s4->StatisticMushroomFarmsDestroyed(player); }  // defined in CS4Scripting.cpp
        virtual unsigned int  StatisticServantsFreed(unsigned int player) { return s4->StatisticServantsFreed(player); }  // defined in CS4Scripting.cpp
        virtual unsigned int  StatisticSpellsCast(unsigned int player) { return s4->StatisticSpellsCast(player); }  // defined in CS4Scripting.cpp
        virtual unsigned int  StatisticUnitsDestroyed(unsigned int player) { return s4->StatisticUnitsDestroyed(player); }  // defined in CS4Scripting.cpp
        virtual bool  ClearMarker() { return s4->ClearMarker(); }  // defined in CS4Scripting.cpp
        virtual bool  DeleteWorldCursor() { return s4->DeleteWorldCursor(); }  // defined in CS4Scripting.cpp
        virtual bool  PressButton(unsigned int dialog, unsigned int control) { return s4->PressButton(dialog, control); }  // defined in CS4Scripting.cpp
        virtual bool  RevealWorldMap(bool enable) { return s4->RevealWorldMap(enable); }  // defined in CS4Scripting.cpp
        virtual bool  SelectNextBuilding(BuildingType building) { return s4->SelectNextBuilding(static_cast<S4_BUILDING_ENUM>(building)); }  // defined in CS4Scripting.cpp
        virtual bool  SetMarker(INT x, INT y) { return s4->SetMarker(x, y); }  // defined in CS4Scripting.cpp
        virtual bool  SetWorldCursor(INT x, INT y) { return s4->SetWorldCursor(x, y); }  // defined in CS4Scripting.cpp
        virtual bool  SetZoom(INT zoom) { return s4->SetZoom(zoom); }  // defined in CS4Scripting.cpp
        virtual unsigned int  VehiclesAdd(VehicleType vehicle, unsigned int direction, unsigned int ammo, unsigned int commands, INT x, INT y, unsigned int player) { return s4->VehiclesAdd(static_cast<S4_VEHICLE_ENUM>(vehicle), direction, ammo, commands, x, y, player); }  // defined in CS4Scripting.cpp
        virtual bool  VehiclesAddWheelerToFerry(unsigned int ferry, VehicleType vehicle) { return s4->VehiclesAddWheelerToFerry(ferry, static_cast<S4_VEHICLE_ENUM>(vehicle)); }  // defined in CS4Scripting.cpp
        virtual unsigned int  VehiclesAmount(VehicleType vehicle, unsigned int player) { return s4->VehiclesAmount(static_cast<S4_VEHICLE_ENUM>(vehicle), player); }  // defined in CS4Scripting.cpp
        virtual unsigned int  VehiclesAmountInArea(VehicleType vehicle, INT x, INT y, INT r, unsigned int player) { return s4->VehiclesAmountInArea(static_cast<S4_VEHICLE_ENUM>(vehicle), x, y, r, player); }  // defined in CS4Scripting.cpp
        virtual unsigned int  VehiclesGetFerryCargoInArea(INT x, INT y, INT r, unsigned int player) { return s4->VehiclesGetFerryCargoInArea(x, y, r, player); }  // defined in CS4Scripting.cpp
        virtual unsigned int  VehiclesGetHealth(INT x, INT y) { return s4->VehiclesGetHealth(x, y); }  // defined in CS4Scripting.cpp
        virtual bool  VehiclesIsSelected(VehicleType vehicle, INT amount) { return s4->VehiclesIsSelected(static_cast<S4_VEHICLE_ENUM>(vehicle), amount); }  // defined in CS4Scripting.cpp
        virtual bool  VehiclesKill(VehicleType vehicle, INT x, INT y, INT r, unsigned int player) { return s4->VehiclesKill(static_cast<S4_VEHICLE_ENUM>(vehicle), x, y, r, player); }  // defined in CS4Scripting.cpp
        virtual bool  SetGround(INT x, INT y, INT r, unsigned int ground) { return s4->SetGround(x, y, r, ground); }  // defined in CS4Scripting.cpp
        virtual bool  ShowTextMessage(String^ message, unsigned int icon, unsigned int reserved) {
            pin_ptr<const wchar_t> wch = PtrToStringChars(message);
            size_t convertedChars = 0;
            size_t  sizeInBytes = wcslen(wch) * 2 + 1;
            errno_t err = 0;
            char* ch = (char*)malloc(sizeInBytes);

            err = wcstombs_s(&convertedChars,
                ch, sizeInBytes,
                wch, sizeInBytes);
            return s4->ShowTextMessage(ch, icon, reserved);
        }  // defined in CS4Scripting.cpp
        virtual bool  SoundPlay(SoundType sound, void* reserved, void* reserved2) { return s4->SoundPlay(static_cast<S4_SOUND_ENUM>(sound), reserved, reserved2); }  // defined in CS4Scripting.cpp
    };

}
