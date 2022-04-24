#ifndef IAT_HOOK_H
#define IAT_HOOK_H

#include <Windows.h>

class IAT
{
public:
	static LPVOID Hook(LPCSTR lpFunctionName, const LPVOID lpFunction, LPCSTR lpModuleName);
	static LPVOID Hook(LPCSTR lpFunctionName, const LPVOID lpFunction);
private:
	static LPVOID GetCurrentProcessModule();
};

#endif