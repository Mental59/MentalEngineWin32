#pragma once

#include "BaseException.h"

class GraphicsException : public BaseException
{
public:
	GraphicsException(const std::string& description) : Description(description) {}
	GraphicsException(std::string&& description) : Description(std::move(description)) {}

	virtual std::wstring ToString() const override;

	std::string Description;
};