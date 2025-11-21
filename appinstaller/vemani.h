#pragma once
#include <Windows.h>
#include <pugiconfig.hpp>
#include <pugixml.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <codecvt>
#include <locale>
#include <cstdio>
#include "strcmp.h"
#include "dynarr.h"
#include "themeinfo.h"
#include "nstring.h"

class vemanifest
{
	public:
	enum class TextColor { dark = 0x000000, light = 0xFFFFFF };
	vemanifest (): available (false) {}
	vemanifest (LPCSTR filename): available (false) { create (filename); }
	vemanifest (const std::string &filename): available (false) { create (filename); }
	vemanifest (LPCWSTR filename): available (false) { create (filename); }
	vemanifest (const std::wstring &filename): available (false) { create (filename); }
	vemanifest (std::ifstream &stream): available (false) { create (stream); }
	vemanifest (std::fstream &stream): available (false) { create (stream); }
	vemanifest (FILE *file): available (false) { create (file); }
	~vemanifest () { destroy (); }
	bool create (LPCSTR filename)
	{
		destroy ();
		pugi::xml_parse_result result = doc.load_file (filename);
		available = result; return available;
	}
	bool create (const std::string &filename) { return create (filename.c_str ()); }
	bool create (LPCWSTR filename)
	{
		destroy ();
		std::wstring ws (filename);
		std::string s = to_utf8 (ws);
		pugi::xml_parse_result result = doc.load_file (s.c_str ());
		available = result; return available;
	}
	bool create (const std::wstring &filename) { return create (filename.c_str ()); }
	bool create (std::ifstream &stream)
	{
		destroy ();
		std::stringstream buffer; buffer << stream.rdbuf ();
		std::string content = buffer.str ();
		pugi::xml_parse_result result = doc.load_string (content.c_str ());
		available = result; return available;
	}
	bool create (std::fstream &stream)
	{
		destroy ();
		std::stringstream buffer; buffer << stream.rdbuf ();
		std::string content = buffer.str ();
		pugi::xml_parse_result result = doc.load_string (content.c_str ());
		available = result; return available;
	}
	bool create (FILE *file)
	{
		destroy ();
		if (!file) { return false; }
		fseek (file, 0, SEEK_END);
		long length = ftell (file);
		fseek (file, 0, SEEK_SET);
		std::string content (length, '\0');
		fread (&content [0], 1, length, file);
		pugi::xml_parse_result result = doc.load_string (content.c_str ());
		available = result; return available;
	}
	void destroy () { if (!available) return; doc.reset (); available = false; }
	bool valid () const { return available; }
	std::string display_name (const std::string &id = "App") const
	{
		pugi::xml_node visual = visual_element_node (id);
		return visual ? visual.attribute ("DisplayName").as_string () : "";
	}
	std::wstring display_name (const std::wstring &id = L"App") const { return pugi::as_wide (this->display_name (pugi::as_utf8 (id))); }
	std::string logo (const std::string &id = "App") const
	{
		pugi::xml_node visual = visual_element_node (id);
		if (!visual) return "";
		std::string logo = visual.attribute ("Logo").as_string ();
		return !logo.empty () ? logo : visual.attribute ("Square150x150Logo").as_string ();
	}
	std::wstring logo (const std::wstring &id = L"App") const { return pugi::as_wide (this->logo (pugi::as_utf8 (id))); }
	std::string small_logo (const std::string &id = "App") const
	{
		pugi::xml_node visual = visual_element_node (id);
		if (!visual) return "";
		std::string smallLogo = visual.attribute ("SmallLogo").as_string ();
		return !smallLogo.empty () ? smallLogo : visual.attribute ("Square70x70Logo").as_string ();
	}
	std::wstring small_logo (const std::wstring &id = L"App") const { return pugi::as_wide (this->small_logo (pugi::as_utf8 (id))); }
	TextColor foreground_text (const std::string &id = "App") const
	{
		pugi::xml_node visual = visual_element_node (id);
		if (!visual) return TextColor::dark;
		std::string fg = visual.attribute ("ForegroundText").as_string ();
		return (fg == "light") ? TextColor::light : TextColor::dark;
	}
	TextColor foreground_text (const std::wstring &id = L"App") const { return (this->foreground_text (pugi::as_utf8 (id))); }
	std::string lnk_32x32_logo (const std::string &id = "App") const
	{
		pugi::xml_node visual = visual_element_node (id);
		return visual ? visual.attribute ("Lnk32x32Logo").as_string () : "";
	}
	std::wstring lnk_32x32_logo (const std::wstring &id = L"App") const { return pugi::as_wide (this->lnk_32x32_logo (pugi::as_utf8 (id))); }
	std::string item_display_logo (const std::string &id = "App") const
	{
		pugi::xml_node visual = visual_element_node (id);
		if (!visual) return "";
		std::string itemLogo = visual.attribute ("ItemDisplayLogo").as_string ();
		if (!itemLogo.empty ()) return itemLogo;
		itemLogo = visual.attribute ("Lnk32x32Logo").as_string ();
		return !itemLogo.empty () ? itemLogo : visual.attribute ("Square44x44Logo").as_string ();
	}
	std::wstring item_display_logo (const std::wstring &id = L"App") const { return pugi::as_wide (this->item_display_logo (pugi::as_utf8 (id))); }
	bool show_name_on_tile (const std::string &id = "App") const
	{
		pugi::xml_node visual = visual_element_node (id);
		return visual ? (std::string (visual.attribute ("ShowNameOnSquare150x150Logo").as_string ()) == "on") : false;
	}
	bool show_name_on_tile (const std::wstring &id = L"App") const { return (this->show_name_on_tile (pugi::as_utf8 (id))); }
	std::string background_color (const std::string &id = "App") const
	{
		pugi::xml_node visual = visual_element_node (id);
		return visual ? visual.attribute ("BackgroundColor").as_string () : "";
	}
	std::wstring background_color (const std::wstring &id = L"App") const { return pugi::as_wide (this->background_color (pugi::as_utf8 (id))); }
	std::string splash_screen_image (const std::string &id = "App") const
	{
		pugi::xml_node visual = visual_element_node (id);
		if (!visual) return "";
		pugi::xml_node splash = visual.child ("SplashScreen");
		return splash ? splash.attribute ("Image").as_string () : "";
	}
	std::wstring splash_screen_image (const std::wstring &id = L"App") const { return pugi::as_wide (this->splash_screen_image (pugi::as_utf8 (id))); }
	std::string splash_screen_backgroundcolor (const std::string &id = "App") const
	{
		pugi::xml_node visual = visual_element_node (id);
		if (!visual) return "";
		pugi::xml_node splash = visual.child ("SplashScreen");
		std::string bg = splash ? splash.attribute ("BackgroundColor").as_string () : "";
		return !bg.empty () ? bg : visual.attribute ("BackgroundColor").as_string ();
	}
	std::wstring splash_screen_backgroundcolor (const std::wstring &id = L"App") const { return pugi::as_wide (this->splash_screen_backgroundcolor (pugi::as_utf8 (id))); }
	bool is_appid_exists (const std::string &id) const
	{
		pugi::xml_node root = doc.document_element ();
		std::string rootName = root.name ();
		if (rootName == "Applications")
		{
			for (pugi::xml_node app : root.children ("Application"))
			{
				pugi::xml_attribute attr = app.attribute ("Id");
				if (attr && LabelEqual (std::string (attr.value ()), id)) return true;
			}
			return pugi::xml_node ();
		}
		else if (rootName == "Application")
		{
			pugi::xml_attribute attr = root.attribute ("Id");
			if (attr && LabelEqual (std::string (attr.value ()), id)) return true;
		}
		return false;
	}
	bool is_appid_exists (const std::wstring &id) const { return this->is_appid_exists (pugi::as_utf8 (id)); }
	size_t app_ids (std::vector <std::string> &output) const
	{
		if (!&output) return 0;
		output.clear ();
		pugi::xml_node root = doc.document_element ();
		std::string rootName = root.name ();
		if (rootName == "Applications")
		{
			for (pugi::xml_node app : root.children ("Application"))
			{
				pugi::xml_attribute attr = app.attribute ("Id");
				if (attr)
				{
					LPCSTR lp = attr.value ();
					if (lp) push_unique (output, std::string (lp));
				}
			}
		}
		else if (rootName == "Application")
		{
			pugi::xml_attribute attr = root.attribute ("Id");
			if (attr)
			{
				if (attr)
				{
					LPCSTR lp = attr.value ();
					if (lp) push_unique (output, std::string (lp));
				}
			}
		}
		if (!output.size ()) push_unique (output, std::string ("App"));
		return output.size ();
	}
	size_t app_ids (std::vector <std::wstring> &output) const
	{
		if (!&output) return 0;
		output.clear ();
		pugi::xml_node root = doc.document_element ();
		std::string rootName = root.name ();
		if (rootName == "Applications")
		{
			for (pugi::xml_node app : root.children ("Application"))
			{
				pugi::xml_attribute attr = app.attribute ("Id");
				if (attr)
				{
					LPCSTR lp = attr.value ();
					if (lp) push_unique (output, pugi::as_wide (lp));
				}
			}
		}
		else if (rootName == "Application")
		{
			pugi::xml_attribute attr = root.attribute ("Id");
			if (attr)
			{
				if (attr)
				{
					LPCSTR lp = attr.value ();
					if (lp) push_unique (output, pugi::as_wide (lp));
				}
			}
		}
		if (!output.size ()) push_unique (output, std::wstring (L"App"));
		return output.size ();
	}
	private:
	pugi::xml_document doc;
	bool available;
	// 根据 id 查找 VisualElements 节点：若根节点为 <Applications>，遍历所有 <Application> 查找其 <VisualElements> 节点中属性 Id 等于 id；若根节点为 <Application>，直接返回其 <VisualElements>
	pugi::xml_node visual_element_node (const std::string &id) const
	{
		pugi::xml_node root = doc.document_element ();
		std::string rootName = root.name ();
		if (rootName == "Applications")
		{
			for (pugi::xml_node app : root.children ("Application"))
			{
				pugi::xml_attribute attr = app.attribute ("Id");
				if (attr && LabelEqual (std::string (attr.value ()), id))
				{
					pugi::xml_node visual = app.child ("VisualElements");
					if (visual) return visual;
				}
			}
			return pugi::xml_node ();
		}
		else if (rootName == "Application") return root.child ("VisualElements");
		return pugi::xml_node ();
	}
	std::string to_utf8 (const std::wstring &wstr) const
	{
		std::wstring_convert <std::codecvt_utf8 <wchar_t>> conv;
		return conv.to_bytes (wstr);
	}
};

