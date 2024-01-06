#include <Windows.h>
#include "Graphics/App.h"
#include "Exceptions/ComException.h"

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
		Graphics::App app(hInstance);

		if (!app.Initialize())
		{
			return 0;
		}

		return app.Run();
	}
	catch (ComException& e)
	{
		MessageBox(nullptr, e.ToString().c_str(), L"COM Exception", MB_OK);
		return 0;
	}
}
