#include "PipeServer.h"

PipeServer::PipeServer()
{
	ZeroMemory(&WinData, sizeof(WMData));
	WinData.messageCode = -1;
}

PipeServer::~PipeServer()
{
	CloseHandle(namedPipe);
	CloseHandle(ConnectClientHandle);
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
		20000,       // ��� Timeout �ð�
		&sa
	);
	if (INVALID_HANDLE_VALUE == namedPipe)
	{
		wprintf(L"CreateNamePipe error! \n");
		return FALSE;
	}

	successFunc = WaitNamedPipeW(pipeName, 2000);
	if (NULL == successFunc)
	{
		wprintf(L"WaitNamedPipeW error! \n");
		return FALSE;
	}

	ConnectClientHandle = (HANDLE)_beginthreadex(NULL, 0, &ConnectClientThread, this, NULL, NULL);
	return TRUE;
}

// ������ ����
BOOL PipeServer::ConnectClient()
{
	BOOL successFunc = FALSE;
	
	//������ Named Pipe�� �ڵ��� ������ �� ������ ���..
	// ConnectNamedPipe
	successFunc = WaitNamedPipeW(
		pipeName,//namedPipe,
		NULL);
	if (!successFunc)
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
	BOOL successFunc = ReadFile(namedPipe, &WinData, sizeof(WMData), &recvSize, NULL); // ����
	if (FALSE == successFunc)
	{
		wprintf(L"ReadFile Fail\n");
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
	returnString += L")";
	return TRUE;
}

BOOL PipeServer::GetexitThread()
{
	return exitThread;
}