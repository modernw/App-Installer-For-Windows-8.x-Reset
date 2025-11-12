// pkgmgr.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "pkgmgr.h"
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
using onprogress = AsyncOperationProgressHandler <DeploymentResult ^, DeploymentProgress>;
using onprogresscomp = AsyncOperationWithProgressCompletedHandler <DeploymentResult ^, DeploymentProgress>;
using progressopt = IAsyncOperationWithProgress <DeploymentResult ^, DeploymentProgress> ^;
template <typename TAsyncOpCreator> HRESULT RunPackageManagerOperation (TAsyncOpCreator asyncCreator, PKGMRR_PROGRESSCALLBACK pfCallback, void *pCustom, LPWSTR *pErrorCode, LPWSTR *pDetailMsg)
{
	g_swExceptionCode = L"";
	g_swExceptionDetail = L"";
	if (pErrorCode) *pErrorCode = nullptr;
	if (pDetailMsg) *pDetailMsg = nullptr;
	try
	{
		HANDLE hCompEvt = nullptr;
		destruct closeevt ([&hCompEvt] () {
			if (hCompEvt) { CloseHandle (hCompEvt); hCompEvt = nullptr; }
		});
		hCompEvt = CreateEventExW (nullptr, nullptr, CREATE_EVENT_MANUAL_RESET, EVENT_ALL_ACCESS);
		if (!hCompEvt) return E_FAIL;
		auto depopt = asyncCreator ();
		depopt->Progress = ref new onprogress ([pfCallback, pCustom] (progressopt operation, DeploymentProgress progress) {
			if (pfCallback) pfCallback ((DWORD)progress.percentage, pCustom);
		});
		depopt->Completed = ref new onprogresscomp ([&hCompEvt] (progressopt, AsyncStatus) {
			SetEvent (hCompEvt);
		});
		WaitForSingleObject (hCompEvt, INFINITE);
		switch (depopt->Status)
		{
			case AsyncStatus::Completed:
				return S_OK;
			case AsyncStatus::Error:
			{
				auto depresult = depopt->GetResults ();
				auto errorcode = depopt->ErrorCode;
				HResult hr = errorcode;
				String ^errstr = Exception::CreateException (errorcode.Value)->ToString ();
				g_swExceptionCode += errstr && errstr->Data () ? errstr->Data () : L"";
				g_swExceptionDetail += depresult->ErrorText->Data ();
				if (pErrorCode) *pErrorCode = _wcsdup (g_swExceptionCode.c_str ());
				if (pDetailMsg) *pDetailMsg = _wcsdup (g_swExceptionDetail.c_str ());
				return (HRESULT)errorcode.Value;
			}
			case AsyncStatus::Canceled:
				g_swExceptionDetail = L"Installation Canceled";
				if (pDetailMsg) *pDetailMsg = _wcsdup (g_swExceptionDetail.c_str ());
				return E_ABORT;
			case AsyncStatus::Started:
				g_swExceptionDetail = L"Installation is Running";
				if (pDetailMsg) *pDetailMsg = _wcsdup (g_swExceptionDetail.c_str ());
				return E_PENDING;
		}
	}
	catch (Exception ^e)
	{
		g_swExceptionDetail = e->ToString ()->Data ();
		if (pDetailMsg) *pDetailMsg = _wcsdup (g_swExceptionDetail.c_str ());
		return (SUCCEEDED ((HRESULT)e->HResult) ? E_FAIL : (HRESULT)e->HResult);
	}
	catch (const std::exception &e)
	{
		g_swExceptionDetail = StringToWString (e.what () ? e.what () : "Unknown exception.");
		if (pDetailMsg) *pDetailMsg = _wcsdup (g_swExceptionDetail.c_str ());
		return E_FAIL;
	}
	catch (...)
	{
		g_swExceptionDetail = L"Unknown exception";
		if (pDetailMsg) *pDetailMsg = _wcsdup (g_swExceptionDetail.c_str ());
		return E_FAIL;
	}
	return E_FAIL;
}
[MTAThread]
HRESULT AddAppxPackageFromURI (LPCWSTR lpPkgFileUri, PCREGISTER_PACKAGE_DEFENDENCIES alpDepUrlList, DWORD dwDeployOption, PKGMRR_PROGRESSCALLBACK pfCallback, void *pCustom, LPWSTR *pErrorCode, LPWSTR *pDetailMsg)
{
	return RunPackageManagerOperation ([=] () {
		auto pkgmgr = ref new PackageManager ();
		auto depuris = ref new Platform::Collections::Vector <Uri ^> ();
		if (alpDepUrlList)
		{
			for (size_t i = 0; i < alpDepUrlList->dwSize; i ++)
			{
				auto &pstr = alpDepUrlList->alpDepUris [i];
				try
				{
					if (pstr && *pstr)
					{
						auto depuristr = ref new String (pstr);
						Uri ^duri = nullptr;
						try { duri = ref new Uri (depuristr); }
						catch (Exception ^e)
						{
							try
							{
								std::wstring fullpath = GetFullPathName (pstr ? pstr : L"");
								if (fullpath.empty ()) fullpath = pstr ? pstr : L"";
								duri = ref new Uri (ref new String (fullpath.c_str ()));
							}
							catch (Exception ^e) { continue; }
						}
						depuris->Append (duri);
					}
				}
				catch (Exception ^e) { continue; }
			}
		}
		if (depuris->Size > 0) depuris = nullptr;
		auto pkguri = ref new Uri (ref new String (lpPkgFileUri));
		auto pkguristr = pkguri->ToString ();
		auto ope = pkgmgr->AddPackageAsync (pkguri, depuris, (DeploymentOptions)dwDeployOption);
		return ope;
	}, pfCallback, pCustom, pErrorCode, pDetailMsg);
}
[MTAThread]
HRESULT AddAppxPackageFromPath (LPCWSTR lpPkgPath, PCREGISTER_PACKAGE_DEFENDENCIES alpDepUrlList, DWORD dwDeployOption, PKGMRR_PROGRESSCALLBACK pfCallback, void *pCustom, LPWSTR *pErrorCode, LPWSTR *pDetailMsg)
{
	std::wstring fullpath = GetFullPathName (lpPkgPath ? lpPkgPath : L"");
	if (fullpath.empty ()) fullpath = lpPkgPath ? lpPkgPath : L"";
	return AddAppxPackageFromURI (fullpath.c_str (), alpDepUrlList, dwDeployOption, pfCallback, pCustom, pErrorCode, pDetailMsg);
}
std::wstring WinRTStringToStdString (String ^pstr)
{
	try { if (!pstr) return L""; return (pstr->Data () ? pstr->Data () : L""); }
	catch (...) {} return L"";
}
#define PlatformGetStringValue(mpObj, _prop_) \
    ([&] () -> std::wstring { \
        auto strptr = (mpObj)->_prop_; \
        return WinRTStringToStdString (strptr); \
    }) ()
