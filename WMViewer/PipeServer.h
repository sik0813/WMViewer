#pragma
#include <Windows.h>
#include <stdio.h>
#include <process.h>

class PipeServer
{
public:
	PipeServer();
	~PipeServer();

private:
	typedef struct WinMessageData
	{
		WCHAR winMessage[50];
		int messageCode;
		WPARAM wParam;
		LPARAM lParam;
	} WMData;

	LPCWSTR pipeName = L"\\\\.\\pipe\\InjectionPipe";
	HANDLE namedPipe = INVALID_HANDLE_VALUE;
	BOOL ConnectFlag = FALSE;
	HANDLE ConnectClientHandle = INVALID_HANDLE_VALUE;
	WMData WinData;

public:
	BOOL CreateServer();
	BOOL ConnectClient();
	static UINT WINAPI ConnectClientThread(LPVOID arg);
	BOOL DisconnectClient();
	void Send();
	void Receive();
	WMData GetWinDate()
	{
		return WinData;
	}
};