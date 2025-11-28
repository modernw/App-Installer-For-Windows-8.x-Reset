#pragma once
#include <vector>
#include <string>
#include <cstdint>
typedef uint64_t UINT64;
typedef int64_t INT64;
typedef uint16_t UINT16;
typedef struct version
{
	UINT16 major = 0, minor = 0, build = 0, revision = 0;
	version (UINT64 value):
		major ((value >> 0x30) & 0xFFFF), minor ((value >> 0x20) & 0xFFFF),
		build ((value >> 0x10) & 0xFFFF), revision ((value) & 0xFFFF) {}
	version (UINT16 major, UINT16 minor, UINT16 build, UINT16 revision):
		major (major), minor (minor), build (build), revision (revision) {}
	version (const std::wstring &verstr) { this->interpret (verstr); }
	version (const std::string &verstr) { this->interpret (verstr); }
	version () {}
	version (const version &other): major (other.major), minor (other.minor), build (other.build), revision (other.revision) {}
	version (version &&other) noexcept: major (other.major), minor (other.minor), build (other.build), revision (other.revision) {}
	version &operator = (const version &other)
	{
		if (this != &other) 
		{
			major = other.major;
			minor = other.minor;
			build = other.build;
			revision = other.revision;
		}
		return *this;
	}
	version &operator = (version &&other) noexcept
	{
		if (this != &other)
		{
			major = other.major;
			minor = other.minor;
			build = other.build;
			revision = other.revision;
		}
		return *this;
	}
	version &operator = (UINT64 value) { this->data (value); return *this; }
	UINT64 data () const { return (((UINT64)major) << 48) | (((UINT64)minor) << 32) | (((UINT64)build) << 16) | ((UINT64)revision); }
	UINT64 data (UINT64 value)
	{
		major = (value >> 48) & 0xFFFF;
		minor = (value >> 32) & 0xFFFF;
		build = (value >> 16) & 0xFFFF;
		revision = value & 0xFFFF;
		return value;
	}
	std::wstring stringifyw () const
	{
		std::wstringstream ss;
		ss << major << L'.' << minor << L'.' << build << L'.' << revision;
		return ss.str ();
	}
	std::string stringify () const
	{
		std::stringstream ss;
		ss << major << '.' << minor << '.' << build << '.' << revision;
		return ss.str ();
	}
	version &interpret (const std::wstring &verstr)
	{
		auto result = split (verstr);
		if (result.size () > 0) this->major = _wtoi (result [0].c_str ());
		if (result.size () > 1) this->minor = _wtoi (result [1].c_str ());
		if (result.size () > 2) this->build = _wtoi (result [2].c_str ());
		if (result.size () > 3) this->revision = _wtoi (result [3].c_str ());
		return *this;
	}
	version &interpret (const std::string &verstr)
	{
		auto result = split (verstr);
		if (result.size () > 0) this->major = atoi (result [0].c_str ());
		if (result.size () > 1) this->minor = atoi (result [1].c_str ());
		if (result.size () > 2) this->build = atoi (result [2].c_str ());
		if (result.size () > 3) this->revision = atoi (result [3].c_str ());
		return *this;
	}
	bool empty () const { return *(UINT64 *)this == 0; }
	friend bool operator == (const version &l, const version &r) { return *(UINT64 *)&l == *(UINT64 *)&r; }
	friend bool operator == (const version &l, const UINT64 &r) { return l.data () == r; }
	friend bool operator == (const UINT64 &r, const version &l) { return l.data () == r; }
	friend bool operator < (const version &l, const version &r) { return l.data () < r.data (); }
	friend bool operator > (const version &l, const version &r) { return l.data () > r.data (); }
	friend bool operator <= (const version &l, const version &r) { return l.data () <= r.data (); }
	friend bool operator >= (const version &l, const version &r) { return l.data () >= r.data (); }
	friend bool operator != (const version &l, const version &r) { return *(UINT64 *)&l != *(UINT64 *)&r; }
	explicit operator bool () const { return !this->empty (); }
	bool operator ! () { return this->empty (); }
	friend std::ostream &operator << (std::ostream &o, const version &v) { return o << v.major << '.' << v.minor << '.' << v.build << '.' << v.revision; }
	friend std::wostream &operator << (std::wostream &o, const version &v) { return o << v.major << '.' << v.minor << '.' << v.build << '.' << v.revision; }
	bool equals (const version &r) const { return *this == r; }
	INT64 compare (const version &r) const { return this->data () - r.data (); }
	static version parse (const std::wstring &value) { return version (value); }
	static version parse (const std::string &value) { return version (value); }
	static std::wstring stringifyw (const version &v) { return v.stringifyw (); }
	static std::string stringify (const version &v) { return v.stringify (); }
	static bool equals (const version &l, const version &r) { return l == r; }
	static INT64 compare (const version &l, const version &r) { return l.data () - r.data (); }
	static version decode (UINT64 value) { return version (value); }
	static UINT64 encode (const version &v) { return v.data (); }
	protected:
	template <typename StringType> std::vector <StringType> split (const StringType &str, typename StringType::value_type delimiter1 = '.', typename StringType::value_type delimiter2 = ',')
	{
		std::vector <StringType> result;
		std::basic_stringstream<typename StringType::value_type> ss (str);
		StringType segment;
		while (std::getline (ss, segment, delimiter1))
		{
			size_t pos = 0;
			while ((pos = segment.find (delimiter2)) != StringType::npos)
			{
				result.push_back (segment.substr (0, pos));
				segment.erase (0, pos + 1);
			}
			if (!segment.empty ()) result.push_back (segment);
		}
		return result;
	}
} Version;

