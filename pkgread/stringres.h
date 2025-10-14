#pragma once
#include <string>
#include <map>
#include "version.h"
#include "dynarr.h"
#include "norstr.h"
#include "syncutil.h"
static const std::pair <UINT64, LPCWSTR> captable [] = {
	{APPX_CAPABILITY_INTERNET_CLIENT, L"internetClient"},
	{APPX_CAPABILITY_INTERNET_CLIENT_SERVER, L"internetClientServer"},
	{APPX_CAPABILITY_PRIVATE_NETWORK_CLIENT_SERVER, L"privateNetworkClientServer"},
	{APPX_CAPABILITY_DOCUMENTS_LIBRARY, L"documentsLibrary"},
	{APPX_CAPABILITY_PICTURES_LIBRARY, L"picturesLibrary"},
	{APPX_CAPABILITY_VIDEOS_LIBRARY, L"videosLibrary"},
	{APPX_CAPABILITY_MUSIC_LIBRARY, L"musicLibrary"},
	{APPX_CAPABILITY_ENTERPRISE_AUTHENTICATION, L"enterpriseAuthentication"},
	{APPX_CAPABILITY_SHARED_USER_CERTIFICATES, L"sharedUserCertificates"},
	{APPX_CAPABILITY_REMOVABLE_STORAGE, L"removableStorage"},
	{0x00000400, L"appointments"},            // APPX_CAPABILITY_APPOINTMENTS
	{0x00000800, L"contacts"},                // APPX_CAPABILITY_CONTACTS
	{0x00001000, L"phoneCall"},
	{0x00002000, L"blockedChatMessages"},
	{0x00004000, L"objects3D"},
	{0x00008000, L"allJoyn"},
	{0x00010000, L"codeGeneration"},
	{0x00020000, L"remoteSystem"},
	{0x00040000, L"userAccountInformation"},
	{0x00080000, L"extendedExecution"},
	{0x00100000, L"location"},
	{0x00200000, L"microphone"},
	{0x00400000, L"proximity"},
	{0x00800000, L"webcam"},
	{0x01000000, L"spatialPerception"},
	{0x02000000, L"appointmentsSystem"},
	{0x04000000, L"contactsSystem"},
	{0x08000000, L"phoneCallSystem"},
	{0x10000000, L"smsSend"},
	{0x20000000, L"userDataTasks"},
	{0x40000000, L"userDataTasksSystem"},
	{0x80000000, L"userDataAccounts"}
};
void CapabilitiesFlagsToNames (APPX_CAPABILITIES caps, std::vector<std::wstring> &output)
{
	output.clear ();
	output.reserve (16);
	UINT64 packageCap = (UINT64)caps;
	for (auto &entry : captable)
	{
		if (packageCap & entry.first)
		{
			push_unique (output, std::wstring (entry.second));
		}
	}
}

static const std::pair <version, LPCWSTR> vertable [] = {
	{version (10, 0, 25299, 0), L"Windows 11 Version 25H2"},
	{version (10, 0, 24699, 0), L"Windows 11 Version 24H2"},
	{version (10, 0, 24000, 0), L"Windows 11 Version 23H2"},
	{version (10, 0, 22621, 0), L"Windows 11 Version 22H2"},
	{version (10, 0, 22159, 0), L"Windows 11 Initial Release (Version 21H2)"},
	{version (10, 0, 19645, 0), L"Windows 10 Insider Preview (Build 19645)"},
	{version (10, 0, 19541, 0), L"Windows 10 Insider Preview (Build 19541)"},
	{version (10, 0, 19045, 0), L"Windows 10 Version 22H2"},
	{version (10, 0, 19044, 0), L"Windows 10 Version 21H2 (November 2021 Update)"},
	{version (10, 0, 19043, 0), L"Windows 10 Version 21H2 (May 2021 Update)"},
	{version (10, 0, 19042, 0), L"Windows 10 Version 20H2"},
	{version (10, 0, 18362, 0), L"Windows 10 May 2019 Update"},
	{version (10, 0, 17763, 0), L"Windows 10 October 2018 Update"},
	{version (10, 0, 17134, 0), L"Windows 10 April 2018 Update"},
	{version (10, 0, 16299, 0), L"Windows 10 Fall Creators Update"},
	{version (10, 0, 15063, 0), L"Windows 10 Creators Update"},
	{version (10, 0, 14393, 0), L"Windows 10 Anniversary Update"},
	{version (10, 0, 10240, 0), L"Windows 10"},
	{version (10, 0, 0, 0),     L"Windows 10"},
	{version (6, 4, 9807, 0),   L"Windows 10 Technical Preview"},
	{version (6, 3, 1, 0),      L"Windows Phone 8.1"},
	{version (6, 3, 0, 0),      L"Windows 8.1"},
	{version (6, 2, 1, 0),      L"Windows 8"},
	{version (6, 2, 0, 0),      L"Windows 8 Develop Preview"}
};
std::wstring GetPrerequistOSVersionDescription (const version &ver)
{
	for (const auto &v : vertable)
	{
		if (ver >= v.first) return std::wstring (v.second);
	}
	return L"Unknown Windows Version";
}

std::vector <std::wstring> appitems =
{
	L"Id",
	L"DisplayName",
	L"BackgroundColor",
	L"ForegroundText",
	L"ShortName",
	L"Square44x44Logo"
};
std::vector <std::wstring> &GetApplicationAttributeItems () { return appitems; }
static CriticalSection g_appcs;
bool PushApplicationAttributeItem (const std::wstring &lpstr)
{
	CreateScopedLock (g_appcs);
	size_t len1 = appitems.size ();
	push_unique <std::wstring> (appitems, lpstr, [] (const std::wstring &v1, const std::wstring &v2) -> bool {
		return std::wnstring (v1) == std::wnstring (v2);
	});
	return appitems.size () > len1;
}
bool RemoveApplicationAttributeItem (const std::wstring &lpstr)
{
	CreateScopedLock (g_appcs);
	auto it = std::find_if (appitems.begin (), appitems.end (), [&] (const std::wstring &v) {
		return std::wnstring (v) == std::wnstring (lpstr);
	});
	if (it != appitems.end ())
	{
		size_t len1 = appitems.size ();
		appitems.erase (it);
		return appitems.size () < len1;
	}
	return false;
}