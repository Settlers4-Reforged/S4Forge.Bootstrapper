#include "pch.h"
#pragma unmanaged
#include "S4Hooks.h"

using namespace S4Hook;

DWORD S4_Main;

TPostToMessageQueue S4Hook::PostToMessageQueue;
TCreateS4Event S4Hook::CreateS4Event;
TCreateS4UIEvent S4Hook::CreateS4UIEvent;

typedef void* (__thiscall* TPlayS4Sound) (void* _this, void* a2, unsigned int a3, int a4, int a5, int a6, int a7);
TPlayS4Sound PlayS4Sound;

DWORD* MusicVolume;
DWORD* MusicToggle;

DWORD* SoundVolume;
DWORD* SoundToggle;

void *GetCurrentSoundInfo() {
	return *reinterpret_cast<void**>(S4_Main + 0x10540C4);
}

int S4Hook::GetMusicVolume() {
	return *MusicVolume;
}

void S4Hook::SetMusicVolume(int volume) {
	DWORD S4SetMusicVol = S4_Main + 0x10C4A0;
	void* SoundInfo = GetCurrentSoundInfo();

	*MusicVolume = volume;

	if(*MusicToggle == 0)
		volume = 0;

	__asm {
		mov ecx, SoundInfo
		push volume
		call S4SetMusicVol
	}
}

int S4Hook::GetMusicEnabled() {
	return *MusicToggle;
}

void S4Hook::ToggleMusic(bool enabled) {
	*MusicToggle = enabled;
	SetMusicVolume(*MusicVolume);
}

int S4Hook::GetSoundVolume() {
	return *SoundVolume;
}

void S4Hook::SetSoundVolume(int volume) {
	DWORD S4SetSoundVol = S4_Main + 0x10C500;
	void* SoundInfo = GetCurrentSoundInfo();

	*SoundVolume = volume;

	if (*SoundToggle == 0)
		volume = 0;

	__asm {
		mov ecx, SoundInfo
		push volume
		call S4SetSoundVol
	}
}

int S4Hook::GetSoundEnabled() {
	return *SoundToggle;
}

void S4Hook::ToggleSound(bool enabled) {
	*SoundToggle = enabled;
	SetSoundVolume(*SoundVolume);
}

void S4Hook::PlaySound(int id) {
	DWORD GlobalMusic = *(DWORD*)(S4_Main + 0x10540C4);

	__asm {
		mov esi, 0
		mov ecx, GlobalMusic
		push 0
		push GlobalMusic
		push dword ptr [ecx]
		push 0x64
		push id
		call PlayS4Sound
	}
}


void S4Hook::CallEventManagement(void* _event) {
	DWORD GlobalEventManager = *(DWORD*)(S4_Main + 0xE94804);
	DWORD EventManagement = S4_Main + 0x1172F0;
	__asm {
		mov ecx, GlobalEventManager
		mov eax, [ecx]
		mov edx,0
		mov edi,0
		push _event
		call EventManagement
	}
}

struct CEvn_Event {
	void* __vftable /*VFT*/;
	int event;
	int wparam;
	int lparam;
	DWORD eventEngineAttribute;
};

void __stdcall PostToEventEngine(CEvn_Event *e) {
	// TODO: Add event handling
}

DWORD PostToEventEngineHookReturn;

void __declspec(naked) EventEnginePostHook() {
	__asm {
		push ebx
		call PostToEventEngine
		jmp PostToEventEngineHookReturn
	}
}

void S4Hook::LoadS4Hooks() {
	S4_Main = (DWORD)GetModuleHandleW(nullptr);
	void* EventEngine = (void*)(S4_Main + 0x106B11C);
	MusicVolume = (DWORD*)(S4_Main + 0xDB596C);
	MusicToggle = (DWORD*)(S4_Main + 0xDB5978);

	SoundVolume = (DWORD*)(S4_Main + 0xDB5984);
	SoundToggle = (DWORD*)(S4_Main + 0xDB5990);

	PostToMessageQueue = (TPostToMessageQueue)(S4_Main + 0x5A7C0);
	CreateS4Event = (TCreateS4Event)(S4_Main + 0x4CC70);
	CreateS4UIEvent = (TCreateS4UIEvent)(S4_Main + 0x4CCC0);
	PlayS4Sound = (TPlayS4Sound)(S4_Main + 0x10C390);

#ifdef DEBUG
	//PostToEventEngineHookReturn = S4_Main + 0x5A827;
	//hlib::JmpPatch PostToEventEngineHook(S4_Main + 0x5A7DC, (DWORD)EventEnginePostHook);
	//PostToEventEngineHook.patch();
#endif

}