#pragma once
#include <windef.h>
#include <string>
#include <vcclr.h>

using namespace System;
using namespace System::Text;

String ^CStringToMPString (LPCSTR lpstr) { return (lpstr ? gcnew String (lpstr) : String::Empty); }
String ^CStringToMPString (LPCWSTR lpstr) { return (lpstr ? gcnew String (lpstr) : String::Empty); }
String ^CStringToMPString (const std::string &objstr) { return CStringToMPString (objstr.c_str ()); }
String ^CStringToMPString (const std::wstring &objstr) { return CStringToMPString (objstr.c_str ()); }
// 转换为 UTF-16，指针不需要释放（本质是指针转换）
LPCWSTR MPStringToPtrW (String ^in)
{
	if (in == nullptr) return NULL;
	pin_ptr <const wchar_t> wch = PtrToStringChars (in);
	return wch;
}
// 转换为 std::wstring（UTF-16）
std::wstring MPStringToStdW (String^ in)
{
	if (in == nullptr) return std::wstring ();
	pin_ptr <const wchar_t> wch = PtrToStringChars (in);
	return std::wstring (wch, in->Length);
}
// 转换为 ANSI 编码的 std::string
std::string MPStringToStdA (String^ in)
{
	if (in == nullptr) return std::string ();
	array <unsigned char> ^bytes = Encoding::Default->GetBytes (in);
	pin_ptr <unsigned char> pinned = &bytes [0];
	return std::string (reinterpret_cast <const char *> (pinned), bytes->Length);
}
// 转换为 UTF-8 编码的 std::string
std::string MPStringToStdU8 (String^ in)
{
	if (in == nullptr) return std::string ();
	array <unsigned char> ^bytes = Encoding::UTF8->GetBytes (in);
	pin_ptr <unsigned char> pinned = &bytes [0];
	return std::string (reinterpret_cast <const char*> (pinned), bytes->Length);
}