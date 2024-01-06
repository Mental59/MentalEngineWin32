#pragma once

#include <windows.h>
#include <string>

class ComException
{
public:
	HRESULT ErrorCode;
	std::string FunctionName;
	std::string Filename;
	int LineNumber;

	ComException(HRESULT hr, const char* functionName, const char* filename, int linenumber) :
		ErrorCode(hr), FunctionName(functionName), Filename(filename), LineNumber(linenumber)
	{}

	std::wstring ToString() const;
};
