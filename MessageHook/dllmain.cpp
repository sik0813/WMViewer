// dllmain.cpp : DLL 응용 프로그램의 진입점을 정의합니다.
#include "stdafx.h"
#include "MessageHook.h"
#include <Psapi.h>
#include "PipeClient.h"
#include <strsafe.h>

HINSTANCE kdllInstanceHandle = NULL;
static HHOOK kconnnectHook = NULL;
ClientPipe *Client = NULL;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	WCHAR szProcessName[MAX_PATH] = { 0, };
	DWORD successFunc = GetModuleFileName(NULL, szProcessName, sizeof(szProcessName) / sizeof(WCHAR));
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		kdllInstanceHandle = (HINSTANCE)hModule;
		if (NULL != wcsrchr(szProcessName, L'\\'))
		{
			if (0 == wcscmp(L"notepad.exe", wcsrchr(szProcessName, L'\\') + 1))
			{
				Client = new ClientPipe();
			}
		}
		
		break;

	case DLL_THREAD_ATTACH:
		break;

	case DLL_THREAD_DETACH:
		break;

	case DLL_PROCESS_DETACH:
		if (NULL != wcsrchr(szProcessName, L'\\'))
		{
			if (0 != wcscmp(L"notepad.exe", wcsrchr(szProcessName, L'\\') + 1))
			{
				delete Client;
			}
		}
		break;
	}
	return TRUE;
}

EXPORT void StartHook(DWORD threadID)
{
	kconnnectHook = SetWindowsHookExW(WH_CALLWNDPROC, (HOOKPROC)GetMsgProc, kdllInstanceHandle, threadID);
	return;
}


EXPORT void StopHook(void)
{
	UnhookWindowsHookEx(kconnnectHook);
	return;
}

LRESULT WINAPI GetMsgProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	WCHAR szProcessName[MAX_PATH] = { 0, };
	HANDLE thisProcess = GetCurrentProcess();
	DWORD successFunc = GetModuleFileNameEx(thisProcess, 0, szProcessName, sizeof(szProcessName) / sizeof(WCHAR));

	if (nCode < 0)
		return CallNextHookEx(kconnnectHook, nCode, wParam, lParam);

	LPWSTR comProcessName = wcsrchr(szProcessName, L'\\');
	if (NULL != comProcessName)
	{
		if (0 == wcscmp(L"notepad.exe", comProcessName + 1))
		{
			if (INVALID_HANDLE_VALUE == Client->GetNamedPipe())
			{
				Client->ConnectServer();
			}

			LPMSG senMSG = (LPMSG)lParam;
			switch (nCode)
			{
			case HC_ACTION:
				switch (wParam)
				{
				case PM_REMOVE:
				case PM_NOREMOVE:					
					if (NULL != wmTranslation[senMSG->message] && senMSG->message > 0)
					{
						Client->SetWinData(wmTranslation[senMSG->message], senMSG->message, NULL, NULL);
						Client->Send();
					}
					break;

				default:
					break;
				}
				break;

			default:
				break;
			}
		}
	}

	return CallNextHookEx(kconnnectHook, nCode, wParam, lParam);
}

ClientPipe::ClientPipe()
{
	ZeroMemory(&WinData, sizeof(WMData));
	WinData.messageCode = -1;
}

 ClientPipe::~ClientPipe()
{

}

BOOL ClientPipe::ConnectServer()
{
	// 서버에서 생성한 파이프 이름으로 핸들 열기 CallNamedPipe
	namedPipe = CreateFile(
		pipeName,
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		0,
		NULL);
	if (INVALID_HANDLE_VALUE == namedPipe)
	{
		wprintf(L"CreateFile error! \n");
		return FALSE;
	}

	DWORD pipeMode = PIPE_READMODE_MESSAGE | PIPE_WAIT;
	BOOL sucessFunc = SetNamedPipeHandleState(
		namedPipe,
		&pipeMode,
		NULL,
		NULL);
	if (!sucessFunc)
	{
		wprintf(L"SetNamedPipeHandleState error! \n");
		CloseHandle(namedPipe);
		namedPipe = INVALID_HANDLE_VALUE;
		return FALSE;
	}

	return TRUE;
}

BOOL ClientPipe::DisconnectServer()
{
	BOOL sucessFunc = CloseHandle(namedPipe);
	namedPipe = INVALID_HANDLE_VALUE;
	if (!sucessFunc)
	{
		wprintf(L"CloseHandle error! \n");
		return FALSE;
	}
	return TRUE;
}

void ClientPipe::SetWinData(LPWSTR _WinMessage, int _WinCode, WPARAM _wParam, LPARAM _lParam)
{
	StringCchCopyW(WinData.winMessage, wcslen(_WinMessage) + 1, _WinMessage);
	WinData.messageCode = _WinCode;
	WinData.wParam = _wParam;
	WinData.lParam = _lParam;
}

BOOL ClientPipe::Send()
{
	DWORD sendSize = 0;
	BOOL successFunc = WriteFile(
		namedPipe,
		&WinData,
		sizeof(WMData),
		&sendSize,
		NULL);
	if (!successFunc)
	{
		//MessageBoxW((HWND)kdllInstanceHandle, L"Send error", L"", MB_OK);
		wprintf(L"Send error! \n");
		return FALSE;
	}

	ZeroMemory(&WinData, sizeof(WMData));
	return TRUE;
}

BOOL ClientPipe::Receive()
{
	DWORD recvSize = 0;
	BOOL successFunc = ReadFile(
		namedPipe,
		&WinData,
		sizeof(WMData),
		&recvSize,
		NULL);
	if (!successFunc)
	{
		wprintf(L"Receive error! \n");
		return FALSE;
	}

	FlushFileBuffers(namedPipe);

	return TRUE;
}

HANDLE ClientPipe::GetNamedPipe()
{
	return namedPipe;
}
