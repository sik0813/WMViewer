// MessageHook.cpp : DLL 응용 프로그램을 위해 내보낸 함수를 정의합니다.
//

#include "stdafx.h"
#include "MessageHook.h"


// 내보낸 변수의 예제입니다.
MESSAGEHOOK_API int nMessageHook=0;

// 내보낸 함수의 예제입니다.
MESSAGEHOOK_API int fnMessageHook(void)
{
	return 42;
}

// 내보낸 클래스의 생성자입니다.
// 클래스 정의를 보려면 MessageHook.h를 참조하십시오.
CMessageHook::CMessageHook()
{
	return;
}
