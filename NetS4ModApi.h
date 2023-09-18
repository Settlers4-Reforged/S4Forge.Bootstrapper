#pragma once

#include "S4Hooks.h"

#pragma make_public(IDirectDrawSurface7)

using namespace System;
using namespace Runtime::InteropServices;
using namespace Collections::Generic;

namespace NetModAPI {
	ref class NetS4ModApi {
	private:
		static S4API s4;

	public:
		NetS4ModApi() { }

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
		}

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

#define CALLBACK_ADD(name, list, type) GCHandle::Alloc(name);std::function<type> fun = static_cast<LP##type>(Marshal::GetFunctionPointerForDelegate(name).ToPointer());auto callback = CreateCallback<type>(&fun); list##.emplace_back(callback); return callback->id

		/** Hooks/Observers **/
		void RemoveListener(S4HOOK hook) {
			RemoveAnyCallback(hook);
		}
		S4HOOK  AddFrameListener(S4FrameCallback ^clbk) {
			CALLBACK_ADD(clbk, S4FrameCallbackList, S4FRAMECALLBACK);
		}
		S4HOOK  AddUIFrameListener(S4FrameCallback ^clbk, S4_GUI_ENUM gui) {
			CALLBACK_ADD(clbk, S4UIFrameCallbackList, S4FRAMECALLBACK);
		}
		S4HOOK  AddMapInitListener(S4MapInitCallback ^clbk) {
			CALLBACK_ADD(clbk, S4MapInitCallbackList, S4MAPINITCALLBACK);
		}
		S4HOOK  AddMouseListener(S4MouseCallback ^clbk) {
			CALLBACK_ADD(clbk, S4MouseCallbackList, S4MOUSECALLBACK);
		}
		S4HOOK  AddSettlerSendListener(S4SettlerSendCallback ^clbk) {
			CALLBACK_ADD(clbk, S4SettlerSendCallbackList, S4SETTLERSENDCALLBACK);
		}
		S4HOOK  AddTickListener(S4TickCallback ^clbk) {
			CALLBACK_ADD(clbk,S4TickCallbackList , S4TICKCALLBACK);
		}
		S4HOOK  AddLuaOpenListener(S4LuaOpenCallback ^clbk) {
			CALLBACK_ADD(clbk, S4LuaOpenCallbackList, S4LUAOPENCALLBACK);
		}
		S4HOOK  AddBltListener(S4BltCallback ^clbk) {
			CALLBACK_ADD(clbk, S4BltCallbackList, S4BLTCALLBACK);
		}
		S4HOOK  AddEntityListener(S4EntityCallback ^clbk) {
			CALLBACK_ADD(clbk, S4EntityCallbackList, S4ENTITYCALLBACK);
		}
		S4HOOK  AddGuiBltListener(S4GuiBltCallback ^clbk) {
			CALLBACK_ADD(clbk, S4GUIBltCallbackList, S4GUIBLTCALLBACK);
		}
		S4HOOK  AddGuiClearListener(S4GuiClearCallback ^clbk) {
			CALLBACK_ADD(clbk, S4GUIClearCallbackList, S4GUICLEARCALLBACK);
		}
		S4HOOK  AddGuiElementBltListener(S4GuiDrawCallback^ clbk) {
			CALLBACK_ADD(clbk, S4GUIDrawCallbackList, S4GUIDRAWCALLBACK);
		}


		/** IUnknown methods **/
		HRESULT QueryInterface(REFIID riid, LPVOID FAR* ppvObj) { return s4->QueryInterface(riid, ppvObj); }
		ULONG  AddRef() { return s4->AddRef(); }
		ULONG  Release() { return s4->Release(); }

		/** ISettlers4Api methods **/
		LPVOID  GetDebugData(LPVOID a, LPVOID b) { return s4->GetDebugData(a, b); }
		DWORD  GetLastError() { return s4->GetLastError(); }

		/** Misc helper functions **/
		HRESULT GetMD5OfModule(HMODULE module, LPSTR out, SIZE_T sz) { return s4->GetMD5OfModule(module, out, sz); };
		BOOL  IsEdition(S4_EDITION_ENUM edition) { return s4->IsEdition(edition); }
		HWND  GetHwnd() { return s4->GetHwnd(); }