void SidToAccountName (const std::wstring &sidString, std::wstring &stringSid)
{
	PSID sid = nullptr;
	raii lrel ([&sid] () {
		if (sid) LocalFree (sid);
		sid = nullptr;
	});
	if (ConvertStringSidToSidW (sidString.c_str (), &sid))
	{
		DWORD nameCharCount = 0;
		DWORD domainNameCharCount = 0;
		SID_NAME_USE sidType;
		LookupAccountSidW (nullptr, sid, nullptr, &nameCharCount, nullptr, &domainNameCharCount, &sidType);
		std::vector <WCHAR> namebuf (nameCharCount + 1);
		std::vector <WCHAR> domainNameBuf (domainNameCharCount + 1);
		ZeroMemory (namebuf.data (), (nameCharCount + 1) * sizeof (WCHAR));
		ZeroMemory (domainNameBuf.data (), (domainNameCharCount + 1) * sizeof (WCHAR));
		if (LookupAccountSidW (nullptr, sid, namebuf.data (), &nameCharCount, domainNameBuf.data (), &domainNameCharCount, &sidType))
		{
			stringSid = domainNameBuf.data ();
			stringSid += stringSid + L"\\" + namebuf.data ();
		}
	}
	if (stringSid.length () == 0) stringSid = sidString;
}
struct pkg_info
{
	struct identity
	{
		std::wstring name = L"";
		std::wstring publisher = L"";
		std::wstring fullname = L"";
		std::wstring familyname = L"";
		std::wstring resourceid = L"";
		std::wstring publisherid = L"";
		uint16_t architecture = -1;
		Version version;
	} id;
	struct properities
	{
		std::wstring displayname = L"";
		std::wstring description = L"";
		std::wstring publisher = L"";
		std::wstring logo = L"";
		bool framework = false;
		bool resource_package = false;
	} prop;
	bool bundle = false;
	bool development_mode = false;
	std::wstring installlocation = L"";
	std::wstring users = L"";
	std::wstring sids = L"";
	std::vector <pkg_info> dependencies;
	static pkg_info parse (Windows::ApplicationModel::Package ^pkg, Windows::Management::Deployment::PackageManager ^mgr)
	{
		pkg_info pi;
		if (!pkg) throw ref new InvalidArgumentException ("No package found.");
	#define WAPParseSetValue(_left_part_, _right_part_, _default_value_) \
	do { try { _left_part_ = _right_part_; } catch (...) { _left_part_ = _default_value_; } } while (false)
	#define WAPParseSetStringValue(_left_part_, _right_part_) \
	WAPParseSetValue (_left_part_, WinRTStringToStdString (_right_part_), L"")
		WAPParseSetStringValue (pi.id.name, pkg->Id->Name);
		WAPParseSetStringValue (pi.id.publisher, pkg->Id->Publisher);
		WAPParseSetStringValue (pi.id.fullname, pkg->Id->FullName);
		WAPParseSetStringValue (pi.id.familyname, pkg->Id->FamilyName);
		WAPParseSetStringValue (pi.id.publisherid, pkg->Id->PublisherId);
		WAPParseSetStringValue (pi.id.resourceid, pkg->Id->ResourceId);
		try { pi.id.version = version (pkg->Id->Version.Major, pkg->Id->Version.Minor, pkg->Id->Version.Build, pkg->Id->Version.Revision); } catch (...) {}
		WAPParseSetValue (pi.id.architecture, (WORD)pkg->Id->Architecture, (WORD)-1);
		WAPParseSetStringValue (pi.prop.displayname, pkg->DisplayName);
		WAPParseSetStringValue (pi.prop.description, pkg->Description);
		WAPParseSetStringValue (pi.prop.publisher, pkg->PublisherDisplayName);
		WAPParseSetStringValue (pi.prop.logo, pkg->Logo->ToString ());
		WAPParseSetValue (pi.prop.framework, pkg->IsFramework, false);
		WAPParseSetValue (pi.prop.resource_package, pkg->IsResourcePackage, false);
		WAPParseSetValue (pi.bundle, pkg->IsBundle, false);
		WAPParseSetValue (pi.development_mode, pkg->IsDevelopmentMode, false);
		WAPParseSetStringValue (pi.installlocation, pkg->InstalledLocation->Path);
		try
		{
			size_t i = 0;
			auto users = mgr->FindUsers (pkg->Id->FullName);
			for (auto it : users)
			{
				{
					std::wstring sid = L"";
					SidToAccountName (it->UserSecurityId->Data (), sid);
					if (i) pi.users += L';';
					pi.users += sid;
				}
				{
					std::wstring sid;
					WAPParseSetStringValue (sid, it->UserSecurityId);
					if (i) pi.sids += L';';
					pi.users += sid;
				}
				i ++;
			}
		}
		catch (...) {}
		try
		{
			auto deps = pkg->Dependencies;
			for (auto it : deps)
			{
				auto deppkg = pkg_info::parse (it, mgr);
				deppkg.dependencies.clear ();
				pi.dependencies.push_back (deppkg);
			}
		}
		catch (...) {}
		return pi;
	#ifdef WAPParseSetStringValue
	#undef WAPParseSetStringValue
	#endif
	#ifdef WAPParseSetValue
	#undef WAPParseSetValue
	#endif
	}
	// 返回的反而是缓冲区，因为是柔性结构体要动态申请。这个方法很常用
	void to_c_struct (std::vector <BYTE> &bytesret) const
	{
		bytesret.clear ();
		bytesret.resize (sizeof (FIND_PACKAGE_INFO) * (this->dependencies.size () + 1));
		FIND_PACKAGE_INFO &fpi = *(FIND_PACKAGE_INFO *)bytesret.data ();
		fpi.piIdentity.lpFamilyName = this->id.familyname.c_str ();
		fpi.piIdentity.lpFullName = this->id.fullname.c_str ();
		fpi.piIdentity.lpName = this->id.name.c_str ();
		fpi.piIdentity.lpPublisher = this->id.publisher.c_str ();
		fpi.piIdentity.lpPublisherId = this->id.publisherid.c_str ();
		fpi.piIdentity.lpResourceId = this->id.resourceid.c_str ();
		fpi.piIdentity.qwVersion = this->id.version.data ();
		fpi.piIdentity.wProcessArchitecture = this->id.architecture;
		fpi.piProperties.bIsBundle = this->bundle;
		fpi.piProperties.bIsDevelopmentMode = this->development_mode;
		fpi.piProperties.bIsFramework = this->prop.framework;
		fpi.piProperties.bIsResourcePackage = this->prop.resource_package;
		fpi.piProperties.lpDescription = this->prop.description.c_str ();
		fpi.piProperties.lpDisplayName = this->prop.displayname.c_str ();
		fpi.piProperties.lpLogoUri = this->prop.logo.c_str ();
		fpi.piProperties.lpPublisher = this->prop.publisher.c_str ();
		fpi.dwDependencesSize = this->dependencies.size ();
		fpi.lpInstallLocation = this->installlocation.c_str ();
		fpi.lpUsers = this->users.c_str ();
		FIND_PACKAGE_INFO *deps = (FIND_PACKAGE_INFO *)&fpi.ullBuffer;
		for (size_t i = 0; i < this->dependencies.size (); i ++)
		{
			std::vector <BYTE> depbytes;
			this->dependencies [i].to_c_struct (depbytes);
			deps [i] = *(FIND_PACKAGE_INFO *)depbytes.data ();
		}
	}
};
[STAThread]
HRESULT FindAppxPackageByCallback (std::function <void (pkg_info &)> pfCallback, LPWSTR *pErrorCode, LPWSTR *pDetailMsg)
{
	g_swExceptionCode = L"";
	g_swExceptionDetail = L"";
	try
	{
		auto pkgmgr = ref new PackageManager ();
		auto pkgarr = pkgmgr->FindPackages ();
		for (auto pkg : pkgarr)
		{
			auto pkginfo = pkg_info::parse (pkg, pkgmgr);
			if (pfCallback) pfCallback (pkginfo);
		}
		return S_OK;
	}
	catch (AccessDeniedException ^e)
	{
		g_swExceptionDetail = e->ToString ()->Data ();
		if (pDetailMsg) *pDetailMsg = _wcsdup (g_swExceptionDetail.c_str ());
		return (SUCCEEDED ((HRESULT)e->HResult) ? E_FAIL : (HRESULT)e->HResult);
	}
	catch (Exception ^e)
	{
		g_swExceptionDetail = e->ToString ()->Data ();
		if (pDetailMsg) *pDetailMsg = _wcsdup (g_swExceptionDetail.c_str ());
		return (SUCCEEDED ((HRESULT)e->HResult) ? E_FAIL : (HRESULT)e->HResult);
	}
	catch (const std::exception &e)
	{
		g_swExceptionDetail = StringToWString (e.what () ? e.what () : "Unknown exception.");
		if (pDetailMsg) *pDetailMsg = _wcsdup (g_swExceptionDetail.c_str ());
		return E_FAIL;
	}
	catch (...)
	{
		g_swExceptionDetail = L"Unknown exception";
		if (pDetailMsg) *pDetailMsg = _wcsdup (g_swExceptionDetail.c_str ());
		return E_FAIL;
	}
	return E_FAIL;
}
[STAThread]
HRESULT GetAppxPackages (PKGMGR_FINDENUMCALLBACK pfCallback, void *pCustom, LPWSTR *pErrorCode, LPWSTR *pDetailMsg)
{
	return FindAppxPackageByCallback ([&pCustom, &pfCallback] (pkg_info &pi) {
		std::vector <BYTE> bytes;
		pi.to_c_struct (bytes);
		if (pfCallback) pfCallback ((FIND_PACKAGE_INFO *)bytes.data (), pCustom);
	}, pErrorCode, pDetailMsg);
}
[MTAThread]
HRESULT RemoveAppxPackage (LPCWSTR lpPkgFullName, PKGMRR_PROGRESSCALLBACK pfCallback, void *pCustom, LPWSTR *pErrorCode, LPWSTR *pDetailMsg)
{
	return RunPackageManagerOperation ([=] () {
		auto pkgmgr = ref new PackageManager ();
		return pkgmgr->RemovePackageAsync (ref new String (lpPkgFullName));
	}, pfCallback, pCustom, pErrorCode, pDetailMsg);
}
[MTAThread]
HRESULT CleanupAppxPackage (LPCWSTR lpPkgName, LPCWSTR lpUserSID, PKGMRR_PROGRESSCALLBACK pfCallback, void *pCustom, LPWSTR *pErrorCode, LPWSTR *pDetailMsg)
{
	return RunPackageManagerOperation ([=] () {
		auto pkgmgr = ref new PackageManager ();
		return pkgmgr->CleanupPackageForUserAsync (ref new String (lpPkgName), ref new String (lpUserSID));
	}, pfCallback, pCustom, pErrorCode, pDetailMsg);
}
[MTAThread]
HRESULT RegisterAppxPackageByUri (LPCWSTR lpManifestUri, PCREGISTER_PACKAGE_DEFENDENCIES alpDependencyUriList, DWORD dwDeployOption, PKGMRR_PROGRESSCALLBACK pfCallback, void *pCustom, LPWSTR *pErrorCode, LPWSTR *pDetailMsg)
{
	return RunPackageManagerOperation ([=] () {
		auto pkgmgr = ref new PackageManager ();
		auto depuris = ref new Platform::Collections::Vector <Uri ^> ();
		if (alpDependencyUriList)
		{
			for (size_t i = 0; i < alpDependencyUriList->dwSize; i ++)
			{
				auto &pstr = alpDependencyUriList->alpDepUris [i];
				try
				{
					if (pstr && *pstr)
					{
						auto depuristr = ref new String (pstr);
						Uri ^duri = nullptr;
						try { duri = ref new Uri (depuristr); }
						catch (Exception ^e)
						{
							try
							{
								std::wstring fullpath = GetFullPathName (pstr ? pstr : L"");
								if (fullpath.empty ()) fullpath = pstr ? pstr : L"";
								duri = ref new Uri (ref new String (fullpath.c_str ()));
							}
							catch (Exception ^e) { continue; }
						}
						depuris->Append (duri);
					}
				}
				catch (Exception ^e) { continue; }
			}
		}
		if (depuris->Size > 0) depuris = nullptr;
		return pkgmgr->RegisterPackageAsync (ref new Uri (ref new String (lpManifestUri)), depuris, (DeploymentOptions)dwDeployOption);
	}, pfCallback, pCustom, pErrorCode, pDetailMsg);
}
[MTAThread]
HRESULT RegisterAppxPackageByPath (LPCWSTR lpManifestPath, PCREGISTER_PACKAGE_DEFENDENCIES alpDependencyUriList, DWORD dwDeployOption, PKGMRR_PROGRESSCALLBACK pfCallback, void *pCustom, LPWSTR *pErrorCode, LPWSTR *pDetailMsg)
{
	std::wstring fullpath = GetFullPathName (lpManifestPath ? lpManifestPath : L"");
	if (fullpath.empty ()) fullpath = lpManifestPath ? lpManifestPath : L"";
	return RegisterAppxPackageByUri (fullpath.c_str (), alpDependencyUriList, dwDeployOption, pfCallback, pCustom, pErrorCode, pDetailMsg);
}
[MTAThread]
HRESULT RegisterAppxPackageByFullName (LPCWSTR lpPackageFullName, PCREGISTER_PACKAGE_DEFENDENCIES alpDepFullNameList, DWORD dwDeployOption, PKGMRR_PROGRESSCALLBACK pfCallback, void *pCustom, LPWSTR *pErrorCode, LPWSTR *pDetailMsg)
{
	return RunPackageManagerOperation ([=] () {
		auto pkgmgr = ref new PackageManager ();
		auto depuris = ref new Platform::Collections::Vector <String ^> ();
		if (alpDepFullNameList)
		{
			for (size_t i = 0; i < alpDepFullNameList->dwSize; i ++)
			{
				auto &pstr = alpDepFullNameList->alpDepUris [i];
				try { if (pstr && *pstr) depuris->Append (ref new String (pstr)); }
				catch (Exception ^e) { continue; }
			}
		}
		if (depuris->Size > 0) depuris = nullptr;
		return pkgmgr->RegisterPackageByFullNameAsync (ref new String (lpPackageFullName), depuris, (DeploymentOptions)dwDeployOption);
	}, pfCallback, pCustom, pErrorCode, pDetailMsg);
}
template <typename TFunction> HRESULT ExecPackageManagerFunctionNoReturn (TFunction func, LPWSTR *pErrorCode, LPWSTR *pDetailMsg)
{
	g_swExceptionCode = L"";
	g_swExceptionDetail = L"";
	try
	{
		func ();
		return S_OK;
	}
	catch (Exception ^e)
	{
		g_swExceptionDetail = e->ToString ()->Data ();
		if (pDetailMsg) *pDetailMsg = _wcsdup (g_swExceptionDetail.c_str ());
		return (SUCCEEDED ((HRESULT)e->HResult) ? E_FAIL : (HRESULT)e->HResult);
	}
	catch (const std::exception &e)
	{
		g_swExceptionDetail = StringToWString (e.what () ? e.what () : "Unknown exception.");
		if (pDetailMsg) *pDetailMsg = _wcsdup (g_swExceptionDetail.c_str ());
		return E_FAIL;
	}
	catch (...)
	{
		g_swExceptionDetail = L"Unknown exception";
		if (pDetailMsg) *pDetailMsg = _wcsdup (g_swExceptionDetail.c_str ());
		return E_FAIL;
	}
	return E_FAIL;
}
[STAThread]
HRESULT SetAppxPackageStatus (LPCWSTR lpPackageFullName, DWORD dwStatus, LPWSTR *pErrorCode, LPWSTR *pDetailMsg)
{
	return ExecPackageManagerFunctionNoReturn ([=] () {
		auto pkgmgr = ref new PackageManager ();
		pkgmgr->SetPackageState (ref new String (lpPackageFullName), (Windows::Management::Deployment::PackageState)dwStatus);
	}, pErrorCode, pDetailMsg);
}
[MTAThread]
HRESULT StageAppxPackageFromURI (LPCWSTR lpFileUri, PCREGISTER_PACKAGE_DEFENDENCIES alpDepUriList, DWORD dwDeployOption, PKGMRR_PROGRESSCALLBACK pfCallback, void *pCustom, LPWSTR *pErrorCode, LPWSTR *pDetailMsg)
{
	return RunPackageManagerOperation ([=] () {
		auto pkgmgr = ref new PackageManager ();
		auto depuris = ref new Platform::Collections::Vector <Uri ^> ();
		if (alpDepUriList)
		{
			for (size_t i = 0; i < alpDepUriList->dwSize; i ++)
			{
				auto &pstr = alpDepUriList->alpDepUris [i];
				try
				{
					if (pstr && *pstr)
					{
						auto depuristr = ref new String (pstr);
						Uri ^duri = nullptr;
						try { duri = ref new Uri (depuristr); }
						catch (Exception ^e)
						{
							try
							{
								std::wstring fullpath = GetFullPathName (pstr ? pstr : L"");
								if (fullpath.empty ()) fullpath = pstr ? pstr : L"";
								duri = ref new Uri (ref new String (fullpath.c_str ()));
							}
							catch (Exception ^e) { continue; }
						}
						depuris->Append (duri);
					}
				}
				catch (Exception ^e) { continue; }
			}
		}
		if (depuris->Size > 0) depuris = nullptr;
		return pkgmgr->StagePackageAsync (ref new Uri (ref new String (lpFileUri)), depuris, (DeploymentOptions)dwDeployOption);
	}, pfCallback, pCustom, pErrorCode, pDetailMsg);
}
[MTAThread]
HRESULT StageAppxPackageFromPath (LPCWSTR lpPkgPath, PCREGISTER_PACKAGE_DEFENDENCIES alpDepUriList, DWORD dwDeployOption, PKGMRR_PROGRESSCALLBACK pfCallback, void *pCustom, LPWSTR *pErrorCode, LPWSTR *pDetailMsg)
{
	std::wstring fullpath = GetFullPathName (lpPkgPath ? lpPkgPath : L"");
	if (fullpath.empty ()) fullpath = lpPkgPath ? lpPkgPath : L"";
	return StageAppxPackageFromURI (fullpath.c_str (), alpDepUriList, dwDeployOption, pfCallback, pCustom, pErrorCode, pDetailMsg);
}
[MTAThread]
HRESULT StageAppxUserData (LPCWSTR lpPackageFullName, PKGMRR_PROGRESSCALLBACK pfCallback, void *pCustom, LPWSTR *pErrorCode, LPWSTR *pDetailMsg)
{
	return RunPackageManagerOperation ([=] () {
		auto pkgmgr = ref new PackageManager ();
		return pkgmgr->StageUserDataAsync (ref new String (lpPackageFullName));
	}, pfCallback, pCustom, pErrorCode, pDetailMsg);
}
[MTAThread]
HRESULT UpdateAppxPackageFromURI (LPCWSTR lpPkgFileUri, PCREGISTER_PACKAGE_DEFENDENCIES alpDepUrlList, DWORD dwDeployOption, PKGMRR_PROGRESSCALLBACK pfCallback, void *pCustom, LPWSTR *pErrorCode, LPWSTR *pDetailMsg)
{
	return RunPackageManagerOperation ([=] () {
		auto pkgmgr = ref new PackageManager ();
		auto depuris = ref new Platform::Collections::Vector <Uri ^> ();
		if (alpDepUrlList)
		{
			for (size_t i = 0; i < alpDepUrlList->dwSize; i ++)
			{
				auto &pstr = alpDepUrlList->alpDepUris [i];
				try
				{
					if (pstr && *pstr)
					{
						auto depuristr = ref new String (pstr);
						Uri ^duri = nullptr;
						try { duri = ref new Uri (depuristr); }
						catch (Exception ^e)
						{
							try
							{
								std::wstring fullpath = GetFullPathName (pstr ? pstr : L"");
								if (fullpath.empty ()) fullpath = pstr ? pstr : L"";
								duri = ref new Uri (ref new String (fullpath.c_str ()));
							}
							catch (Exception ^e) { continue; }
						}
						depuris->Append (duri);
					}
				}
				catch (Exception ^e) { continue; }
			}
		}
		if (depuris->Size > 0) depuris = nullptr;
		return pkgmgr->UpdatePackageAsync (ref new Uri (ref new String (lpPkgFileUri)), depuris, (DeploymentOptions)dwDeployOption);
	}, pfCallback, pCustom, pErrorCode, pDetailMsg);
}
[MTAThread]
HRESULT UpdateAppxPackageFromPath (LPCWSTR lpPkgPath, PCREGISTER_PACKAGE_DEFENDENCIES alpDepUrlList, DWORD dwDeployOption, PKGMRR_PROGRESSCALLBACK pfCallback, void *pCustom, LPWSTR *pErrorCode, LPWSTR *pDetailMsg)
{
	std::wstring fullpath = GetFullPathName (lpPkgPath ? lpPkgPath : L"");
	if (fullpath.empty ()) fullpath = lpPkgPath ? lpPkgPath : L"";
	return UpdateAppxPackageFromURI (fullpath.c_str (), alpDepUrlList, dwDeployOption, pfCallback, pCustom, pErrorCode, pDetailMsg);
}
[STAThread]
HRESULT FindAppxPackage (LPCWSTR lpPackageFullName, PKGMGR_FINDENUMCALLBACK pfCallback, void *pCustom, LPWSTR *pErrorCode, LPWSTR *pDetailMsg)
{
	g_swExceptionCode = L"";
	g_swExceptionDetail = L"";
	try
	{
		auto pkgmgr = ref new PackageManager ();
		auto pkg = pkgmgr->FindPackage (ref new String (lpPackageFullName));
		auto pkginfo = pkg_info::parse (pkg, pkgmgr);
		std::vector <BYTE> bytes;
		pkginfo.to_c_struct (bytes);
		if (pfCallback) pfCallback ((FIND_PACKAGE_INFO *)bytes.data (), pCustom);
		return S_OK;
	}
	catch (AccessDeniedException ^e)
	{
		g_swExceptionDetail = e->ToString ()->Data ();
		if (pDetailMsg) *pDetailMsg = _wcsdup (g_swExceptionDetail.c_str ());
		return (SUCCEEDED ((HRESULT)e->HResult) ? E_FAIL : (HRESULT)e->HResult);
	}
	catch (Exception ^e)
	{
		g_swExceptionDetail = e->ToString ()->Data ();
		if (pDetailMsg) *pDetailMsg = _wcsdup (g_swExceptionDetail.c_str ());
		return (SUCCEEDED ((HRESULT)e->HResult) ? E_FAIL : (HRESULT)e->HResult);
	}
	catch (const std::exception &e)
	{
		g_swExceptionDetail = StringToWString (e.what () ? e.what () : "Unknown exception.");
		if (pDetailMsg) *pDetailMsg = _wcsdup (g_swExceptionDetail.c_str ());
		return E_FAIL;
	}
	catch (...)
	{
		g_swExceptionDetail = L"Unknown exception";
		if (pDetailMsg) *pDetailMsg = _wcsdup (g_swExceptionDetail.c_str ());
		return E_FAIL;
	}
	return E_FAIL;
}
[STAThread]
LPCWSTR GetPackageManagerLastErrorCode () { return g_swExceptionCode.c_str (); }
[STAThread]
LPCWSTR GetPackageManagerLastErrorDetailMessage () { return g_swExceptionDetail.c_str (); }

