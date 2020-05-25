#pragma once
#include <Windows.h>

typedef void(*Starting)(DWORD);
typedef void(*Stoping)(void);

class HookLibrary
{
public:
	HookLibrary()
	{
		LoadDll = LoadLibraryW(L"MessageHook");
		StartHooking = (Starting)GetProcAddress(LoadDll, "StartHook");
		StopHooking = (Stoping)GetProcAddress(LoadDll, "StopHook");
	}

	HookLibrary(DWORD _GetWindowID)
	{
		GetWindowID = _GetWindowID;
		LoadDll = LoadLibraryW(L"MessageHook");
		StartHooking = (Starting)GetProcAddress(LoadDll, "StartHook");
		StopHooking = (Stoping)GetProcAddress(LoadDll, "StopHook");
	}

	~HookLibrary()
	{
		FreeLibrary(LoadDll);
	}

private:
	HINSTANCE LoadDll;
	Starting StartHooking;
	Stoping StopHooking;
	DWORD GetWindowID = 0;

public:
	BOOL StartHook()
	{
		if (NULL == StartHooking)
		{
			wprintf(L"StartHooking Error");
			return FALSE;
		}

		StartHooking(GetWindowID);
		return TRUE;
	}

	BOOL StopHook()
	{
		if (NULL == StopHooking)
		{
			wprintf(L"StopHooking Error");
			return FALSE;
		}

		StopHooking();
		return TRUE;
	}
};