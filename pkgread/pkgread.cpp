// pkgread.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "readobj.h"
#include "pkgread.h"
#include "localeex.h"

#define ToHandleRead(_cpp_ptr_) reinterpret_cast <HPKGREAD> (_cpp_ptr_) 
#define ToPtrPackage(_cpp_ptr_) reinterpret_cast <package *> (_cpp_ptr_)
HPKGREAD CreatePackageReader () { return ToHandleRead (new package ()); }
BOOL LoadPackageFromFile (_In_ HPKGREAD hReader, _In_ LPCWSTR lpFilePath)
{
	auto ptr = ToPtrPackage (hReader);
	if (!ptr) return false;
	return ptr->create (lpFilePath);
}
void DestroyPackageReader (_In_ HPKGREAD hReader)
{
	auto ptr = ToPtrPackage (hReader);
	if (!ptr) return;
	return delete ptr;
}
WORD GetPackageType (_In_ HPKGREAD hReader)
{
	auto ptr = ToPtrPackage (hReader);
	if (!ptr) return PKGTYPE_UNKNOWN;
 	switch (ptr->type ())
	{
		case PackageType::unknown: return PKGTYPE_UNKNOWN;
		case PackageType::single: return PKGTYPE_APPX;
		case PackageType::bundle: return PKGTYPE_BUNDLE;
	}
	return PKGTYPE_UNKNOWN;
}
BOOL IsPackageValid (_In_ HPKGREAD hReader)
{
	auto ptr = ToPtrPackage (hReader);
	if (!ptr) return false;
	return ptr->valid ();
}
WORD GetPackageRole (_In_ HPKGREAD hReader)
{
	auto ptr = ToPtrPackage (hReader);
	if (!ptr) return PKGROLE_UNKNOWN;
	switch (ptr->type ())
	{
		case PackageType::unknown: return PKGROLE_UNKNOWN;
		case PackageType::bundle: return PKGROLE_APPLICATION;
		case PackageType::single: {
			auto ar = ptr->appx_reader ();
			switch (ar.package_role ())
			{
				case PackageRole::unknown: return PKGROLE_UNKNOWN;
				case PackageRole::application: return PKGROLE_APPLICATION;
				case PackageRole::framework: return PKGROLE_FRAMEWORK;
				case PackageRole::resource: return PKGROLE_RESOURCE;
			}
		} break;
	}
	return PKGROLE_UNKNOWN;
}

