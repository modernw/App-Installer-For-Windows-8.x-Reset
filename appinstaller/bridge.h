#pragma once
#include <Windows.h>
#include "mpstr.h"
#include "nstring.h"
#include <combaseapi.h>
using namespace System;
using namespace System::Runtime::InteropServices;

std::wstring HResultToMessage (HRESULT hr)
{
	_com_error err (hr);
	auto msgptr = err.ErrorMessage ();
	return msgptr ? msgptr : L"";
}
[ComVisible (true)]
public ref class _I_HResult
{
	private:
	HRESULT hr = S_OK;
	String ^errorcode = "";
	String ^detailmsg = "";
	public:
	_I_HResult (HRESULT hres)
	{
		hr = hres;
		detailmsg = CStringToMPString (HResultToMessage (hres));
	}
	_I_HResult (HRESULT hres, String ^error, String ^message)
	{
		hr = hres;
		errorcode = error;
		detailmsg = message;
	}
	property HRESULT HResult { HRESULT get () { return hr; }}
	property String ^ErrorCode { String ^get () { return errorcode; }}
	property String ^Message { String ^get () { return detailmsg; }}
	property bool Succeeded { bool get () { return SUCCEEDED (hr); }}
	property bool Failed { bool get () { return FAILED (hr); }}
};
System::String ^FormatString (System::String ^fmt, ... array <Object ^> ^args) { return System::String::Format (fmt, args); }

String ^ EscapeToInnerXml (String ^str)
{
	using namespace System::Xml;
	auto doc = gcnew System::Xml::XmlDocument ();
	doc->LoadXml ("<body></body>");
	auto root = doc->FirstChild;
	root->InnerText = str;
	return root->InnerXml;
}
std::wstring EscapeToInnerXml (const std::wstring &str) { return MPStringToStdW (EscapeToInnerXml (CStringToMPString (str))); }
[ComVisible (true)]
public ref class _I_String
{
	public:
	ref class _I_NString
	{
		public:
		bool NEquals (String ^l, String ^r) { return IsNormalizeStringEquals (MPStringToPtrW (l), MPStringToPtrW (r)); }
		bool Empty (String ^l) { return IsNormalizeStringEmpty (MPStringToStdW (l)); }
		int Compare (String ^l, String ^r) { return NormalizeStringCompare (MPStringToPtrW (l), MPStringToPtrW (r)); }
		int Length (String ^l) { return GetNormalizeStringLength (MPStringToStdW (l)); }
	};
	private:
	_I_NString ^nstr = gcnew _I_NString ();
	public:
	property _I_NString ^NString { _I_NString ^get () { return nstr; }}
	String ^Trim (String ^src)
	{
		std::wstring csrc = MPStringToStdW (src);
		return CStringToMPString (::StringTrim (csrc));
	}
	String ^ToLower (String ^src) { return CStringToMPString (StringToLower (MPStringToStdW (src))); }
	String ^ToUpper (String ^src) { return CStringToMPString (StringToUpper (MPStringToStdW (src))); }
	String ^Format (String ^fmt, ... array <Object ^> ^args) { return FormatString (fmt, args); }
	String ^FormatInnerHTML (String ^fmt, ... array <Object ^> ^args)
	{
		std::wstring ihtml = EscapeToInnerXml (MPStringToStdW (fmt));
		auto pih = CStringToMPString (ihtml);
		auto newargs = gcnew array <Object ^> (args->Length);
		for (size_t i = 0; i < args->Length; i ++)
		{
			auto %p = newargs [i];
			p = Format ("<span>{0}</span>", EscapeToInnerXml (Format ("{0}", args [i])));
		}
		return Format (pih, newargs);
	}
};