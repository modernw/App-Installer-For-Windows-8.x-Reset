#include <Windows.h>
#include "..\pkgread\pkgread.h"
#include <iostream>
#include <string>

int main (int argc, char *argv [])
{
	// 1. 创建 Reader 对象
	HPKGREAD hReader = CreatePackageReader ();
	if (!hReader) {
		std::wcerr << L"CreatePackageReader failed!" << std::endl;
		return 1;
	}
	std::wcout << L"CreatePackageReader succeeded!" << std::endl;

	// 2. 加载包文件
	std::wcout << L"Please enter the file path: " << std::endl;
	std::wcout << L"\\> ";
	std::wstring pkgPathStr;
	std::getline (std::wcin, pkgPathStr);
	if (!pkgPathStr.empty () && pkgPathStr.front () == L'\"' && pkgPathStr.back () == L'\"' && pkgPathStr.size () >= 2) {
		pkgPathStr = pkgPathStr.substr (1, pkgPathStr.size () - 2);
	}
	LPCWSTR pkgPath = pkgPathStr.c_str ();
	if (!LoadPackageFromFile (hReader, pkgPath)) {
		std::wcerr << L"LoadPackageFromFile failed for " << pkgPath << std::endl;
		DestroyPackageReader (hReader);
		return 1;
	}
	std::wcout << L"LoadPackageFromFile succeeded!" << std::endl;

	// 3. 验证包类型
	WORD type = GetPackageType (hReader);
	std::wcout << L"Package Type: " << type << std::endl;

	// 4. 验证包有效性
	BOOL valid = IsPackageValid (hReader);
	std::wcout << L"IsPackageValid: " << (valid ? L"True" : L"False") << std::endl;

	// 5. 获取包名称
	LPWSTR pkgName = GetPackageIdentityStringValue (hReader, PKG_IDENTITY_NAME);
	if (pkgName)
		std::wcout << L"Package Name: " << pkgName << std::endl;
	else
		std::wcerr << L"GetPackageIdentityStringValue PKG_IDENTITY_NAME failed!" << std::endl;

	// 6. 获取包版本
	VERSION ver;
	if (GetPackageIdentityVersion (hReader, &ver, FALSE)) {
		std::wcout << L"Package Version: " << ver.major << L"." << ver.minor << L"." << ver.build << L"." << ver.revision << std::endl;
	}
	else {
		std::wcerr << L"GetPackageIdentityVersion failed!" << std::endl;
	}

	// 7. 列举所有 Application 名称
	HLIST_PVOID appList = GetAllApplicationItemsName ();
	if (appList) {
		std::wcout << L"Application Count: " << appList->dwSize << std::endl;
		for (DWORD i = 0; i < appList->dwSize; ++i) {
			LPCWSTR appName = (LPCWSTR)appList->alpVoid [i];
			if (appName)
				std::wcout << L"App[" << i << L"]: " << appName << std::endl;
		}
		DestroyApplicationItemsName (appList);
	}
	else {
		std::wcerr << L"GetAllApplicationItemsName failed!" << std::endl;
	}

	// 8. 清理
	DestroyPackageReader (hReader);

	system ("pause");
	return 0;
}