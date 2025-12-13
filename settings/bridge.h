#pragma once
#include <Windows.h>
#include "mpstr.h"
#include "nstring.h"
#include "filepath.h"
#include <combaseapi.h>
#include <rapidjson\document.h>
#include <rapidjson\writer.h>
#include <rapidjson\stringbuffer.h>
#include <codecvt>
#include <locale>
#include <ShlObj.h>
#include "mpstr.h"
#include "strcode.h"
using namespace System;
using namespace System::IO;
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
String ^StringArrayToJson (array <String ^> ^strs)
{
	using namespace rapidjson;
	Document doc;
	doc.SetArray ();
	Document::AllocatorType &allocator = doc.GetAllocator ();
	for each (String ^s in strs)
	{
		std::wstring ws = MPStringToStdW (s);  // String^ → std::wstring
		std::string  utf8 = WStringToString (ws, CP_UTF8); // 简易宽转 UTF-8，如需更严谨可用 WideCharToMultiByte
		doc.PushBack (Value (utf8.c_str (), allocator), allocator);
	}
	StringBuffer buffer;
	Writer <StringBuffer> writer (buffer);
	doc.Accept (writer);
	std::string json = buffer.GetString ();
	std::wstring wjson = StringToWString (json, CP_UTF8);
	return CStringToMPString (wjson);
}
std::wstring StringArrayToJson (const std::vector<std::wstring>& arr)
{
	using namespace rapidjson;
	Document doc;
	doc.SetArray ();
	auto &allocator = doc.GetAllocator ();
	for (const auto &ws : arr)
	{
		std::string utf8 = WStringToUtf8 (ws);
		doc.PushBack (Value (utf8.c_str (), allocator), allocator);
	}
	StringBuffer buffer;
	Writer <StringBuffer> writer (buffer);
	doc.Accept (writer);
	return Utf8ToWString (buffer.GetString ());
}
[ComVisible (true)]
public ref class _I_Path
{
	public:
	property String ^Current
	{
		String ^get () { return CStringToMPString (GetCurrentDirectoryW ()); }
		void set (String ^dir) { SetCurrentDirectoryW (MPStringToStdW (dir).c_str ()); }
	}
	property String ^Program { String ^get () { return CStringToMPString (GetCurrentProgramPathW ()); } }
	property String ^Root { String ^get () { return CStringToMPString (GetFileDirectoryW (GetCurrentProgramPathW ())); }}
	String ^Combine (String ^l, String ^r) { return CStringToMPString (CombinePath (MPStringToStdW (l), MPStringToStdW (r))); }
	String ^GetName (String ^path)
	{
		std::wstring cpath = MPStringToStdW (path);
		LPWSTR lp = PathFindFileNameW (cpath.c_str ());
		return lp ? CStringToMPString (lp) : String::Empty;
	}
	String ^GetDirectory (String ^path) { return CStringToMPString (GetFileDirectoryW (MPStringToStdW (path))); }
	String ^GetDir (String ^path) { return GetDirectory (path); }
	bool Exist (String ^path) { return IsPathExists (MPStringToStdW (path)); }
	bool FileExist (String ^filepath) { return IsFileExists (MPStringToStdW (filepath)); }
	bool DirectoryExist (String ^dirpath) { return IsDirectoryExists (MPStringToStdW (dirpath)); }
	bool DirExist (String ^dirpath) { return DirectoryExist (dirpath); }
	String ^GetEnvironmentString (String ^str) { return CStringToMPString (ProcessEnvVars (MPStringToStdW (str))); }
	bool ValidName (String ^filename) { return IsValidWindowsName (MPStringToStdW (filename)); }
	// 过滤器用"\"分隔每个类型
	String ^EnumFilesToJson (String ^dir, String ^filter, bool withpath, bool sort, bool includesub)
	{
		std::vector <std::wstring> res;
		::EnumFiles (MPStringToStdW (dir), MPStringToStdW (filter), res, withpath, sort, includesub);
		return CStringToMPString (StringArrayToJson (res));
	}
	String ^EnumDirsToJson (String ^dir, bool withpath, bool sort, bool includesub)
	{
		std::vector <std::wstring> res;
		EnumDirectory (MPStringToStdW (dir), res, withpath, sort, includesub);
		return CStringToMPString (StringArrayToJson (res));
	}
	String ^EnumSubDirsToJson (String ^dir, bool withpath)
	{
		std::vector <std::wstring> res = EnumSubdirectories (MPStringToStdW (dir), withpath);
		return CStringToMPString (StringArrayToJson (res));
	}
	array <String ^> ^EnumFiles (String ^dir, String ^filter, bool withpath, bool sort, bool includesub)
	{
		std::vector <std::wstring> res;
		::EnumFiles (MPStringToStdW (dir), MPStringToStdW (filter), res, withpath, sort, includesub);
		auto retarr = gcnew array <String ^> (res.size ());
		for (size_t i = 0; i < res.size (); i ++)
		{
			retarr [i] = CStringToMPString (res [i]);
		}
		return retarr;
	}
	array <String ^> ^EnumDirs (String ^dir, bool withpath, bool sort, bool includesub)
	{
		std::vector <std::wstring> res;
		EnumDirectory (MPStringToStdW (dir), res, withpath, sort, includesub);
		auto retarr = gcnew array <String ^> (res.size ());
		for (size_t i = 0; i < res.size (); i ++)
		{
			retarr [i] = CStringToMPString (res [i]);
		}
		return retarr;
	}
	array <String ^> ^EnumSubDirs (String ^dir, bool withpath)
	{
		std::vector <std::wstring> res = EnumSubdirectories (MPStringToStdW (dir), withpath);
		auto retarr = gcnew array <String ^> (res.size ());
		for (size_t i = 0; i < res.size (); i ++)
		{
			retarr [i] = CStringToMPString (res [i]);
		}
		return retarr;
	}
	String ^CommonPrefix (String ^path1, String ^path2) { return CStringToMPString (PathCommonPrefix (MPStringToStdW (path1), MPStringToStdW (path2))); }
	String ^EnsureDirSlash (String ^dir) { return CStringToMPString (EnsureTrailingSlash (MPStringToStdW (dir))); }
	String ^Normalize (String ^path) { return CStringToMPString (NormalizePath (MPStringToStdW (path))); }
	String ^FullPathName (String ^path) { return CStringToMPString (GetFullPathName (MPStringToStdW (path))); }
	String ^FullPath (String ^path) { return FullPathName (path); }
	String ^Expand (String ^path) { return CStringToMPString (ProcessEnvVars (MPStringToStdW (path))); }
	String ^GetFolder (int csidl)
	{
		WCHAR buf [1024] = {0};
		HRESULT hr = SHGetFolderPathW (NULL, csidl, NULL, 0, buf);
		if (SUCCEEDED (hr)) return CStringToMPString (buf);
		else return String::Empty;
	}
	String ^KnownFolder (String ^guidString)
	{
		if (String::IsNullOrWhiteSpace (guidString)) return String::Empty;
		std::wstring wguid = MPStringToStdW (guidString);
		KNOWNFOLDERID kfid;
		HRESULT hr = CLSIDFromString (wguid.c_str (), &kfid);
		if (FAILED (hr)) return String::Empty;
		PWSTR path = nullptr;
		hr = SHGetKnownFolderPath (kfid, 0, NULL, &path);
		if (FAILED (hr) || path == nullptr) return L"";
		std::wstring result (path ? path : L"");
		if (path) CoTaskMemFree (path);
		return CStringToMPString (result);
	}
	bool PEquals (String ^l, String ^r) { return PathEquals (MPStringToStdW (l), MPStringToStdW (r)); }
};
[ComVisible (true)]
public ref class _I_Entry
{
	protected:
	String ^path;
	public:
	_I_Entry (String ^path): path (path) {}
	_I_Entry (): path (String::Empty) {}
	property String ^Path { String ^get () { return path; } void set (String ^file) { path = file; } }
	property String ^Name
	{
		String ^get ()
		{
			std::wstring file = MPStringToStdW (path);
			LPWSTR lpstr = PathFindFileNameW (file.c_str ());
			return lpstr ? CStringToMPString (lpstr) : String::Empty;
		}
	}
	property String ^Directory { String ^get () { return CStringToMPString (GetFileDirectoryW (MPStringToStdW (path))); }}
	property String ^Root { String ^get () { return Directory; }}
	property bool Exist { virtual bool get () { return IsPathExists (MPStringToStdW (path)); }}
	property String ^Uri
	{
		String ^get ()
		{
			using namespace System;
			try
			{
				System::Uri ^uri = nullptr;
				try
				{
					uri = gcnew System::Uri (path);
				} 
				catch (Exception ^)
				{
					uri = gcnew System::Uri (System::IO::Path::GetFullPath (path));
				}
				if (!uri) return String::Empty;
				auto uriText = uri->AbsoluteUri;
				return uriText;
			}
			catch (...) { return String::Empty; }
		}
	}
	property String ^FullPath { String ^get () { return System::IO::Path::GetFullPath (path); }}
	String ^RelativePath (String ^frontdir)
	{
		auto filepath = path;
		if (String::IsNullOrEmpty (filepath) || String::IsNullOrEmpty (frontdir)) return String::Empty;
		try
		{
			String ^fullFile = System::IO::Path::GetFullPath (filepath);
			String ^fullDir = System::IO::Path::GetFullPath (frontdir);
			if (!fullDir->EndsWith (System::IO::Path::DirectorySeparatorChar.ToString ()))
				fullDir += System::IO::Path::DirectorySeparatorChar;

			// 比较盘符（跨盘直接失败）
			if (!String::Equals (
				System::IO::Path::GetPathRoot (fullFile),
				System::IO::Path::GetPathRoot (fullDir),
				StringComparison::OrdinalIgnoreCase))
			{
				return String::Empty;
			}

			// 必须以目录为前缀
			if (!fullFile->StartsWith (fullDir, StringComparison::OrdinalIgnoreCase))
				return String::Empty;

			// 截取相对部分
			return fullFile->Substring (fullDir->Length);
		}
		catch (Exception^)
		{
			return String::Empty;
		}
	}
};
[ComVisible (true)]
public ref class _I_File: public _I_Entry
{
	protected:
	System::Text::Encoding ^lastEncoding;
	public:
	_I_File (String ^filepath): _I_Entry (filepath) {}
	_I_File (): _I_Entry (String::Empty) {}
	String ^Get ()
	{
		using namespace System::IO;
		if (String::IsNullOrEmpty (path)) return String::Empty;
		FileStream ^fs = nullptr;
		StreamReader ^sr = nullptr;
		try
		{
			fs = gcnew FileStream (
				path,
				FileMode::OpenOrCreate,
				FileAccess::ReadWrite,
				FileShare::ReadWrite
			);
			sr = gcnew StreamReader (fs, Encoding::UTF8, true);
			String ^text = sr->ReadToEnd ();
			auto lastEncoding = sr->CurrentEncoding;
			return text;
		}
		catch (...)
		{
			return nullptr;
		}
		finally
		{
			if (sr) delete sr;
			if (fs) delete fs;
		}
	}
	void Set (String ^content)
	{
		using namespace System::IO;
		if (String::IsNullOrEmpty (path)) return;
		String ^dir = System::IO::Path::GetDirectoryName (path);
		if (!String::IsNullOrEmpty (dir) && !System::IO::Directory::Exists (dir))
		{
			System::IO::Directory::CreateDirectory (dir);
		}
		Encoding ^enc = lastEncoding ? lastEncoding : Encoding::UTF8;
		FileStream ^fs = nullptr;
		StreamWriter ^sw = nullptr;
		try
		{
			fs = gcnew FileStream (
				path,
				FileMode::Create,        // 创建或覆盖
				FileAccess::Write,       // 只写即可
				FileShare::Read          // 允许别人读
			);
			sw = gcnew StreamWriter (fs, enc);
			sw->Write (content);
			sw->Flush ();
		}
		finally
		{
			if (sw) delete sw;
			if (fs) delete fs;
		}
	}
	property String ^Content
	{
		String ^get () { return Get (); }
		void set (String ^value) { Set (value); }
	}
	property bool Exist { bool get () override { return IsFileExists (MPStringToStdW (path)); }}
	property String ^FilePath { String ^get () { return this->Path; } void set (String ^value) { this->Path = value; }}
};
[ComVisible (true)]
public ref class _I_Directory: public _I_Entry
{
	public:
	_I_Directory (String ^dirpath): _I_Entry (dirpath) {}
	_I_Directory (_I_Entry ^file): _I_Entry (file->Directory) {}
	_I_Directory (): _I_Entry (String::Empty) {}
	property String ^DirectoryPath { String ^get () { return this->Path; } void set (String ^value) { this->Path = value; } }
	property String ^DirPath { String ^get () { return this->DirectoryPath; } void set (String ^value) { this->DirectoryPath = value; } }
	property bool Exist { bool get () override { return IsDirectoryExists (MPStringToStdW (path)); }}
	String ^EnumFilesToJson (String ^filter, bool withpath, bool sort, bool includesub)
	{
		std::vector <std::wstring> res;
		::EnumFiles (MPStringToStdW (DirPath), MPStringToStdW (filter), res, withpath, sort, includesub);
		return CStringToMPString (StringArrayToJson (res));
	}
	String ^EnumDirsToJson (bool withpath, bool sort, bool includesub)
	{
		std::vector <std::wstring> res;
		EnumDirectory (MPStringToStdW (DirPath), res, withpath, sort, includesub);
		return CStringToMPString (StringArrayToJson (res));
	}
	String ^EnumSubDirsToJson (bool withpath)
	{
		std::vector <std::wstring> res = EnumSubdirectories (MPStringToStdW (DirPath), withpath);
		return CStringToMPString (StringArrayToJson (res));
	}
	array <String ^> ^EnumFiles (String ^filter, bool withpath, bool sort, bool includesub)
	{
		std::vector <std::wstring> res;
		::EnumFiles (MPStringToStdW (DirPath), MPStringToStdW (filter), res, withpath, sort, includesub);
		auto retarr = gcnew array <String ^> (res.size ());
		for (size_t i = 0; i < res.size (); i ++)
		{
			retarr [i] = CStringToMPString (res [i]);
		}
		return retarr;
	}
	array <String ^> ^EnumDirs (bool withpath, bool sort, bool includesub)
	{
		std::vector <std::wstring> res;
		EnumDirectory (MPStringToStdW (DirPath), res, withpath, sort, includesub);
		auto retarr = gcnew array <String ^> (res.size ());
		for (size_t i = 0; i < res.size (); i ++)
		{
			retarr [i] = CStringToMPString (res [i]);
		}
		return retarr;
	}
	array <String ^> ^EnumSubDirs (bool withpath)
	{
		std::vector <std::wstring> res = EnumSubdirectories (MPStringToStdW (DirPath), withpath);
		auto retarr = gcnew array <String ^> (res.size ());
		for (size_t i = 0; i < res.size (); i ++)
		{
			retarr [i] = CStringToMPString (res [i]);
		}
		return retarr;
	}
};
[ComVisible (true)]
public ref class _I_Storage
{
	protected:
	_I_Path ^path = gcnew _I_Path ();
	public:
	property _I_Path ^Path { _I_Path ^get () { return path; }}
	_I_File ^GetFile (String ^path) { return gcnew _I_File (path); }
	_I_Directory ^GetDirectory (String ^path) { return gcnew _I_Directory (path); }
	_I_Directory ^GetDir (String ^path) { return GetDirectory (path); }
};