		/** Settlers 4 functions **/
		HRESULT GetHoveringUiElement(LPS4UIELEMENT element) { return s4->GetHoveringUiElement(element); };
		BOOL  GetEntitiesCount(WORD* counter) { return s4->GetEntitiesCount(counter); }

		BOOL  IsCurrentlyOnScreen(S4_GUI_ENUM gui) { return s4->IsCurrentlyOnScreen(gui); }
		S4_ENTITY_ENUM  EntityGetClass(WORD entity) { return s4->EntityGetClass(entity); }
		BOOL  GetEntities(DWORD* entities, size_t size) { return s4->GetEntities(entities, size); };
		BOOL  EntityGetPosition(WORD entity, LPINT x, LPINT y) { return s4->EntityGetPosition(entity, x, y); }
		BOOL  EntityGetDirection(WORD entity, LPDWORD dir) { return s4->EntitygGetDirection(entity, dir); }
		BOOL  EntityGetHealth(WORD entity, LPDWORD health) { return s4->EntityGetHealth(entity, health); }
		BOOL  EntityGetMaxHealth(WORD entity, LPDWORD maxHealth) { return s4->EntityGetMaxHealth(entity, maxHealth); }
		BOOL  EntityGetTribe(WORD entity, S4_TRIBE_ENUM* tribe) { return s4->EntityGetTribe(entity, tribe); }
		BOOL  EntityTreeGetType(WORD entity, S4_TREE_ENUM* tree) { return s4->EntityTreeGetType(entity, tree); }
		BOOL  EntityPileGetType(WORD entity, S4_GOOD_ENUM* good) { return s4->EntityPileGetType(entity, good); }
		BOOL  EntityDecoGetType(WORD entity, LPDWORD deco) { return s4->EntityDecoGetType(entity, deco); }
		BOOL  EntityPileGetAmount(WORD entity, LPDWORD amount) { return s4->EntityPileGetAmount(entity, amount); }
		BOOL  EntitySettlerGetType(WORD entity, S4_SETTLER_ENUM* settler) { return s4->EntitySettlerGetType(entity, settler); }
		BOOL  EntityVehicleGetType(WORD entity, S4_VEHICLE_ENUM* vehicle) { return s4->EntityVehicleGetType(entity, vehicle); }
		BOOL  EntityAnimalGetType(WORD entity, S4_ANIMAL_ENUM* animal) { return s4->EntityAnimalGetType(entity, animal); }
		BOOL  EntityBuildingGetType(WORD entity, S4_BUILDING_ENUM* building) { return s4->EntityBuildingGetType(entity, building); }
		BOOL  EntityStoneGetLevel(WORD entity, LPDWORD level) { return s4->EntityStoneGetLevel(entity, level); }
		BOOL  EntityGetRole(WORD entity, LPVOID* role) { return s4->EntityGetRole(entity, role); }
		BOOL  EntityGetRoleClass(WORD entity, S4_ROLE_ENUM* role) { return s4->EntitygGetRoleClass(entity, role); }
		BOOL  EntityGetOwner(WORD entity, LPDWORD player) { return s4->EntitygGetOwner(entity, player); }

		BOOL  ClearSelection() { return s4->ClearSelection(); }  // defined in CS4Selection.cpp
		BOOL  GetSelection(PWORD out, SIZE_T outlen, PSIZE_T selectionCount) { return s4->GetSelection(out, outlen, selectionCount); }  // defined in CS4Selection.cpp
		BOOL  RemoveSelection(PWORD settlers, SIZE_T settlerslen, PSIZE_T removedCount) { return s4->RemoveSelection(settlers, settlerslen, removedCount); }  // defined in CS4Selection.cpp
		DWORD  SetMaxSelection(DWORD maxSelection) { return s4->SetMaxSelection(maxSelection); }  // defined in CS4Selection.cpp
		DWORD  GetMaxSelection() { return s4->GetMaxSelection(); }  // defined in CS4Selection.cpp
		BOOL  StartBuildingPlacement(S4_BUILDING_ENUM building) { return s4->StartBuildingPlacement(building); }
		DWORD  GetGameTime() { return s4->GetGameTime(); }

