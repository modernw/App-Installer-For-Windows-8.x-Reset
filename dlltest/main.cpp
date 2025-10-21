#include <Windows.h>
#include "..\pkgread\pkgread.h"
#include <iostream>
#include <string>
#include <algorithm>
#include <thread>

void read_package (const std::wstring &filepath)
{
	package_reader pr (filepath);
	std::wcout << L"Is Valid: " << (pr.valid () ? L"true" : L"false") << std::endl;
	std::wcout << L"Package Type: ";
	switch (pr.package_type ())
	{
		case PKGTYPE_APPX: std::wcout << L"Appx"; break;
		case PKGTYPE_BUNDLE: std::wcout << L"AppxBundle"; break;
	}
	std::wcout << std::endl;
	std::wcout << L"Package Role: ";
	switch (pr.package_role ())
	{
		case PKGROLE_APPLICATION: std::wcout << L"Application"; break;
		case PKGROLE_FRAMEWORK: std::wcout << L"Framework"; break;
		case PKGROLE_RESOURCE: std::wcout << L"Resource"; break;
		case PKGROLE_UNKNOWN: std::wcout << L"Unknown"; break;
	}
	std::wcout << std::endl;
	auto id = pr.get_identity ();
	std::wcout << L"Identity" << std::endl;
	std::wcout << L"\tName: " << id.name () << std::endl;
	std::wcout << L"\tPublisher: " << id.publisher () << std::endl;
	auto ver = id.version ();
	std::wcout << L"\tVersion: " << ver.major << L"." << ver.minor << L"." << ver.build << L"." << ver.revision << std::endl;
	std::wcout << L"\tPackage Family Name: " << id.package_family_name () << std::endl;
	std::wcout << L"\tPackage Full Name: " << id.package_full_name () << std::endl;
	std::wcout << L"\tResource Id: " << id.resource_id () << std::endl;
	std::wcout << L"\tArchitecture: ";
	DWORD archi = id.architecture ();
	for (size_t i = 0xF; i > 0; i /= 2)
	{
		if (archi & i)
		{
			switch (i)
			{
				case PKG_ARCHITECTURE_ARM: std::wcout << "Arm"; break;
				case PKG_ARCHITECTURE_ARM64: std::wcout << "Arm64"; break;
				case PKG_ARCHITECTURE_NEUTRAL: std::wcout << "Neutral"; break;
				case PKG_ARCHITECTURE_X64: std::wcout << "x64"; break;
				case PKG_ARCHITECTURE_X86: std::wcout << "x86"; break;
				default: continue;
			}
			std::wcout << L", ";
		}
	}
	std::wcout << std::endl;
	ver = id.version (true);
	std::wcout << L"\tVersion (Appx): " << ver.major << L"." << ver.minor << L"." << ver.build << L"." << ver.revision << std::endl;
	std::wcout << L"Properties" << std::endl;
	auto prop = pr.get_properties ();
	std::wcout << L"\tDisplay Name: " << prop.display_name () << std::endl;
	std::wcout << L"\tDescription: " << prop.description () << std::endl;
	std::wcout << L"\tPublisher Display Name: " << prop.publisher_display_name () << std::endl;
	std::wcout << L"\tLogo: " << prop.logo () << std::endl;
	std::wcout << L"\tFramework: " << (prop.framework () ? L"true" : L"false") << std::endl;
	std::wcout << L"\tResource Package: " << (prop.resource_package () ? L"true" : L"false") << std::endl;
	auto preq = pr.get_prerequisites ();
	std::wcout << L"Prerequisites" << std::endl;
	ver = preq.os_min_version ();
	std::wcout << L"\tOS Min Version: " << ver.major << L"." << ver.minor << L"." << ver.build << L"." << ver.revision << std::endl;
	ver = preq.os_max_version_tested ();
	std::wcout << L"\tOS Max Version Tested: " << ver.major << L"." << ver.minor << L"." << ver.build << L"." << ver.revision << std::endl;
	std::wcout << L"Resources" << std::endl;
	auto res = pr.get_resources ();
	{
		std::vector <std::wstring> langs;
		std::vector <UINT32> scales;
		std::vector <DWORD> dx_levels;
		res.languages (langs);
		res.scales (scales);
		res.dx_feature_level (dx_levels);
		std::wcout << L"\tSupported Languages: ";
		for (auto &it : langs)
		{
			std::wcout << it << L", ";
		}
		std::wcout << std::endl;
		std::wcout << L"\tSupported Scales: ";
		for (auto &it : scales)
		{
			std::wcout << it << L", ";
		}
		std::wcout << std::endl;
		std::wcout << L"\tSupported Languages: ";
		for (auto &it : dx_levels)
		{
			switch (it)
			{
				case PKG_RESOURCES_DXFEATURE_LEVEL9: std::wcout << L"9, "; break;
				case PKG_RESOURCES_DXFEATURE_LEVEL10: std::wcout << L"10, "; break;
				case PKG_RESOURCES_DXFEATURE_LEVEL11: std::wcout << L"11, "; break;
				case PKG_RESOURCES_DXFEATURE_LEVEL12: std::wcout << L"12, "; break;
			}
		}
		std::wcout << std::endl;
	}
	std::wcout << L"Capabilities" << std::endl;
	auto cap = pr.get_capabilities ();
	{
		std::vector <std::wstring> caps;
		std::vector <std::wstring> devicecaps;
		cap.capabilities_name (caps);
		cap.device_capabilities (devicecaps);
		std::wcout << L"\tCapabilities: ";
		for (auto &it : caps)
		{
			std::wcout << it << L", ";
		}
		std::wcout << std::endl;
		std::wcout << L"\tDevice Capabilities: ";
		for (auto &it : devicecaps)
		{
			std::wcout << it << L", ";
		}
		std::wcout << std::endl;
	}
	std::wcout << L"Applications" << std::endl;
	auto apps = pr.get_applications ();
	{
		std::vector <package_reader::application> appmaps;
		apps.get (appmaps);
		size_t cnt = 1;
		for (auto &it : appmaps)
		{
			std::wcout << L"\tApplication" << cnt ++ << std::endl;
			for (auto &it_s : it)
			{
				std::wcout << L"\t\t" << it_s.first << L": " << it_s.second << std::endl;
			}
		}
	}
	auto deps = pr.get_dependencies ();
	std::wcout << L"Dependencies" << std::endl;
	{
		std::vector <package_reader::dependency> deparr;
		deps.get (deparr);
		size_t cnt = 1;
		for (auto &it : deparr)
		{
			std::wcout << L"\tDependency" << cnt ++ << std::endl;
			std::wcout << L"\t\tName: " << it.name << std::endl;
			std::wcout << L"\t\tPublisher: " << it.publisher << std::endl;
			ver = it.vermin;
			std::wcout << L"\t\tMin Version: " << ver.major << L"." << ver.minor << L"." << ver.build << L"." << ver.revision << std::endl;
		}
	}
}

