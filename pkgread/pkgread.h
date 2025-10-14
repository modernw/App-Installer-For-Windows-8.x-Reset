// 下列 ifdef 块是创建使从 DLL 导出更简单的
// 宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 PKGREAD_EXPORTS
// 符号编译的。在使用此 DLL 的
// 任何其他项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
// PKGREAD_API 函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。
#ifndef _APPX_PACKAGE_READER_H_
#define _APPX_PACKAGE_READER_H_
#ifdef PKGREAD_EXPORTS
#define PKGREAD_API __declspec(dllexport)
#else
#define PKGREAD_API __declspec(dllimport)
#endif

#ifdef __cplusplus
extern "C"
{
#endif
#ifdef __cplusplus
#define _DEFAULT_INIT_VALUE_(_init_value_) = _init_value_
	#ifdef PKGREAD_EXPORTS
	#define _DEFAULT_INIT_VALUE_FORFUNC_(_init_value_) = _init_value_
	#else 
	#define _DEFAULT_INIT_VALUE_FORFUNC_(_init_value_)
	#endif
#else
#define _DEFAULT_INIT_VALUE_(_init_value_)
#define _DEFAULT_INIT_VALUE_FORFUNC_(_init_value_)
#endif

#ifndef _TYPE_STRUCT_VERSION_
#define _TYPE_STRUCT_VERSION_
	typedef struct _VERSION
	{
	#if defined (__cplusplus) && defined (PKGREAD_EXPORTS)
		_VERSION (UINT16 major = 0, UINT16 minor = 0, UINT16 build = 0, UINT16 revision = 0):
			major (major), minor (minor), build (build), revision (revision) {}
	#endif
		UINT16 major _DEFAULT_INIT_VALUE_ (0), 
			minor _DEFAULT_INIT_VALUE_ (0), 
			build _DEFAULT_INIT_VALUE_ (0), 
			revision _DEFAULT_INIT_VALUE_ (0);
	} VERSION;
#endif

#ifndef _TYPE_STRUCT_LIST_PVOID_
#define _TYPE_STRUCT_LIST_PVOID_
	typedef struct _LIST_PVOID
	{
		DWORD dwSize _DEFAULT_INIT_VALUE_ (0);
		LPVOID alpVoid [1] _DEFAULT_INIT_VALUE_ ({NULL});
	} LIST_PVOID, *HLIST_PVOID;
#endif

#ifndef _TYPE_STRUCT_PAIR_PVOID_
#define _TYPE_STRUCT_PAIR_PVOID_
	typedef struct _PAIR_PVOID_
	{
		LPVOID lpKey _DEFAULT_INIT_VALUE_ (NULL);
		LPVOID lpValue _DEFAULT_INIT_VALUE_ (NULL);
	} PAIR_PVOID, *HPAIR_PVOID;
#endif

#define TEMPLATE_STRUCT(_typename_) typedef struct _typename_##__ _typename_
	TEMPLATE_STRUCT (PKGREADER);
	typedef PKGREADER *HPKGREAD;
	TEMPLATE_STRUCT (APPENUMERATOR);
	typedef APPENUMERATOR *HAPPENUMERATOR;

#define PKGTYPE_UNKNOWN   0
#define PKGTYPE_APPX      1
#define PKGTYPE_BUNDLE    2
#define PKGROLE_UNKNOWN       0
#define PKGROLE_APPLICATION   1
#define PKGROLE_FRAMEWORK     2
#define PKGROLE_RESOURCE      3
// 创建包读取器
	PKGREAD_API HPKGREAD CreatePackageReader ();
	// 通过包读取器打开包
	PKGREAD_API BOOL LoadPackageFromFile (_In_ HPKGREAD hReader, _In_ LPCWSTR lpFilePath);
	// 销毁包读取器（必须）
	PKGREAD_API void DestroyPackageReader (_In_ HPKGREAD hReader);
	// 获取包的类型：Appx 包还是 AppxBundle 包。返回 PKGTYPE_* 宏 
	PKGREAD_API WORD GetPackageType (_In_ HPKGREAD hReader);
	// 返回包是否有效。无效的包会返回假。
	PKGREAD_API BOOL IsPackageValid (_In_ HPKGREAD hReader);
	// 获取包的类型：是应用包、框架包还是资源包。
	// 对于 AppxBundle 包永远返回是资源包（因为框架包无法打包成 AppxBundle，资源包必须与应用包一同打包进 AppxBundle）。
	// 返回 PKGROLE_* 宏。
	PKGREAD_API WORD GetPackageRole (_In_ HPKGREAD hReader);

	// Identity
	// 对于 AppxBundle 包来说，AppxBundle 包与储存的 Appx 的包的身份信息并不一致（不一致在版本号和处理器架构上）
#define PKG_IDENTITY_BUNDLE_GETAPPXINFO ((PKGTYPE_BUNDLE && 0xFFFF) << 16)
#define PKG_IDENTITY_NAME 0
#define PKG_IDENTITY_PUBLISHER 1
#define PKG_IDENTITY_PACKAGEFAMILYNAME 2
#define PKG_IDENTITY_PACKAGEFULLNAME 3
#define PKG_IDENTITY_RESOURCEID 4
#define PKG_ARCHITECTURE_UNKNOWN 0x0
#define PKG_ARCHITECTURE_X86 0x1
#define PKG_ARCHITECTURE_X64 0x2
#define PKG_ARCHITECTURE_ARM 0x4
#define PKG_ARCHITECTURE_ARM64 0x8
#define PKG_ARCHITECTURE_NEUTRAL 0xF
	// 获取身份信息能获取文本值的项。支持 PKG_IDENTITY_* 前缀的宏 NAME, PUBLISHER, PACKAGEFAMILYNAME, PACKAGEFULLNAME, RESOURCEID。
	// 当与 PKG_IDENTITY_BUNDLE_GETAPPXINFO 通过位或结合时，则获取 AppxBundle 中某一个应用包。
	PKGREAD_API LPWSTR GetPackageIdentityStringValue (_In_ HPKGREAD hReader, _In_ DWORD dwName);
	// 获取身份信息。当 bGetSubPkgVer 为真，且打开的包为 AppxBundle 包，返回 AppxBundle 包中的某个应用包的版本号。
	PKGREAD_API BOOL GetPackageIdentityVersion (_In_ HPKGREAD hReader, _Out_ VERSION *pVersion, _In_ BOOL bGetSubPkgVer _DEFAULT_INIT_VALUE_FORFUNC_ (0));
	// 对于 AppxBundle 包，当传入的值有 PKG_IDENTITY_BUNDLE_GETAPPXINFO，则返回一个按位值，如 0b0011，则意味着支持 x86 和 x64
	PKGREAD_API BOOL GetPackageIdentityArchitecture (_In_ HPKGREAD hReader, _Out_ DWORD *pdwArchi);

	// Properties
	// 获取字符串值
#define PKG_PROPERTIES_DISPLAYNAME L"DisplayName"
#define PKG_PROPERTIES_DESCRIPTION L"Description"
#define PKG_PROPERTIES_LOGO L"Logo"
#define PKG_PROPERTIES_PUBLISHER L"PublisherDisplayName"
// 获取逻辑型值
#define PKG_PROPERTIES_FRAMEWORD L"Framework"
#define PKG_PROPERTIES_IS_RESOURCE L"ResourcePackage"
	// 传入参数参考：https://learn.microsoft.com/en-us/windows/win32/api/appxpackaging/nf-appxpackaging-iappxmanifestproperties-getstringvalue
	// 可以使用宏 PKG_PROPERTIES_* 前缀的 DISPLAYNAME, DESCRIPTION, LOGO, PUBLISHER
	PKGREAD_API LPWSTR GetPackagePropertiesStringValue (_In_ HPKGREAD hReader, _In_ LPCWSTR lpName);
	// 传入参数参考：https://learn.microsoft.com/zh-cn/windows/win32/api/appxpackaging/nf-appxpackaging-iappxmanifestproperties-getboolvalue
	// 可以使用宏 PKG_PROPERTIES_* 前缀的 FRAMEWORD, IS_RESOURCE (Windows 8.1 支持)
	PKGREAD_API HRESULT GetPackagePropertiesBoolValue (_In_ HPKGREAD hReader, _In_ LPCWSTR lpName, _Outptr_ BOOL *pRet);
//  Applications
	// 注意：由于读取 Application 在官方 API 中是枚举类型，只能枚举一次。
	// 所以请预先安排好需要的信息，并在枚举时进行缓存。
	// 添加和删除的请参考这些：https://learn.microsoft.com/en-us/windows/win32/api/appxpackaging/nf-appxpackaging-iappxmanifestapplication-getstringvalue
	// DLL 本身支持：Id, DisplayName, BackgroundColor, ForegroundText, ShortName, Square44x44Logo
	PKGREAD_API BOOL AddPackageApplicationItemGetName (_In_ LPCWSTR lpName);
	// 移除不需要的信息项名。
	PKGREAD_API BOOL RemovePackageApplicationItemGetName (_In_ LPCWSTR lpName);
	// 获取现在有的信息项名。读取时将指针数组的成员转换为 LPCWSTR 读取，且不能修改。
	// 注意：返回的 HLIST_PVOID 要经过 DestroyApplicationItemsName 销毁。
	PKGREAD_API HLIST_PVOID GetAllApplicationItemsName ();
	// 通过 GetAllApplicationItemsName 获取的 HLIST_PVOID 由 DestroyApplicationItemsName 销毁。
	PKGREAD_API void DestroyApplicationItemsName (_In_ HLIST_PVOID hList);
	// 获取到 Application 信息枚举器。在自己缓存完信息前不要销毁，直到再也不需要后才销毁。
	// 通过 GetPackageApplications 获取的 HAPPENUMERATOR 由 DestroyApplicationsMap 销毁
	PKGREAD_API HAPPENUMERATOR GetPackageApplications (_In_ HPKGREAD hReader);
	// 从 Application 枚举器转换成 HLIST_PVOID { HLIST_PVOID {HPAIR_PVOID} } （类似于多个映射组成的数组）。
	// 读取最深层的指针成员转换为 LPCWSTR 读取。其 LPCWSTR 周期随着 HAPPENUMERATOR 的周期而决定。
	PKGREAD_API HLIST_PVOID ApplicationsToMap (_In_ HAPPENUMERATOR hEnumerator);
	// 通过 ApplicationsToMap 获取的 HLIST_PVOID 由 DestroyApplicationsMap 销毁
	PKGREAD_API void DestroyApplicationsMap (_In_ HLIST_PVOID hEnumerator);
	// 通过 GetPackageApplications 获取的 HAPPENUMERATOR 由 DestroyApplicationsMap 销毁
	PKGREAD_API void DestroyPackageApplications (_In_ HAPPENUMERATOR hEnumerator);

	// Resources
#ifndef _TYPE_STRUCT_LIST_LCID_
#define _TYPE_STRUCT_LIST_LCID_
	typedef struct _LIST_LCID
	{
		DWORD dwSize _DEFAULT_INIT_VALUE_ (0);
		LCID aLcid [1] _DEFAULT_INIT_VALUE_ ({0});
	} LIST_LCID, *HLIST_LCID;
#endif
#ifndef _TYPE_STRUCT_LIST_UINT32_
#define _TYPE_STRUCT_LIST_UINT32_
	typedef struct _LIST_UINT32
	{
		DWORD dwSize _DEFAULT_INIT_VALUE_ (0);
		LCID aUI32 [1] _DEFAULT_INIT_VALUE_ ({0});
	} LIST_UINT32, *HLIST_UINT32;
#endif
	// 返回区域代码 (Locale Code) 数组。以 LPWSTR 读取。注：一定要通过 DestroyResourcesLanguagesList 释放
	PKGREAD_API HLIST_PVOID GetResourcesLanguages (_In_ HPKGREAD hReader);
	// 返回将区域代码转换成 LCID（如 en-US 为 1033，zh-CN 为 2052） 后的数组。以 UINT32 读取。
	// 注：一定要通过 DestroyResourcesLanguagesLcidList 释放
	PKGREAD_API HLIST_LCID GetResourcesLanguagesToLcid (_In_ HPKGREAD hReader);
	// 返回 DPI 支持（如 100，125） 的数组。
	// 注：一定要通过 DestroyResourcesScalesList 宏（或 DestroyUInt32List 函数）释放
	PKGREAD_API HLIST_UINT32 GetResourcesScales (_In_ HPKGREAD hReader);
#define PKG_RESOURCES_DXFEATURE_LEVEL9		0x1
#define PKG_RESOURCES_DXFEATURE_LEVEL10		0x2
#define PKG_RESOURCES_DXFEATURE_LEVEL11		0x4
#define PKG_RESOURCES_DXFEATURE_LEVEL12		0x8
#define PKG_RESOURCES_DXFEATURE_UNSPECIFIED	0
	// 返回一个 DWORD 值。通过按位与来获取支持的 DirectX 功能支持。如果等于 0 则意味着都不支持。该值不必释放。
	PKGREAD_API DWORD GetResourcesDxFeatureLevels (_In_ HPKGREAD hReader);
	// 销毁 GetResourcesLanguages 获取的数组
	PKGREAD_API void DestroyResourcesLanguagesList (_In_ HLIST_PVOID hList);
	// 销毁 GetResourcesLanguagesToLcid 获取的数组
	PKGREAD_API void DestroyResourcesLanguagesLcidList (_In_ HLIST_LCID hList);
	PKGREAD_API void DestroyUInt32List (_In_ HLIST_UINT32 hList);
	// 销毁 GetResourcesScales 获取的数组
#define DestroyResourcesScalesList DestroyUInt32List

	// Dependencies
#ifndef _TYPE_STRUCT_DEPENDENCY_
#define _TYPE_STRUCT_DEPENDENCY_
	typedef struct _DEPENDENCY_INFO
	{
		VERSION verMin;
		LPWSTR lpName _DEFAULT_INIT_VALUE_ (NULL);
		LPWSTR lpPublisher _DEFAULT_INIT_VALUE_ (NULL);
	} DEPENDENCY_INFO;
#endif
#ifndef _TYPE_STRUCT_DEPENDENCY_LIST_
#define _TYPE_STRUCT_DEPENDENCY_LIST_
	typedef struct _LIST_DEPINFO
	{
		DWORD dwSize _DEFAULT_INIT_VALUE_ (0);
		DEPENDENCY_INFO aDepInfo [1];
	} LIST_DEPINFO, *HLIST_DEPINFO;
#endif
	// 获取依赖项信息数组，返回 HLIST_DEPINFO。必须由 DestroyDependencesInfoList 销毁
	PKGREAD_API HLIST_DEPINFO GetDependencesInfoList (_In_ HPKGREAD hReader);
	// 通过 GetDependencesInfoList 返回的指针由此销毁
	PKGREAD_API void DestroyDependencesInfoList (_In_ HLIST_DEPINFO hList);

	// Capabilities
	PKGREAD_API HLIST_PVOID GetCapabilitiesList (_In_ HPKGREAD hReader);
	PKGREAD_API HLIST_PVOID GetDeviceCapabilitiesList (_In_ HPKGREAD hReader);
	PKGREAD_API void DestroyWStringList (_In_ HLIST_PVOID hList);
#define DestroyCapabilitiesList DestroyWStringList
#define DestroyDeviceCapabilitiesList DestroyWStringList

#if defined (__cplusplus) && defined (PKGREAD_EXPORTS)
#include <cstdlib>
#include <string>
#include <functional>
	class package_info
	{
		private:
		HPKGREAD hReader = nullptr;
		std::wstring filepath = L"";
		struct deconstr
		{
			std::function <void ()> endtask = nullptr;
			deconstr (std::function <void ()> pf): endtask (pf) {}
			~deconstr () { if (endtask) endtask (); }
		};
		public:
		class identity
		{
			private:
			HPKGREAD &hReader;
			public:
			identity (HPKGREAD hReader): hReader (hReader) {}
			std::wstring string_value (DWORD dwName) const
			{
				LPWSTR lpstr = nullptr;
				deconstr rel ([&lpstr] () {
					if (lpstr) free (lpstr);
					lpstr = nullptr;
				});
				lpstr = GetPackageIdentityStringValue (hReader, dwName);
				return lpstr;
			}
			VERSION version (bool read_subpkg_ver = false) const
			{
				VERSION ver;
				GetPackageIdentityVersion (hReader, &ver, read_subpkg_ver);
				return ver;
			}
			DWORD architecture () const
			{
				DWORD dw = 0;
				GetPackageIdentityArchitecture (hReader, &dw);
				return dw;
			}
		};
		package_info (): hReader (CreatePackageReader ()) {}
		~package_info () { DestroyPackageReader (hReader); hReader = nullptr; }
		std::wstring file () const { return filepath; }
		bool file (const std::wstring &path)
		{
			return LoadPackageFromFile (hReader, (filepath = path).c_str ());
		}
		WORD package_type () const { return GetPackageType (hReader); }
		bool valid () const { return hReader && IsPackageValid (hReader); }
		WORD package_role () const { return GetPackageRole (hReader); }
		identity get_identity () const { return identity (hReader); }

	};
#endif

#ifdef _DEFAULT_INIT_VALUE_
#undef _DEFAULT_INIT_VALUE_
#endif
#ifdef _DEFAULT_INIT_VALUE_FORFUNC_
#undef _DEFAULT_INIT_VALUE_FORFUNC_
#endif
#ifdef __cplusplus
}
#endif

#endif