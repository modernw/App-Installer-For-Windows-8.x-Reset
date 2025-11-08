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
#ifndef PKGMGR_EXPORTS
#define _DEFAULT_INIT_VALUE_FORFUNC_(_init_value_) = _init_value_
#else 
#define _DEFAULT_INIT_VALUE_FORFUNC_(_init_value_)
#endif
#else
#define _DEFAULT_INIT_VALUE_(_init_value_)
#define _DEFAULT_INIT_VALUE_FORFUNC_(_init_value_)
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
