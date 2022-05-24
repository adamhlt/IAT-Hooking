![Banner](https://user-images.githubusercontent.com/48086737/170094284-7754fd04-fcb4-439c-b928-0cdc5bbfdd71.png)

# IAT Hooking

[![C++](https://img.shields.io/badge/language-C%2B%2B-%23f34b7d.svg?style=for-the-badge&logo=appveyor)](https://en.wikipedia.org/wiki/C%2B%2B) [![Windows](https://img.shields.io/badge/platform-Windows-0078d7.svg?style=for-the-badge&logo=appveyor)](https://en.wikipedia.org/wiki/Microsoft_Windows) [![x86](https://img.shields.io/badge/arch-x86-red.svg?style=for-the-badge&logo=appveyor)](https://en.wikipedia.org/wiki/X86) [![x64](https://img.shields.io/badge/arch-x64-green.svg?style=for-the-badge&logo=appveyor)](https://en.wikipedia.org/wiki/X64)

## :open_book: Project Overview :

This project have been created to show how IAT hooking works. 

You can easily hook any functions in the IAT, you can also change the module you want to target.

This project can be compiled for x86 and x64 architecture.

## :rocket: Getting Started

### Visual Studio :

1. Open the solution file (.sln).
2. Build the project in Release (x86 or x64)

Build for x86 / x64 (Debug and Realese).

## :test_tube: Example

#### MessageBoxA Hook

```cpp
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
```

https://user-images.githubusercontent.com/48086737/164978507-009e19fa-ecb8-4b09-a39b-0ba1bafddfc7.mp4
