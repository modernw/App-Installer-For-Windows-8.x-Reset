#pragma once
#include <Windows.h>
#include <string>
#include <cstdlib>
#include "filepath.h"
void SetWebBrowserEmulation ()
{
	std::wstring instname = GetCurrentProgramNameW ();
	BOOL isWow64 = FALSE;
	IsWow64Process (GetCurrentProcess (), &isWow64);
	HKEY hKey;
	LPCWSTR keyPath = isWow64
		? L"SOFTWARE\\WOW6432Node\\Microsoft\\Internet Explorer\\Main\\FeatureControl\\FEATURE_BROWSER_EMULATION"
		: L"SOFTWARE\\Microsoft\\Internet Explorer\\Main\\FeatureControl\\FEATURE_BROWSER_EMULATION";
	LONG result = RegOpenKeyExW (HKEY_CURRENT_USER, keyPath, 0, KEY_WRITE, &hKey);
	if (result == ERROR_SUCCESS)
	{
		DWORD value = 11001;
		RegSetValueExW (hKey, instname.c_str (), 0, REG_DWORD, reinterpret_cast <const BYTE*> (&value), sizeof (value));
		RegCloseKey (hKey);
	}
}
// 返回系统安装的 Internet Explorer 主版本号（如 8、9、10、11）
int GetInternetExplorerVersionMajor ()
{
	HKEY hKey;
	LPCWSTR IEKeyPath = L"SOFTWARE\\Microsoft\\Internet Explorer";
	if (RegOpenKeyExW (HKEY_LOCAL_MACHINE, IEKeyPath, 0, KEY_READ, &hKey) != ERROR_SUCCESS) return 0;
	WCHAR buffer [128] = {0};
	DWORD bufferSize = sizeof (buffer);
	DWORD type = 0;
	std::wstring versionStr;
	if (RegQueryValueExW (hKey, L"svcVersion", NULL, &type, (LPBYTE)buffer, &bufferSize) == ERROR_SUCCESS) versionStr = buffer;
	else
	{
		bufferSize = sizeof (buffer);
		if (RegQueryValueExW (hKey, L"Version", NULL, &type, (LPBYTE)buffer, &bufferSize) == ERROR_SUCCESS) versionStr = buffer;
	}
	RegCloseKey (hKey);
	if (versionStr.empty ()) return 0;
	int major = 0;
	swscanf_s (versionStr.c_str (), L"%d", &major);
	return major;
}
bool IsInternetExplorer10 () { return GetInternetExplorerVersionMajor () == 10; }
bool IsInternetExplorer11AndLater () { return GetInternetExplorerVersionMajor () >= 11; }
