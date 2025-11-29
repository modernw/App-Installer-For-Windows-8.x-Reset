#pragma once
#include <map>
#include <string>
#include "rctools.h"
#include "resource.h"
#define MAKENAMEIDMAP(_Res_Name_) {#_Res_Name_, _Res_Name_}
std::map <std::string, unsigned> g_nameToId = {
	MAKENAMEIDMAP (IDS_WINTITLE),
	MAKENAMEIDMAP (IDS_DEFAULTWIDTH),
	MAKENAMEIDMAP (IDS_DEFAULTHEIGHT),
	MAKENAMEIDMAP (IDS_MINWIDTH),
	MAKENAMEIDMAP (IDS_MINHIEHGT),
	MAKENAMEIDMAP (IDS_TITLE),
	MAKENAMEIDMAP (IDS_GUIDE),
	MAKENAMEIDMAP (IDS_GUIDETEXT),
	MAKENAMEIDMAP (IDS_TITLEFORMAT),
	MAKENAMEIDMAP (IDS_GUIDETEXT_COMMON)
};

#ifdef MAKENAMEIDMAP
#undef MAKENAMEIDMAP
#endif

#ifdef __cplusplus_cli
using namespace System;
using namespace System::Runtime::InteropServices;
[ComVisible (true)]
public ref class _I_Resources
{
	public:
	String ^GetById (unsigned int uiResId) { return GetRCStringCli (uiResId); }
	unsigned ToId (String ^lpResName)
	{
		auto it = g_nameToId.find (MPStringToStdA (lpResName));
		return (it != g_nameToId.end ()) ? it->second : 0;
	}
	String ^ToName (unsigned int ulResId)
	{
		for (auto &it : g_nameToId) { if (it.second == ulResId) return CStringToMPString (it.first); }
		return "";
	}
	String ^GetByName (String ^lpResId) { return GetById (ToId (lpResId)); }
	String ^operator [] (unsigned int uiResId) { return GetRCStringCli (uiResId); }
	String ^GetFromOthers (String ^filepath, unsigned int resid)
	{
		HMODULE module = nullptr;
		bool needrel = false;
		if (filepath && !IsNormalizeStringEmpty (MPStringToStdW (filepath)))
		{
			HMODULE hRes = LoadLibraryExW (
				MPStringToStdW (filepath).c_str (),
				NULL,
				LOAD_LIBRARY_AS_DATAFILE | LOAD_LIBRARY_AS_IMAGE_RESOURCE
			);
			needrel = hRes;
			module = hRes;
		}
		else module = GetModuleHandleW (NULL);
		auto ret = GetRCStringCli (resid, module);
		if (needrel && module) FreeLibrary (module);
		return ret;
	}
};
#endif