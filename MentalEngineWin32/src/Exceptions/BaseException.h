#pragma once

#include <string>

class BaseException
{
public:
	virtual std::wstring ToString() const = 0;
};