		/** Landscape functions **/
		DWORD  LandscapeGetHeight(INT x, INT y) { return s4->LandscapeGetHeight(x, y); }  // defined in CS4Landscape.cpp
		S4_GROUND_ENUM  LandscapeGetType(INT x, INT y) { return s4->LandscapeGetType(x, y); }  // defined in CS4Landscape.cpp
		BOOL  LandscapeIsPond(INT x, INT y) { return s4->LandscapeIsPond(x, y); }  // defined in CS4Landscape.cpp
		BOOL  LandscapeIsDarkLand(INT x, INT y) { return s4->LandscapeIsDarkLand(x, y); }  // defined in CS4Landscape.cpp
		BOOL  LandscapeIsDarkLandBorder(INT x, INT y) { return s4->LandscapeIsDarkLandBorder(x, y); }  // defined in CS4Landscape.cpp
		DWORD  LandscapeGetFogOfWar(INT x, INT y) { return s4->LandscapeGetFogOfWar(x, y); }  // defined in CS4Landscape.cpp
		BOOL  LandscapeIsFoundingStone(INT x, INT y) { return s4->LandscapeIsFoundingStone(x, y); }  // defined in CS4Landscape.cpp
		S4_RESOURCE_ENUM  LandscapeGetResource(INT x, INT y) { return s4->LandscapeGetResource(x, y); }  // defined in CS4Landscape.cpp
		BOOL  LandscapeSetResource(INT x, INT y, S4_RESOURCE_ENUM res) { return s4->LandscapeSetResource(x, y, res); }  // defined in CS4Landscape.cpp
		DWORD  LandscapeGetObject(INT x, INT y) { return s4->LandscapeGetObject(x, y); }  // defined in CS4Landscape.cpp
		DWORD  LandscapeGetOwner(INT x, INT y) { return s4->LandscapeGetOwner(x, y); }  // defined in CS4Landscape.cpp
		DWORD  LandscapeGetEcoSector(INT x, INT y) { return s4->LandscapeGetEcoSector(x, y); }; // defined in CS4Landscape.cpp	
		BOOL  LandscapeIsOccupied(INT x, INT y) { return s4->LandscapeIsOccupied(x, y); }; // defined in CS4Landscape.cpp

		/** Settlers 4 NetEvents functions **/
		BOOL  SendWarriors(INT x, INT y, S4_MOVEMENT_ENUM mode, PWORD warriors, SIZE_T countOfWarriors, DWORD player) { return s4->SendWarriors(x, y, mode, warriors, countOfWarriors, player); }
		BOOL  BuildBuilding(S4_BUILDING_ENUM buildingType, INT x, INT y, DWORD player) { return s4->BuildBuilding(buildingType, x, y, player); }  // defined in CS4Build.cpp
		BOOL  CrushBuilding(DWORD building, DWORD player) { return s4->CrushBuilding(building, player); }  // defined in CS4Build.cpp
		BOOL  ToggleBuildingPriority(DWORD building, DWORD player) { return s4->ToggleBuildingPriority(building, player); }  // defined in CS4Build.cpp
		BOOL  ToggleBuildingHalt(DWORD building, DWORD player) { return s4->ToggleBuildingHalt(building, player); }  // defined in CS4Build.cpp
		BOOL  SetBuildingWorkarea(DWORD building, INT x, INT y, DWORD player) { return s4->SetBuildingWorkarea(building, x, y, player); }  // defined in CS4Build.cpp
		BOOL  SetBuildingProduction(DWORD building, S4_GOOD_ENUM good, INT amount, DWORD player) { return s4->SetBuildingProduction(building, good, amount, player); }  // defined in CS4Build.cpp
		BOOL  SetBuildingProductionPercentMode(DWORD building, BOOL enable, DWORD player) { return s4->SetBuildingProductionPercentMode(building, enable, player); }  // defined in CS4Build.cpp
		BOOL  SetBuildingProductionPercentage(DWORD building, BYTE swords, BYTE bows, BYTE armors, BYTE racespecialweapons, DWORD player) { return s4->SetBuildingProductionPercentage(building, swords, bows, armors, racespecialweapons, player); }  // defined in CS4Build.cpp
		BOOL  CastSpell(DWORD priest, DWORD spell, INT x, INT y, DWORD player) { return s4->CastSpell(priest, spell, x, y, player); }  // defined in CS4Casting.cpp
		BOOL  GarrisonWarriors(DWORD building, DWORD player) { return s4->GarrisonWarriors(building, player); }  // defined in CS4GarrisonWarriors.cpp
		BOOL  UnGarrisonWarriors(DWORD building, INT column, BOOL bowman, DWORD player) { return s4->UnGarrisonWarriors(building, column, bowman, player); }  // defined in CS4GarrisonWarriors.cpp
		BOOL  ChangeGoodDistribution(S4_GOOD_ENUM good, S4_BUILDING_ENUM building, INT percent, DWORD ecosector, DWORD player) { return s4->ChangeGoodDistribution(good, building, percent, ecosector, player); }  // defined in CS4GoodDistribution.cpp
		BOOL  ChangeGoodPriority(S4_GOOD_ENUM good, INT offset, DWORD ecosector, DWORD player) { return s4->ChangeGoodPriority(good, offset, ecosector, player); }  // defined in CS4GoodDistribution.cpp
		BOOL  RecruitWarriors(DWORD building, S4_SETTLER_ENUM unit, INT amount, DWORD player) { return s4->RecruitWarriors(building, unit, amount, player); }  // defined in CS4Recruit.cpp
		BOOL  RecruitVehicle(DWORD building, S4_VEHICLE_ENUM unit, INT amount, DWORD player) { return s4->RecruitVehicle(building, unit, amount, player); }  // defined in CS4Recruit.cpp
		BOOL  SetTradingRoute(DWORD sourceBuilding, DWORD destinationBuilding, DWORD player) { return s4->SetTradingRoute(sourceBuilding, destinationBuilding, player); }  // defined in CS4Trading.cpp
		BOOL  TradeGood(DWORD buidling, S4_GOOD_ENUM good, INT amount, DWORD player) { return s4->TradeGood(buidling, good, amount, player); }  // defined in CS4Trading.cpp
		BOOL  StoreGood(DWORD buidling, S4_GOOD_ENUM good, BOOL enable, DWORD player) { return s4->StoreGood(buidling, good, enable, player); }  // defined in CS4Trading.cpp

