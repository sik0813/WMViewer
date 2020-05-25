#include <Windows.h>
#include <windowsx.h>
#include <Psapi.h>
#include <iostream>
#include <string>
#include <process.h>
#include "resource.h"


INT_PTR WINAPI RunProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
BOOL ShowProcessList(HWND hwnd);
void PrintProcessNameAndID(DWORD processID, HWND hwnd);
DWORD FindTargetThread(DWORD PID);
UINT WINAPI ShowWinMessage(LPVOID arg);