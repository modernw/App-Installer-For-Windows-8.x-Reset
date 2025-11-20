#pragma once
#include <string>
#include <Windows.h>
std::wstring StringToWString (const std::string &str, UINT codePage = CP_ACP)
{
	if (str.empty ()) return std::wstring ();
	int len = MultiByteToWideChar (codePage, 0, str.c_str (), -1, nullptr, 0);
	if (len == 0) return std::wstring ();
	std::wstring wstr (len - 1, L'\0');
	MultiByteToWideChar (codePage, 0, str.c_str (), -1, &wstr [0], len);
	return wstr;
}
std::string WStringToString (const std::wstring &wstr, UINT codePage = CP_ACP)
{
	if (wstr.empty ()) return std::string ();
	int len = WideCharToMultiByte (codePage, 0, wstr.c_str (), -1, nullptr, 0, nullptr, nullptr);
	if (len == 0) return std::string ();
	std::string str (len - 1, '\0');
	WideCharToMultiByte (codePage, 0, wstr.c_str (), -1, &str [0], len, nullptr, nullptr);
	return str;
}
