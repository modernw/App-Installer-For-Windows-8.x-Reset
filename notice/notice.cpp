// pkgmgr.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "notice.h"
#include "raii.h"
#include "version.h"

#undef GetFullPathName
std::wstring GetFullPathName (const std::wstring &lpFileName)
{
	if (lpFileName.empty ()) return L"";
	DWORD length = GetFullPathNameW (lpFileName.c_str (), 0, nullptr, nullptr);
	if (length == 0) return L"";
	std::vector <WCHAR> buffer (length + 1, L'\0');
	DWORD result = GetFullPathNameW (lpFileName.c_str (), length, buffer.data (), nullptr);
	if (result == 0) return L"";
	return std::wstring (buffer.data (), result);
}

std::wstring g_swExceptionCode = L"";
std::wstring g_swExceptionDetail = L"";

struct destruct
{
	std::function <void ()> endtask = nullptr;
	destruct (std::function <void ()> pfunc): endtask (pfunc) {}
	~destruct () { if (endtask) endtask (); }
};
static std::wstring StringToWString (const std::string &str, UINT codePage = CP_ACP)
{
	if (str.empty ()) return std::wstring ();
	int len = MultiByteToWideChar (codePage, 0, str.c_str (), -1, nullptr, 0);
	if (len == 0) return std::wstring ();
	std::wstring wstr (len - 1, L'\0');
	MultiByteToWideChar (codePage, 0, str.c_str (), -1, &wstr [0], len);
	return wstr;
}

