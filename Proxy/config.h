#pragma once

#include <windows.h>
#include <shellapi.h>
#include "winapi_util.h"
#include "assert_util.h"

#define CONFIG_NAME L"doorstop_config"
#define DEFAULT_TARGET_ASSEMBLY L"Doorstop.dll"
#define EXE_EXTENSION_LENGTH 4

BOOL enabled = FALSE;
wchar_t *targetAssembly = NULL;

#define STR_EQUAL(str1, str2) (lstrcmpiW(str1, str2) == 0)

inline void initConfigFile()
{
	enabled = TRUE;
	/*wchar_t* tgtAsm = L"Beat Saber_Data/Managed/IPA.Injector.dll";
	targetAssembly = memalloc(sizeof(wchar_t) * 41);
	memcpy(targetAssembly, tgtAsm, sizeof(wchar_t) * 41);*/
	targetAssembly = L"Beat Saber_Data/Managed/IPA.Injector.dll";
	/*if (GetFileAttributesW(CONFIG_NAME) == INVALID_FILE_ATTRIBUTES)
		return;

	const size_t len = GetFullPathNameW(CONFIG_NAME, 0, NULL, NULL);
	wchar_t *configPath = memalloc(sizeof(wchar_t) * len);
	GetFullPathNameW(CONFIG_NAME, len, configPath, NULL);

	LOG("Config path: %S\n", configPath);

	LPOFSTRUCT openParams = memcalloc(sizeof(OFSTRUCT));
	openParams->cBytes = sizeof(OFSTRUCT);
	openParams->fFixedDisk = 0;
	HFILE file = OpenFile(configPath, openParams, OF_READ);
	if (file == HFILE_ERROR) {
		enabled = FALSE;
		targetAssembly = NULL;
	}
	else
	{
		wchar_t enabledString[256] = L"enabled";
		//GetPrivateProfileStringW(L"UnityDoorstop", L"enabled", L"true", enabledString, 256, configPath);
		// TODO: proper checks for this

		if (STR_EQUAL(enabledString, L"enabled"))
			enabled = TRUE;
		else
			enabled = FALSE;

		LPDWORD fsize = memalloc(sizeof(DWORD));
		GetFileSize(file, fsize);

		targetAssembly = memalloc(sizeof(wchar_t) * *fsize);
		if (!ReadFile(file, targetAssembly, *fsize, fsize, NULL)) {
			enabled = FALSE;
			LOG("LastError: %d\n", GetLastError());
		}

		//targetAssembly = get_ini_entry(configPath, L"UnityDoorstop", L"targetAssembly", DEFAULT_TARGET_ASSEMBLY);

	}

	LOG("Config; Target assembly: %S\n", targetAssembly);

	memfree(configPath);*/
}

inline void initCmdArgs()
{
	wchar_t *args = GetCommandLineW();
	int argc = 0;
	wchar_t **argv = CommandLineToArgvW(args, &argc);

#define IS_ARGUMENT(arg_name) STR_EQUAL(arg, arg_name) && i < argc

	for (int i = 0; i < argc; i++)
	{
		wchar_t *arg = argv[i];
		if (IS_ARGUMENT(L"--doorstop-enable"))
		{
			wchar_t *par = argv[++i];

			if (STR_EQUAL(par, L"true"))
				enabled = TRUE;
			else if (STR_EQUAL(par, L"false"))
				enabled = FALSE;
		}
		else if (IS_ARGUMENT(L"--doorstop-target"))
		{
			if (targetAssembly != NULL)
				memfree(targetAssembly);
			const size_t len = wcslen(argv[i + 1]) + 1;
			targetAssembly = memalloc(sizeof(wchar_t) * len);
			lstrcpynW(targetAssembly, argv[++i], len);
			LOG("Args; Target assembly: %S\n", targetAssembly);
		}
	}

	LocalFree(argv);
}

inline void initEnvVars()
{
	if (GetEnvironmentVariableW(L"DOORSTOP_DISABLE", NULL, 0) != 0)
	{
		LOG("DOORSTOP_DISABLE is set! Disabling Doorstop!\n");
		enabled = FALSE;
	}
}

inline void loadConfig()
{
	initConfigFile();
	initCmdArgs();
	initEnvVars();
}

inline void cleanupConfig()
{
	/*if (targetAssembly != NULL)
		memfree(targetAssembly);*/
}