		/** UI Rendering **/
		S4CUSTOMUI  ShowMessageBox(LPCWSTR title, LPCWSTR message, INT x, INT y, INT w, INT h, DWORD flags) { return s4->ShowMessageBox(title, message, x, y, w, h, flags); }
		S4CUSTOMUI  CreateCustomUiElement(LPCS4CUSTOMUIELEMENT custom_ui) { return s4->CreateCustomUiElement(custom_ui); }
		BOOL  DestroyCustomUiElement(S4CUSTOMUI custom_ui) { return s4->DestroyCustomUiElement(custom_ui); }
		BOOL  HideCustomUiElement(S4CUSTOMUI custom_ui) { return s4->HideCustomUiElement(custom_ui); }
		BOOL  ShowCustomUiElement(S4CUSTOMUI custom_ui) { return s4->ShowCustomUiElement(custom_ui); }

		/** S4 Scripting **/
		DWORD  GetLocalPlayer() { return s4->GetLocalPlayer(); }  // defined in CS4Scripting.cpp
		DWORD  BuildingsAdd(S4_BUILDING_ENUM building, INT x, INT y, DWORD player) { return s4->BuildingsAdd(building, x, y, player); }  // defined in CS4Scripting.cpp
		DWORD  BuildingsAmount(S4_BUILDING_ENUM building, DWORD status, DWORD player) { return s4->BuildingsAmount(building, status, player); }  // defined in CS4Scripting.cpp
		BOOL  BuildingsCrush(DWORD building) { return s4->BuildingsCrush(building); }  // defined in CS4Scripting.cpp
		BOOL  BuildingsDelete(DWORD building, DWORD mode) { return s4->BuildingsDelete(building, mode); }  // defined in CS4Scripting.cpp
		BOOL  BuildingsExistsBuildingInArea(S4_BUILDING_ENUM building, INT x, INT y, INT r, DWORD status, DWORD player) { return s4->BuildingsExistsBuildingInArea(building, x, y, r, status, player); }  // defined in CS4Scripting.cpp
		DWORD  BuildingsGetFirstBuilding(S4_BUILDING_ENUM building, DWORD player) { return s4->BuildingsGetFirstBuilding(building, player); }  // defined in CS4Scripting.cpp
		DWORD  BuildingsGetInhabitantAmount(DWORD building, DWORD player) { return s4->BuildingsGetInhabitantAmount(building, player); }  // defined in CS4Scripting.cpp
		DWORD  BuildingsGetTarget(DWORD building) { return s4->BuildingsGetTarget(building); }  // defined in CS4Scripting.cpp
		BOOL  BuildingsIsSelected(S4_BUILDING_ENUM building) { return s4->BuildingsIsSelected(building); }  // defined in CS4Scripting.cpp
		DWORD  DarkTribeAddManakopter(INT x, INT y, DWORD player) { return s4->DarkTribeAddManakopter(x, y, player); }  // defined in CS4Scripting.cpp
		BOOL  DarkTribeFlyTo(INT x, INT y) { return s4->DarkTribeFlyTo(x, y); }  // defined in CS4Scripting.cpp
		BOOL  AIActivate(DWORD player, BOOL activate) { return s4->AIActivate(player, activate); }  // defined in CS4Scripting.cpp
		BOOL  MakeDark(INT x, INT y) { return s4->MakeDark(x, y); }  // defined in CS4Scripting.cpp
		BOOL  MakeGreen(INT x, INT y) { return s4->MakeGreen(x, y); }  // defined in CS4Scripting.cpp
		DWORD  EffectsAdd(DWORD effect, DWORD sound, INT x, INT y, DWORD delay) { return s4->EffectsAdd(effect, sound, x, y, delay); }  // defined in CS4Scripting.cpp
		DWORD  ArePlayerAreasConnected(INT x1, INT y1, DWORD player2, INT x2, INT y2, DWORD player) { return s4->ArePlayerAreasConnected(x1, y1, player2, x2, y2, player); }  // defined in CS4Scripting.cpp
		BOOL  GameDefaultGameEndCheck() { return s4->GameDefaultGameEndCheck(); }  // defined in CS4Scripting.cpp
		BOOL  DisableLastNPlayersInStatistic(DWORD n) { return s4->DisableLastNPlayersInStatistic(n); }  // defined in CS4Scripting.cpp
		BOOL  EnableLandExploredCheck() { return s4->EnableLandExploredCheck(); }  // defined in CS4Scripting.cpp
		BOOL  FindAnyUnit(INT x, INT y, INT r, DWORD player) { return s4->FindAnyUnit(x, y, r, player); }  // defined in CS4Scripting.cpp
		DWORD  GetAmountOfTreesInArea(INT x, INT y, INT r) { return s4->GetAmountOfTreesInArea(x, y, r); }  // defined in CS4Scripting.cpp
		DWORD  GetDifficulty() { return s4->GetDifficulty(); }  // defined in CS4Scripting.cpp
		DWORD  GetNumberOfSquaresWithDarkLand(DWORD player) { return s4->GetNumberOfSquaresWithDarkLand(player); }  // defined in CS4Scripting.cpp
		DWORD  GetOffenceFightingStrength(DWORD player) { return s4->GetOffenceFightingStrength(player); }  // defined in CS4Scripting.cpp
		BOOL  HasPlayerLost(DWORD player) { return s4->HasPlayerLost(player); }  // defined in CS4Scripting.cpp
		BOOL  IsAlmostAllLandExplored(DWORD player) { return s4->IsAlmostAllLandExplored(player); }  // defined in CS4Scripting.cpp
		BOOL  IsAreaDarkLand(INT x, INT y, INT r) { return s4->IsAreaDarkLand(x, y, r); }  // defined in CS4Scripting.cpp
		BOOL  IsAreaGreen(INT x, INT y, INT r) { return s4->IsAreaGreen(x, y, r); }  // defined in CS4Scripting.cpp
		BOOL  IsAreaOwned(INT x, INT y, INT r, DWORD player) { return s4->IsAreaOwned(x, y, r, player); }  // defined in CS4Scripting.cpp
		DWORD  GetNumberOfPlayers() { return s4->GetNumberOfPlayers(); }  // defined in CS4Scripting.cpp
		S4_TRIBE_ENUM  GetPlayerTribe(DWORD player) { return s4->GetPlayerTribe(player); }  // defined in CS4Scripting.cpp
		BOOL  ResetFogging() { return s4->ResetFogging(); }  // defined in CS4Scripting.cpp
		BOOL  SetAlliesDontRevealFog(BOOL enable) { return s4->SetAlliesDontRevealFog(enable); }  // defined in CS4Scripting.cpp
		BOOL  SetFightingStrength(DWORD strength, DWORD player) { return s4->SetFightingStrength(strength, player); }  // defined in CS4Scripting.cpp
		BOOL  ShowClock(DWORD seconds) { return s4->ShowClock(seconds); }  // defined in CS4Scripting.cpp
		DWORD  Time() { return s4->Time(); }  // defined in CS4Scripting.cpp
		DWORD  GoodsAddPileEx(S4_GOOD_ENUM good, DWORD amount, INT x, INT y) { return s4->GoodsAddPileEx(good, amount, x, y); }  // defined in CS4Scripting.cpp
		DWORD  GoodsAmount(S4_GOOD_ENUM good, DWORD player) { return s4->GoodsAmount(good, player); }  // defined in CS4Scripting.cpp
		DWORD  GoodsDelete(S4_GOOD_ENUM good, INT x, INT y, INT r) { return s4->GoodsDelete(good, x, y, r); }  // defined in CS4Scripting.cpp
		DWORD  GoodsGetAmountInArea(S4_GOOD_ENUM good, INT x, INT y, INT r, DWORD player) { return s4->GoodsGetAmountInArea(good, x, y, r, player); }  // defined in CS4Scripting.cpp
		DWORD  MagicCastSpell(S4_TRIBE_ENUM tribe, S4_SPELL_ENUM spell, INT x, INT y, DWORD player) { return s4->MagicCastSpell(tribe, spell, x, y, player); }  // defined in CS4Scripting.cpp
		BOOL  MagicIncreaseMana(INT amount, DWORD player) { return s4->MagicIncreaseMana(amount, player); }  // defined in CS4Scripting.cpp
		DWORD  MapAddDecoObject(DWORD object, INT x, INT y) { return s4->MapAddDecoObject(object, x, y); }  // defined in CS4Scripting.cpp
		BOOL  MapDeleteDecoObject(INT x, INT y, INT r) { return s4->MapDeleteDecoObject(x, y, r); }  // defined in CS4Scripting.cpp
		DWORD  MapSize() { return s4->MapSize(); }  // defined in CS4Scripting.cpp
		BOOL  MapPointIsOnScreen(INT x, INT y) { return s4->MapPointIsOnScreen(x, y); }  // defined in CS4Scripting.cpp
		BOOL  MapSetScreenPos(INT x, INT y) { return s4->MapSetScreenPos(x, y); }  // defined in CS4Scripting.cpp
		BOOL  SettlersAdd(S4_SETTLER_ENUM settler, INT amount, INT x, INT y, DWORD player) { return s4->SettlersAdd(settler, amount, x, y, player); }  // defined in CS4Scripting.cpp
		BOOL  SettlersAddToFerry(DWORD ferry, S4_SETTLER_ENUM settler, INT amount) { return s4->SettlersAddToFerry(ferry, settler, amount); }  // defined in CS4Scripting.cpp
		DWORD  SettlersAmount(S4_SETTLER_ENUM settler, DWORD player) { return s4->SettlersAmount(settler, player); }  // defined in CS4Scripting.cpp
		DWORD  SettlersAmountInArea(S4_SETTLER_ENUM settler, INT x, INT y, INT r, DWORD player) { return s4->SettlersAmountInArea(settler, x, y, r, player); }  // defined in CS4Scripting.cpp
		BOOL  SettlersIsSelected(S4_SETTLER_ENUM settler, INT amount) { return s4->SettlersIsSelected(settler, amount); }  // defined in CS4Scripting.cpp
		BOOL  SettlersKillSelectableSettlers(S4_SETTLER_ENUM settler, INT x, INT y, INT r, BOOL animation, DWORD player) { return s4->SettlersKillSelectableSettlers(settler, x, y, r, animation, player); }  // defined in CS4Scripting.cpp
		DWORD  SettlersProductionAmount(S4_SETTLER_ENUM settler) { return s4->SettlersProductionAmount(settler); }  // defined in CS4Scripting.cpp
		BOOL  SettlersSetHealthInArea(S4_SETTLER_ENUM settler, INT x, INT y, INT r, DWORD health, DWORD player) { return s4->SettlersSetHealthInArea(settler, x, y, r, health, player); }  // defined in CS4Scripting.cpp
		DWORD  StatisticBuildingsCaptured(DWORD player) { return s4->StatisticBuildingsCaptured(player); }  // defined in CS4Scripting.cpp
		DWORD  StatisticGoodsProduced(DWORD player) { return s4->StatisticGoodsProduced(player); }  // defined in CS4Scripting.cpp
		DWORD  StatisticLandOwnedByPlayer(DWORD player) { return s4->StatisticLandOwnedByPlayer(player); }  // defined in CS4Scripting.cpp
		DWORD  StatisticManaCollected(DWORD player) { return s4->StatisticManaCollected(player); }  // defined in CS4Scripting.cpp
		DWORD  StatisticMushroomFarmsDestroyed(DWORD player) { return s4->StatisticMushroomFarmsDestroyed(player); }  // defined in CS4Scripting.cpp
		DWORD  StatisticServantsFreed(DWORD player) { return s4->StatisticServantsFreed(player); }  // defined in CS4Scripting.cpp
		DWORD  StatisticSpellsCast(DWORD player) { return s4->StatisticSpellsCast(player); }  // defined in CS4Scripting.cpp
		DWORD  StatisticUnitsDestroyed(DWORD player) { return s4->StatisticUnitsDestroyed(player); }  // defined in CS4Scripting.cpp
		BOOL  ClearMarker() { return s4->ClearMarker(); }  // defined in CS4Scripting.cpp
		BOOL  DeleteWorldCursor() { return s4->DeleteWorldCursor(); }  // defined in CS4Scripting.cpp
		BOOL  PressButton(DWORD dialog, DWORD control) { return s4->PressButton(dialog, control); }  // defined in CS4Scripting.cpp
		BOOL  RevealWorldMap(BOOL enable) { return s4->RevealWorldMap(enable); }  // defined in CS4Scripting.cpp
		BOOL  SelectNextBuilding(S4_BUILDING_ENUM building) { return s4->SelectNextBuilding(building); }  // defined in CS4Scripting.cpp
		BOOL  SetMarker(INT x, INT y) { return s4->SetMarker(x, y); }  // defined in CS4Scripting.cpp
		BOOL  SetWorldCursor(INT x, INT y) { return s4->SetWorldCursor(x, y); }  // defined in CS4Scripting.cpp
		BOOL  SetZoom(INT zoom) { return s4->SetZoom(zoom); }  // defined in CS4Scripting.cpp
		DWORD  VehiclesAdd(S4_VEHICLE_ENUM vehicle, DWORD direction, DWORD ammo, DWORD commands, INT x, INT y, DWORD player) { return s4->VehiclesAdd(vehicle, direction, ammo, commands, x, y, player); }  // defined in CS4Scripting.cpp
		BOOL  VehiclesAddWheelerToFerry(DWORD ferry, S4_VEHICLE_ENUM vehicle) { return s4->VehiclesAddWheelerToFerry(ferry, vehicle); }  // defined in CS4Scripting.cpp
		DWORD  VehiclesAmount(S4_VEHICLE_ENUM vehicle, DWORD player) { return s4->VehiclesAmount(vehicle, player); }  // defined in CS4Scripting.cpp
		DWORD  VehiclesAmountInArea(S4_VEHICLE_ENUM vehicle, INT x, INT y, INT r, DWORD player) { return s4->VehiclesAmountInArea(vehicle, x, y, r, player); }  // defined in CS4Scripting.cpp
		DWORD  VehiclesGetFerryCargoInArea(INT x, INT y, INT r, DWORD player) { return s4->VehiclesGetFerryCargoInArea(x, y, r, player); }  // defined in CS4Scripting.cpp
		DWORD  VehiclesGetHealth(INT x, INT y) { return s4->VehiclesGetHealth(x, y); }  // defined in CS4Scripting.cpp
		BOOL  VehiclesIsSelected(S4_VEHICLE_ENUM vehicle, INT amount) { return s4->VehiclesIsSelected(vehicle, amount); }  // defined in CS4Scripting.cpp
		BOOL  VehiclesKill(S4_VEHICLE_ENUM vehicle, INT x, INT y, INT r, DWORD player) { return s4->VehiclesKill(vehicle, x, y, r, player); }  // defined in CS4Scripting.cpp
		BOOL  SetGround(INT x, INT y, INT r, DWORD ground) { return s4->SetGround(x, y, r, ground); }  // defined in CS4Scripting.cpp
		BOOL  ShowTextMessage(LPCSTR message, DWORD icon, DWORD reserved) { return s4->ShowTextMessage(message, icon, reserved); }  // defined in CS4Scripting.cpp
		BOOL  SoundPlay(S4_SOUND_ENUM sound, LPVOID reserved, LPVOID reserved2) { return s4->SoundPlay(sound, reserved, reserved2); }  // defined in CS4Scripting.cpp
	};

}