// Identity
VERSION VersionClassToStruct (const version &v)
{
	VERSION sv;
	sv.major = v.major;
	sv.minor = v.minor;
	sv.build = v.build;
	sv.revision = v.revision;
	return sv;
}
LPWSTR GetPackageIdentityStringValue (_In_ HPKGREAD hReader, _In_ DWORD dwName)
{
	auto ptr = ToPtrPackage (hReader);
	if (!ptr) return nullptr;
	switch (ptr->type ())
	{
		case PackageType::single: {
			auto reader = ptr->appx_reader ();
			auto id = reader.identity ();
			switch (LOWORD (dwName))
			{
				case PKG_IDENTITY_NAME: return _wcsdup (id.name ().c_str ());
				case PKG_IDENTITY_PUBLISHER: return _wcsdup (id.publisher ().c_str ());
				case PKG_IDENTITY_PACKAGEFAMILYNAME: return _wcsdup (id.package_family_name ().c_str ());
				case PKG_IDENTITY_PACKAGEFULLNAME: return _wcsdup (id.package_full_name ().c_str ());
				case PKG_IDENTITY_RESOURCEID: return _wcsdup (id.resource_id ().c_str ());
			}
		} break;
		case PackageType::bundle: {
			auto reader = ptr->bundle_reader ();
			auto id = reader.identity ();
			if (HIWORD (dwName) == PKGTYPE_BUNDLE)
			{
				CComPtr <IAppxPackageReader> apkg;
				HRESULT hr = reader.random_application_package (&apkg);
				if (SUCCEEDED (hr))
				{
					appxreader aread (apkg.p);
					auto aid = aread.identity ();
					switch (LOWORD (dwName))
					{
						case PKG_IDENTITY_NAME: return _wcsdup (aid.name ().c_str ());
						case PKG_IDENTITY_PUBLISHER: return _wcsdup (aid.publisher ().c_str ());
						case PKG_IDENTITY_PACKAGEFAMILYNAME: return _wcsdup (aid.package_family_name ().c_str ());
						case PKG_IDENTITY_PACKAGEFULLNAME: return _wcsdup (aid.package_full_name ().c_str ());
						case PKG_IDENTITY_RESOURCEID: return _wcsdup (aid.resource_id ().c_str ());
					}
				}
			}
			else
			{
				switch (LOWORD (dwName))
				{
					case PKG_IDENTITY_NAME: return _wcsdup (id.name ().c_str ());
					case PKG_IDENTITY_PUBLISHER: return _wcsdup (id.publisher ().c_str ());
					case PKG_IDENTITY_PACKAGEFAMILYNAME: return _wcsdup (id.package_family_name ().c_str ());
					case PKG_IDENTITY_PACKAGEFULLNAME: return _wcsdup (id.package_full_name ().c_str ());
					case PKG_IDENTITY_RESOURCEID: return _wcsdup (id.resource_id ().c_str ());
				}
			}
		} break;
	}
	return nullptr;
}
BOOL GetPackageIdentityVersion (_In_ HPKGREAD hReader, _Out_ VERSION *pVersion, _In_ BOOL bGetSubPkgVer)
{
	auto ptr = ToPtrPackage (hReader);
	if (!ptr) return FALSE;
	if (!pVersion) return FALSE;
	switch (ptr->type ())
	{
		case PackageType::single: {
			auto reader = ptr->appx_reader ();
			auto id = reader.identity ();
			auto ver = id.version ();
			*pVersion = VersionClassToStruct (ver);
			return !ver.empty ();
		} break;
		case PackageType::bundle: {
			auto reader = ptr->bundle_reader ();
			auto id = reader.identity ();
			if (bGetSubPkgVer)
			{
				CComPtr <IAppxPackageReader> apkg;
				HRESULT hr = reader.random_application_package (&apkg);
				if (SUCCEEDED (hr))
				{
					appxreader aread (apkg.p);
					auto aid = aread.identity ();
					auto ver = aid.version ();
					*pVersion = VersionClassToStruct (ver);
					return !ver.empty ();
				}
			}
			else
			{
				auto ver = id.version ();
				*pVersion = VersionClassToStruct (ver);
				return !ver.empty ();
			}
		} break;
	}
	return FALSE;
}
BOOL GetPackageIdentityArchitecture (_In_ HPKGREAD hReader, _Out_ DWORD *pdwArchi)
{
	auto ptr = ToPtrPackage (hReader);
	if (!ptr) return FALSE;
	if (!pdwArchi) return FALSE;
	switch (ptr->type ())
	{
		case PackageType::single: {
			auto reader = ptr->appx_reader ();
			auto id = reader.identity ();
			auto archi = id.architecture ();
			DWORD ret = 0;
			switch (archi)
			{
				case APPX_PACKAGE_ARCHITECTURE::APPX_PACKAGE_ARCHITECTURE_X86: 
					ret = PKG_ARCHITECTURE_X86; break;
				case APPX_PACKAGE_ARCHITECTURE::APPX_PACKAGE_ARCHITECTURE_ARM:
					ret = PKG_ARCHITECTURE_ARM; break;
				case APPX_PACKAGE_ARCHITECTURE::APPX_PACKAGE_ARCHITECTURE_X64:
					ret = PKG_ARCHITECTURE_X64; break;
				case APPX_PACKAGE_ARCHITECTURE::APPX_PACKAGE_ARCHITECTURE_NEUTRAL:
					ret = PKG_ARCHITECTURE_NEUTRAL; break;
				case (APPX_PACKAGE_ARCHITECTURE)12: // ARM64
					ret = PKG_ARCHITECTURE_ARM64; break;
			}
			*pdwArchi = ret;
			return ret != PKG_ARCHITECTURE_UNKNOWN;
		} break;
		case PackageType::bundle: {
			auto reader = ptr->bundle_reader ();
			DWORD ret = 0;
			size_t size = reader.application_packages ([&ret] (IAppxPackageReader *p) {
				appxreader aread (p);
				auto id = aread.identity ();
				auto archi = id.architecture ();
				switch (archi)
				{
					case APPX_PACKAGE_ARCHITECTURE::APPX_PACKAGE_ARCHITECTURE_X86:
						ret |= PKG_ARCHITECTURE_X86; break;
					case APPX_PACKAGE_ARCHITECTURE::APPX_PACKAGE_ARCHITECTURE_ARM:
						ret |= PKG_ARCHITECTURE_ARM; break;
					case APPX_PACKAGE_ARCHITECTURE::APPX_PACKAGE_ARCHITECTURE_X64:
						ret |= PKG_ARCHITECTURE_X64; break;
					case APPX_PACKAGE_ARCHITECTURE::APPX_PACKAGE_ARCHITECTURE_NEUTRAL:
						ret |= PKG_ARCHITECTURE_NEUTRAL; break;
					case (APPX_PACKAGE_ARCHITECTURE)12: // ARM64
						ret |= PKG_ARCHITECTURE_ARM64; break;
				}
			});
			*pdwArchi = ret;
			return ret != PKG_ARCHITECTURE_UNKNOWN;
		} break;
	}
	return FALSE;
}

