#ifndef IATHOOK_H
#define IATHOOK_H

#include <Windows.h>

class IAT
{
public:
	static LPVOID Hook(LPCSTR lpModuleName, LPCSTR lpFunctionName, LPVOID lpFunction, LPCSTR lpTargetModuleName);
	static LPVOID Hook(LPCSTR lpModuleName, LPCSTR lpFunctionName, LPVOID lpFunction);
private:
	static LPVOID PatchImport(HMODULE hModule, LPCSTR lpModuleName, LPCSTR lpFunctionName, LPVOID lpFunction);
};

#endif
