```
                          _______  ______   __  __            __   _            
                         /  _/   |/_  __/  / / / /___  ____  / /__(_)___  ____ _ 
                         / // /| | / /    / /_/ / __ \/ __ \/ //_/ / __ \/ __ `/
                       _/ // ___ |/ /    / __  / /_/ / /_/ / ,< / / / / / /_/ / 
                      /___/_/  |_/_/    /_/ /_/\____/\____/_/|_/_/_/ /_/\__, /
                                                                       /____/ 
                                                                          
                                                                          
                                     IAT Hooking POC (x86 / x64)
                                    Hook functions through the IAT
```
<p align="center">
    <img src="https://img.shields.io/badge/language-C%2B%2B-%23f34b7d.svg?style=for-the-badge&logo=appveyor" alt="C++">
    <img src="https://img.shields.io/badge/platform-Windows-0078d7.svg?style=for-the-badge&logo=appveyor" alt="Windows">
    <img src="https://img.shields.io/badge/arch-x86-red.svg?style=for-the-badge&logo=appveyor" alt="x86">
    <img src="https://img.shields.io/badge/arch-x64-green.svg?style=for-the-badge&logo=appveyor" alt="x64">
</p>

## :open_book: Project Overview :

This project have been created to show how IAT hooking works. 

You can easily hook any functions in the IAT, you can also change the module you want to target.

> **Note** <br>
> This project can be compiled for x86 and x64 architecture.

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
