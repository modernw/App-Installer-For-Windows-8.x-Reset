// pkgmgr.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "notice.h"
#include "raii.h"
#include "version.h"
#include "nstring.h"
#ifdef GetFullPathName
#undef GetFullPathName
#endif
#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif
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
Windows::Data::Xml::Dom::XmlDocument ^TemplateToastNoticeXml (const std::wstring &lpTemplateName)
{
	std::wnstring temp = lpTemplateName;
	try
	{
		using toastttype = Windows::UI::Notifications::ToastTemplateType;
		auto tttype = Windows::UI::Notifications::ToastTemplateType::ToastImageAndText01;
		if (temp.equals (L"ToastText01")) tttype = toastttype::ToastText01;
		else if (temp.equals (L"ToastText02")) tttype = toastttype::ToastText02;
		else if (temp.equals (L"ToastText03")) tttype = toastttype::ToastText03;
		else if (temp.equals (L"ToastText04")) tttype = toastttype::ToastText04;
		else if (temp.equals (L"ToastImageAndText01")) tttype = toastttype::ToastImageAndText01;
		else if (temp.equals (L"ToastImageAndText02")) tttype = toastttype::ToastImageAndText02;
		else if (temp.equals (L"ToastImageAndText03")) tttype = toastttype::ToastImageAndText03;
		else if (temp.equals (L"ToastImageAndText04")) tttype = toastttype::ToastImageAndText04;
		else tttype = (toastttype)-1;
		if ((INT16)tttype > 0)
		{
			auto tt = Windows::UI::Notifications::ToastNotificationManager::GetTemplateContent (tttype);
			if (tt) return tt;
		}
	}
	catch (...) {}
	auto xmldoc = ref new Windows::Data::Xml::Dom::XmlDocument ();
	auto root = xmldoc->CreateElement ("toast");
	xmldoc->AppendChild (root);
	auto visual = xmldoc->CreateElement ("visual");
	root->AppendChild (visual);
	auto binding = xmldoc->CreateElement ("binding");
	visual->AppendChild (binding);
	if (temp.equals (L"ToastText01"))
	{
		binding->SetAttribute ("template", "ToastText01");
		auto text1 = xmldoc->CreateElement ("text");
		text1->SetAttribute ("id", "1");
		binding->AppendChild (text1);
	}
	else if (temp.equals (L"ToastText02"))
	{
		binding->SetAttribute ("template", "ToastText02");
		auto text1 = xmldoc->CreateElement ("text");
		text1->SetAttribute ("id", "1");
		auto text2 = xmldoc->CreateElement ("text");
		text2->SetAttribute ("id", "2");
		binding->AppendChild (text1);
		binding->AppendChild (text2);
	}
	else if (temp.equals (L"ToastText03"))
	{
		binding->SetAttribute ("template", "ToastText03");
		auto text1 = xmldoc->CreateElement ("text");
		text1->SetAttribute ("id", "1");
		auto text2 = xmldoc->CreateElement ("text");
		text2->SetAttribute ("id", "2");
		binding->AppendChild (text1);
		binding->AppendChild (text2);
	}
	else if (temp.equals (L"ToastText04"))
	{
		binding->SetAttribute ("template", "ToastText04");
		auto text1 = xmldoc->CreateElement ("text");
		text1->SetAttribute ("id", "1");
		auto text2 = xmldoc->CreateElement ("text");
		text2->SetAttribute ("id", "2");
		auto text3 = xmldoc->CreateElement ("text");
		text3->SetAttribute ("id", "3");
		binding->AppendChild (text1);
		binding->AppendChild (text2);
		binding->AppendChild (text3);
	}
	else if (temp.equals (L"ToastImageAndText01"))
	{
		binding->SetAttribute ("template", "ToastImageAndText01");
		auto text1 = xmldoc->CreateElement ("text");
		text1->SetAttribute ("id", "1");
		auto img1 = xmldoc->CreateElement ("image");
		img1->SetAttribute ("id", "1");
		img1->SetAttribute ("src", "");
		img1->SetAttribute ("alt", "");
		binding->AppendChild (img1);
		binding->AppendChild (text1);
	}
	else if (temp.equals (L"ToastImageAndText02"))
	{
		binding->SetAttribute ("template", "ToastImageAndText02");
		auto text1 = xmldoc->CreateElement ("text");
		text1->SetAttribute ("id", "1");
		auto img1 = xmldoc->CreateElement ("image");
		img1->SetAttribute ("id", "1");
		img1->SetAttribute ("src", "");
		img1->SetAttribute ("alt", "");
		binding->AppendChild (img1);
		binding->AppendChild (text1);
	}
	else if (temp.equals (L"ToastImageAndText03"))
	{
		binding->SetAttribute ("template", "ToastImageAndText03");
		auto text1 = xmldoc->CreateElement ("text");
		text1->SetAttribute ("id", "1");
		auto text2 = xmldoc->CreateElement ("text");
		text2->SetAttribute ("id", "2");
		auto img1 = xmldoc->CreateElement ("image");
		img1->SetAttribute ("id", "1");
		img1->SetAttribute ("src", "");
		img1->SetAttribute ("alt", "");
		binding->AppendChild (img1);
		binding->AppendChild (text1);
		binding->AppendChild (text2);
	}
	else if (temp.equals (L"ToastImageAndText04"))
	{
		binding->SetAttribute ("template", "ToastImageAndText04");
		auto text1 = xmldoc->CreateElement ("text");
		text1->SetAttribute ("id", "1");
		auto text2 = xmldoc->CreateElement ("text");
		text2->SetAttribute ("id", "2");
		auto text3 = xmldoc->CreateElement ("text");
		text3->SetAttribute ("id", "3");
		auto img1 = xmldoc->CreateElement ("image");
		img1->SetAttribute ("id", "1");
		img1->SetAttribute ("src", "");
		img1->SetAttribute ("alt", "");
		binding->AppendChild (img1);
		binding->AppendChild (text1);
		binding->AppendChild (text2);
		binding->AppendChild (text3);
	}
	else
	{
		binding->SetAttribute ("template", "ToastImageAndText04");
		auto text1 = xmldoc->CreateElement ("text");
		binding->AppendChild (text1);
	}
	return xmldoc;
}
LPWSTR GetToastNoticeXml (LPCWSTR lpTemplateName)
{
	auto xmldoc = TemplateToastNoticeXml (lpTemplateName);
	return _wcsdup (xmldoc->GetXml ()->Data ());
}
Windows::Data::Xml::Dom::XmlDocument ^SimpleToastNoticeXml (const std::wstring &lpText, const std::wstring &lpImgPath = L"")
{
	Windows::Data::Xml::Dom::XmlDocument ^xmldoc = nullptr;
	std::wnstring img = lpImgPath;
	std::wstring text = std::wnstring::trim (std::wstring (lpText));
	{
		// std::wstring xmltemplate = L"<toast><visual><binding template='ToastGeneric'><text></text></binding></visual></toast>";
		xmldoc = TemplateToastNoticeXml (img.empty () ? L"ToastText01" : L"ToastImageAndText01");
	}
	Windows::Foundation::Uri ^imguri = nullptr;
	try { imguri = ref new Windows::Foundation::Uri (ref new Platform::String (img.c_str ())); }
	catch (...)
	{
		try
		{
			std::wstring fullpath = GetFullPathName (lpImgPath);
			if (fullpath.empty ()) fullpath = lpImgPath;
			imguri = ref new Windows::Foundation::Uri (ref new Platform::String (img.c_str ()));
		}
		catch (...) { imguri = nullptr; }
	}
	auto toast = xmldoc->FirstChild;
	auto visual = toast->FirstChild;
	auto binding = visual->FirstChild;
	auto binds = binding->ChildNodes;
	auto textNodes = binding->SelectNodes (ref new Platform::String (L"text"));
	if (textNodes && textNodes->Length > 0)
	{
		auto node = dynamic_cast <Windows::Data::Xml::Dom::XmlElement ^> (textNodes->Item (0));
		if (node) node->InnerText = ref new Platform::String (text.c_str ());
	}
	auto imageNodes = binding->SelectNodes (ref new Platform::String (L"image"));
	if (imageNodes && imageNodes->Length > 0 && !img.empty ())
	{
		auto node = dynamic_cast <Windows::Data::Xml::Dom::XmlElement ^> (imageNodes->Item (0));
		if (node)
		{
			node->SetAttribute (L"src", ref new Platform::String (imguri && imguri->ToString ()->Data () ? imguri->ToString ()->Data () : img.c_str ()));
			node->SetAttribute (L"alt", ref new Platform::String (L"image"));
		}
	}
	return xmldoc;
}
LPWSTR GenerateSimpleToastNoticeXml (LPCWSTR lpText, LPCWSTR lpImagePath)
{
	auto xmldoc = SimpleToastNoticeXml (lpText ? lpText : L"", lpImagePath ? lpImagePath : L"");
	return _wcsdup (xmldoc->GetXml ()->Data ());
}
Windows::Data::Xml::Dom::XmlDocument ^SimpleToastNoticeXml2 (const std::wstring &lpTitle, const std::wstring &lpText = L"", const std::wstring &lpImagePath = L"")
{
	Windows::Data::Xml::Dom::XmlDocument ^xmldoc = nullptr;
	std::wnstring img = lpImagePath;
	std::wstring title = std::wnstring (lpTitle).trim ();
	std::wstring text = std::wnstring (lpText).trim ();
	{
		// std::wstring xmltemplate = L"<toast><visual><binding template='ToastGeneric'><text></text></binding></visual></toast>";
		std::wstring templatename = L"";
		WORD flag = (bool)(!img.empty ()) << 2 | (bool)title.size () << 1 | (bool)text.size ();
		switch (flag)
		{
			case 1: templatename = L"ToastText01"; break;  // 仅正文
			case 3: templatename = L"ToastText02"; break;  // 标题 + 正文
			case 5: templatename = L"ToastImageAndText01"; break; // 图 + 正文
			case 7: templatename = L"ToastImageAndText02"; break; // 图 + 标题 + 正文
			default:    templatename = L"ToastText01"; break;
		}
		xmldoc = TemplateToastNoticeXml (templatename);
	}
	Windows::Foundation::Uri ^imguri = nullptr;
	try { imguri = ref new Windows::Foundation::Uri (ref new Platform::String (img.c_str ())); }
	catch (...)
	{
		try
		{
			std::wstring fullpath = GetFullPathName (lpImagePath);
			if (fullpath.empty ()) fullpath = lpImagePath;
			imguri = ref new Windows::Foundation::Uri (ref new Platform::String (img.c_str ()));
		}
		catch (...) { imguri = nullptr; }
	}
	auto binding = xmldoc->SelectSingleNode (ref new Platform::String (L"/toast/visual/binding"));
	auto textNodes = binding->SelectNodes (ref new Platform::String (L"text"));
	if (textNodes && textNodes->Length > 0)
	{
		unsigned int idx = 0;
		if (!title.empty ())
		{
			auto node = dynamic_cast <Windows::Data::Xml::Dom::XmlElement ^> (textNodes->Item (idx++));
			if (node) node->InnerText = ref new Platform::String (title.c_str ());
		}
		if (!text.empty () && idx < textNodes->Length)
		{
			auto node = dynamic_cast <Windows::Data::Xml::Dom::XmlElement ^> (textNodes->Item (idx));
			if (node) node->InnerText = ref new Platform::String (text.c_str ());
		}
		else if (!text.empty () && title.empty ())
		{
			auto node = dynamic_cast <Windows::Data::Xml::Dom::XmlElement ^> (textNodes->Item (0));
			if (node) node->InnerText = ref new Platform::String (text.c_str ());
		}
	}
	auto imageNodes = binding->SelectNodes (ref new Platform::String (L"image"));
	if (imageNodes && imageNodes->Length > 0 && !img.empty ())
	{
		auto node = dynamic_cast <Windows::Data::Xml::Dom::XmlElement ^> (imageNodes->Item (0));
		if (node)
		{
			node->SetAttribute (L"src", ref new Platform::String (imguri && imguri->ToString ()->Data () ? imguri->ToString ()->Data () : img.c_str ()));
			node->SetAttribute (L"alt", ref new Platform::String (L"image"));
		}
	}
	return xmldoc;
}
LPWSTR GenerateSimpleToastNoticeXml2 (LPCWSTR lpTitle, LPCWSTR lpText, LPCWSTR lpImagePath)
{
	auto xmldoc = SimpleToastNoticeXml2 (lpTitle ? lpTitle : L"", lpText ? lpText : L"", lpImagePath ? lpImagePath : L"");
	return _wcsdup (xmldoc->GetXml ()->Data ());
}
// 会实时记录 hResult;
HRESULT g_lasthr = S_OK;
// 记录上一次异常（不会根据操作更新）的信息
std::wstring g_lastexc = L"";
#define catch_lasterr(_PHRESULT_Outptr_lphResult_, _HLPWSTR_Outptr_lpExcepMsg_) \
catch (Platform::Exception ^e) \
{ \
	g_lastexc = e->ToString ()->Data (); \
	if (_HLPWSTR_Outptr_lpExcepMsg_) *(_HLPWSTR_Outptr_lpExcepMsg_) = _wcsdup (g_lastexc.c_str ()); \
	g_lasthr = SUCCEEDED ((HRESULT)e->HResult) ? E_FAIL : (HRESULT)e->HResult; \
	if (_PHRESULT_Outptr_lphResult_) *(_PHRESULT_Outptr_lphResult_) = g_lasthr; \
} \
catch (const std::exception &e) \
{ \
	g_lastexc = StringToWString (e.what () ? e.what () : "Unknown exception."); \
	if (_HLPWSTR_Outptr_lpExcepMsg_) *(_HLPWSTR_Outptr_lpExcepMsg_) = _wcsdup (g_lastexc.c_str ()); \
	g_lasthr = E_FAIL; \
	if (_PHRESULT_Outptr_lphResult_) *(_PHRESULT_Outptr_lphResult_) = g_lasthr; \
} \
catch (...) \
{ \
	g_lastexc = L"Unknown exception"; \
	if (_HLPWSTR_Outptr_lpExcepMsg_) *(_HLPWSTR_Outptr_lpExcepMsg_) = _wcsdup (g_lastexc.c_str ()); \
	g_lasthr = E_FAIL; \
	if (_PHRESULT_Outptr_lphResult_) *(_PHRESULT_Outptr_lphResult_) = g_lasthr; \
}
HRESULT CreateToastNoticeFromXml (const std::wstring &lpIdName, Windows::Data::Xml::Dom::XmlDocument ^pIXml, NOTICE_ACTIVECALLBACK pfCallback = nullptr, void *pCustom = nullptr, LPWSTR *lpExceptMsg = nullptr)
{
	using XmlDoc = Windows::Data::Xml::Dom::XmlDocument;
	using ToastNotification = Windows::UI::Notifications::ToastNotification;
	using Toast = ToastNotification;
	using ToastMgr = Windows::UI::Notifications::ToastNotificationManager;
	using String = Platform::String;
	using Object = Platform::Object;
	auto &hr = g_lasthr;
	if (lpExceptMsg) *lpExceptMsg = nullptr;
	try
	{
		Windows::UI::Notifications::ToastNotifier ^notifier = nullptr;
		if (!std::wnstring (lpIdName).empty ()) notifier = ToastMgr::CreateToastNotifier (ref new String (lpIdName.c_str ()));
		else notifier = ToastMgr::CreateToastNotifier ();
		auto &xmldoc = pIXml;
		auto toast = ref new Toast (xmldoc);
		toast->Activated += ref new Windows::Foundation::TypedEventHandler <
			Windows::UI::Notifications::ToastNotification ^,
			Platform::Object ^
		> ([=] (Windows::UI::Notifications::ToastNotification ^sender, Platform::Object ^args) {
			if (pfCallback) pfCallback (pCustom);
		});
		notifier->Show (toast);
		return hr = S_OK;
	}
	catch_lasterr (&hr, lpExceptMsg);
	return hr;
}
HRESULT CreateToastNoticeFromXml (const std::wstring &lpIdName, const std::wstring &lpXmlString, NOTICE_ACTIVECALLBACK pfCallback = nullptr, void *pCustom = nullptr, LPWSTR *lpExceptMsg = nullptr)
{
	using XmlDoc = Windows::Data::Xml::Dom::XmlDocument;
	using String = Platform::String;
	using Object = Platform::Object;
	if (lpExceptMsg) *lpExceptMsg = nullptr;
	auto &hr = g_lasthr;
	try
	{
		auto xmldoc = ref new XmlDoc ();
		xmldoc->LoadXml (ref new String (lpXmlString.c_str ()));
		return hr = CreateToastNoticeFromXml (lpIdName, xmldoc, pfCallback, pCustom, lpExceptMsg);
	}
	catch_lasterr (&hr, lpExceptMsg);
	return hr;
}
HRESULT CreateToastNoticeFromXmlDocument (LPCWSTR lpIdName, LPCWSTR lpXmlString, NOTICE_ACTIVECALLBACK pfCallback, void *pCustom, LPWSTR *lpExceptMsg)
{
	return CreateToastNoticeFromXml (lpIdName ? lpIdName : L"", lpXmlString ? lpXmlString : L"", pfCallback, pCustom, lpExceptMsg);
}
HRESULT CreateToastNotice2 (LPCWSTR lpIdName, LPCWSTR lpTitle, LPCWSTR lpText, LPCWSTR lpImgPath, NOTICE_ACTIVECALLBACK pfCallback, void *pCustom, LPWSTR *lpExceptMsg)
{
	auto &hr = g_lasthr;
	try
	{
		auto xmldoc = SimpleToastNoticeXml2 (lpTitle ? lpTitle : L"", lpText ? lpText : L"", lpImgPath ? lpImgPath : L"");
		return hr = CreateToastNoticeFromXml (lpIdName, xmldoc, pfCallback, pCustom, lpExceptMsg);
	}
	catch_lasterr (&hr, lpExceptMsg);
	return hr;
}
HRESULT CreateToastNotice (LPCWSTR lpIdName, LPCWSTR lpText, LPCWSTR lpImgPath, NOTICE_ACTIVECALLBACK pfCallback, void *pCustom, LPWSTR *lpExceptMsg)
{
	return CreateToastNotice2 (lpIdName, lpText, nullptr, lpImgPath, pfCallback, pCustom, lpExceptMsg);
}
std::wstring GetRandomText (size_t length = 16, const std::wstring &charset = L"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ")
{
	if (charset.empty ()) return L"";
	static  bool seeded = false;
	if (!seeded)
	{
		std::srand ((unsigned int)(std::time (nullptr) ^ (uintptr_t)&seeded));
		seeded = true;
	}
	std::wstring ret;
	ret.reserve (length);
	for (size_t i = 0; i < length; ++i)
	{
		size_t index = (size_t)(std::rand () % charset.size ());
		ret.push_back (charset [index]);
	}
	return ret;
}
std::wstring CombinePath (const std::wstring &left, const std::wstring &right)
{
	std::vector <WCHAR> buf (left.capacity () + right.capacity () + 2);
	PathCombineW (buf.data (), left.c_str (), right.c_str ());
	return buf.data ();
}
std::wstring GetRamdomFileName (const std::wstring &directory, const std::wstring &ext = L".tmp", bool retfullpath = true)
{
	std::wnstring name = std::wnstring::trim (GetRandomText (3) + L"-" + GetRandomText (5) + L"-" + GetRandomText (3)) + std::wnstring::trim (ext);
	std::wnstring path = CombinePath (directory, name);
	while (GetFileAttributesW (path.c_str ()) != INVALID_FILE_ATTRIBUTES)
	{
		name = GetRandomText (3) + L"-" + GetRandomText (5) + L"-" + GetRandomText (3) + ext;
		path = CombinePath (directory, name);
	}
	if (retfullpath) return path;
	else return name;
}
std::wstring GetTempDirectory ()
{
	std::vector <WCHAR> ret (GetTempPathW (0, nullptr) + 2);
	GetTempPathW (ret.size (), ret.data ());
	return ret.data ();
}
std::wstring GetRamdomTempFileName (const std::wstring &ext = L".tmp", bool retfullpath = true)
{
	return GetRamdomFileName (GetTempDirectory (), ext, retfullpath);
}
std::wstring IStreamToTempFile (IStream *p, const std::wstring &ext = L".tmp")
{
	if (!p) throw ref new Platform::InvalidArgumentException ("IStream is nullptr.");
	std::wnstring outpath = GetRamdomTempFileName (ext);
	auto &stream = p;
	STATSTG stat;
	HRESULT hr = stream->Stat (&stat, STATFLAG_NONAME);
	if (FAILED (hr)) throw Platform::Exception::CreateException (hr);
	LARGE_INTEGER liZero = {};
	stream->Seek (liZero, STREAM_SEEK_SET, nullptr);
	HANDLE hFile = CreateFileW (outpath.c_str (), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_TEMPORARY | FILE_FLAG_DELETE_ON_CLOSE, nullptr);
	const size_t bufsize = 4096;
	BYTE buf [bufsize] = {0};
	ULONG bytesRead = 0;
	DWORD bytesWritten = 0;
	while (true)
	{
		hr = stream->Read (buf, bufsize, &bytesRead);
		if (FAILED (hr)) { CloseHandle (hFile); throw Platform::Exception::CreateException (hr); }
		if (bytesRead == 0) break; 
		if (!WriteFile (hFile, buf, bytesRead, &bytesWritten, nullptr) || bytesWritten != bytesRead)
		{
			CloseHandle (hFile);
			throw std::runtime_error ("WriteFile failed.");
		}
	}
	CloseHandle (hFile);
	return outpath;
}
void DeleteFileThreadSafe (LPWSTR filepath)
{
	raii endt ([&] () {
		if (filepath) free (filepath);
		filepath = nullptr;
	});
	Sleep (5000); 
	DeleteFileW (filepath);
}
HRESULT CreateToastNoticeWithIStream2 (LPCWSTR lpIdName, LPCWSTR lpTitle, LPCWSTR lpText, HANDLE pIImgStream, NOTICE_ACTIVECALLBACK pfCallback, void *pCustom, LPWSTR *lpExceptMsg)
{
	auto &hr = g_lasthr;
	try
	{
		std::wnstring imgpath = L"";
		try
		{
			if (pIImgStream)
			{
				IStream *img = (IStream *)pIImgStream;
				LARGE_INTEGER li;
				li.QuadPart = 0;
				img->Seek (li, STREAM_SEEK_SET, nullptr);
				STATSTG stat;
				hr = img->Stat (&stat, STATFLAG_DEFAULT);
				if (SUCCEEDED (hr) && stat.pwcsName)
				{
					if (stat.pwcsName) imgpath = stat.pwcsName;
					CoTaskMemFree (stat.pwcsName);
				}
				else
				{
					imgpath = IStreamToTempFile (img, L".jpg");
					CreateThread (nullptr, 0, (LPTHREAD_START_ROUTINE)DeleteFileThreadSafe, _wcsdup (imgpath.c_str ()), 0, nullptr);
				}
			}
			Windows::Foundation::Uri ^uri = ref new Windows::Foundation::Uri (ref new Platform::String (imgpath.c_str ()));
		}
		catch (...) { imgpath = L""; }
		return hr = CreateToastNotice2 (lpIdName, lpTitle, lpText, imgpath.c_str (), pfCallback, pCustom, lpExceptMsg);
	}
	catch_lasterr (&hr, lpExceptMsg);
	return hr;
}
HRESULT CreateToastNoticeWithIStream (LPCWSTR lpIdName, LPCWSTR lpText, HANDLE pIImgStream, NOTICE_ACTIVECALLBACK pfCallback, void *pCustom, LPWSTR *lpExceptMsg)
{
	return CreateToastNoticeWithIStream2 (lpIdName, lpText, nullptr, pIImgStream, pfCallback, pCustom, lpExceptMsg);
}
HRESULT NoticeGetLastHResult () { return g_lasthr; }
LPCWSTR NoticeGetLastDetailMessage () { return g_lastexc.c_str (); }
HRESULT CreateShortcutWithAppIdW (LPCWSTR pszShortcutPath, LPCWSTR pszTargetPath, LPCWSTR pszAppId)
{
	HRESULT hr;
	if (FAILED (hr)) return hr;
	IShellLinkW *pShellLinkW = nullptr;
	raii reltask1 ([&] () {
		if (pShellLinkW) pShellLinkW->Release ();
		pShellLinkW = nullptr;
	});
	hr = CoCreateInstance (CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLinkW, (void **)&pShellLinkW);
	if (FAILED (hr)) return hr;
	hr = pShellLinkW->SetPath (pszTargetPath); return hr;
	IPropertyStore *pPropStore = nullptr;
	raii reltask2 ([&] () {
		if (pPropStore) pPropStore->Release ();
		pPropStore = nullptr;
	});
	hr = pShellLinkW->QueryInterface (IID_IPropertyStore, (void **)&pPropStore);
	if (SUCCEEDED (hr))
	{
		PROPVARIANT propvar;
		hr = InitPropVariantFromString (pszAppId, &propvar);
		if (SUCCEEDED (hr))
		{
			hr = pPropStore->SetValue (PKEY_AppUserModel_ID, propvar);
			if (SUCCEEDED (hr)) hr = pPropStore->Commit ();
			PropVariantClear (&propvar);
		}
	}
	else pPropStore = nullptr;
	IPersistFile *pPersistFile = nullptr;
	raii reltask3 ([&] () {
		if (pPersistFile) pPersistFile->Release ();
		pPersistFile = nullptr;
	});
	hr = pShellLinkW->QueryInterface (IID_IPersistFile, (void **)&pPersistFile);
	if (SUCCEEDED (hr)) hr = pPersistFile->Save (pszShortcutPath, TRUE);
	else pPersistFile = nullptr;
	return hr;
}
