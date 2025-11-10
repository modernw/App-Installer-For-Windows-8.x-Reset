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
LPWSTR GetToastNoticeXml (LPCWSTR lpTemplateName)
{
	auto xmldoc = ref new Windows::Data::Xml::Dom::XmlDocument ();
	auto root = xmldoc->CreateElement ("toast");
	xmldoc->AppendChild (root);
	auto visual = xmldoc->CreateElement ("visual");
	root->AppendChild (visual);
	auto binding = xmldoc->CreateElement ("binding");
	visual->AppendChild (binding);
	std::wnstring temp = lpTemplateName ? lpTemplateName : L"";
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
	return _wcsdup (xmldoc->GetXml ()->Data ());
}
LPWSTR GenerateSimpleToastNoticeXml (LPCWSTR lpText, LPCWSTR lpImagePath)
{
	auto xmldoc = ref new Windows::Data::Xml::Dom::XmlDocument ();
	std::wnstring img = lpImagePath ? lpImagePath : L"";
	std::wstring text = std::wnstring::trim (std::wstring (lpText ? lpText : L""));
	{
		std::wstring xmltemplate = L"<toast><visual><binding template='ToastGeneric'><text></text></binding></visual></toast>";
		LPWSTR xt = nullptr;
		raii relt ([&xt] () {
			if (xt) free (xt);
			xt = nullptr;
		});
		xt = GetToastNoticeXml (img.empty () ? L"ToastText01" : L"ToastImageAndText01");
		if (xt && *xt) xmltemplate = xt;
		xmldoc->LoadXml (ref new Platform::String (xmltemplate.c_str ()));
	}
	Windows::Foundation::Uri ^imguri = nullptr;
	try { imguri = ref new Windows::Foundation::Uri (ref new Platform::String (img.c_str ())); }
	catch (...)
	{
		try
		{
			std::wstring fullpath = GetFullPathName (lpImagePath ? lpImagePath : L"");
			if (fullpath.empty ()) fullpath = lpImagePath ? lpImagePath : L"";
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
	return _wcsdup (xmldoc->GetXml ()->Data ());
}
LPWSTR GenerateSimpleToastNoticeXml2 (LPCWSTR lpTitle, LPCWSTR lpText, LPCWSTR lpImagePath)
{
	auto xmldoc = ref new Windows::Data::Xml::Dom::XmlDocument ();
	std::wnstring img = lpImagePath ? lpImagePath : L"";
	std::wstring title = std::wnstring (lpTitle ? lpTitle : L"").trim ();
	std::wstring text = std::wnstring (lpText ? lpText : L"").trim ();
	{
		std::wstring xmltemplate = L"<toast><visual><binding template='ToastGeneric'><text></text></binding></visual></toast>";
		LPWSTR xt = nullptr;
		raii relt ([&xt] () {
			if (xt) free (xt);
			xt = nullptr;
		});
		std::wstring templatename = L"";
		WORD flag = (bool)(!img.empty ()) << 2 | (bool)title.size () << 1 | (bool)text.size ();
		switch (flag)
		{
			case 0b001: templatename = L"ToastText01"; break;  // 仅正文
			case 0b011: templatename = L"ToastText02"; break;  // 标题 + 正文
			case 0b101: templatename = L"ToastImageAndText01"; break; // 图 + 正文
			case 0b111: templatename = L"ToastImageAndText02"; break; // 图 + 标题 + 正文
			default:    templatename = L"ToastText01"; break;
		}
		xt = GetToastNoticeXml (templatename.c_str ());
		if (xt && *xt) xmltemplate = xt;
		xmldoc->LoadXml (ref new Platform::String (xmltemplate.c_str ()));
	}
	Windows::Foundation::Uri ^imguri = nullptr;
	try { imguri = ref new Windows::Foundation::Uri (ref new Platform::String (img.c_str ())); }
	catch (...)
	{
		try
		{
			std::wstring fullpath = GetFullPathName (lpImagePath ? lpImagePath : L"");
			if (fullpath.empty ()) fullpath = lpImagePath ? lpImagePath : L"";
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
HRESULT CreateToastNoticeFromXmlDocument (LPCWSTR lpIdName, LPCWSTR lpXmlString, NOTICE_ACTIVECALLBACK pfCallback, void *pCustom, LPWSTR *lpExceptMsg)
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
		if (lpIdName && *lpIdName) notifier = ToastMgr::CreateToastNotifier (ref new String (lpIdName));
		else notifier = ToastMgr::CreateToastNotifier ();
		auto xmldoc = ref new XmlDoc ();
		xmldoc->LoadXml (ref new String (lpXmlString));
		auto toast = ref new Toast (xmldoc);
		toast->Activated += ref new Windows::Foundation::TypedEventHandler <
			Windows::UI::Notifications::ToastNotification ^,
			Platform::Object ^
		> ([=] (Windows::UI::Notifications::ToastNotification ^sender, Platform::Object ^args) {
			if (pfCallback) pfCallback (pCustom);
		});
		notifier->Show (toast);
		return S_OK;
	}
	catch_lasterr (&hr, lpExceptMsg);
	return hr;
}



