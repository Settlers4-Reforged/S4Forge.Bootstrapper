#pragma once

namespace S4Hook {
	typedef char(__thiscall* TPostToMessageQueue)(void* _this, void* _event);
	typedef void* (__thiscall* TCreateS4Event) (void* _this, int a2, int a3, int a4, char a5);
	typedef void* (__thiscall* TCreateS4UIEvent) (void* _this, int a2, int a3, int a4, char a5, int a6, __int16 a7);


	extern TPostToMessageQueue PostToMessageQueue;
	extern TCreateS4Event CreateS4Event;
	extern TCreateS4UIEvent CreateS4UIEvent;

	extern int GetMusicVolume();
	extern void SetMusicVolume(int volume);

	extern int GetMusicEnabled();
	extern void ToggleMusic(bool enabled);

	extern int GetSoundVolume();
	extern void SetSoundVolume(int volume);

	extern int GetSoundEnabled();
	extern void ToggleSound(bool enabled);

	extern void PlaySound(int id);

	extern void CallEventManagement(void *_event);

	extern void LoadS4Hooks();
}