// Properties
LPWSTR GetPackagePropertiesStringValue (_In_ HPKGREAD hReader, _In_ LPCWSTR lpName)
{
	auto ptr = ToPtrPackage (hReader);
	if (!ptr) return nullptr;
	switch (ptr->type ())
	{
		case PackageType::single: {
			auto reader = ptr->appx_reader ();
			auto prop = reader.properties ();
			return _wcsdup (prop.string_value (lpName ? lpName : L"").c_str ());
		} break;
		case PackageType::bundle: {
			auto reader = ptr->bundle_reader ();
			CComPtr <IAppxPackageReader> apppkg;
			if (SUCCEEDED (reader.random_application_package (&apppkg)))
			{
				appxreader apkg (apppkg.p);
				auto prop = apkg.properties ();
				return _wcsdup (prop.string_value (lpName ? lpName : L"").c_str ());
			}
		} break;
	}
	return nullptr;
}
HRESULT GetPackagePropertiesBoolValue (_In_ HPKGREAD hReader, _In_ LPCWSTR lpName, _Outptr_ BOOL *pRet)
{
	auto ptr = ToPtrPackage (hReader);
	if (!ptr) return E_INVALIDARG;
	if (!pRet) return E_INVALIDARG;
	*pRet = FALSE;
	switch (ptr->type ())
	{
		case PackageType::single: {
			auto reader = ptr->appx_reader ();
			CComPtr <IAppxManifestReader> m;
			HRESULT hr = reader.manifest (&m);
			if (FAILED (hr)) return hr;
			CComPtr <IAppxManifestProperties> p;
			hr = m->GetProperties (&p);
			if (FAILED (hr)) return hr;
			return p->GetBoolValue (lpName, pRet);
		} break;
		case PackageType::bundle: {
			auto reader = ptr->bundle_reader ();
			CComPtr <IAppxPackageReader> apppkg;
			HRESULT hr = S_OK;
			if (SUCCEEDED (hr = reader.random_application_package (&apppkg)))
			{
				appxreader apkg (apppkg.p);
				CComPtr <IAppxManifestReader> m;
				hr = apkg.manifest (&m);
				if (FAILED (hr)) return hr;
				CComPtr <IAppxManifestProperties> p;
				hr = m->GetProperties (&p);
				if (FAILED (hr)) return hr;
				return p->GetBoolValue (lpName, pRet);
			}
			else return hr;
		} break;
	}
	return E_FAIL;
}

