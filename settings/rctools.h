#pragma once
#include <Windows.h>
#include <WinBase.h>
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#include "typestrans.h"
#include "module.h"

// 返回的指针如果非空则一定需要用 free 释放
LPWSTR GetRCStringW (UINT resID, HMODULE hModule hModule_DefaultParam)
{
	std::vector <WCHAR> buf (256);
	size_t cnt = 0;
CopyStringLoop_GetRCStringW:
	{
		size_t len = LoadStringW (hModule, resID, buf.data (), buf.size ());
		if (cnt > 1625) return _wcsdup (buf.data ());
		if (len >= buf.size () - 1) 
		{
			buf.resize (buf.size () + 20); 
			cnt ++;
			goto CopyStringLoop_GetRCStringW;
		}
		else return _wcsdup (buf.data ());
	}
}
// 返回的指针如果非空则一定需要用 free 释放
LPSTR GetRCStringA (UINT resID, HMODULE hModule hModule_DefaultParam)
{
	std::vector <CHAR> buf (256);
	size_t cnt = 0;
CopyStringLoop_GetRCStringA:
	{
		size_t len = LoadStringA (hModule, resID, buf.data (), buf.size ());
		if (cnt > 1625) return _strdup (buf.data ());
		if (len >= buf.size () - 1)
		{
			buf.resize (buf.size () + 20);
			cnt ++;
			goto CopyStringLoop_GetRCStringA;
		}
		else return _strdup (buf.data ());
	}
}

HICON LoadRCIcon (UINT resID, HMODULE hModule hModule_DefaultParam)
{
	return (HICON)LoadImageW (hModule, MAKEINTRESOURCEW (resID), IMAGE_ICON, 0, 0, LR_DEFAULTSIZE);
}
HRSRC FindResourceByName (LPCWSTR resourceName, LPCWSTR resourceType, HMODULE hModule hModule_DefaultParam)
{
	return FindResourceW (hModule, resourceName, resourceType);
}
#ifdef __cplusplus
#include <string>
std::wstring GetRCStringSW (UINT resID, HMODULE hModule hModule_DefaultParam)
{
	std::vector <WCHAR> buf (256);
	size_t cnt = 0;
CopyStringLoop_GetRCStringSW:
	{
		size_t len = LoadStringW (hModule, resID, buf.data (), buf.size ());
		if (cnt > 1625) return buf.data ();
		if (len >= buf.size () - 1)
		{
			buf.resize (buf.size () + 20);
			cnt ++;
			goto CopyStringLoop_GetRCStringSW;
		}
		else return buf.data ();
	}
}
std::string GetRCStringSA (UINT resID, HMODULE hModule hModule_DefaultParam)
{
	std::vector <CHAR> buf (256);
	size_t cnt = 0;
CopyStringLoop_GetRCStringSA:
	{
		size_t len = LoadStringA (hModule, resID, buf.data (), buf.size ());
		if (cnt > 1625) return buf.data ();
		if (len >= buf.size () - 1)
		{
			buf.resize (buf.size () + 20);
			cnt ++;
			goto CopyStringLoop_GetRCStringSA;
		}
		else return buf.data ();
	}
}
#endif
#if defined (__cplusplus) && defined (__cplusplus_cli)
using namespace System;
String ^GetRCStringCli (UINT resID, HMODULE hModule hModule_DefaultParam)
{
	std::vector <WCHAR> buf (256);
	size_t cnt = 0;
CopyStringLoop_GetRCStringCli:
	{
		size_t len = LoadStringW (hModule, resID, buf.data (), buf.size ());
		if (cnt > 1625) return gcnew String (buf.data ());
		if (len >= buf.size () - 1)
		{
			buf.resize (buf.size () + 20);
			cnt ++;
			goto CopyStringLoop_GetRCStringCli;
		}
		else return gcnew String (buf.data ());
	}
}
#define GetRCIntValue(_UINT__resID_) toInt (GetRCStringCli (_UINT__resID_))
#define GetRCDoubleValue(_UINT__resID_) toDouble (GetRCStringCli (_UINT__resID_))
#define GetRCBoolValue(_UINT__resID_) toBool (GetRCStringCli (_UINT__resID_))
#define GetRCDateTimeValue(_UINT__resID_) toDateTime (GetRCStringCli (_UINT__resID_))
#define rcString(resID) GetRCStringCli (resID)
#define rcInt(resID) GetRCIntValue (resID)
#define rcDouble(resID) GetRCDoubleValue (resID)
#define rcBool(resID) GetRCBoolValue (resID)
#define rcDTime(resID) GetRCDateTimeValue (resID)
#define rcIcon(resID) LoadRCIcon (resID)
#endif