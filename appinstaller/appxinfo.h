#pragma once
#include <Windows.h>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <rapidjson\document.h>
#include <rapidjson\writer.h>
#include <rapidjson\stringbuffer.h>
#include <codecvt>
#include <locale>
#include "nstring.h"
#include "priformatcli.h"
#include "pkgread.h"
static std::string ws2utf8 (const std::wstring &ws)
{
	std::wstring_convert <std::codecvt_utf8 <wchar_t>> conv;
	return conv.to_bytes (ws);
}
static rapidjson::Value ver_to_json (const VERSION &ver, rapidjson::Document::AllocatorType &alloc)
{
	rapidjson::Value obj (rapidjson::kObjectType);
	obj.AddMember ("major", ver.major, alloc);
	obj.AddMember ("minor", ver.minor, alloc);
	obj.AddMember ("build", ver.build, alloc);
	obj.AddMember ("revision", ver.revision, alloc);
	return obj;
}

struct pkginfo
{
	std::wstring filepath;
	bool valid = false;
	// 包的类型：Appx 包还是 AppxBundle 包。返回 PKGTYPE_* 宏 
	WORD type = 0;
	// 包的类型：是应用包、框架包还是资源包。
	// 对于 AppxBundle 包永远返回是资源包（因为框架包无法打包成 AppxBundle，资源包必须与应用包一同打包进 AppxBundle）。
	// 返回 PKGROLE_* 宏。
	WORD role = 0;
	struct
	{
		std::wstring name,
			publisher,
			package_family_name,
			package_full_name,
			resource_id;
		VERSION version, realver;
		DWORD architecture;
	} identity;
	struct
	{
		std::wstring display_name;
		std::wstring publisher_display_name;
		std::wstring description;
		std::wstring logo;
		std::wstring logo_base64;
		bool framework = false;
		bool resource_package = false;
	} properties;
	struct application: std::map <std::wnstring, std::wstring>
	{
		using base = std::map <std::wnstring, std::wstring>;
		using base::base;
		application () = default;
		std::wstring user_model_id () { return this->at (L"AppUserModelID"); }
		friend bool operator == (application &a1, application &a2) { return a1.user_model_id () == a2.user_model_id (); }
		friend bool operator != (application &a1, application &a2) { return a1.user_model_id () != a2.user_model_id (); }
		explicit operator bool () { return this->user_model_id ().empty (); }
	};
	std::vector <application> applications;
	struct
	{
		std::vector <std::wnstring> capabilities_name;
		std::vector <std::wnstring> device_capabilities;
	} capabilities;
	struct dependency
	{
		std::wstring name, publisher;
		VERSION vermin;
		dependency (const std::wstring &name = L"", const std::wstring &pub = L"", const VERSION &ver = VERSION ()):
			name (name), publisher (pub), vermin (ver) {}
	};
	std::vector <dependency> dependencies;
	struct
	{
		std::set <std::wnstring> languages;
		std::set <UINT32> scales;
		// PKG_RESOURCES_DXFEATURE_LEVEL* 前缀常量
		std::set <DWORD> dx_feature_levels;
	} resources;
	struct
	{
		VERSION os_min_version, os_max_version_tested;
	} prerequisites;
	static pkginfo parse (const std::wstring &filepath)
	{
		pkginfo pkg;
		pkg.filepath = filepath;
		package_reader pread (filepath);
		pread.use_pri (true);
		pread.enable_pri_convert (true);
		pkg.valid = pread.valid ();
		if (!pkg.valid) return pkg;
		pkg.type = pread.package_type ();
		pkg.role = pread.package_role ();
		{
			auto id = pread.get_identity ();
			pkg.identity.architecture = id.architecture ();
			pkg.identity.name = id.name ();
			pkg.identity.package_family_name = id.package_family_name ();
			pkg.identity.package_full_name = id.package_full_name ();
			pkg.identity.publisher = id.publisher ();
			pkg.identity.resource_id = id.resource_id ();
			pkg.identity.version = id.version ();
			pkg.identity.realver = id.version (true);
		}
		{
			auto prop = pread.get_properties ();
			pkg.properties.description = prop.description ();
			pkg.properties.display_name = prop.display_name ();
			pkg.properties.framework = prop.framework ();
			pkg.properties.logo = prop.logo ();
			pkg.properties.logo_base64 = prop.logo_base64 ();
			pkg.properties.publisher_display_name = prop.publisher_display_name ();
			pkg.properties.resource_package = prop.resource_package ();
		}
		{
			auto prer = pread.get_prerequisites ();
			pkg.prerequisites.os_min_version = prer.os_min_version ();
			pkg.prerequisites.os_max_version_tested = prer.os_max_version_tested ();
		}
		{
			auto apps = pread.get_applications ();
			std::vector <package_reader::application> appmap;
			apps.get (appmap);
			pkg.applications.reserve (appmap.size ());
			for (auto &mp : appmap)
			{
				application app;
				for (auto &it : mp)
				{
					if (std::find (g_filepathitems.begin (), g_filepathitems.end (), it.first) != g_filepathitems.end ())
						app [it.first] = mp.newat_base64 (it.first);
					else app [it.first] = mp.newat (it.first);
				}
				pkg.applications.push_back (app);
			}
		}
		{
			auto caps = pread.get_capabilities ();
			std::vector <std::wstring> vws;
			caps.capabilities_name (vws);
			pkg.capabilities.capabilities_name.reserve (vws.size ());
			for (auto &it : vws) pkg.capabilities.capabilities_name.push_back (it);
			vws.clear ();
			caps.device_capabilities (vws);
			pkg.capabilities.device_capabilities.reserve (vws.size ());
			for (auto &it : vws) pkg.capabilities.device_capabilities.push_back (it);
		}
		{
			std::vector <package_reader::dependency> deps;
			auto depr = pread.get_dependencies ();
			pkg.dependencies.reserve (depr.get (deps));
			for (auto &it : deps)
			{
				dependency dep;
				dep.name = it.name;
				dep.publisher = it.publisher;
				dep.vermin = it.vermin;
				pkg.dependencies.push_back (dep);
			}
		}
		{
			auto ress = pread.get_resources ();
			{
				std::vector <std::wstring> langs;
				ress.languages (langs);
				for (auto &it : langs) pkg.resources.languages.insert (it);
			}
			{
				std::vector <UINT32> ss;
				ress.scales (ss);
				for (auto &it : ss) pkg.resources.scales.insert (it);
			}
			{
				DWORD dx = ress.dx_feature_level ();
				if (dx & PKG_RESOURCES_DXFEATURE_LEVEL9)
					pkg.resources.dx_feature_levels.insert (PKG_RESOURCES_DXFEATURE_LEVEL9);
				else if (dx & PKG_RESOURCES_DXFEATURE_LEVEL10)
					pkg.resources.dx_feature_levels.insert (PKG_RESOURCES_DXFEATURE_LEVEL10);
				else if (dx & PKG_RESOURCES_DXFEATURE_LEVEL11)
					pkg.resources.dx_feature_levels.insert (PKG_RESOURCES_DXFEATURE_LEVEL11);
				else if (dx & PKG_RESOURCES_DXFEATURE_LEVEL12)
					pkg.resources.dx_feature_levels.insert (PKG_RESOURCES_DXFEATURE_LEVEL12);
			}
		}
		return pkg;
	}
	std::wstring parseJson ()
	{
		using namespace rapidjson;
		Document doc;
		doc.SetObject ();
		auto &alloc = doc.GetAllocator ();
		doc.AddMember ("filepath", Value (ws2utf8 (filepath).c_str (), alloc), alloc);
		doc.AddMember ("valid", valid, alloc);
		doc.AddMember ("type", type, alloc);
		doc.AddMember ("role", role, alloc);
		{
			Value obj (kObjectType);
			obj.AddMember ("name", Value (ws2utf8 (identity.name).c_str (), alloc), alloc);
			obj.AddMember ("publisher", Value (ws2utf8 (identity.publisher).c_str (), alloc), alloc);
			obj.AddMember ("package_family_name", Value (ws2utf8 (identity.package_family_name).c_str (), alloc), alloc);
			obj.AddMember ("package_full_name", Value (ws2utf8 (identity.package_full_name).c_str (), alloc), alloc);
			obj.AddMember ("resource_id", Value (ws2utf8 (identity.resource_id).c_str (), alloc), alloc);
			obj.AddMember ("architecture", (uint32_t)identity.architecture, alloc);
			obj.AddMember ("version", ver_to_json (identity.version, alloc), alloc);
			obj.AddMember ("realver", ver_to_json (identity.realver, alloc), alloc);
			doc.AddMember ("identity", obj, alloc);
		}
		{
			Value obj (kObjectType);
			obj.AddMember ("display_name", Value (ws2utf8 (properties.display_name).c_str (), alloc), alloc);
			obj.AddMember ("publisher_display_name", Value (ws2utf8 (properties.publisher_display_name).c_str (), alloc), alloc);
			obj.AddMember ("description", Value (ws2utf8 (properties.description).c_str (), alloc), alloc);
			obj.AddMember ("logo", Value (ws2utf8 (properties.logo).c_str (), alloc), alloc);
			obj.AddMember ("logo_base64", Value (ws2utf8 (properties.logo_base64).c_str (), alloc), alloc);
			obj.AddMember ("framework", properties.framework, alloc);
			obj.AddMember ("resource_package", properties.resource_package, alloc);
			doc.AddMember ("properties", obj, alloc);
		}
		{
			Value arr (kArrayType);
			for (auto &app : applications)
			{
				Value obj (kObjectType);
				for (auto &it : app)
				{
					std::string key = ws2utf8 (it.first);
					std::string val = ws2utf8 (it.second);
					obj.AddMember (
						Value (key.c_str (), alloc),
						Value (val.c_str (), alloc),
						alloc
					);
				}
				arr.PushBack (obj, alloc);
			}
			doc.AddMember ("applications", arr, alloc);
		}
		{
			Value obj (kObjectType);
			{
				Value arr (kArrayType);
				for (auto &it : capabilities.capabilities_name)
					arr.PushBack (Value (ws2utf8 (it).c_str (), alloc), alloc);
				obj.AddMember ("capabilities_name", arr, alloc);
			}
			{
				Value arr (kArrayType);
				for (auto &it : capabilities.device_capabilities) arr.PushBack (Value (ws2utf8 (it).c_str (), alloc), alloc);
				obj.AddMember ("device_capabilities", arr, alloc);
			}
			doc.AddMember ("capabilities", obj, alloc);
		}
		{
			Value arr (kArrayType);

			for (auto& d : dependencies)
			{
				Value obj (kObjectType);
				obj.AddMember ("name", Value (ws2utf8 (d.name).c_str (), alloc), alloc);
				obj.AddMember ("publisher", Value (ws2utf8 (d.publisher).c_str (), alloc), alloc);
				obj.AddMember ("vermin", ver_to_json (d.vermin, alloc), alloc);
				arr.PushBack (obj, alloc);
			}
			doc.AddMember ("dependencies", arr, alloc);
		}
		{
			Value obj (kObjectType);
			{
				Value arr (kArrayType);
				for (auto &it : resources.languages) arr.PushBack (Value (ws2utf8 (it).c_str (), alloc), alloc);
				obj.AddMember ("languages", arr, alloc);
			}
			{
				Value arr (kArrayType);
				for (auto &it : resources.scales) arr.PushBack ((uint32_t)it, alloc);
				obj.AddMember ("scales", arr, alloc);
			}
			{
				Value arr (kArrayType);
				for (auto &it : resources.dx_feature_levels) arr.PushBack ((uint32_t)it, alloc);
				obj.AddMember ("dx_feature_levels", arr, alloc);
			}
			doc.AddMember ("resources", obj, alloc);
		}
		{
			Value obj (kObjectType);
			obj.AddMember ("os_min_version", ver_to_json (prerequisites.os_min_version, alloc), alloc);
			obj.AddMember ("os_max_version_tested", ver_to_json (prerequisites.os_max_version_tested, alloc), alloc);
			doc.AddMember ("prerequisites", obj, alloc);
		}
		StringBuffer buffer;
		Writer<StringBuffer> writer (buffer);
		doc.Accept (writer);
		std::string utf8 = buffer.GetString ();
		std::wstring_convert <std::codecvt_utf8 <wchar_t>> conv;
		return conv.from_bytes (utf8);
	}
};