// Applications
BOOL AddPackageApplicationItemGetName (_In_ LPCWSTR lpName)
{
	if (std::wnstring (lpName ? lpName : L"").empty ()) return FALSE;
	return PushApplicationAttributeItem (lpName);
}
BOOL RemovePackageApplicationItemGetName (_In_ LPCWSTR lpName)
{
	if (std::wnstring (lpName ? lpName : L"").empty ()) return FALSE;
	return RemoveApplicationAttributeItem (lpName);
}
HLIST_PVOID GetAllApplicationItemsName ()
{
	size_t len = sizeof (LIST_PVOID) + sizeof (LPCWSTR) * appitems.size ();
	HLIST_PVOID hList = (HLIST_PVOID)malloc (len);
	ZeroMemory (hList, len);
	hList->dwSize = 0;
	try
	{
		for (auto &it : appitems)
		{
			((LPCWSTR *)hList->alpVoid) [hList->dwSize ++] = (LPCWSTR)it.c_str ();
		}
	}
	catch (const std::exception &e) {}
	return hList;
}
void DestroyApplicationItemsName (_In_ HLIST_PVOID hList)
{
	if (!hList) return;
	free (hList);
}
#define ToHandleAppEnumerator(_cpp_ptr_) reinterpret_cast <HAPPENUMERATOR> (_cpp_ptr_)
typedef std::vector <app_info> app_enumerator;
#define ToPtrAppxApps(_cpp_ptr_) reinterpret_cast <app_enumerator *> (_cpp_ptr_)
HAPPENUMERATOR GetPackageApplications (_In_ HPKGREAD hReader)
{
	auto ptr = ToPtrPackage (hReader);
	if (!ptr) return nullptr;
	switch (ptr->type ())
	{
		case PackageType::single: {
			auto reader = ptr->appx_reader ();
			auto app = reader.applications ();
			auto appvec = new app_enumerator ();
			app.applications (*appvec);
			return ToHandleAppEnumerator (appvec);
		} break;
		case PackageType::bundle: {
			auto br = ptr->bundle_reader ();
			CComPtr <IAppxPackageReader> pr;
			HRESULT hr = br.random_application_package (&pr);
			if (SUCCEEDED (hr))
			{
				appxreader reader (pr.p);
				auto app = reader.applications ();
				auto appvec = new app_enumerator ();
				app.applications (*appvec);
				return ToHandleAppEnumerator (appvec);
			}
		} break;
	}
	return nullptr;
}
HLIST_PVOID ApplicationsToMap (_In_ HAPPENUMERATOR hEnumerator)
{
	auto ptr = ToPtrAppxApps (hEnumerator);
	if (!ptr || ptr->empty ()) return nullptr;
	size_t count = ptr->size ();
	size_t len = sizeof (LIST_PVOID) + sizeof (LPVOID) * count;
	auto list = (HLIST_PVOID)malloc (len);
	ZeroMemory (list, len);
	if (!list) return nullptr;
	list->dwSize = 0;
	bool outer_throw = false;
	raii list_guard ([&] () {
		if (outer_throw)
		{
			for (DWORD i = 0; i < list->dwSize; ++i)
			{
				auto map = (HLIST_PVOID)list->alpVoid [i];
				if (map)
				{
					for (DWORD j = 0; j < map->dwSize; ++j)
					{
						auto pair = (HPAIR_PVOID)map->alpVoid [j];
						if (pair)
						{
							free (pair);
							map->alpVoid [j] = nullptr;
						}
					}
					free (map);
					list->alpVoid [i] = nullptr;
				}
			}
			free (list);
			list = nullptr;
		}
	});
	try
	{
		for (auto &app : *ptr)
		{
			size_t map_size = app.size ();
			size_t mlen = sizeof (LIST_PVOID) + sizeof (LPVOID) * map_size;
			auto map = (HLIST_PVOID)malloc (mlen);
			ZeroMemory (map, mlen);
			if (!map)
			{
				outer_throw = true;
				throw std::bad_alloc ();
			}
			map->dwSize = 0;
			bool inner_throw = false;
			raii map_guard ([&] () {
				if (inner_throw)
				{
					for (DWORD j = 0; j < map->dwSize; ++j)
					{
						auto pair = (HPAIR_PVOID)map->alpVoid [j];
						if (pair)
						{
							free (pair);
							map->alpVoid [j] = nullptr;
						}
					}
					free (map);
					map = nullptr;
				}
			});
			try
			{
				for (auto &kv : app)
				{
					auto pair = (HPAIR_PVOID)malloc (sizeof (PAIR_PVOID));
					ZeroMemory (pair, sizeof (PAIR_PVOID));
					if (!pair)
					{
						inner_throw = true;
						throw std::bad_alloc ();
					}
					pair->lpKey = (LPVOID)kv.first.c_str ();
					pair->lpValue = (LPVOID)kv.second.c_str ();
					map->alpVoid [map->dwSize++] = pair;
				}
			}
			catch (const std::exception &)
			{
				inner_throw = true;
				throw;
			}
			list->alpVoid [list->dwSize ++] = map;
		}
	}
	catch (const std::exception &) { outer_throw = true; }
	if (outer_throw) return nullptr;
	return list;
}
void DestroyApplicationsMap (_In_ HLIST_PVOID hEnumerator)
{
	if (!hEnumerator) return;
	HLIST_PVOID list = hEnumerator;
	for (DWORD i = 0; i < list->dwSize; ++ i)
	{
		HLIST_PVOID map = (HLIST_PVOID)list->alpVoid [i];
		if (!map) continue;
		for (DWORD j = 0; j < map->dwSize; ++ j)
		{
			HPAIR_PVOID pair = (HPAIR_PVOID)map->alpVoid [j];
			if (pair)
			{
				free (pair);
				map->alpVoid [j] = nullptr;
			}
		}
		free (map);
		list->alpVoid [i] = nullptr;
	}
	free (list);
}
void DestroyPackageApplications (_In_ HAPPENUMERATOR hEnumerator)
{
	auto ptr = ToPtrAppxApps (hEnumerator);
	if (ptr) delete ptr;
}

