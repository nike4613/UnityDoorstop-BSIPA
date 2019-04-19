#pragma once

#pragma warning( disable : 4267 6387 )

#include <windows.h>
#include "crt.h"

inline size_t get_module_path(HMODULE module, wchar_t **result, size_t *size, size_t free_space)
{
	size_t i = 0;
	size_t len, s;
	*result = NULL;
	do
	{
		if (*result != NULL)
			memfree(*result);
		i++;
		s = i * MAX_PATH + 1;
		*result = memalloc(sizeof(wchar_t) * s);
		len = GetModuleFileNameW(module, *result, s);
	}
	while (GetLastError() == ERROR_INSUFFICIENT_BUFFER && s - len >= free_space);

	if (size != NULL)
		*size = s;
	return len;
}

inline wchar_t *get_ini_entry(const wchar_t *config_file, const wchar_t *section, const wchar_t *key,
                              const wchar_t *default_val)
{
	size_t i = 0;
	size_t size, read;
	wchar_t *result = NULL;
	do
	{
		if (result != NULL)
			memfree(result);
		i++;
		size = i * MAX_PATH + 1;
		result = memalloc(sizeof(wchar_t) * size);
		read = GetPrivateProfileStringW(section, key, default_val, result, size, config_file);
	}
	while (read == size - 1);
	return result;
}

inline wchar_t *get_file_name_no_ext(wchar_t *str, size_t len)
{
	size_t ext_index = len;
	size_t i;
	for (i = len; i > 0; i--)
	{
		wchar_t c = *(str + i);
		if (c == L'.' && ext_index == len)
			ext_index = i;
		else if (c == L'\\')
			break;
	}

	size_t result_len = ext_index - i;
	wchar_t *result = memcalloc(sizeof(wchar_t) * result_len);
	wmemcpy(result, str + i + 1, result_len - 1);
	return result;
}
