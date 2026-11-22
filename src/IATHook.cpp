#include "IATHook.h"

#include <cstring>

/**
 * Function to hook a function in the IAT of a module.
 * \param hModule : base address of the module whose IAT is patched.
 * \param lpModuleName : name of the module wich contains the function you want to hook.
 * \param lpFunctionName : name of the function you want to hook.
 * \param lpFunction : pointer of the new function.
 * \return : the pointer of the original function or nullptr if it failed.
 */
LPVOID IAT::PatchImport(HMODULE hModule, LPCSTR lpModuleName, LPCSTR lpFunctionName, LPVOID lpFunction)
{
	if (hModule == nullptr || lpModuleName == nullptr || lpFunctionName == nullptr)
		return nullptr;

	const auto lpBase = (DWORD_PTR)hModule;

	const auto lpImageDOSHeader = (PIMAGE_DOS_HEADER)lpBase;
	if (lpImageDOSHeader->e_magic != IMAGE_DOS_SIGNATURE)
		return nullptr;

	const auto lpImageNtHeader = (PIMAGE_NT_HEADERS)(lpBase + lpImageDOSHeader->e_lfanew);
	if (lpImageNtHeader->Signature != IMAGE_NT_SIGNATURE)
		return nullptr;

	const IMAGE_DATA_DIRECTORY ImportDataDirectory = lpImageNtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
	if (ImportDataDirectory.VirtualAddress == 0)
		return nullptr;

	auto lpImageImportDescriptor = (PIMAGE_IMPORT_DESCRIPTOR)(lpBase + ImportDataDirectory.VirtualAddress);

	for (; lpImageImportDescriptor->Name != 0; lpImageImportDescriptor++)
	{
		const auto lpCurrentModuleName = (LPCSTR)(lpBase + lpImageImportDescriptor->Name);
		if (_stricmp(lpCurrentModuleName, lpModuleName) != 0)
			continue;

		// Bound imports can leave OriginalFirstThunk empty: fall back to FirstThunk.
		const DWORD dwNameTableRva = lpImageImportDescriptor->OriginalFirstThunk != 0
			? lpImageImportDescriptor->OriginalFirstThunk
			: lpImageImportDescriptor->FirstThunk;

		auto lpImageOrgThunkData = (PIMAGE_THUNK_DATA)(lpBase + dwNameTableRva);
		auto lpImageThunkData = (PIMAGE_THUNK_DATA)(lpBase + lpImageImportDescriptor->FirstThunk);

		for (; lpImageOrgThunkData->u1.AddressOfData != 0; lpImageThunkData++, lpImageOrgThunkData++)
		{
			// Skip functions imported by ordinal.
			if (lpImageOrgThunkData->u1.Ordinal & IMAGE_ORDINAL_FLAG)
				continue;

			const auto lpImportData = (PIMAGE_IMPORT_BY_NAME)(lpBase + lpImageOrgThunkData->u1.AddressOfData);
			if (strcmp(lpFunctionName, lpImportData->Name) != 0)
				continue;

			// Only unprotect the single pointer that is patched.
			DWORD dwOldProtect = 0;
			if (!VirtualProtect(&lpImageThunkData->u1.Function, sizeof(LPVOID), PAGE_READWRITE, &dwOldProtect))
				return nullptr;

			const auto lpOrgFunction = (LPVOID)lpImageThunkData->u1.Function;
			lpImageThunkData->u1.Function = (ULONG_PTR)lpFunction;

			// The hook is applied at this point: a failed restore must not be reported as a failed hook.
			DWORD dwJunk = 0;
			VirtualProtect(&lpImageThunkData->u1.Function, sizeof(LPVOID), dwOldProtect, &dwJunk);

			return lpOrgFunction;
		}
	}

	return nullptr;
}

/**
 * Function to hook functions in the IAT of a specified module.
 * \param lpModuleName : name of the module wich contains the function you want to hook.
 * \param lpFunctionName : name of the function you want to hook.
 * \param lpFunction : pointer of the new function.
 * \param lpTargetModuleName : name of the module you want to target.
 * \return : the pointer of the original function or nullptr if it failed.
 */
LPVOID IAT::Hook(LPCSTR lpModuleName, LPCSTR lpFunctionName, LPVOID lpFunction, LPCSTR lpTargetModuleName)
{
	return PatchImport(GetModuleHandleA(lpTargetModuleName), lpModuleName, lpFunctionName, lpFunction);
}

/**
 * Function to hook functions in the IAT of a the main module of the process.
 * \param lpModuleName : name of the module wich contains the function.
 * \param lpFunctionName : name of the function you want to hook.
 * \param lpFunction : pointer of the new function.
 * \return : the pointer of the original function or nullptr if it failed.
 */
LPVOID IAT::Hook(LPCSTR lpModuleName, LPCSTR lpFunctionName, LPVOID lpFunction)
{
	return PatchImport(GetModuleHandleA(nullptr), lpModuleName, lpFunctionName, lpFunction);
}
