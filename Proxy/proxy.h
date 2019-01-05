/*
 * Proxy.h -- Definitions for proxy-related functionality
 * 
 * The proxy works roughly as follows:
 * - We define our exports in proxy.c (computer generated)
 * - loadProxy initializes the proxy:
 *     1. Look up the name of this DLL
 *     2. Find the original DLL with the same name
 *     3. Load the original DLL
 *     4. Load all functions into originalFunctions array
 *     
 * For more information, refer to proxy.c 
 */

#pragma once

#pragma warning( disable : 4267 )

#include <Windows.h>
#include <Shlwapi.h>
#include "assert_util.h"
#include <crtdbg.h>

#define ALT_POSTFIX L"_alt.dll"
#define DLL_POSTFIX L".dll"

extern FARPROC originalFunctions[];
extern void loadFunctions(HMODULE dll);

// Load the proxy functions into memory
inline void loadProxy(wchar_t *moduleName)
{
	size_t module_name_len = wcslen(moduleName);

	size_t alt_name_len = module_name_len + STR_LEN(ALT_POSTFIX);
	wchar_t *alt_name = memalloc(sizeof(wchar_t) * alt_name_len);
	wmemcpy(alt_name, moduleName, module_name_len);
	wmemcpy(alt_name + module_name_len, ALT_POSTFIX, STR_LEN(ALT_POSTFIX));

	wchar_t *dll_path = NULL; // The final DLL path

	const int alt_full_path_len = GetFullPathNameW(alt_name, 0, NULL, NULL);
	wchar_t *alt_full_path = memalloc(sizeof(wchar_t) * alt_full_path_len);
	GetFullPathNameW(alt_name, alt_full_path_len, alt_full_path, NULL);
	memfree(alt_name);

	LOG("Looking for original DLL from %S\n", alt_full_path);

	// Try to look for the alternative first in the same directory.
	HMODULE handle = LoadLibrary(alt_full_path);

	if (handle == NULL)
	{
		size_t system_dir_len = GetSystemDirectoryW(NULL, 0);
		dll_path = memalloc(sizeof(wchar_t) * (system_dir_len + module_name_len + STR_LEN(DLL_POSTFIX)));
		_ASSERTE(dll_path != nullptr);
		GetSystemDirectoryW(dll_path, system_dir_len);
		dll_path[system_dir_len - 1] = L'\\';
		wmemcpy(dll_path + system_dir_len, moduleName, module_name_len);
		wmemcpy(dll_path + system_dir_len + module_name_len, DLL_POSTFIX, STR_LEN(DLL_POSTFIX));

		LOG("Looking for original DLL from %S\n", dll_path);

		handle = LoadLibraryW(dll_path);
	}

	ASSERT_F(handle != NULL, L"Unable to load the original %s.dll (looked from system directory and from %s_alt.dll)!",
		moduleName, moduleName);

	memfree(alt_full_path);
	memfree(dll_path);

	loadFunctions(handle);
}