#ifdef __cplusplus_cli
using namespace System;
using namespace System::Runtime::InteropServices;
[ComVisible (true)]
public ref class _I_Version
{
	private:
	UINT16 major = 0, minor = 0, build = 0, revision = 0;
	public:
	property UINT16 Major { UINT16 get () { return major; } void set (UINT16 value) { major = value; } }
	property UINT16 Minor { UINT16 get () { return minor; } void set (UINT16 value) { minor = value; } }
	property UINT16 Build { UINT16 get () { return build; } void set (UINT16 value) { build = value; } }
	property UINT16 Revision { UINT16 get () { return revision; } void set (UINT16 value) { revision = value; } }
	property array <UINT16> ^Data
	{
		array <UINT16> ^get ()
		{
			return gcnew array <UINT16> {
				major, minor, build, revision
			};
		}
		void set (array <UINT16> ^arr)
		{
			major = minor = build = revision = 0;
			for (size_t i = 0; i < arr->Length; i ++)
			{
				switch (i)
				{
					case 0: major = arr [i]; break;
					case 1: minor = arr [i]; break;
					case 2: build = arr [i]; break;
					case 3: revision = arr [i]; break;
					default: break;
				}
			}
		}
	}
	property String ^DataStr
	{
		String ^get () { return Stringify (); }
		void set (String ^str) { Parse (str); }
	}
	_I_Version (UINT16 p_ma, UINT16 p_mi, UINT16 p_b, UINT16 p_r):
		major (p_ma), minor (p_mi), build (p_b), revision (p_r) {}
	_I_Version (UINT16 p_ma, UINT16 p_mi, UINT16 p_b):
		major (p_ma), minor (p_mi), build (p_b), revision (0) {}
	_I_Version (UINT16 p_ma, UINT16 p_mi):
		major (p_ma), minor (p_mi), build (0), revision (0) {}
	_I_Version (UINT16 p_ma):
		major (p_ma), minor (0), build (0), revision (0) {}
	_I_Version () {}
	_I_Version %Parse (String ^ver)
	{
		auto strarr = ver->Split ('.');
		auto arr = gcnew array <UINT16> (4);
		for (size_t i = 0; i < strarr->Length; i ++)
		{
			try { arr [i] = Convert::ToUInt16 (strarr [i]); }
			catch (...) {}
		}
		Data = arr;
		return *this;
	}
	String ^Stringify () { return major + "." + minor + "." + build + "." + revision; }
	String ^ToString () override { return Stringify (); }
	bool Valid () { return Major != 0 && Minor != 0 && Build != 0 && Revision != 0; }
};
#endif