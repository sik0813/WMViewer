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

// 파이프 만들기
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
		TRUE, // TRUE 세번째 인자 사용
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
		20000,       // 대기 Timeout 시간
		&sa
	);
	if (INVALID_HANDLE_VALUE == namedPipe)
	{
		wprintf(L"CreateNamePipe error! \n");
		return -1;
	}

	ConnectClientHandle = (HANDLE)_beginthreadex(NULL, 0, &ConnectClientThread, this, NULL, NULL);
	return TRUE;
}

// 파이프 연결
BOOL PipeServer::ConnectClient()
{
	BOOL successFunc = FALSE;
	
	//생성한 Named Pipe의 핸들을 누군가 얻어갈 때까지 대기..
	successFunc = ConnectNamedPipe(
		namedPipe,
		NULL);
	if (!successFunc)
	{
		CloseHandle(namedPipe);
		namedPipe = INVALID_HANDLE_VALUE;
		return FALSE;
	}

	ConnectFlag = TRUE;
	return TRUE;
}

// 연결 대기 스레드 함수
UINT WINAPI PipeServer::ConnectClientThread(LPVOID arg)
{
	((PipeServer*)arg)->ConnectClient();
	return 0;
}

// 파이프 연결 해제
BOOL PipeServer::DisconnectClient()
{
	BOOL successFunc = FALSE;
	successFunc = DisconnectNamedPipe(namedPipe);
	if (!successFunc)
	{
		wprintf(L"DisconnectNamedPipe failed with %d.\n", GetLastError());
		return FALSE;
	}

	wprintf(L"Send & Disconnect Done\n");
	CloseHandle(ConnectClientHandle);
	ConnectFlag = FALSE;
	
	return TRUE;
}

// 데이터 송신
void PipeServer::Send()
{
	if (FALSE == ConnectFlag)
	{
		wprintf(L"Pipe Not Connnect\n");
		return ;
	}

	DWORD sendSize = 0;
	BOOL successFunc = WriteFile(namedPipe, &WinData, sizeof(WMData), &sendSize, NULL); // 동일 Message 전송
	if (FALSE == successFunc)
	{
		wprintf(L"WriteFile Fail\n");
	}
	return;
}

// 데이터 수신
void PipeServer::Receive()
{
	if (FALSE == ConnectFlag)
	{
		wprintf(L"Pipe Not Connnect\n");
		return;
	}

	DWORD recvSize = 0;
	ZeroMemory(&WinData, sizeof(WMData)); // 데이터 수신 전 초기화
	WinData.messageCode = -1;
	BOOL successFunc = ReadFile(namedPipe, &WinData, sizeof(WMData), &recvSize, NULL); // 수신
	if (FALSE == successFunc)
	{
		wprintf(L"ReadFile Fail\n");
	}
	return;
}