#pragma once

#include <windows.h>
#include <string>

namespace Helpers
{
	inline std::wstring AnsiToWString(const std::string& str)
	{
		constexpr int bufferSize = 512;
		WCHAR buffer[bufferSize];
		MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, buffer, bufferSize);
		return std::wstring(buffer);
	}
}
