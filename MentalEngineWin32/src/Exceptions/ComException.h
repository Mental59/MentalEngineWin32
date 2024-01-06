#pragma once

#include <windows.h>
#include "BaseException.h"

class ComException : public BaseException
{
public:
	ComException(HRESULT hr, const char* functionName, const char* filename, int linenumber) :
		ErrorCode(hr), FunctionName(functionName), Filename(filename), LineNumber(linenumber)
	{}

	virtual std::wstring ToString() const override;

	HRESULT ErrorCode;
	std::string FunctionName;
	std::string Filename;
	int LineNumber;
};
