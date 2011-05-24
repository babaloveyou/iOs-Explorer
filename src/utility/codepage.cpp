#include "utility/codepage.h"

#include <Windows.h>

namespace utility
{
	std::wstring UTF8ToUTF16(const std::string &utf8Str)
	{
		int u16Len = ::MultiByteToWideChar(CP_UTF8, NULL,utf8Str.c_str(),(int)utf8Str.size(), NULL, 0);
		wchar_t* wstrBuf = new wchar_t[u16Len + 1];
		::MultiByteToWideChar(CP_UTF8, NULL, utf8Str.c_str(),(int)utf8Str.size(), wstrBuf, u16Len);
		wstrBuf[u16Len] = L'\0';
		std::wstring wStr = wstrBuf;
		delete [] wstrBuf;
		return wStr;
	}

	std::string UTF16ToUTF8(const std::wstring &utf16Str)
	{
		int u8Len = ::WideCharToMultiByte(CP_UTF8, NULL, utf16Str.c_str(), (int)utf16Str.size(), NULL, 0, NULL, NULL);
		char* strBuf = new char[u8Len + 1];
		::WideCharToMultiByte(CP_UTF8, NULL, utf16Str.c_str(), (int)utf16Str.size(), strBuf, u8Len, NULL, NULL);
		strBuf[u8Len] = '\0';
		std::string str = strBuf;
		delete [] strBuf;
		return str;
	}
}

