#include "WMViewer.h"
#include "HookLibrary.h"
#include "PipeServer.h"

HookLibrary *AllHookLibrary;
PipeServer *Server;
BOOL EndShow = FALSE;

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nShowCmd)
{
	LocalAlloc(LOCALE_ALL, 0);
	DialogBoxW(hInstance, MAKEINTRESOURCEW(IDD_DIALOG), NULL, RunProc);

	return 0;
}

INT_PTR WINAPI RunProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
		HANDLE_WM_INITDIALOG(hwnd, wParam, lParam, OnInitDialog);
		break;
	case WM_COMMAND:
		HANDLE_WM_COMMAND(hwnd, wParam, lParam, OnCommand);
		break;
	}

	return FALSE;
}

void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	BOOL SuccessFuc = FALSE;
	switch (id)
	{
	case IDCANCEL:
	case IDOK:
		AllHookLibrary->~HookLibrary();
		delete AllHookLibrary;
		delete Server;
		EndDialog(hwnd, id);
		break;

	case IDC_REFRESH:
		ShowProcessList(hwnd);
		break;

	case IDC_START:
		SuccessFuc = AllHookLibrary->StartHook();
		if (TRUE == SuccessFuc)
		{
			Server->ConnectClient();
			MessageBoxW(hwnd, L"Start Hooking", L"", MB_OK);
		}

		break;

	case IDC_STOP:
		SuccessFuc = AllHookLibrary->StopHook();
		if (TRUE == SuccessFuc)
		{
			Server->DisconnectClient();
			MessageBoxW(hwnd, L"Stop Hooking", L"", MB_OK);
		}

		break;
	}
}

// 다이얼로그 초기화
BOOL OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	BOOL successFunc = FALSE;
	//successFunc = ShowProcessList(hwnd);
	AllHookLibrary = new HookLibrary();
	Server = new PipeServer();

	return TRUE;
}

void PrintProcessNameAndID(DWORD processID, HWND hwnd)
{
	WCHAR szProcessName[MAX_PATH] = { 0, };
	// Get a handle to the process.

	HANDLE hProcess = OpenProcess(
		PROCESS_QUERY_INFORMATION |	PROCESS_VM_READ,
		FALSE, 
		processID);

	// Get the process name.
	if (NULL != hProcess)
	{
		DWORD successFunc = GetModuleFileNameEx( // GetModuleBaseNameW : 실행 파일명
			hProcess, 
			0, 
			szProcessName, 
			sizeof(szProcessName) / sizeof(WCHAR)); 
		if (NULL != successFunc)
		{
			std::wstring addItem(wcsrchr(szProcessName, L'\\') + 1);
			addItem += L"(PID: ";
			addItem += std::to_wstring(processID);
			addItem += L")";
			ListBox_AddString(hwnd, addItem.c_str());
		}
	}
	CloseHandle(hProcess);
}

BOOL ShowProcessList(HWND hwnd)
{
	// Get the list of process identifiers.
	DWORD aProcesses[1024] = { 0, }, listByte = 0;
	memset(&aProcesses, 0, sizeof(aProcesses));

	BOOL sucessFunc = EnumProcesses(aProcesses, sizeof(aProcesses), &listByte);
	if (NULL == sucessFunc)
	{
		return FALSE;
	}

	DWORD listLen = listByte / sizeof(DWORD);
	HWND listHwnd = GetDlgItem(hwnd, IDC_PIDLIST);
	for (unsigned int i = 0; i < listLen; i++)
	{
		if (aProcesses[i] != 0)
		{
			PrintProcessNameAndID(aProcesses[i], listHwnd);
		}
	}
	return TRUE;
}

UINT WINAPI ShowWinMessage(LPVOID arg)
{
	while (1)
	{
		if (TRUE == EndShow)
		{
			break;
		}
	}
	EndShow = FALSE;
	return 0;
}