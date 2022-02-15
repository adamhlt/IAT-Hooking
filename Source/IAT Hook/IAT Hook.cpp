#include <Windows.h>
#include <cstdio>

using MessageBoxPtr = int(WINAPI*)(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType);
MessageBoxPtr MessageBoxTest;

//MessageBoxA function hook.
int WINAPI MessageBoxHook(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType)
{
	printf("MessageBoxA have been called !\n");

	return MessageBoxTest(nullptr, "This function have been hooked !", "test", 0);
}

/**
 * Function to hook functions in the IAT. 
 * \param lpFunctionName : name of the function you want to hook.
 * \param lpFunction : pointer of the new function.
 * \param lpModuleName : name of the module.
 * \return : the pointer of the original function or nullptr if it failed.
 */
LPVOID IATHook(LPCSTR lpFunctionName, const LPVOID lpFunction, LPCSTR lpModuleName = nullptr)
{
	const HANDLE hModule = GetModuleHandleA(lpModuleName);
	const auto lpImageDOSHeader = (PIMAGE_DOS_HEADER)(hModule);
	if (lpImageDOSHeader == nullptr)
		return nullptr;

	const auto lpImageNtHeader = (PIMAGE_NT_HEADERS)((DWORD_PTR)lpImageDOSHeader + lpImageDOSHeader->e_lfanew);

	const IMAGE_DATA_DIRECTORY ImportDataDirectory = lpImageNtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
	auto lpImageImportDescriptor = (PIMAGE_IMPORT_DESCRIPTOR)((DWORD_PTR)hModule + ImportDataDirectory.VirtualAddress);

	while (lpImageImportDescriptor->Characteristics != 0)
	{
		auto lpImageOrgThunkData = (PIMAGE_THUNK_DATA)((DWORD_PTR)lpImageDOSHeader + lpImageImportDescriptor->OriginalFirstThunk);
		auto lpImageThunkData = (PIMAGE_THUNK_DATA)((DWORD_PTR)lpImageDOSHeader + lpImageImportDescriptor->FirstThunk);

		while (lpImageOrgThunkData->u1.AddressOfData != 0)
		{
			const auto lpImportData = (PIMAGE_IMPORT_BY_NAME)((DWORD_PTR)lpImageDOSHeader + lpImageOrgThunkData->u1.AddressOfData);

			if (strcmp(lpFunctionName, lpImportData->Name) == 0)
			{
				DWORD dwJunk = 0;
				MEMORY_BASIC_INFORMATION mbi;

				VirtualQuery(lpImageThunkData, &mbi, sizeof(MEMORY_BASIC_INFORMATION));
				if (!VirtualProtect(mbi.BaseAddress, mbi.RegionSize, PAGE_READWRITE, &mbi.Protect))
					return nullptr;

				const auto lpOrgFunction = (LPVOID)lpImageThunkData->u1.Function;

				#if defined _M_IX86
					lpImageThunkData->u1.Function = (DWORD_PTR)lpFunction;
				#elif defined _M_X64
					lpImageThunkData->u1.Function = (DWORD_PTR)lpFunction;
				#endif

				if (VirtualProtect(mbi.BaseAddress, mbi.RegionSize, mbi.Protect, &dwJunk))
					return lpOrgFunction;
			}

			lpImageThunkData++;
			lpImageOrgThunkData++;
		}

		lpImageImportDescriptor++;
	}

	return nullptr;
}

int main()
{
	//Hook the MessageBoxA function
	const LPVOID lpOrgFunction = IATHook("MessageBoxA", &MessageBoxHook);
	if (lpOrgFunction == nullptr)
		return -1;

	MessageBoxTest = (MessageBoxPtr)lpOrgFunction;

	MessageBoxA(nullptr, "This will never be displayed !", "test", 0);

	//Unhook the MessageBoxA function
	IATHook("MessageBoxA", lpOrgFunction);

	MessageBoxA(nullptr, "This function have been unhooked !", "test", 0);

	return 0;
}