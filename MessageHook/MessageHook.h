#ifdef MESSAGEHOOK_EXPORTS
#define MESSAGEHOOK_API __declspec(dllexport)
#else
#define MESSAGEHOOK_API __declspec(dllimport)
#endif

#ifdef __cplusplus
#define EXPORT extern "C" MESSAGEHOOK_API
#else
#define EXPORT MESSAGEHOOK_API
#endif

class MESSAGEHOOK_API Hook
{
public:
	Hook(HANDLE _dllHandle)
	{
		dllInstanceHandle = _dllHandle; // dll วฺต้
	}

public:
	void startHook(void);
	void stopHook(void);

private:
	HANDLE dllInstanceHandle;
};

EXPORT void startHook(void);
EXPORT void stopHook(void);
