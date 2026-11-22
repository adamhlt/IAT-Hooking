<div align="center">

# IAT Hooking

**Hook functions through the Import Address Table, in x86 and x64.**

<img src="https://img.shields.io/badge/language-C%2B%2B-%23f34b7d.svg?style=for-the-badge&logo=appveyor" alt="C++">
<img src="https://img.shields.io/badge/platform-Windows-0078d7.svg?style=for-the-badge&logo=appveyor" alt="Windows">
<img src="https://img.shields.io/badge/arch-x86-red.svg?style=for-the-badge&logo=appveyor" alt="x86">
<img src="https://img.shields.io/badge/arch-x64-green.svg?style=for-the-badge&logo=appveyor" alt="x64">
<a href="LICENSE"><img src="https://img.shields.io/badge/license-GPLv3-blue.svg?style=for-the-badge" alt="GPLv3"></a>

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

## Project layout

```
include/                  public header (IATHook.h)
src/                      library implementation (IATHook.cpp)
IATHook/                  static library project
examples/MessageBoxHook/  demo hooking MessageBoxA
```

## License

This project is licensed under the [GNU GPL v3](LICENSE).