// Resources
HLIST_PVOID GetResourcesLanguages (_In_ HPKGREAD hReader)
{
	auto ptr = ToPtrPackage (hReader);
	if (!ptr) return nullptr;
	std::vector <std::wnstring> langs;
	switch (ptr->type ()) 
	{
		case PackageType::single:
		{
			auto reader = ptr->appx_reader ();
			auto res = reader.resources ();
			res.languages (langs);
			break;
		}
		case PackageType::bundle:
		{
			auto br = ptr->bundle_reader ();
			auto res = br.package_id_items ();
			res.enumerate ([&langs] (IAppxBundleManifestPackageInfo *inf) {
				auto item = appx_info::appx_iditem (inf);
				std::vector <std::wnstring> l;
				auto qr = item.qualified_resources ();
				qr.languages (l);
				for (auto &it : l) push_unique <std::wnstring> (langs, it);
			});
			break;
		}
		default: return nullptr;
	}
	if (langs.empty ()) return nullptr;
	size_t count = langs.size ();
	size_t bytes = sizeof (LIST_PVOID) + sizeof (LPWSTR) * (count - 1);
	auto list = (HLIST_PVOID)malloc (bytes);
	ZeroMemory (list, bytes);
	if (!list) return nullptr;
	list->dwSize = 0;
	for (auto &it : langs) list->alpVoid [list->dwSize ++] = _wcsdup (it.c_str ());
	return list;
}
HLIST_LCID GetResourcesLanguagesToLcid (_In_ HPKGREAD hReader)
{
	auto ptr = ToPtrPackage (hReader);
	if (!ptr) return nullptr;
	std::vector <std::wnstring> langs;
	switch (ptr->type ())
	{
		case PackageType::single:
		{
			auto reader = ptr->appx_reader ();
			auto res = reader.resources ();
			res.languages (langs);
			break;
		}
		case PackageType::bundle:
		{
			auto br = ptr->bundle_reader ();
			auto res = br.package_id_items ();
			res.enumerate ([&langs] (IAppxBundleManifestPackageInfo *inf) {
				auto item = appx_info::appx_iditem (inf);
				std::vector <std::wnstring> l;
				auto qr = item.qualified_resources ();
				qr.languages (l);
				for (auto &it : l) push_unique <std::wnstring> (langs, it);
			});
			break;
		}
		default: return nullptr;
	}
	if (langs.empty ()) return nullptr;
	size_t len = sizeof (LIST_LCID) + sizeof (LCID) * langs.size ();
	HLIST_LCID hList = (HLIST_LCID)malloc (len);
	ZeroMemory (hList, len);
	hList->dwSize = 0;
	for (auto &it : langs)
	{
		LCID lcid = LocaleCodeToLcid (it);
		if (lcid)
		{
			hList->aLcid [hList->dwSize ++] = lcid;
		}
	}
	return hList;
}
HLIST_UINT32 GetResourcesScales (_In_ HPKGREAD hReader)

