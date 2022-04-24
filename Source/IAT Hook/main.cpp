#include <cstdio>

#include "IAT Hook.h"

using MessageBoxPtr = int(WINAPI*)(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType);
MessageBoxPtr MessageBoxTest;

//MessageBoxA function hook.
int WINAPI MessageBoxHook(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType)
{
	printf("MessageBoxA have been called !\n");

	return MessageBoxTest(nullptr, "This function have been hooked !", "test", 0);
}

int main()
{
	//Hook the MessageBoxA function
	const LPVOID lpOrgFunction = IAT::Hook("user32.dll", "MessageBoxA", &MessageBoxHook);
	if (lpOrgFunction == nullptr)
		return -1;

	MessageBoxTest = (MessageBoxPtr)lpOrgFunction;

	MessageBoxA(nullptr, "This will never be displayed !", "test", 0);

	//Unhook the MessageBoxA function
	IAT::Hook("user32.dll", "MessageBoxA", lpOrgFunction);

	MessageBoxA(nullptr, "This function have been unhooked !", "test", 0);

	return 0;
}