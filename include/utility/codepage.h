#ifndef UTILITY_UTILITY_H_
#define UTILITY_UTILITY_H_

#include <string>

namespace utility
{
	std::wstring UTF8ToUTF16(const std::string &utf8Str);
	std::string UTF16ToUTF8(const std::wstring &utf16Str);
}

#endif // UTILITY_UTILITY_H_

