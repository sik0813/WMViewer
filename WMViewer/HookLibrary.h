#pragma once
#include <Windows.h>
#include "..\MessageHook\MessageHook.h"

typedef void(*Starting)(DWORD);
typedef void(*Stoping)(void);

class HookLibrary
{
public:
	HookLibrary()
	{
		//LoadDll = LoadLibraryW(L"MessageHook");
		//StartHooking = (Starting)GetProcAddress(LoadDll, "StartHook");
		//StopHooking = (Stoping)GetProcAddress(LoadDll, "StopHook");
	}

	HookLibrary(DWORD _GetWindowID)
	{
		GetWindowID = _GetWindowID;
		LoadDll = LoadLibraryW(L"MessageHook");
		StartHookingFunc = (Starting)GetProcAddress(LoadDll, "StartHook");
		StopHookingFunc = (Stoping)GetProcAddress(LoadDll, "StopHook");
	}

	~HookLibrary()
	{
		FreeLibrary(LoadDll);
	}

private:
	HINSTANCE LoadDll;
	Starting StartHookingFunc;
	Stoping StopHookingFunc;
	BOOL runningHooking = FALSE;
	DWORD GetWindowID = 0;

public:
	BOOL StartHooking()
	{
		//if (NULL == StartHookingFunc)
		//{
		//	wprintf(L"StartHooking Error");
		//	return FALSE;
		//}

		if (TRUE == runningHooking)
		{
			wprintf(L"Already Running Hooking");
			return FALSE;
		}

		//StartHooking(GetWindowID);
		StartHook();
		runningHooking = TRUE;
		return TRUE;
	}

	BOOL StopHooking()
	{
		//if (NULL == StopHookingFunc)
		//{
		//	wprintf(L"StopHooking Error");
		//	return FALSE;
		//}

		if (FALSE == runningHooking)
		{
			wprintf(L"Not Running Hooking");
			return FALSE;
		}

		StopHook();
		runningHooking = FALSE;
		return TRUE;
	}
};