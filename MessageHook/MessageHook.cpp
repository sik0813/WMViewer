// MessageHook.cpp : DLL ���� ���α׷��� ���� ������ �Լ��� �����մϴ�.
//

#include "stdafx.h"
#include "MessageHook.h"


// ������ ������ �����Դϴ�.
MESSAGEHOOK_API int nMessageHook=0;

// ������ �Լ��� �����Դϴ�.
MESSAGEHOOK_API int fnMessageHook(void)
{
	return 42;
}

// ������ Ŭ������ �������Դϴ�.
// Ŭ���� ���Ǹ� ������ MessageHook.h�� �����Ͻʽÿ�.
CMessageHook::CMessageHook()
{
	return;
}