{
	auto ptr = ToPtrPackage (hReader);
	if (!ptr) return nullptr;
	std::vector <UINT32> scales;
	switch (ptr->type ())
	{
		case PackageType::single:
		{
			auto reader = ptr->appx_reader ();
			auto res = reader.resources ();
			res.scales (scales);
			break;
		}
		case PackageType::bundle:
		{
			auto br = ptr->bundle_reader ();
			auto res = br.package_id_items ();
			res.enumerate ([&scales] (IAppxBundleManifestPackageInfo *inf) {
				auto item = appx_info::appx_iditem (inf);
				std::vector <UINT32> s;
				auto qr = item.qualified_resources ();
				qr.scales (s);
				for (auto &it : s) push_unique (scales, it);
			});
			break;
		}
		default: return nullptr;
	}
	if (scales.empty ()) return nullptr;
	size_t len = sizeof (LIST_UINT32) + sizeof (UINT32) * scales.size ();
	HLIST_UINT32 hList = (HLIST_UINT32)malloc (len);
	ZeroMemory (hList, len);
	hList->dwSize = 0;
	for (auto &it : scales)
	{
		if (!it) continue;
		hList->aUI32 [hList->dwSize ++] = it;
	}
	return hList;
}
DWORD GetResourcesDxFeatureLevels (_In_ HPKGREAD hReader)

