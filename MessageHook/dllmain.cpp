// dllmain.cpp : DLL 응용 프로그램의 진입점을 정의합니다.
#include "stdafx.h"
#include "MessageHook.h"
#include <strsafe.h>
#include <Psapi.h>

typedef struct sendStruct
{
	WCHAR message[50];
	WPARAM wParam;
	LPARAM lParam;
}sendStruct;

HINSTANCE kdllInstanceHandle = NULL;
HHOOK kconnnectHook = NULL;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		kdllInstanceHandle = (HINSTANCE)hModule;
		break;

	case DLL_THREAD_ATTACH:
		break;

	case DLL_THREAD_DETACH:
		break;

	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

EXPORT void startHook(DWORD threadID)
{
	kconnnectHook = SetWindowsHookExW(WH_CALLWNDPROC, (HOOKPROC)GetMsgProc, kdllInstanceHandle, threadID);
	return;
}


EXPORT void stopHook(void)
{
	UnhookWindowsHookEx(kconnnectHook);
	return;
}

LRESULT WINAPI GetMsgProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	WCHAR szProcessName[MAX_PATH] = { 0, };
	HANDLE thisProcess = GetCurrentProcess();
	DWORD successFunc = GetModuleFileNameEx(thisProcess, 0, szProcessName, sizeof(szProcessName) / sizeof(WCHAR));
	if (NULL != successFunc)
	{
		if (0 == wcscmp(L"notepad.exe", wcsrchr(szProcessName, L'\\') + 1))
		{
			sendStruct sendInfo;
			ZeroMemory(&sendInfo, sizeof(sendInfo));
			LPWSTR sendWM = wmTranslation[nCode];
			StringCchCopyW(sendInfo.message, wcslen(sendWM) + 1, sendWM);
			sendInfo.wParam = wParam;
			sendInfo.lParam = lParam;			
		}
	}
	return CallNextHookEx(kconnnectHook, nCode, wParam, lParam);
}