// 下列 ifdef 块是创建使从 DLL 导出更简单的
// 宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 PKGMGR_EXPORTS
// 符号编译的。在使用此 DLL 的
// 任何其他项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
// PKGMGR_API 函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。
#ifdef PKGMGR_EXPORTS
#define PKGMGR_API __declspec(dllexport)
#else
#define PKGMGR_API __declspec(dllimport)
#endif

// 这里的 API 常用“Appx”在函数名中，此举是为了防止与其他类似的函数名混淆。

#ifdef __cplusplus
extern "C"
{
#endif
#ifdef __cplusplus
#define _DEFAULT_INIT_VALUE_(_init_value_) = _init_value_
#ifndef PKGMGR_EXPORTS
#define _DEFAULT_INIT_VALUE_FORFUNC_(_init_value_) = _init_value_
#else 
#define _DEFAULT_INIT_VALUE_FORFUNC_(_init_value_)
#endif
#else
#define _DEFAULT_INIT_VALUE_(_init_value_)
#define _DEFAULT_INIT_VALUE_FORFUNC_(_init_value_)
#endif

	// 进度回调。pCustom 可以传入自定义内容
	typedef void (*PKGMRR_PROGRESSCALLBACK) (DWORD dwProgress, void *pCustom);

#define PKGMGR_RESULT_OK S_OK
#define PKGMGR_RESULT_ERROR E_FAIL
#define PKGMGR_RESULT_CANCEL E_ABORT
#define PKGMGR_RESULT_STARTED E_PENDING

	typedef struct _REGISTER_PACKAGE_DEFENDENCIES
	{
		DWORD dwSize _DEFAULT_INIT_VALUE_ (0);
		LPWSTR alpDepUris [1];
	} REGISTER_PACKAGE_DEFENDENCIES, *PREGISTER_PACKAGE_DEFENDENCIES;
	typedef const REGISTER_PACKAGE_DEFENDENCIES CREGISTER_PACKAGE_DEFENDENCIES, *PCREGISTER_PACKAGE_DEFENDENCIES;

	// 下面值与 Windows::Management::Deployment::DeploymentOptions 一一对应。由于需要写成 API，所以
	// 只能如此。
	// 可查阅文档 https://learn.microsoft.com/zh-cn/uwp/api/windows.management.deployment.deploymentoptions?view=winrt-26100

	// 设置此选项时，应用将在开发模式下安装。 有关开发模式的信息，请参阅 备注。
	// 使用此选项可启用关键应用开发方案。不能将此选项与捆绑包结合使用。 
	// 如果将此选项与捆绑包一起使用，调用将返回ERROR_INSTALL_FAILED。
#define DEPOLYOPTION_FORCE_APP_SHUTDOWN		0x00000001
	// 如果当前正在使用此包或依赖于此包的任何包，则会强制关闭与该包关联的进程，以便可以继续注册。
#define DEPOLYOPTION_DEVELOPMENT_MODE		0x00000002
	// 设置此选项时，将指示应用跳过资源适用性检查。 这会有效地暂存或注册用户传递给 命令的所有资源包，
	// 这会强制对捆绑包中包含的所有包具有适用性。 如果用户传入捆绑包，则将注册所有包含的资源包。 
	// 仅限 Windows。
#define DEPOLYOPTION_INSTALL_ALL_RESOURCES	0x00000020
	// 使用默认行为。
#define DEPOLYOPTION_NONE					0x00000000
	// 安装一个 Appx/AppxBundle/Msix/MsixBundle 包
	// 注意：传入的文件路径为 DOS/NT 风格。如：C:\Windows\...
	// 该函数类似于 PowerShell 的 Add-AppxPackage
	// dwDeployOption 接受 DEPOLYOPTION_* 常量
	// 输出的错误信息别忘了用 free 释放
	PKGMGR_API HRESULT AddAppxPackageFromPath (LPCWSTR lpPkgPath, PCREGISTER_PACKAGE_DEFENDENCIES alpDepUrlList _DEFAULT_INIT_VALUE_FORFUNC_ (NULL), DWORD dwDeployOption _DEFAULT_INIT_VALUE_FORFUNC_ (DEPOLYOPTION_NONE), PKGMRR_PROGRESSCALLBACK pfCallback _DEFAULT_INIT_VALUE_FORFUNC_ (NULL), void *pCustom _DEFAULT_INIT_VALUE_FORFUNC_ (NULL), LPWSTR *pErrorCode _DEFAULT_INIT_VALUE_FORFUNC_ (NULL), LPWSTR *pDetailMsg _DEFAULT_INIT_VALUE_FORFUNC_ (NULL));
	// 安装一个 Appx/AppxBundle/Msix/MsixBundle 包
	// 注意：传入的文件路径为 URI。
	// 该函数类似于 PowerShell 的 Add-AppxPackage
	// dwDeployOption 接受 DEPOLYOPTION_* 常量
	// 输出的错误信息别忘了用 free 释放
	PKGMGR_API HRESULT AddAppxPackageFromURI (LPCWSTR lpFileUri, PCREGISTER_PACKAGE_DEFENDENCIES alpDepFullNameList _DEFAULT_INIT_VALUE_FORFUNC_ (NULL), DWORD dwDeployOption _DEFAULT_INIT_VALUE_FORFUNC_ (DEPOLYOPTION_NONE), PKGMRR_PROGRESSCALLBACK pfCallback _DEFAULT_INIT_VALUE_FORFUNC_ (NULL), void *pCustom _DEFAULT_INIT_VALUE_FORFUNC_ (NULL), LPWSTR *pErrorCode _DEFAULT_INIT_VALUE_FORFUNC_ (NULL), LPWSTR *pDetailMsg _DEFAULT_INIT_VALUE_FORFUNC_ (NULL));

	typedef struct _FIND_PACKAGE_ID
	{
		UINT64 qwVersion _DEFAULT_INIT_VALUE_ (0);
		// x86: 0
		// x64: 9
		// Arm: 5
		// Neutral: 11
		// Arm64: 12
		// Unknown: 65535(-1)
		WORD wProcessArchitecture _DEFAULT_INIT_VALUE_ ((WORD)-1);
		WORD wPadding [3];
		LPCWSTR lpName _DEFAULT_INIT_VALUE_ (NULL);
		LPCWSTR lpFullName _DEFAULT_INIT_VALUE_ (NULL);
		LPCWSTR lpFamilyName _DEFAULT_INIT_VALUE_ (NULL);
		LPCWSTR lpPublisher _DEFAULT_INIT_VALUE_ (NULL);
		LPCWSTR lpPublisherId _DEFAULT_INIT_VALUE_ (NULL);
		LPCWSTR lpResourceId _DEFAULT_INIT_VALUE_ (NULL);
	} FIND_PACKAGE_ID, *PFIND_PACKAGE_ID;
	typedef struct _FIND_PACKAGE_PROPERTIES
	{
		LPCWSTR lpDisplayName _DEFAULT_INIT_VALUE_ (NULL);
		LPCWSTR lpDescription _DEFAULT_INIT_VALUE_ (NULL);
		LPCWSTR lpPublisher _DEFAULT_INIT_VALUE_ (NULL);
		LPCWSTR lpLogoUri _DEFAULT_INIT_VALUE_ (NULL);
		BOOL bIsFramework _DEFAULT_INIT_VALUE_ (FALSE);
		BOOL bIsResourcePackage _DEFAULT_INIT_VALUE_ (FALSE);
		BOOL bIsBundle _DEFAULT_INIT_VALUE_ (FALSE);
		BOOL bIsDevelopmentMode _DEFAULT_INIT_VALUE_ (FALSE);
	} FIND_PACKAGE_PROPERTIES, *PFIND_PACKAGE_PROPERTIES;
	typedef struct _FIND_PACKAGE_INFO
	{
		FIND_PACKAGE_ID piIdentity;
		FIND_PACKAGE_PROPERTIES piProperties;
		LPCWSTR lpInstallLocation _DEFAULT_INIT_VALUE_ (NULL);
		// 多个用户由 ";" 分隔
		LPCWSTR lpUsers _DEFAULT_INIT_VALUE_ (NULL);
		// 多个用户由 ";" 分隔
		LPCWSTR lpSIDs _DEFAULT_INIT_VALUE_ (NULL);
		DWORD dwDependencesSize _DEFAULT_INIT_VALUE_ (0);
		// 这个项无意义，仅用于内存对齐
		DWORD dwPadding _DEFAULT_INIT_VALUE_ (0);
		// 这个项无意义，除非之后我想通过动态内存方式写依赖项的数组。
		UINT64 ullBuffer _DEFAULT_INIT_VALUE_ (0);
	} FIND_PACKAGE_INFO, *PFIND_PACKAGE_INFO;
	typedef const FIND_PACKAGE_INFO CFIND_PACKAGE_INFO, *LPCFIND_PACKAGE_INFO;
	// 注：回调时的指针不要手动释放
	typedef void (*PKGMGR_FINDENUMCALLBACK) (LPCFIND_PACKAGE_INFO pNowItem, void *pCustom);
	// 通过回调来获取所有包的信息。回调函数允许传自定义内容。返回的值可以判断是否成功。
	// 该函数类似于 PowerShell 的 Get-AppxPackage
	// 注意：回调函数中包的信息无法修改，也不能擅自释放。且随着函数的执行完毕自动释放。这就要求在回调中自行拷贝一份。
	// 输出的错误信息别忘了用 free 释放
	PKGMGR_API HRESULT GetAppxPackages (PKGMGR_FINDENUMCALLBACK pfCallback, void *pCustom _DEFAULT_INIT_VALUE_FORFUNC_ (NULL), LPWSTR *pErrorCode _DEFAULT_INIT_VALUE_FORFUNC_ (NULL), LPWSTR *pDetailMsg _DEFAULT_INIT_VALUE_FORFUNC_ (NULL));
	// 移除一个已经安装到电脑上的应用。注意：传入的是包完整名。
	// 输出的错误信息别忘了用 free 释放
	PKGMGR_API HRESULT RemoveAppxPackage (LPCWSTR lpPkgFullName, PKGMRR_PROGRESSCALLBACK pfCallback _DEFAULT_INIT_VALUE_FORFUNC_ (NULL), void *pCustom _DEFAULT_INIT_VALUE_FORFUNC_ (NULL), LPWSTR *pErrorCode _DEFAULT_INIT_VALUE_FORFUNC_ (NULL), LPWSTR *pDetailMsg _DEFAULT_INIT_VALUE_FORFUNC_ (NULL));
	// 清理指定用户的指定 包 。 用于清除删除用户用户配置文件后为用户安装的包。
	// 输出的错误信息别忘了用 free 释放
	PKGMGR_API HRESULT CleanupAppxPackage (LPCWSTR lpPkgName, LPCWSTR lpUserSID, PKGMRR_PROGRESSCALLBACK pfCallback _DEFAULT_INIT_VALUE_FORFUNC_ (NULL), void *pCustom _DEFAULT_INIT_VALUE_FORFUNC_ (NULL), LPWSTR *pErrorCode _DEFAULT_INIT_VALUE_FORFUNC_ (NULL), LPWSTR *pDetailMsg _DEFAULT_INIT_VALUE_FORFUNC_ (NULL));
	
	// 注册应用包
	// dwDeployOption 接受 DEPOLYOPTION_* 常量
	PKGMGR_API HRESULT RegisterAppxPackageByPath (LPCWSTR lpManifestPath, PCREGISTER_PACKAGE_DEFENDENCIES alpDependencyUriList _DEFAULT_INIT_VALUE_FORFUNC_ (NULL), DWORD dwDeployOption _DEFAULT_INIT_VALUE_FORFUNC_ (DEPOLYOPTION_NONE), PKGMRR_PROGRESSCALLBACK pfCallback _DEFAULT_INIT_VALUE_FORFUNC_ (NULL), void *pCustom _DEFAULT_INIT_VALUE_FORFUNC_ (NULL), LPWSTR *pErrorCode _DEFAULT_INIT_VALUE_FORFUNC_ (NULL), LPWSTR *pDetailMsg _DEFAULT_INIT_VALUE_FORFUNC_ (NULL));
	// 注册应用包
	// dwDeployOption 接受 DEPOLYOPTION_* 常量
	PKGMGR_API HRESULT RegisterAppxPackageByUri (LPCWSTR lpManifestUri, PCREGISTER_PACKAGE_DEFENDENCIES alpDependencyUriList _DEFAULT_INIT_VALUE_FORFUNC_ (NULL), DWORD dwDeployOption _DEFAULT_INIT_VALUE_FORFUNC_ (DEPOLYOPTION_NONE), PKGMRR_PROGRESSCALLBACK pfCallback _DEFAULT_INIT_VALUE_FORFUNC_ (NULL), void *pCustom _DEFAULT_INIT_VALUE_FORFUNC_ (NULL), LPWSTR *pErrorCode _DEFAULT_INIT_VALUE_FORFUNC_ (NULL), LPWSTR *pDetailMsg _DEFAULT_INIT_VALUE_FORFUNC_ (NULL));
	// 注册应用包
	// dwDeployOption 接受 DEPOLYOPTION_* 常量
	PKGMGR_API HRESULT RegisterAppxPackageByFullName (LPCWSTR lpPackageFullName, PCREGISTER_PACKAGE_DEFENDENCIES alpDepFullNameList _DEFAULT_INIT_VALUE_FORFUNC_ (NULL), DWORD dwDeployOption _DEFAULT_INIT_VALUE_FORFUNC_ (DEPOLYOPTION_NONE), PKGMRR_PROGRESSCALLBACK pfCallback _DEFAULT_INIT_VALUE_FORFUNC_ (NULL), void *pCustom _DEFAULT_INIT_VALUE_FORFUNC_ (NULL), LPWSTR *pErrorCode _DEFAULT_INIT_VALUE_FORFUNC_ (NULL), LPWSTR *pDetailMsg _DEFAULT_INIT_VALUE_FORFUNC_ (NULL));
	
	// 请查阅 https://learn.microsoft.com/zh-cn/uwp/api/windows.management.deployment.packagestate?view=winrt-26100

	// 包可用。
#define PACKAGESTATUS_NORMAL			0
	// 包的许可证无效。
#define PACKAGESTATUS_LICENSE_INVALID	1
	// 包有效负载由未知源修改。
#define PACKAGESTATUS_MODIFIED			2
	// 包有效负载被故意篡改。
#define PACKAGESTATUS_TAMPERED			3
	// 例如，将 包的状态设置为“可用”、“已篡改”等。
	// dwStatus 请使用 PACKAGESTATUS_* 常量
	PKGMGR_API HRESULT SetAppxPackageStatus (LPCWSTR lpPackageFullName, DWORD dwStatus, LPWSTR *pErrorCode _DEFAULT_INIT_VALUE_FORFUNC_ (NULL), LPWSTR *pDetailMsg _DEFAULT_INIT_VALUE_FORFUNC_ (NULL));
	// 在不注册包的情况下将 包 暂存到系统。
	// dwDeployOption 接受 DEPOLYOPTION_* 常量
	PKGMGR_API HRESULT StageAppxPackageFromURI (LPCWSTR lpFileUri, PCREGISTER_PACKAGE_DEFENDENCIES alpDepUriList _DEFAULT_INIT_VALUE_FORFUNC_ (NULL), DWORD dwDeployOption _DEFAULT_INIT_VALUE_FORFUNC_ (DEPOLYOPTION_NONE), PKGMRR_PROGRESSCALLBACK pfCallback _DEFAULT_INIT_VALUE_FORFUNC_ (NULL), void *pCustom _DEFAULT_INIT_VALUE_FORFUNC_ (NULL), LPWSTR *pErrorCode _DEFAULT_INIT_VALUE_FORFUNC_ (NULL), LPWSTR *pDetailMsg _DEFAULT_INIT_VALUE_FORFUNC_ (NULL));
	// 在不注册包的情况下将 包 暂存到系统。
	// dwDeployOption 接受 DEPOLYOPTION_* 常量
	PKGMGR_API HRESULT StageAppxPackageFromPath (LPCWSTR lpFileUri, PCREGISTER_PACKAGE_DEFENDENCIES alpDepUriList _DEFAULT_INIT_VALUE_FORFUNC_ (NULL), DWORD dwDeployOption _DEFAULT_INIT_VALUE_FORFUNC_ (DEPOLYOPTION_NONE), PKGMRR_PROGRESSCALLBACK pfCallback _DEFAULT_INIT_VALUE_FORFUNC_ (NULL), void *pCustom _DEFAULT_INIT_VALUE_FORFUNC_ (NULL), LPWSTR *pErrorCode _DEFAULT_INIT_VALUE_FORFUNC_ (NULL), LPWSTR *pDetailMsg _DEFAULT_INIT_VALUE_FORFUNC_ (NULL));
	// 在不注册包的情况下将 包的 漫游数据暂存到系统。 在暂存包之后、为特定用户注册包之前调用此方法。
	// dwDeployOption 接受 DEPOLYOPTION_* 常量
	PKGMGR_API HRESULT StageAppxUserData (LPCWSTR lpPackageFullName, PKGMRR_PROGRESSCALLBACK pfCallback _DEFAULT_INIT_VALUE_FORFUNC_ (NULL), void *pCustom _DEFAULT_INIT_VALUE_FORFUNC_ (NULL), LPWSTR *pErrorCode _DEFAULT_INIT_VALUE_FORFUNC_ (NULL), LPWSTR *pDetailMsg _DEFAULT_INIT_VALUE_FORFUNC_ (NULL));
	// 为当前用户汇报已安装的包或其依赖项包。
	// 仅当更新包的版本高于已安装包的版本时，才能更新已安装的包。 如果不存在以前版本的包，则操作将失败。
	// 注意：传入的文件路径为 DOS/NT 风格。如：C:\Windows\...
	// dwDeployOption 接受 DEPOLYOPTION_* 常量
	// 输出的错误信息别忘了用 free 释放
	PKGMGR_API HRESULT UpdateAppxPackageFromPath (LPCWSTR lpPkgPath, PCREGISTER_PACKAGE_DEFENDENCIES alpDepUrlList _DEFAULT_INIT_VALUE_FORFUNC_ (NULL), DWORD dwDeployOption _DEFAULT_INIT_VALUE_FORFUNC_ (DEPOLYOPTION_NONE), PKGMRR_PROGRESSCALLBACK pfCallback _DEFAULT_INIT_VALUE_FORFUNC_ (NULL), void *pCustom _DEFAULT_INIT_VALUE_FORFUNC_ (NULL), LPWSTR *pErrorCode _DEFAULT_INIT_VALUE_FORFUNC_ (NULL), LPWSTR *pDetailMsg _DEFAULT_INIT_VALUE_FORFUNC_ (NULL));
	// 为当前用户汇报已安装的包或其依赖项包。
	// 仅当更新包的版本高于已安装包的版本时，才能更新已安装的包。 如果不存在以前版本的包，则操作将失败。
	// 注意：传入的文件路径为 URI。
	// dwDeployOption 接受 DEPOLYOPTION_* 常量
	// 输出的错误信息别忘了用 free 释放
	PKGMGR_API HRESULT UpdateAppxPackageFromURI (LPCWSTR lpFileUri, PCREGISTER_PACKAGE_DEFENDENCIES alpDepFullNameList _DEFAULT_INIT_VALUE_FORFUNC_ (NULL), DWORD dwDeployOption _DEFAULT_INIT_VALUE_FORFUNC_ (DEPOLYOPTION_NONE), PKGMRR_PROGRESSCALLBACK pfCallback _DEFAULT_INIT_VALUE_FORFUNC_ (NULL), void *pCustom _DEFAULT_INIT_VALUE_FORFUNC_ (NULL), LPWSTR *pErrorCode _DEFAULT_INIT_VALUE_FORFUNC_ (NULL), LPWSTR *pDetailMsg _DEFAULT_INIT_VALUE_FORFUNC_ (NULL));
	// 检索有关为任何用户安装的指定 包 的信息。
	PKGMGR_API HRESULT FindAppxPackage (LPCWSTR lpPackageFullName, PKGMGR_FINDENUMCALLBACK pfCallback, void *pCustom _DEFAULT_INIT_VALUE_FORFUNC_ (NULL), LPWSTR *pErrorCode _DEFAULT_INIT_VALUE_FORFUNC_ (NULL), LPWSTR *pDetailMsg _DEFAULT_INIT_VALUE_FORFUNC_ (NULL));
	// 获取错误代码。错误代码不一定有，且更多有效信息来自错误详细信息。
	PKGMGR_API LPCWSTR GetPackageManagerLastErrorCode ();
	// 获取错误详细信息。这个是常用的
	PKGMGR_API LPCWSTR GetPackageManagerLastErrorDetailMessage ();
#ifdef _DEFAULT_INIT_VALUE_
#undef _DEFAULT_INIT_VALUE_
#endif
#ifdef _DEFAULT_INIT_VALUE_FORFUNC_
#undef _DEFAULT_INIT_VALUE_FORFUNC_
#endif
#ifdef __cplusplus
}
#endif