struct head
{
	CHAR szMagic [8];
	WORD wPlaceholder1, // 0
		wPlaceholder2; // 1
	DWORD dwFileSize,
		dwToCOffset,
		dwSectStartOffset;
	WORD wSectCount,
		wPlaceholder3; // 0xFFFF
	DWORD dwPlaceholder4; // 0
};
struct footer
{
	DWORD dwChkCode, // 0xDEFFFADE
		dwTotalFileSize;
};
struct tocentry
{
	CHAR szIdentifier [16] = {0};
	WORD wFlags = 0;
	WORD wSectFlags = 0;
	DWORD dwSectQualifier = 0;
	DWORD dwSectOffset = 0;
	DWORD dwSectLength = 0;
};
// #pragma pack (push, 1)
struct section_header
{
	CHAR szIdentifier [16] = {0};
	DWORD dwQualifier = 0;
	WORD wFlags = 0;
	WORD wSectFlags = 0;
	DWORD dwLength = 0;
	DWORD dwPlaceholder1 = -1; // 0
};
struct section_check
{
	DWORD dwChkCode = 0, // 0xDEF5FADE
		dwSectLength = 0;
};

int main (int argc, char *argv [])
{
	setlocale (LC_ALL, "");
	std::wcout.imbue (std::locale ("", LC_CTYPE));
	std::wcout << L"Please enter the file path: " << std::endl;
	std::wcout << L"\\> ";
	std::wstring pkgPathStr = L"E:\\Profiles\\Bruce\\Desktop\\resources.pri";
	//std::getline (std::wcin, pkgPathStr);
	pkgPathStr.erase (
		std::remove (pkgPathStr.begin (), pkgPathStr.end (), L'\"'),
		pkgPathStr.end ()
	);
	HANDLE hFile = CreateFileW (
		pkgPathStr.c_str (),           // 文件名
		GENERIC_READ,          // 只读
		FILE_SHARE_READ,       // 允许其他进程读取
		NULL,                  // 默认安全属性
		OPEN_EXISTING,         // 必须文件存在
		FILE_ATTRIBUTE_NORMAL, // 普通文件
		NULL                   // 无模板
	);
	head header;
	footer foot;
	DWORD headRead = 0, footRead = 0;
	ReadFile (hFile, &header, sizeof (header), &headRead, NULL);
	SetFilePointer (hFile, header.dwFileSize - 16, NULL, FILE_BEGIN);
	ReadFile (hFile, &foot, sizeof (foot), &footRead, NULL);
	SetFilePointer (hFile, header.dwToCOffset, NULL, FILE_BEGIN);
	std::vector <tocentry> tocs;
	for (size_t i = 0; i < header.wSectCount; i ++)
	{
		tocentry toc;
		DWORD dwRead;
		ReadFile (hFile, &toc, sizeof (toc), &dwRead, NULL);
		tocs.push_back (toc);
	}
	for (size_t i = 0; i < header.wSectCount; i ++)
	{
		SetFilePointer (hFile, header.dwSectStartOffset + tocs [i].dwSectOffset, NULL, FILE_BEGIN);
		section_header sh;
		section_check sc;
		DWORD dwsh, dwsc;
		ReadFile (hFile, &sh, sizeof (sh), &dwsh, NULL);
		SetFilePointer (hFile, sh.dwLength - 16 - 24, NULL, FILE_CURRENT);
		ReadFile (hFile, &sc, sizeof (sc), &dwsc, NULL);
		SetFilePointer (hFile, 32 - sh.dwLength, NULL, FILE_CURRENT);
		//subsection ss;
		//ss.dwOffset = SetFilePointer (hFile, 0, NULL, FILE_CURRENT);
		//ss.dwLength = sh.dwLength - 16 - 24;
		//SetFilePointer (hFile, 32, NULL, FILE_CURRENT);
	}
	CloseHandle (hFile);
	system ("pause");
	return 0;
}