class resxmldoc
{
	private:
	std::wstring filepath;
	pugi::xml_document doc;
	bool available = false;
	std::string to_utf8 (const std::wstring &wstr) const
	{
		std::wstring_convert <std::codecvt_utf8 <wchar_t>> conv;
		return conv.to_bytes (wstr);
	}
	public:
	resxmldoc (const std::wstring &xmlpath) { create (xmlpath); }
	void destroy () { if (!available) return; doc.reset (); available = false; }
	~resxmldoc () { destroy (); }
	bool create (const std::wstring &xmlpath)
	{
		destroy ();
		std::wstring ws (filepath = xmlpath);
		std::string s = to_utf8 (ws);
		pugi::xml_parse_result result = doc.load_file (s.c_str ());
		available = result; return available;
	}
	std::string get (const std::string &id) const
	{
		auto root = doc.first_child ();
		auto nodes = root.children ();
		for (auto it : nodes)
		{
			if (IsNormalizeStringEquals (std::string (it.attribute ("id").as_string ()), id))
			{
				auto scales = it.children ();
				std::map <int, std::string> s_v;
				for (auto it_s : scales)
				{
					std::string dpi = it_s.attribute ("dpi").as_string ();
					if (IsNormalizeStringEquals (dpi.c_str (), "default")) { s_v [0] = it_s.text ().get (); }
					else
					{
						try { s_v [it_s.attribute ("dpi").as_int ()] = it_s.text ().get (); }
						catch (...) {}
					}
				}
				std::vector <std::pair <int, std::string>> existfilepath;
				{
					std::vector <std::pair <int, std::string>> vec (s_v.begin (), s_v.end ());
					std::sort (vec.begin (), vec.end (), [] (auto &a, auto &b) { return a.second < b.second; });
					for (auto &it_ss : vec)
					{
						it_ss.second = pugi::as_utf8 (CombinePath (GetFileDirectoryW (filepath), pugi::as_wide (it_ss.second)));
						if (IsFileExists (pugi::as_wide (it_ss.second))) existfilepath.push_back (it_ss);
					}
				}
				int dpipercent = GetDPI ();
				for (auto &it_path : existfilepath)
				{
					if (it_path.first >= dpipercent) return it_path.second;
				}
				if (!existfilepath.empty ()) return existfilepath.at (0).second;
				return "";
				break;
			}
		}
		return "";
	}
	std::wstring get (const std::wstring &id) const { return pugi::as_wide (get (WStringToString (id))); }
	std::wstring operator [] (const std::wstring &id) const { return get (id); }
	std::wstring operator [] (const std::wstring &id) { return get (id); }
	std::string operator [] (const std::string &id) const { return get (id); }
	std::string operator [] (const std::string &id) { return get (id); }
};