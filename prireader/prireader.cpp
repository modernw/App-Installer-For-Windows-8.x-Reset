// prireader.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "prireader.h"


// 这是导出变量的一个示例
PRIREADER_API int nprireader=0;

// 这是导出函数的一个示例。
PRIREADER_API int fnprireader(void)
{
    return 42;
}

// 这是已导出类的构造函数。
// 有关类定义的信息，请参阅 prireader.h
Cprireader::Cprireader()
{
    return;
}
