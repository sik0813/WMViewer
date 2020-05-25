#pragma once
class ClientPipe
{
public:
	ClientPipe();
	~ClientPipe();

private:
	typedef struct WinMessageData
	{
		WCHAR winMessage[50];
		int messageCode;
		WPARAM wParam;
		LPARAM lParam;
	} WMData;

	LPCWSTR PIPE = L"\\\\.\\pipe\\InjectionPipe";
	WMData WinData;

public:
	BOOL ConnectServer();
	BOOL Send();
	BOOL Receive();

};