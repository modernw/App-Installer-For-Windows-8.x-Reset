// 下列 ifdef 块是创建使从 DLL 导出更简单的
// 宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 PKGMGR_EXPORTS
// 符号编译的。在使用此 DLL 的
// 任何其他项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
// PKGMGR_API 函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。
#ifdef NOTICE_EXPORTS
#define NOTICE_API __declspec(dllexport)
#else
#define NOTICE_API __declspec(dllimport)
#endif

// 这里的 API 常用“Appx”在函数名中，此举是为了防止与其他类似的函数名混淆。

#ifdef __cplusplus
extern "C"
{
#endif
#ifdef __cplusplus
#define _DEFAULT_INIT_VALUE_(_init_value_) = _init_value_
#ifndef NOTICE_EXPORTS
#define _DEFAULT_INIT_VALUE_FORFUNC_(_init_value_) = _init_value_
#else 
#define _DEFAULT_INIT_VALUE_FORFUNC_(_init_value_)
#endif
#else
#define _DEFAULT_INIT_VALUE_(_init_value_)
#define _DEFAULT_INIT_VALUE_FORFUNC_(_init_value_)
#endif
	typedef void (*NOTICE_ACTIVECALLBACK) (void *pCustom);
	// 参考：https://learn.microsoft.com/zh-cn/previous-versions/windows/apps/hh761494(v=win.10)
	// 通过 Toast 通知名来获取 XML 模板。
	// 不符合会返回一个默认模板（只会有一个 text 节点。根据需要的话可以自己添加）
	// 注意：返回的指针要自己 free 释放
	NOTICE_API LPWSTR GetToastNoticeXml (LPCWSTR lpTemplateName);
	// 获取一个简单的 Toast 通知 XML 文档。第一个参数是必须的。第二个参数为图片 URI（file:///）。
	// 第二个参数如果为 NULL 或去掉首尾空的长度为 0 的文本则不会使用带图片的模板。
	// 注意：返回的指针要自己通过 free 释放
	NOTICE_API LPWSTR GenerateSimpleToastNoticeXml (LPCWSTR lpText, LPCWSTR lpImagePath);
	// 获取一个简单的 Toast 通知 XML 文档。第一个参数是必须的。第三个参数为图片 URI（file:///）。
	// 第三个参数如果为 NULL 或去掉首尾空的长度为 0 的文本则不会使用带图片的模板。
	// 第二个参数可以为 NULL 或空文本。当为空时不会使用相关模板
	// 注意：返回的指针要自己通过 free 释放
	NOTICE_API LPWSTR GenerateSimpleToastNoticeXml2 (LPCWSTR lpTitle, LPCWSTR lpText, LPCWSTR lpImagePath);
	// 创建并显示一个 Toast 通知
	// 参数1 为非必须项，这意味着可以传入 NULL 或空文本。但是建议必须填。桌面应用
	// 必须在开始菜单快捷方式储存处创建一个含有 AppUserModelID 的快捷方式才能使用 Toast 通知。而这个限制
	// 在 Windows 10 已经去除。
	// pfCallback 为点击 Toast 通知本体后触发的回调函数。注意：仅运行期才能用，且不一定会调用成功
	// pCustom 可以传入自定义内容并在回调中使用
	// lpExceptMsg 返回异常信息。获取到的指针必须由 free 释放。
	NOTICE_API HRESULT CreateToastNoticeFromXmlDocument (LPCWSTR lpIdName, LPCWSTR lpXmlString, NOTICE_ACTIVECALLBACK pfCallback, void *pCustom, LPWSTR *lpExceptMsg);
	// 创建一个简单的 Toast 通知。仅支持一段文本和一张图片（图片若不需要则设置为 NULL 或空文本）
	// 一些参数作用与 CreateToastNoticeFromXmlDocument 中的同名参数作用一致。
	NOTICE_API HRESULT CreateToastNotice (LPCWSTR lpIdName, LPCWSTR lpText, LPCWSTR lpImgPath, NOTICE_ACTIVECALLBACK pfCallback, void *pCustom, LPWSTR *lpExceptMsg);
	// 创建一个简单的 Toast 通知。支持两段文本和一张图片（图片若不需要则设置为 NULL 或空文本）
	// lpText 可以设置为 NULL 或空文本。此时函数的作用与 CreateToastNotice 一致。
	// 一些参数作用与 CreateToastNoticeFromXmlDocument 中的同名参数作用一致。
	NOTICE_API HRESULT CreateToastNotice2 (LPCWSTR lpIdName, LPCWSTR lpTitle, LPCWSTR lpText, LPCWSTR lpImgPath, NOTICE_ACTIVECALLBACK pfCallback, void *pCustom, LPWSTR *lpExceptMsg);
	// 创建一个简单的 Toast 通知。支持两段文本和一张图片（图片是 IStream 流，如果不想设置则置 NULL）
	// lpText 可以设置为 NULL 或空文本。此时函数的作用与 CreateToastNoticeWithIStream 一致。
	// 一些参数作用与 CreateToastNoticeFromXmlDocument 中的同名参数作用一致。
	NOTICE_API HRESULT CreateToastNoticeWithIStream2 (LPCWSTR lpIdName, LPCWSTR lpTitle, LPCWSTR lpText, HANDLE pIImgStream, NOTICE_ACTIVECALLBACK pfCallback, void *pCustom, LPWSTR *lpExceptMsg);
	// 创建一个简单的 Toast 通知。支持两段文本和一张图片（图片是 IStream 流，如果不想设置则置 NULL）
	// 一些参数作用与 CreateToastNoticeFromXmlDocument 中的同名参数作用一致。
	NOTICE_API HRESULT CreateToastNoticeWithIStream (LPCWSTR lpIdName, LPCWSTR lpText, HANDLE pIImgStream, NOTICE_ACTIVECALLBACK pfCallback, void *pCustom, LPWSTR *lpExceptMsg);
	// 获取上一次操作的 HReuslt（注意：返回的 HResult 不一定代表错误，因为这是记录每一个步骤的 HResult）
	NOTICE_API HRESULT NoticeGetLastHResult ();
	// 获取上一次异常操作的错误信息。（注意：仅在发生异常时才会记录）
	NOTICE_API LPCWSTR NoticeGetLastDetailMessage ();
	// 创建快捷方式
	// （不用安装程序原生的创建，因为需要 AppUserID 才能使用 Toast 通知，当然这个限制只有 Windows 8.x 有，Windows 10 没有这个限制了）
	NOTICE_API HRESULT CreateShortcutWithAppIdW (LPCWSTR pszShortcutPath, LPCWSTR pszTargetPath, LPCWSTR pszAppId);
#ifdef _DEFAULT_INIT_VALUE_
#undef _DEFAULT_INIT_VALUE_
#endif
#ifdef _DEFAULT_INIT_VALUE_FORFUNC_
#undef _DEFAULT_INIT_VALUE_FORFUNC_
#endif
#ifdef __cplusplus
}
#endif
