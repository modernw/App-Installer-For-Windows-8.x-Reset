// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 从 Windows 头中排除极少使用的资料
// Windows 头文件: 
#include <windows.h>



// TODO:  在此处引用程序需要的其他头文件
#include <wrl.h>
#include <windows.ui.notifications.h>
#include <windows.data.xml.dom.h>
#include <stdio.h>
#include <shobjidl.h> 
#include <wrl/client.h>
#include <fstream>
#include <propvarutil.h>
#include <propkey.h>
#include <wrl.h>
#include <string>
#include <shlobj.h>
#include <propkey.h>
#include <comdef.h>
#using <Windows.winmd>
using namespace Microsoft::WRL;
using namespace ABI::Windows::UI::Notifications;
using namespace ABI::Windows::Data::Xml::Dom;
using namespace Microsoft::WRL::Wrappers;
