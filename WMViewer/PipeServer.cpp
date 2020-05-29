#include "PipeServer.h"

PipeServer::PipeServer()
{
	ZeroMemory(&WinData, sizeof(WMData));
	WinData.messageCode = -1;
	eventHandle = CreateEventW(NULL, FALSE, FALSE, NULL);
	ZeroMemory(&op, sizeof(OVERLAPPED));
	op.hEvent = eventHandle;
}

PipeServer::~PipeServer()
{
	CloseHandle(namedPipe);
	CloseHandle(ConnectClientHandle);
	CloseHandle(eventHandle);
	namedPipe = INVALID_HANDLE_VALUE;
}

// ������ �����
BOOL PipeServer::CreateServer()
{
	wprintf(L"Run Server \n");

	SECURITY_DESCRIPTOR sd;
	BOOL successFunc = InitializeSecurityDescriptor(
		&sd,
		SECURITY_DESCRIPTOR_REVISION);
	if (!successFunc)
	{
		wprintf(L"InitializeSecurityDescriptor Fail \n");
		return -1;
	}

	successFunc = SetSecurityDescriptorDacl(
		&sd,
		TRUE, // TRUE ����° ���� ���
		NULL, // NULL allows all access to the objeclt
		FALSE);
	if (!successFunc)
	{
		wprintf(L"SetSecurityDescriptorDacl Fail \n");
		return -1;
	}

	SECURITY_ATTRIBUTES sa;
	memset(&sa, 0, sizeof(SECURITY_ATTRIBUTES));
	sa.bInheritHandle = TRUE;
	sa.lpSecurityDescriptor = &sd;

	namedPipe = CreateNamedPipe(
		pipeName,
		PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,
		PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
		PIPE_UNLIMITED_INSTANCES,
		0,
		0,
		1000,       // Client Timeout �ð�
		&sa
	);
	if (INVALID_HANDLE_VALUE == namedPipe)
	{
		wprintf(L"CreateNamePipe error! \n");
		return FALSE;
	}

	ConnectClientHandle = (HANDLE)_beginthreadex(NULL, 0, &ConnectClientThread, this, NULL, NULL);
	return TRUE;
}

// ������ ����
BOOL PipeServer::ConnectClient()
{
	BOOL successFunc = FALSE;
	HANDLE eventHandle = CreateEventW(
		NULL,    // default security attribute 
		TRUE,    // manual-reset event 
		TRUE,    // initial state = signaled 
		NULL);   // unnamed event object 
	
	//������ Named Pipe�� �ڵ��� ������ �� ������ ���..
	// WaitNamedPipeW
	successFunc = ConnectNamedPipe(
		namedPipe,
		NULL);
	if (FALSE == successFunc)
	{
		//CloseHandle(namedPipe);
		//namedPipe = INVALID_HANDLE_VALUE;
		return FALSE;
	}	

	ConnectFlag = TRUE;
	return TRUE;
}

// ���� ��� ������ �Լ�
UINT WINAPI PipeServer::ConnectClientThread(LPVOID arg)
{
	BOOL successFunc = FALSE;
	while (1)
	{
		successFunc = ((PipeServer*)arg)->ConnectClient();
		if (TRUE == successFunc || TRUE == ((PipeServer*)arg)->GetexitThread())
		{
			break;
		}
	}
	
	return 0;
}

// ������ ���� ����
BOOL PipeServer::DisconnectClient()
{
	BOOL successFunc = FALSE;
	if (INVALID_HANDLE_VALUE != ConnectClientHandle)
	{
		exitThread = TRUE;
		WaitForSingleObject(ConnectClientHandle, INFINITE);
		ConnectClientHandle = INVALID_HANDLE_VALUE;
		exitThread = FALSE;
		CloseHandle(ConnectClientHandle);
	}

	if (INVALID_HANDLE_VALUE != namedPipe)
	{
		successFunc = DisconnectNamedPipe(namedPipe);
		if (!successFunc)
		{
			wprintf(L"DisconnectNamedPipe failed with %d.\n", GetLastError());
			return FALSE;
		}
	}

	wprintf(L"Send & Disconnect Done\n");
	ConnectFlag = FALSE;

	return TRUE;
}

// ������ �۽�
BOOL PipeServer::Send()
{
	if (FALSE == ConnectFlag)
	{
		wprintf(L"Pipe Not Connnect\n");
		return FALSE;
	}

	DWORD sendSize = 0;
	BOOL successFunc = WriteFile(namedPipe, &WinData, sizeof(WMData), &sendSize, NULL); // ���� Message ����
	if (FALSE == successFunc)
	{
		wprintf(L"WriteFile Fail\n");
		return FALSE;
	}
	return TRUE;
}

// ������ ����
BOOL PipeServer::Receive()
{
	if (FALSE == ConnectFlag)
	{
		wprintf(L"Pipe Not Connnect\n");
		return FALSE;
	}

	DWORD recvSize = 0;
	ZeroMemory(&WinData, sizeof(WMData)); // ������ ���� �� �ʱ�ȭ
	WinData.messageCode = -1;

	//PIPE Read
	BOOL successFunc = ReadFile(namedPipe, &WinData, sizeof(WMData), &recvSize, &op); // ����
	if (GetLastError() == ERROR_IO_PENDING)
	{
		//���� �ð� �޽��� ���
		DWORD waitRead = WaitForSingleObject(eventHandle, 50);
		if (waitRead != WAIT_OBJECT_0)
		{
			return FALSE;
		}

		if (FALSE == GetOverlappedResult(eventHandle, &op, &recvSize, TRUE))
		{
			return FALSE;
		}
	}
	

	return TRUE;
}

BOOL PipeServer::GetWinData(std::wstring& returnString)
{
	if (FALSE == ConnectFlag)
	{
		wprintf(L"Pipe Not Connnect\n");
		return FALSE;
	}
	returnString = std::wstring(WinData.winMessage);
	returnString += L"(";
	returnString += std::to_wstring(WinData.messageCode);
	returnString += L")      wParam:";
	returnString += std::to_wstring(WinData.wParam);
	returnString += L"   |    lParam:";
	returnString += std::to_wstring(WinData.lParam);;
	return TRUE;
}

BOOL PipeServer::GetexitThread()
{
	return exitThread;
}