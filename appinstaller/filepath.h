#pragma once
#include <Windows.h>
#include <string>
#include <algorithm>
#include <shlwapi.h>
#include <vector>
#include <iomanip>
#include <functional>
#include <sstream>
#include "strcmp.h"
#include "version.h"
#include "module.h"
typedef version S_VERSION;
template <typename T> constexpr T Max (T l, T r) { return l > r ? l : r; }
template <typename T> constexpr T Max (T l, T m, T r) { return Max (Max (l, r), m); }
template <typename T> constexpr T Max (T l, T ml, T mr, T r) { return Max (Max (l, ml), Max (mr, r)); }
template <typename CharT> std::basic_string <CharT> replace_substring
(
	const std::basic_string <CharT> &str,
	const std::basic_string <CharT> &from,
	const std::basic_string <CharT> &to
)
{
	if (from.empty ()) return str;
	std::basic_string  <CharT> result;
	size_t pos = 0;
	size_t start_pos;
	while ((start_pos = str.find (from, pos)) != std::basic_string<CharT>::npos)
	{
		result.append (str, pos, start_pos - pos);
		result.append (to);
		pos = start_pos + from.length ();
	}
	result.append (str, pos, str.length () - pos);
	return result;
}
std::string GetProgramRootDirectoryA (HMODULE hModule hModule_DefaultParam)
{
	char path [MAX_PATH];
	if (GetModuleFileNameA (hModule, path, MAX_PATH))
	{
		std::string dir (path);
		size_t pos = dir.find_last_of ("\\/");
		if (pos != std::string::npos)
		{
			dir = dir.substr (0, pos);
		}
		return dir;
	}
	return "";
}
std::wstring GetProgramRootDirectoryW (HMODULE hModule hModule_DefaultParam)
{
	wchar_t path [MAX_PATH];
	if (GetModuleFileNameW (hModule, path, MAX_PATH))
	{
		std::wstring dir (path);
		size_t pos = dir.find_last_of (L"\\/");
		if (pos != std::wstring::npos)
		{
			dir = dir.substr (0, pos);
		}
		return dir;
	}
	return L"";
}
std::string EnsureTrailingSlash (const std::string &path)
{
	if (path.empty ()) return path;  // 空路径直接返回

	char lastChar = path.back ();
	if (lastChar == '\\' || lastChar == '/')
		return path;  // 已有分隔符，直接返回
					  // 根据系统或原路径格式添加适当的分隔符
	char separator = (path.find ('/') != std::string::npos) ? '/' : '\\';
	return path + separator;
}
std::wstring EnsureTrailingSlash (const std::wstring &path)
{
	if (path.empty ()) return path;

	wchar_t lastChar = path.back ();
	if (lastChar == L'\\' || lastChar == L'/')
		return path;

	wchar_t separator = (path.find (L'/') != std::wstring::npos) ? L'/' : L'\\';
	return path + separator;
}
bool IsFileExistsW (LPCWSTR filename)
{
	DWORD dwAttrib = GetFileAttributesW (filename);
	return (dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}
bool IsFileExistsA (LPCSTR filename)
{
	DWORD dwAttrib = GetFileAttributesA (filename);
	return (dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}
bool IsFileExists (LPWSTR filePath) { return IsFileExistsW (filePath); }
bool IsFileExists (LPCSTR filePath) { return IsFileExistsA (filePath); }
bool IsFileExists (const std::string &filePath) { return IsFileExistsA (filePath.c_str ()); }
bool IsFileExists (const std::wstring &filePath) { return IsFileExistsW (filePath.c_str ()); }
bool IsDirectoryExistsA (LPCSTR path)
{
	DWORD attributes = GetFileAttributesA (path);
	return (attributes != INVALID_FILE_ATTRIBUTES && (attributes & FILE_ATTRIBUTE_DIRECTORY));
}
bool IsDirectoryExistsW (LPCWSTR path)
{
	DWORD attributes = GetFileAttributesW (path);
	return (attributes != INVALID_FILE_ATTRIBUTES && (attributes & FILE_ATTRIBUTE_DIRECTORY));
}
bool IsDirectoryExists (const std::string &path) { return IsDirectoryExistsA (path.c_str ()); }
bool IsDirectoryExists (const std::wstring &path) { return IsDirectoryExistsW (path.c_str ()); }
bool IsDirectoryExists (LPCSTR path) { return IsDirectoryExistsA (path); }
bool IsDirectoryExists (LPCWSTR path) { return IsDirectoryExistsW (path); }
bool IsPathExistsW (LPCWSTR filename)
{
	DWORD dwAttrib = GetFileAttributesW (filename);
	return (dwAttrib != INVALID_FILE_ATTRIBUTES);
}
bool IsPathExistsA (LPCSTR filename)
{
	DWORD dwAttrib = GetFileAttributesA (filename);
	return (dwAttrib != INVALID_FILE_ATTRIBUTES);
}
bool IsPathExists (const std::string &path) { return IsPathExistsA (path.c_str ()); }
bool IsPathExists (const std::wstring &path) { return IsPathExistsW (path.c_str ()); }
bool IsPathExists (LPCSTR path) { return IsPathExistsA (path); }
bool IsPathExists (LPCWSTR path) { return IsPathExistsW (path); }
std::string NormalizePath (const std::string &path)
{
	if (!path.empty () && path.back () == '\\')
		return path.substr (0, path.size () - 1);
	return path.c_str ();
}
std::wstring NormalizePath (const std::wstring &path)
{
	if (!path.empty () && path.back () == L'\\')
		return path.substr (0, path.size () - 1);
	return path.c_str ();
}
std::vector <std::string> EnumSubdirectories (const std::string &directory, bool includeParentPath)
{
	std::vector<std::string> subdirs;
	std::string normPath = NormalizePath (directory);
	std::string searchPath = normPath + "\\*";
	WIN32_FIND_DATAA findData;
	HANDLE hFind = FindFirstFileA (searchPath.c_str (), &findData);
	if (hFind == INVALID_HANDLE_VALUE) return subdirs;
	do
	{
		// 过滤 "." 和 ".."
		if (strcmp (findData.cFileName, ".") == 0 || strcmp (findData.cFileName, "..") == 0)
			continue;
		// 判断是否为目录
		if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (includeParentPath)
				subdirs.push_back (normPath + "\\" + findData.cFileName);
			else
				subdirs.push_back (findData.cFileName);
		}
	} while (FindNextFileA (hFind, &findData));
	FindClose (hFind);
	return subdirs;
}
std::vector <std::wstring> EnumSubdirectories (const std::wstring &directory, bool includeParentPath)
{
	std::vector<std::wstring> subdirs;
	std::wstring normPath = NormalizePath (directory);
	std::wstring searchPath = normPath + L"\\*";
	WIN32_FIND_DATAW findData;
	HANDLE hFind = FindFirstFileW (searchPath.c_str (), &findData);
	if (hFind == INVALID_HANDLE_VALUE) return subdirs;
	do
	{
		if (wcscmp (findData.cFileName, L".") == 0 || wcscmp (findData.cFileName, L"..") == 0)
			continue;
		if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (includeParentPath)
				subdirs.push_back (normPath + L"\\" + findData.cFileName);
			else
				subdirs.push_back (findData.cFileName);
		}
	} while (FindNextFileW (hFind, &findData));
	FindClose (hFind);
	return subdirs;
}
std::string GetCurrentProgramPathA (HMODULE hModule hModule_DefaultParam)
{
	std::vector <CHAR> buf (Max <size_t> (MAX_PATH, GetModuleFileNameA (hModule, nullptr, 0)) + 1);
	GetModuleFileNameA (hModule, buf.data (), buf.capacity ());
	return buf.data ();
}
std::wstring GetCurrentProgramPathW (HMODULE hModule hModule_DefaultParam)
{
	std::vector <WCHAR> buf (Max <size_t> (MAX_PATH, GetModuleFileNameW (hModule, nullptr, 0)) + 1);
	GetModuleFileNameW (hModule, buf.data (), buf.capacity ());
	return buf.data ();
}
std::string GetCurrentProgramNameA (HMODULE hModule hModule_DefaultParam) { return PathFindFileNameA (GetCurrentProgramPathA (hModule).c_str ()); }
std::wstring GetCurrentProgramNameW (HMODULE hModule hModule_DefaultParam) { return PathFindFileNameW (GetCurrentProgramPathW (hModule).c_str ()); }
S_VERSION GetExeFileVersion (LPCSTR lpszFilePath)
{
	S_VERSION ver (0);
	DWORD dummy;
	DWORD size = GetFileVersionInfoSizeA (lpszFilePath, &dummy);
	std::vector <BYTE> pVersionInfo (size);
	if (!GetFileVersionInfoA (lpszFilePath, 0, size, pVersionInfo.data ()))
	{
		return ver;
	}
	VS_FIXEDFILEINFO* pFileInfo = nullptr;
	UINT len = 0;
	if (!VerQueryValueA (pVersionInfo.data (), "\\", (LPVOID *)&pFileInfo, &len))
	{
		return ver;
	}
	if (len == 0 || pFileInfo == nullptr)
	{
		return ver;
	}
	ver = S_VERSION (
		HIWORD (pFileInfo->dwFileVersionMS),
		LOWORD (pFileInfo->dwFileVersionMS),
		HIWORD (pFileInfo->dwFileVersionLS),
		LOWORD (pFileInfo->dwFileVersionLS)
	);
	return ver;
}
S_VERSION GetExeFileVersion (LPCWSTR lpswFilePath)
{
	S_VERSION ver (0);
	DWORD dummy;
	DWORD size = GetFileVersionInfoSizeW (lpswFilePath, &dummy);
	std::vector <BYTE> pVersionInfo (size);
	if (!GetFileVersionInfoW (lpswFilePath, 0, size, pVersionInfo.data ()))
	{
		return ver;
	}
	VS_FIXEDFILEINFO* pFileInfo = nullptr;
	UINT len = 0;
	if (!VerQueryValueA (pVersionInfo.data (), "\\", (LPVOID *)&pFileInfo, &len))
	{
		return ver;
	}
	if (len == 0 || pFileInfo == nullptr)
	{
		return ver;
	}
	ver = S_VERSION (
		HIWORD (pFileInfo->dwFileVersionMS),
		LOWORD (pFileInfo->dwFileVersionMS),
		HIWORD (pFileInfo->dwFileVersionLS),
		LOWORD (pFileInfo->dwFileVersionLS)
	);
	return ver;
}
S_VERSION GetExeFileVersion (std::wstring objswFilePath)
{
	return GetExeFileVersion (objswFilePath.c_str ());
}
S_VERSION GetExeFileVersion (std::string objszFilePath)
{
	return GetExeFileVersion (objszFilePath.c_str ());
}
// 设置当前进程的环境变量RunPath和ProgramPath
void SetupInstanceEnvironment (HMODULE hModule hModule_DefaultParam)
{
	// 设置RunPath为当前工作目录（无结尾反斜杠）
	std::vector <WCHAR> currentDir (Max <size_t> (GetCurrentDirectoryW (0, nullptr), MAX_PATH) + 1);
	DWORD len = GetCurrentDirectoryW (currentDir.capacity (), currentDir.data ());
	if (len > 0)
	{
		std::wstring runPath (currentDir.data ());
		if (!runPath.empty () && (runPath.back () == L'\\' || runPath.back () == L'/'))
		{
			runPath.pop_back ();
		}
		SetEnvironmentVariableW (L"RunPath", runPath.c_str ());
	}
	// 设置ProgramPath为程序所在目录（无结尾反斜杠）
	std::vector <WCHAR> modulePath (Max <size_t> (GetModuleFileNameW (hModule, nullptr, 0), MAX_PATH) + 1);
	len = GetModuleFileNameW (hModule, modulePath.data (), MAX_PATH);
	if (len > 0 && len < MAX_PATH)
	{
		wchar_t* lastSlash = wcsrchr (modulePath.data (), L'\\');
		if (!lastSlash) lastSlash = wcsrchr (modulePath.data (), L'/');
		if (lastSlash) *lastSlash = L'\0';
		std::wstring programPath (modulePath.data ());
		if (!programPath.empty () && (programPath.back () == L'\\' || programPath.back () == L'/'))
		{
			programPath.pop_back ();
		}
		SetEnvironmentVariableW (L"ProgramPath", programPath.c_str ());
	}
}
// 处理宽字符串环境变量展开
std::wstring ProcessEnvVars (const std::wstring &input)
{
	DWORD requiredSize = ExpandEnvironmentStringsW (input.c_str (), nullptr, 0);
	if (requiredSize == 0) return input;
	std::wstring buffer (requiredSize, L'\0');
	if (!ExpandEnvironmentStringsW (input.c_str (), &buffer [0], requiredSize))
	{
		return input;
	}
	buffer.resize (requiredSize - 1); // 去除终止空字符
	return buffer.c_str ();
}
std::wstring ProcessEnvVars (LPCWSTR input)
{
	return ProcessEnvVars (std::wstring (input));
}
// 处理ANSI字符串环境变量展开
std::string ProcessEnvVars (const std::string &input)
{
	DWORD requiredSize = ExpandEnvironmentStringsA (input.c_str (), nullptr, 0);
	if (requiredSize == 0) return input;
	std::string buffer (requiredSize, '\0');
	if (!ExpandEnvironmentStringsA (input.c_str (), &buffer [0], requiredSize))
	{
		return input;
	}
	buffer.resize (requiredSize - 1); // 去除终止空字符
	return buffer.c_str ();
}
std::string ProcessEnvVars (LPCSTR input)
{
	return ProcessEnvVars (std::string (input));
}
std::string GetCurrentDirectoryA ()
{
	std::vector <CHAR> buf (Max <size_t> (GetCurrentDirectoryA (0, nullptr), MAX_PATH) + 1);
	GetCurrentDirectoryA (buf.size (), buf.data ());
	return buf.data ();
}
std::wstring GetCurrentDirectoryW ()
{
	std::vector <WCHAR> buf (Max <size_t> (GetCurrentDirectoryW (0, nullptr), MAX_PATH) + 1);
	GetCurrentDirectoryW (buf.size (), buf.data ());
	return buf.data ();
}
std::wstring GetFileDirectoryW (const std::wstring &filePath)
{
	std::vector <WCHAR> buf (filePath.capacity () + 1);
	lstrcpyW (buf.data (), filePath.c_str ());
	PathRemoveFileSpecW (buf.data ());
	return buf.data ();
}
std::string GetFileDirectoryA (const std::string &filePath)
{
	std::vector <CHAR> buf (filePath.capacity () + 1);
	lstrcpyA (buf.data (), filePath.c_str ());
	PathRemoveFileSpecA (buf.data ());
	return buf.data ();
}
size_t EnumerateFilesW (const std::wstring &directory, const std::wstring &filter,
	std::vector <std::wstring> &outFiles, bool recursive = false)
{
	std::wstring searchPath = directory;
	if (!searchPath.empty () && searchPath.back () != L'\\')
	{
		searchPath += L'\\';
	}
	searchPath += filter;
	WIN32_FIND_DATAW findData;
	HANDLE hFind = FindFirstFileW (searchPath.c_str (), &findData);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		do {
			if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				outFiles.push_back (directory + L"\\" + findData.cFileName);
			}
		} while (FindNextFileW (hFind, &findData));
		FindClose (hFind);
	}
	if (recursive) {
		std::wstring subDirSearchPath = directory + L"\\*";
		hFind = FindFirstFileW (subDirSearchPath.c_str (), &findData);
		if (hFind != INVALID_HANDLE_VALUE)
		{
			do {
				if ((findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&
					wcscmp (findData.cFileName, L".") != 0 && wcscmp (findData.cFileName, L"..") != 0)
				{
					EnumerateFilesW (directory + L"\\" + findData.cFileName, filter, outFiles, true);
				}
			} while (FindNextFileW (hFind, &findData));
			FindClose (hFind);
		}
	}
	return outFiles.size ();
}
// 检查是否为 Windows 设备名（大小写不敏感）
bool IsReservedName (const std::wstring &name)
{
	static const wchar_t* reserved [] = {
		L"CON", L"PRN", L"AUX", L"NUL", L"COM1", L"COM2", L"COM3", L"COM4", L"COM5", L"COM6", L"COM7", L"COM8", L"COM9",
		L"LPT1", L"LPT2", L"LPT3", L"LPT4", L"LPT5", L"LPT6", L"LPT7", L"LPT8", L"LPT9"
	};
	std::wstring upperName = StringToUpper (name);
	for (const auto& res : reserved)
	{
		if (upperName == res || (upperName.rfind (res, 0) == 0 && upperName.length () > wcslen (res) && upperName [wcslen (res)] == L'.'))
		{
			return true;
		}
	}
	return false;
}
// Windows 文件命名规范检查 (Unicode)
bool IsValidWindowsNameW (LPCWSTR name)
{
	if (!name || !*name) return false;
	std::wstring wname (name);
	if (wname.find_first_of (L"<>:\"/\\|?*") != std::wstring::npos) return false;
	if (IsReservedName (wname)) return false;
	if (wname.back () == L' ' || wname.back () == L'.') return false;
	return true;
}
// Windows 文件命名规范检查 (ANSI)
bool IsValidWindowsNameA (LPCSTR name)
{
	if (!name || !*name) return false;
	std::string str (name);
	if (str.find_first_of ("<>:\"/\\|?*") != std::string::npos) return false;

	// 转换 ANSI 到宽字符
	int len = MultiByteToWideChar (CP_ACP, 0, name, -1, NULL, 0);
	if (len <= 0) return false;
	std::wstring wname (len - 1, L'\0');
	MultiByteToWideChar (CP_ACP, 0, name, -1, &wname [0], len);
	if (IsReservedName (wname)) return false;
	if (str.back () == ' ' || str.back () == '.') return false;
	return true;
}
bool IsValidWindowsName (LPCSTR name) { return IsValidWindowsNameA (name); }
bool IsValidWindowsName (LPCWSTR name) { return IsValidWindowsNameW (name); }
bool IsValidWindowsName (const std::wstring &name) { return IsValidWindowsName (name.c_str ()); }
bool IsValidWindowsName (const std::string &name) { return IsValidWindowsName (name.c_str ()); }
std::wstring GetRootFolderNameFromFilePath (const std::wstring &lpFilePath)
{
	std::vector <WCHAR> szPath (Max <size_t> (lpFilePath.length (), MAX_PATH) + 1);
	if (!PathCanonicalizeW (szPath.data (), lpFilePath.c_str ())) return L"";
	if (PathRemoveFileSpecW (szPath.data ()) == FALSE) return L"";
	LPCWSTR pszFolder = PathFindFileNameW (szPath.data ());
	if (*pszFolder != L'\0') return std::wstring (pszFolder);
	WCHAR rootName [3] = {szPath [0], L':', L'\0'};
	return std::wstring (rootName);
}
std::wstring GetSafeTimestampForFilename ()
{
	::FILETIME ft;
	GetSystemTimeAsFileTime (&ft);
	SYSTEMTIME st;
	FileTimeToSystemTime (&ft, &st);
	std::wstringstream wss;
	wss << std::setfill (L'0')
		<< st.wYear
		<< std::setw (2) << st.wMonth
		<< std::setw (2) << st.wDay << L"_"
		<< std::setw (2) << st.wHour
		<< std::setw (2) << st.wMinute
		<< std::setw (2) << st.wSecond
		<< std::setw (3) << st.wMilliseconds;
	return wss.str ();
}
size_t EnumFiles (
	const std::wstring &lpDir,
	const std::wstring &lpFilter,
	std::vector <std::wstring> &aszOutput,
	bool bOutputWithPath = false,
	bool bSortByLetter = false,
	bool bIncludeSubDir = false
) {
	if (!bIncludeSubDir) aszOutput.clear ();
	std::vector<std::wstring> filters;
	size_t start = 0;
	while (start < lpFilter.length ())
	{
		size_t pos = lpFilter.find (L'\\', start);
		if (pos == std::wstring::npos) pos = lpFilter.length ();
		filters.emplace_back (lpFilter.substr (start, pos - start));
		start = pos + 1;
	}

	std::function <void (const std::wstring &, std::wstring)> enumDir;
	enumDir = [&] (const std::wstring &physicalPath, std::wstring relativePath)
	{
		WIN32_FIND_DATAW ffd;
		HANDLE hFind = FindFirstFileW ((physicalPath + L"\\*").c_str (), &ffd);
		if (hFind == INVALID_HANDLE_VALUE) return;
		do {
			if (wcscmp (ffd.cFileName, L".") == 0 ||
				wcscmp (ffd.cFileName, L"..") == 0) continue;
			const bool isDir = (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
			const std::wstring newPhysical = physicalPath + L"\\" + ffd.cFileName;
			std::wstring newRelative = relativePath;
			if (isDir) {
				if (bIncludeSubDir) {
					newRelative += ffd.cFileName;
					newRelative += L"\\";
					enumDir (newPhysical, newRelative);
				}
			}
			else
			{
				for (const auto &filter : filters)
				{
					if (PathMatchSpecW (ffd.cFileName, filter.c_str ()))
					{
						aszOutput.push_back
						(
							bOutputWithPath ? newPhysical : (relativePath + ffd.cFileName)
						);
						break;
					}
				}
			}
		} while (FindNextFileW (hFind, &ffd));
		FindClose (hFind);
	};
	enumDir (lpDir, L"");
	if (bSortByLetter) std::sort (aszOutput.begin (), aszOutput.end ());
	return aszOutput.size ();
}
std::wstring GetRelativePath (
	const std::wstring &pszBaseDir,
	const std::wstring &pszFullPath,
	DWORD cchRelative
) {
	std::vector <WCHAR> szBase (Max <size_t> (pszBaseDir.length (), pszFullPath.length (), MAX_PATH) + 1);
	wcscpy_s (szBase.data (), MAX_PATH, pszBaseDir.c_str ());
	if (szBase [wcslen (szBase.data ()) - 1] != L'\\')
	{
		wcscat_s (szBase.data (), MAX_PATH, L"\\");
	}
	std::vector <WCHAR> buf (Max <size_t> (MAX_PATH, szBase.size ()) + 1);
	BOOL res = PathRelativePathToW (
		buf.data (),
		szBase.data (),
		FILE_ATTRIBUTE_DIRECTORY,
		pszFullPath.c_str (),
		FILE_ATTRIBUTE_NORMAL
	);
	if (res) return buf.data ();
	else return L"";
}
size_t EnumDirectory (
	const std::wstring &lpDir,
	std::vector<std::wstring> &aszOutput,
	bool bOutputWithPath = false,
	bool bSortByLetter = false,
	bool bIncludeSubDir = false
) {
	aszOutput.clear ();
	std::function <void (const std::wstring &, const std::wstring &)> enumDir;
	enumDir = [&] (const std::wstring &physicalPath, const std::wstring &relativePath) {
		WIN32_FIND_DATAW ffd;
		HANDLE hFind = FindFirstFileW ((physicalPath + L"\\*").c_str (), &ffd);
		if (hFind == INVALID_HANDLE_VALUE) return;
		do
		{
			const std::wstring name = ffd.cFileName;
			if (name == L"." || name == L"..") continue;
			const bool isDir = (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
			std::wstring newPhysical = physicalPath + L"\\" + name;
			std::wstring newRelative = relativePath + name;
			if (isDir)
			{
				if (bIncludeSubDir) enumDir (newPhysical, newRelative + L"\\");
				if (bOutputWithPath) aszOutput.push_back (newPhysical);
				else aszOutput.push_back (newRelative);
			}
		} while (FindNextFileW (hFind, &ffd));
		FindClose (hFind);
	};
	enumDir (lpDir, L"");
	if (bSortByLetter) std::sort (aszOutput.begin (), aszOutput.end ());
	return aszOutput.size ();
}

static DWORD CALLBACK ProgressRoutine (
	LARGE_INTEGER TotalFileSize,
	LARGE_INTEGER TotalBytesTransferred,
	LARGE_INTEGER /*StreamSize*/,
	LARGE_INTEGER /*StreamBytesTransferred*/,
	DWORD /*dwStreamNumber*/,
	DWORD /*dwCallbackReason*/,
	HANDLE /*hSourceFile*/,
	HANDLE /*hDestinationFile*/,
	LPVOID lpData
) {
	auto *pCallback = reinterpret_cast <std::function <void (int)> *> (lpData);
	if (pCallback && *pCallback)
	{
		int progress = static_cast <int> (
			(TotalBytesTransferred.QuadPart * 100) / TotalFileSize.QuadPart
			);
		(*pCallback) (progress);
	}
	return PROGRESS_CONTINUE;
}
bool RenameFileW (
	const std::wstring &lpSrcPath,
	const std::wstring &lpDestPath,
	std::function <void (int)> fProgress = nullptr
) {
	LPPROGRESS_ROUTINE pRoutine = nullptr;
	LPVOID pData = nullptr;
	if (fProgress)
	{
		pRoutine = ProgressRoutine;
		pData = &fProgress;
	}
	DWORD flags = MOVEFILE_COPY_ALLOWED;
	BOOL ok = MoveFileWithProgressW (
		lpSrcPath.c_str (),
		lpDestPath.c_str (),
		pRoutine,
		pData,
		flags
	);
	return ok != FALSE;
}
bool RenameFileA (
	const std::string &lpSrcPath,
	const std::string &lpDestPath,
	std::function <void (int)> fProgress = nullptr
) {
	LPPROGRESS_ROUTINE pRoutine = nullptr;
	LPVOID pData = nullptr;
	if (fProgress)
	{
		pRoutine = ProgressRoutine;
		pData = &fProgress;
	}
	DWORD flags = MOVEFILE_COPY_ALLOWED;
	BOOL ok = MoveFileWithProgressA (
		lpSrcPath.c_str (),
		lpDestPath.c_str (),
		pRoutine,
		pData,
		flags
	);
	return ok != FALSE;
}
bool RenameFileW (const std::wstring &lpSrcDir, const std::wstring &lpSrcName, const std::wstring &lpDestName, std::function <void (int)> fProgress = nullptr)
{
	struct BuildTask
	{
		LPWSTR src = nullptr, dest = nullptr;
		~BuildTask ()
		{
			if (src != nullptr)
			{
				delete [] src;
				src = nullptr;
			}
			if (dest != nullptr)
			{
				delete [] dest;
				dest = nullptr;
			}
		}
	};
	BuildTask bt;
	bt.src = new WCHAR [lpSrcDir.length () + lpSrcName.length () + 2];
	bt.dest = new WCHAR [lpSrcDir.length () + lpDestName.length () + 2];
	PathCombineW (bt.src, lpSrcDir.c_str (), lpSrcName.c_str ());
	PathCombineW (bt.dest, lpSrcDir.c_str (), lpDestName.c_str ());
	return RenameFileW (bt.src, bt.dest, fProgress);
}
bool RenameFileA (const std::string &lpSrcDir, const std::string &lpSrcName, const std::string &lpDestName, std::function <void (int)> fProgress = nullptr)
{
	struct BuildTask
	{
		LPSTR src = nullptr, dest = nullptr;
		~BuildTask ()
		{
			if (src != nullptr)
			{
				delete [] src;
				src = nullptr;
			}
			if (dest != nullptr)
			{
				delete [] dest;
				dest = nullptr;
			}
		}
	};
	BuildTask bt;
	bt.src = new CHAR [lpSrcDir.length () + lpSrcName.length () + 2];
	bt.dest = new CHAR [lpSrcDir.length () + lpDestName.length () + 2];
	PathCombineA (bt.src, lpSrcDir.c_str (), lpSrcName.c_str ());
	PathCombineA (bt.dest, lpSrcDir.c_str (), lpDestName.c_str ());
	return RenameFileA (bt.src, bt.dest, fProgress);
}
bool RenameFile (const std::wstring &lpSrcPath, const std::wstring &lpDestPath, std::function <void (int)> fProgress = nullptr)
{
	return RenameFileW (lpSrcPath, lpDestPath, fProgress);
}
bool RenameFile (const std::string &lpSrcPath, const std::string &lpDestPath, std::function <void (int)> fProgress = nullptr)
{
	return RenameFileA (lpSrcPath, lpDestPath, fProgress);
}
bool RenameFile (const std::wstring &lpSrcDir, const std::wstring &lpSrcName, const std::wstring &lpDestName, std::function <void (int)> fProgress = nullptr)
{
	return RenameFileW (lpSrcDir, lpSrcName, lpDestName, fProgress);
}
bool RenameFile (const std::string &lpSrcDir, const std::string &lpSrcName, const std::string &lpDestName, std::function <void (int)> fProgress = nullptr)
{
	return RenameFileA (lpSrcDir, lpSrcName, lpDestName, fProgress);
}
bool RenameDirectoryW (
	const std::wstring &lpSrcPath,
	const std::wstring &lpDestPath,
	std::function <void (int)> fProgress = nullptr
) {
	LPPROGRESS_ROUTINE pRoutine = nullptr;
	LPVOID pData = nullptr;
	if (fProgress)
	{
		pRoutine = ProgressRoutine;
		pData = &fProgress;
	}
	DWORD flags = MOVEFILE_COPY_ALLOWED;
	BOOL ok = MoveFileWithProgressW (
		lpSrcPath.c_str (),
		lpDestPath.c_str (),
		pRoutine,
		pData,
		flags
	);
	return ok != FALSE;
}
bool RenameDirectoryA (
	const std::string &lpSrcPath,
	const std::string &lpDestPath,
	std::function <void (int)> fProgress = nullptr
) {
	LPPROGRESS_ROUTINE pRoutine = nullptr;
	LPVOID pData = nullptr;
	if (fProgress)
	{
		pRoutine = ProgressRoutine;
		pData = &fProgress;
	}
	DWORD flags = MOVEFILE_COPY_ALLOWED;
	BOOL ok = MoveFileWithProgressA (
		lpSrcPath.c_str (),
		lpDestPath.c_str (),
		pRoutine,
		pData,
		flags
	);
	return ok != FALSE;
}
bool RenameDirectoryW (
	const std::wstring &lpParentDir,
	const std::wstring &lpSrcName,
	const std::wstring &lpDestName,
	std::function <void (int)> fProgress = nullptr
) {
	struct PathBuilder
	{
		LPWSTR src = nullptr;
		LPWSTR dest = nullptr;
		~PathBuilder ()
		{
			delete [] src;
			delete [] dest;
		}
	} pb;
	pb.src = new WCHAR [lpParentDir.length () + lpSrcName.length () + 2];
	pb.dest = new WCHAR [lpParentDir.length () + lpDestName.length () + 2];
	PathCombineW (pb.src, lpParentDir.c_str (), lpSrcName.c_str ());
	PathCombineW (pb.dest, lpParentDir.c_str (), lpDestName.c_str ());
	return RenameDirectoryW (pb.src, pb.dest, fProgress);
}
bool RenameDirectoryA (
	const std::string &lpParentDir,
	const std::string &lpSrcName,
	const std::string &lpDestName,
	std::function <void (int)> fProgress = nullptr
) {
	struct PathBuilder
	{
		LPSTR src = nullptr;
		LPSTR dest = nullptr;
		~PathBuilder ()
		{
			delete [] src;
			delete [] dest;
		}
	} pb;
	pb.src = new CHAR [lpParentDir.length () + lpSrcName.length () + 2];
	pb.dest = new CHAR [lpParentDir.length () + lpDestName.length () + 2];
	PathCombineA (pb.src, lpParentDir.c_str (), lpSrcName.c_str ());
	PathCombineA (pb.dest, lpParentDir.c_str (), lpDestName.c_str ());
	return RenameDirectoryA (pb.src, pb.dest, fProgress);
}
bool RenameDirectory (
	const std::wstring &src,
	const std::wstring &dst,
	std::function <void (int)> fProgress = nullptr
) {
	return RenameDirectoryW (src, dst, fProgress);
}
bool RenameDirectory (
	const std::string &src,
	const std::string &dst,
	std::function <void (int)> fProgress = nullptr
) {
	return RenameDirectoryA (src, dst, fProgress);
}
bool RenameDirectory (
	const std::wstring &parentDir,
	const std::wstring &srcName,
	const std::wstring &dstName,
	std::function <void (int)> fProgress = nullptr
) {
	return RenameDirectoryW (parentDir, srcName, dstName, fProgress);
}
bool RenameDirectory (
	const std::string &parentDir,
	const std::string &srcName,
	const std::string &dstName,
	std::function <void (int)> fProgress = nullptr
) {
	return RenameDirectoryA (parentDir, srcName, dstName, fProgress);
}
std::wstring CombinePath (const std::wstring &left, const std::wstring &right)
{
	std::vector <WCHAR> buf (left.capacity () + right.capacity () + 2);
	PathCombineW (buf.data (), left.c_str (), right.c_str ());
	return buf.data ();
}
#ifdef PathCommonPrefix
#undef PathCommonPrefix
#endif
std::wstring PathCommonPrefix (const std::wstring &file1, const std::wstring &file2)
{
	std::vector <WCHAR> buf (Max <size_t> (file1.capacity (), file2.capacity (), MAX_PATH) + 2);
	PathCommonPrefixW (file1.c_str (), file2.c_str (), buf.data ());
	return buf.data ();
}
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
bool PathEquals (const std::wstring &path1, const std::wstring &path2)
{
	size_t maxlen = Max <size_t> (path1.capacity () + 1, path2.capacity () + 1, MAX_PATH);
	std::vector <WCHAR> buf1 (maxlen), buf2 (maxlen);
	PathCanonicalizeW (buf1.data (), path1.c_str ());
	PathCanonicalizeW (buf2.data (), path2.c_str ());
	return IsNormalizeStringEquals (buf1.data (), buf2.data ());
}
#ifdef __cplusplus_cli
#include <rapidjson\document.h>
#include <rapidjson\writer.h>
#include <rapidjson\stringbuffer.h>
#include <codecvt>
#include <locale>
#include <ShlObj.h>
#include <combaseapi.h>
#include "mpstr.h"
#include "strcode.h"
using namespace System;
using namespace System::Runtime::InteropServices;
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
				auto uri = gcnew System::Uri (System::IO::Path::GetFullPath (path));
				auto uriText = uri->AbsoluteUri;
				return uriText;
			}
			catch (...) { return String::Empty; }
		}
	}
	property String ^FullPath { String ^get () { return System::IO::Path::GetFullPath (path); }}
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
		Encoding ^enc = lastEncoding ? lastEncoding : Encoding::UTF8;
		FileStream ^fs = nullptr;
		StreamWriter ^sw = nullptr;
		try
		{
			fs = gcnew FileStream (
				path,
				FileMode::Create,
				FileAccess::ReadWrite,
				FileShare::ReadWrite
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


#endif