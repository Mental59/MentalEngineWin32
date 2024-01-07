#include "GraphicsException.h"

#include "Utils/Helpers.h"

std::wstring GraphicsException::ToString() const
{
	return Helpers::AnsiToWString(Description);
}
