#include <Windows.h>
#include <format>
#include "Utils/d3dx12.h"
#include "Graphics/Core.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	const bool isDirectXMathSupported = Graphics::CheckDirectXMathSupport();

	MessageBox(
		nullptr,
		std::format(L"isDirectXMathSupported={}", isDirectXMathSupported).c_str(),
		L"Title",
		MB_SETFOREGROUND
	);
}