HRESULT ActivateAppxApplication (LPCWSTR lpAppUserId, PDWORD pdwProcessId)
{
	if (!lpAppUserId) return E_INVALIDARG;
	std::wstring strAppUserModelId (L"");
	if (lpAppUserId) strAppUserModelId += lpAppUserId;
	IApplicationActivationManager *spAppActivationManager = nullptr;
	destruct relaamgr ([&] () {
		if (spAppActivationManager) spAppActivationManager->Release ();
		spAppActivationManager = nullptr;
	});
	HRESULT hResult = E_INVALIDARG;
	if (!strAppUserModelId.empty ())
	{
		// Instantiate IApplicationActivationManager
		hResult = CoCreateInstance (CLSID_ApplicationActivationManager, NULL, CLSCTX_LOCAL_SERVER, IID_IApplicationActivationManager, (LPVOID *)&spAppActivationManager);
		if (SUCCEEDED (hResult))
		{
			// This call ensures that the app is launched as the foreground window
			hResult = CoAllowSetForegroundWindow (spAppActivationManager, NULL);
			// Launch the app
			if (SUCCEEDED (hResult)) hResult = spAppActivationManager->ActivateApplication (strAppUserModelId.c_str (), NULL, AO_NONE, pdwProcessId);
		}
	}
	return hResult;
}