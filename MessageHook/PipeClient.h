#pragma once
#include <Windows.h>

typedef struct WinMessageData
{
	WCHAR winMessage[50];
	int messageCode;
	WPARAM wParam;
	LPARAM lParam;
} WMData;

class ClientPipe
{
public:
	ClientPipe();
	~ClientPipe();

private:
	LPCWSTR pipeName = L"\\\\.\\pipe\\InjectionPipe";
	HANDLE namedPipe = INVALID_HANDLE_VALUE;
	WMData WinData;

public:
	BOOL ConnectServer();
	BOOL DisconnectServer();
	void SetWinData(LPWSTR _WinMessage, int _WinCode, WPARAM _wParam, LPARAM _lParam);
	void InitWMData();
	BOOL Send();
	BOOL Receive();
	HANDLE GetNamedPipe();
};