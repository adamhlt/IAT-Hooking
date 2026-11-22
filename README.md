<div align="center">

# IAT Hooking

**Hook functions through the Import Address Table, in x86 and x64.**

![Language](https://img.shields.io/badge/C%2B%2B-16171C?logo=cplusplus)
![Platform](https://img.shields.io/badge/Windows-16171C)
![Arch](https://img.shields.io/badge/x86%20%7C%20x64-16171C)
[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-16171C)](LICENSE)

</div>

---

A small C++ static library that shows how IAT hooking works. It patches the import table of a module so that calls to an imported function are redirected to your own function, and gives you back the original pointer so you can call it or restore it.

It is meant for reverse engineering and learning how the PE import mechanism works.

https://user-images.githubusercontent.com/48086737/164978507-009e19fa-ecb8-4b09-a39b-0ba1bafddfc7.mp4

## Features

**Hook**
- Hook any function imported by name, in the main module or in any loaded module
- Get the original function pointer back from `IAT::Hook`
- Unhook by calling `IAT::Hook` again with the original pointer
- Works in x86 and x64

**Safe patching**
- Only the patched pointer is unprotected, not the whole memory region
- Validates the DOS and NT headers and the import directory before reading
- Functions imported by ordinal are skipped

**Try it**
- `examples/MessageBoxHook` hooks `MessageBoxA` and then unhooks it

## Usage

```cpp
using MessageBoxPtr = int(WINAPI*)(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType);
MessageBoxPtr MessageBoxTest;

// MessageBoxA function hook.
int WINAPI MessageBoxHook(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType)
{
	printf("MessageBoxA have been called !\n");

	return MessageBoxTest(nullptr, "This function have been hooked !", "test", 0);
}

int main()
{
	// Hook the MessageBoxA function
	const LPVOID lpOrgFunction = IAT::Hook("user32.dll", "MessageBoxA", &MessageBoxHook);
	if (lpOrgFunction == nullptr)
		return -1;

	MessageBoxTest = (MessageBoxPtr)lpOrgFunction;

	MessageBoxA(nullptr, "This will never be displayed !", "test", 0);

	// Unhook the MessageBoxA function
	IAT::Hook("user32.dll", "MessageBoxA", lpOrgFunction);

	MessageBoxA(nullptr, "This function have been unhooked !", "test", 0);

	return 0;
}
```

To target another module than the main one, pass its name as a fourth argument:

```cpp
IAT::Hook("user32.dll", "MessageBoxA", &MessageBoxHook, "target.dll");
```

## Build

1. Open `IATHook.sln` in Visual Studio.
2. Build the `MessageBoxHook` project in Release or Debug, x86 or x64.

- **Library:** `MessageBoxHook` references the `IATHook` static library, which is built automatically.
- **Include path:** to use the library in your own project, add `include/` to the include directories, link `IATHook.lib` and include `IATHook.h`.

## License

This project is licensed under the [GNU GPL v3](LICENSE).