{
	auto ptr = ToPtrPackage (hReader);
	if (!ptr) return 0;
	DWORD dwFlags = 0;
	switch (ptr->type ())
	{
		case PackageType::single:
		{
			auto reader = ptr->appx_reader ();
			auto res = reader.resources ();
			std::vector <DX_FEATURE_LEVEL> dxlevels;
			res.dx_feature_level (dxlevels);
			for (auto &it : dxlevels)
			{
				switch (it)
				{
					case DX_FEATURE_LEVEL::DX_FEATURE_LEVEL_9:
						dwFlags |= PKG_RESOURCES_DXFEATURE_LEVEL9; break;
					case DX_FEATURE_LEVEL::DX_FEATURE_LEVEL_10:
						dwFlags |= PKG_RESOURCES_DXFEATURE_LEVEL10; break;
					case DX_FEATURE_LEVEL::DX_FEATURE_LEVEL_11:
						dwFlags |= PKG_RESOURCES_DXFEATURE_LEVEL11; break;
					case (DX_FEATURE_LEVEL)4:
						dwFlags |= PKG_RESOURCES_DXFEATURE_LEVEL12; break;
				}
			}
			break;
		}
		case PackageType::bundle:
		{
			auto br = ptr->bundle_reader ();
			auto res = br.package_id_items ();
			res.enumerate ([&dwFlags] (IAppxBundleManifestPackageInfo *inf) {
				auto item = appx_info::appx_iditem (inf);
				std::vector <DX_FEATURE_LEVEL> dxlevels;
				auto qr = item.qualified_resources ();
				qr.dx_feature_level (dxlevels);
				for (auto &it : dxlevels)
				{
					switch (it)
					{
						case DX_FEATURE_LEVEL::DX_FEATURE_LEVEL_9:
							dwFlags |= PKG_RESOURCES_DXFEATURE_LEVEL9; break;
						case DX_FEATURE_LEVEL::DX_FEATURE_LEVEL_10:
							dwFlags |= PKG_RESOURCES_DXFEATURE_LEVEL10; break;
						case DX_FEATURE_LEVEL::DX_FEATURE_LEVEL_11:
							dwFlags |= PKG_RESOURCES_DXFEATURE_LEVEL11; break;
						case (DX_FEATURE_LEVEL)4:
							dwFlags |= PKG_RESOURCES_DXFEATURE_LEVEL12; break;
					}
				}
			});
			break;
		}
		default: return 0;
	}
	return dwFlags;
}
void DestroyResourcesLanguagesList (_In_ HLIST_PVOID hList)
{
	if (!hList) return;
	for (DWORD i = 0; i < hList->dwSize; i++)
	{
		if (hList->alpVoid [i])
		{
			free (hList->alpVoid [i]);
			hList->alpVoid [i] = nullptr;
		}
	}
	free (hList);
}
void DestroyResourcesLanguagesLcidList (_In_ HLIST_LCID hList)
{
	if (!hList) return;
	free (hList);
}
void DestroyUInt32List (_In_ HLIST_UINT32 hList)
{
	if (!hList) return;
	free (hList); 
}

// Dependencies
HLIST_DEPINFO GetDependencesInfoList (_In_ HPKGREAD hReader)
{
	auto ptr = ToPtrPackage (hReader);
	if (!ptr) return nullptr;
	std::vector <dep_info> vec;
	switch (ptr->type ())
	{
		case PackageType::single: {
			auto reader = ptr->appx_reader ();
			auto deps = reader.dependencies ();
			deps.dependencies (vec);
		} break;
		case PackageType::bundle: {
			auto br = ptr->bundle_reader ();
			CComPtr <IAppxPackageReader> pr;
			HRESULT hr = br.random_application_package (&pr);
			if (FAILED (hr)) return nullptr;
			appxreader reader (pr.p);
			auto deps = reader.dependencies ();
			deps.dependencies (vec);
		}
	} 
	size_t len = sizeof (LIST_DEPINFO) + sizeof (DEPENDENCY_INFO) * vec.size ();
	HLIST_DEPINFO hList = (HLIST_DEPINFO)malloc (len);
	ZeroMemory (hList, len);
	hList->dwSize = 0;
	for (auto &it : vec)
	{
		auto &dep = hList->aDepInfo [hList->dwSize ++];
		dep.lpName = _wcsdup (it.name.c_str ());
		dep.lpPublisher = _wcsdup (it.publisher.c_str ());
		dep.verMin = VersionClassToStruct (it.minversion);
	}
	return hList;
}
void DestroyDependencesInfoList (_In_ HLIST_DEPINFO hList)
{
	if (!hList) return;
	for (DWORD i = 0; i < hList->dwSize; i ++)
	{
		auto &dep = hList->aDepInfo [i];
		if (dep.lpName) { free (dep.lpName); dep.lpName = nullptr; }
		if (dep.lpPublisher) { free (dep.lpPublisher); dep.lpPublisher = nullptr; }
	}
	free (hList);
}

