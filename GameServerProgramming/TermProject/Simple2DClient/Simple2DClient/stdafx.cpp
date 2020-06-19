#include "stdafx.h"

std::wstring utf8_to_wstring(const std::string_view& str) {
	std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
	return myconv.from_bytes(str.data());
}