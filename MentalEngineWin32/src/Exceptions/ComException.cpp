#include "ComException.h"

#include <format>
#include <comdef.h>

#include "../Utils/Helpers.h"

std::wstring ComException::ToString() const
{
	_com_error err(ErrorCode);
	std::wstring errorMessage = err.ErrorMessage();

	return std::format(
		L"{} failed in {}; line: {}; error: {}",
		Helpers::AnsiToWString(FunctionName),
		Helpers::AnsiToWString(Filename),
		std::to_wstring(LineNumber),
		errorMessage);
}

