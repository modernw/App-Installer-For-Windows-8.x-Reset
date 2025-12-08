#pragma once
#using "./PriFileFormat.dll"
using namespace PriFileFormat;
#include <comip.h>
#include <atlbase.h>
#include <atlsafe.h>
#include <objidl.h>
#include <msclr/marshal.h>
#include <msclr/gcroot.h>
System::Runtime::InteropServices::ComTypes::IStream ^ComIStreamToCliIStream (IStream *pNativeStream)
{
	if (pNativeStream == nullptr) throw gcnew System::ArgumentNullException ("pNativeStream");
	pNativeStream->AddRef ();
	System::IntPtr ptr (pNativeStream);
	// 将 IUnknown 转换为托管 IStream
	System::Object ^obj = System::Runtime::InteropServices::Marshal::GetObjectForIUnknown (ptr);
	// 返回为 ComTypes::IStream^
	return (System::Runtime::InteropServices::ComTypes::IStream ^)obj;
}
