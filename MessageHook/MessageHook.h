// ���� ifdef ����� DLL���� ���������ϴ� �۾��� ���� �� �ִ� ��ũ�θ� ����� 
// ǥ�� ����Դϴ�. �� DLL�� ��� �ִ� ������ ��� ����ٿ� ���ǵ� _EXPORTS ��ȣ��
// �����ϵǸ�, �ٸ� ������Ʈ������ �� ��ȣ�� ������ �� �����ϴ�.
// �̷��� �ϸ� �ҽ� ���Ͽ� �� ������ ��� �ִ� �ٸ� ��� ������Ʈ������ 
// MESSAGEHOOK_API �Լ��� DLL���� �������� ������ ����, �� DLL��
// �� DLL�� �ش� ��ũ�η� ���ǵ� ��ȣ�� ���������� ������ ���ϴ�.
#ifdef MESSAGEHOOK_EXPORTS
#define MESSAGEHOOK_API __declspec(dllexport)
#else
#define MESSAGEHOOK_API __declspec(dllimport)
#endif

// �� Ŭ������ MessageHook.dll���� ������ ���Դϴ�.
class MESSAGEHOOK_API CMessageHook {
public:
	CMessageHook(void);
	// TODO: ���⿡ �޼��带 �߰��մϴ�.
};

extern MESSAGEHOOK_API int nMessageHook;

MESSAGEHOOK_API int fnMessageHook(void);
