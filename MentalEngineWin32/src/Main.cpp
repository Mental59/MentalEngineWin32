#include <windows.h>
#include "Graphics/DirectX/DXAppComponent.h"
#include "App/Win32App.h"
#include "Exceptions/BaseException.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	// Enable run-time memory check for debug builds.
#if defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	try
	{
		Graphics::DXAppComponent dxComponent(800, 600, 200, 200, L"D3D12 App");
		return App::Win32App::Run(&dxComponent, hInstance, nCmdShow);
	}
	catch (BaseException& e)
	{
		MessageBox(nullptr, e.ToString().c_str(), L"Exception", MB_OK);
		return 0;
	}
}
