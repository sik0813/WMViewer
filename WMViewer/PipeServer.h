#pragma
#include <Windows.h>
#include <stdio.h>
#include <process.h>
#include <string>

typedef struct WinMessageData
{
	WCHAR winMessage[50];
	int messageCode;
	WPARAM wParam;
	LPARAM lParam;
} WMData;

class PipeServer
{
public:
	PipeServer();
	~PipeServer();

private:
	LPCWSTR pipeName = L"\\\\.\\pipe\\InjectionPipe";
	HANDLE namedPipe = INVALID_HANDLE_VALUE;
	BOOL ConnectFlag = FALSE;
	HANDLE ConnectClientHandle = INVALID_HANDLE_VALUE;
	HANDLE eventHandle;
	OVERLAPPED op;
	WMData WinData;
	BOOL exitThread = FALSE;
public:
	BOOL CreateServer();
	BOOL ConnectClient();
	static UINT WINAPI ConnectClientThread(LPVOID arg);
	BOOL DisconnectClient();
	BOOL Send();
	BOOL Receive();
	BOOL GetWinData(std::wstring& returnString);
	BOOL GetexitThread();
};