// Capabilities
HLIST_PVOID GetCapabilitiesList (_In_ HPKGREAD hReader)
{
	auto ptr = ToPtrPackage (hReader);
	if (!ptr) return nullptr;
	std::vector <std::wnstring> caps;
	switch (ptr->type ())
	{
		case PackageType::single: {
			auto reader = ptr->appx_reader ();
			auto cap = reader.capabilities ();
			std::vector <std::wstring> vec;
			cap.capabilities_names (vec);
			for (auto &it : vec)
			{
				auto cname = std::wnstring (it.c_str ());
				if (cname.empty ()) continue;
				push_unique (caps, cname);
			}
		} break;
		case PackageType::bundle: {
			auto br = ptr->bundle_reader ();
			CComPtr <IAppxPackageReader> ar;
			if (FAILED (br.random_application_package (&ar))) return nullptr;
			appxreader reader (ar.p);
			auto cap = reader.capabilities ();
			std::vector <std::wstring> vec;
			cap.capabilities_names (vec);
			for (auto &it : vec)
			{
				auto cname = std::wnstring (it.c_str ());
				if (cname.empty ()) continue;
				push_unique (caps, cname);
			}
		} break;
	}
	size_t len = sizeof (LIST_PVOID) * sizeof (LPWSTR) * caps.size ();
	HLIST_PVOID hList = (HLIST_PVOID)malloc (len);
	ZeroMemory (hList, len);
	hList->dwSize = 0;
	for (auto &it : caps)
	{
		hList->alpVoid [hList->dwSize ++] = (LPVOID)_wcsdup (it.c_str ());
	}
	return hList;
}
HLIST_PVOID GetDeviceCapabilitiesList (_In_ HPKGREAD hReader)
{
	auto ptr = ToPtrPackage (hReader);
	if (!ptr) return nullptr;
	std::vector <std::wnstring> caps;
	switch (ptr->type ())
	{
		case PackageType::single: {
			auto reader = ptr->appx_reader ();
			auto cap = reader.capabilities ();
			std::vector <std::wstring> vec;
			cap.device_capabilities (vec);
			for (auto &it : vec)
			{
				auto cname = std::wnstring (it.c_str ());
				if (cname.empty ()) continue;
				push_unique (caps, cname);
			}
		} break;
		case PackageType::bundle: {
			auto br = ptr->bundle_reader ();
			CComPtr <IAppxPackageReader> ar;
			if (FAILED (br.random_application_package (&ar))) return nullptr;
			appxreader reader (ar.p);
			auto cap = reader.capabilities ();
			std::vector <std::wstring> vec;
			cap.device_capabilities (vec);
			for (auto &it : vec)
			{
				auto cname = std::wnstring (it.c_str ());
				if (cname.empty ()) continue;
				push_unique (caps, cname);
			}
		} break;
	}
	size_t len = sizeof (LIST_PVOID) * sizeof (LPWSTR) * caps.size ();
	HLIST_PVOID hList = (HLIST_PVOID)malloc (len);
	ZeroMemory (hList, len);
	hList->dwSize = 0;
	for (auto &it : caps)
	{
		hList->alpVoid [hList->dwSize ++] = (LPVOID)_wcsdup (it.c_str ());
	}
	return hList;
}
void DestroyWStringList (_In_ HLIST_PVOID hList)
{
	if (!hList) return;
	for (size_t cnt = 0; cnt < hList->dwSize; cnt ++)
	{
		free (hList->alpVoid [cnt]);
		hList->alpVoid [cnt] = nullptr;
	}
	free (hList);
}
