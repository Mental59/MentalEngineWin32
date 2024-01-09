#include "Win32App.h"

#include <windowsx.h>

#include "Exceptions/GraphicsException.h"

HWND App::Win32App::mHWND(nullptr);
Graphics::DXAppComponent* App::Win32App::mDXComponent(nullptr);
Game::Timer App::Win32App::mTimer;

UINT App::Win32App::mWindowWidth;
UINT App::Win32App::mWindowHeight;

bool App::Win32App::mResizing;
bool App::Win32App::mAppPaused;
bool App::Win32App::mMaximized;
bool App::Win32App::mMinimized;
bool App::Win32App::mFullscreenEnabled;

int App::Win32App::Run(Graphics::DXAppComponent* dxComponent, HINSTANCE hInstance, int nCmdShow)
{
	mDXComponent = dxComponent;

	mResizing = false;
	mAppPaused = false;
	mMaximized = false;
	mMinimized = false;
	mFullscreenEnabled = false;

	mWindowWidth = dxComponent->GetWidth();
	mWindowHeight = dxComponent->GetHeight();

	//Initialize window class
	WNDCLASSEX windowClass{ 0 };
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc = WindowProc;
	windowClass.hInstance = hInstance;
	windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	windowClass.lpszClassName = L"DXSampleClass";
	RegisterClassEx(&windowClass);

	RECT windowRect = { 0, 0, static_cast<LONG>(mWindowWidth), static_cast<LONG>(mWindowHeight) };
	AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

	// Create the window and store a handle to it.
	mHWND = CreateWindow(
		windowClass.lpszClassName,
		dxComponent->GetTitle(),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		nullptr,        // We have no parent window.
		nullptr,        // We aren't using menus.
		hInstance,
		nullptr);

	dxComponent->OnInit();

	ShowWindow(mHWND, nCmdShow);

	mTimer.Reset();

	MSG msg = {};
	while (msg.message != WM_QUIT)
	{
		// Process any messages in the queue.
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	dxComponent->OnDestroy();

	return static_cast<char>(msg.wParam);
}

LRESULT CALLBACK App::Win32App::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_ACTIVATE:
	{
		if (LOWORD(wParam) == WA_INACTIVE)
		{
			mAppPaused = true;
			mTimer.Stop();
		}
		else
		{
			mAppPaused = false;
			mTimer.Start();
		}
		return 0;
	}

	case WM_PAINT:
	{
		if (!mAppPaused)
		{
			mTimer.Tick();
			mDXComponent->Update(mTimer);
			mDXComponent->Render(mTimer);
		}
		return 0;
	}

	case WM_SIZE:
	{
		RECT clientRect{};
		GetClientRect(hWnd, &clientRect);
		mWindowWidth = clientRect.right - clientRect.left;
		mWindowHeight = clientRect.bottom - clientRect.top;

		if (wParam == SIZE_MINIMIZED)
		{
			mAppPaused = true;
			mMinimized = true;
			mMaximized = false;
		}
		else if (wParam == SIZE_MAXIMIZED)
		{
			mAppPaused = false;
			mMinimized = false;
			mMaximized = true;
			mDXComponent->OnResize(mWindowWidth, mWindowHeight);
		}
		else if (wParam == SIZE_RESTORED)
		{
			if (mMinimized) // Restoring from minimized state?
			{
				mAppPaused = false;
				mMinimized = false;
				mDXComponent->OnResize(mWindowWidth, mWindowHeight);
			}
			else if (mMaximized) // Restoring from maximized state?
			{
				mAppPaused = false;
				mMaximized = false;
				mDXComponent->OnResize(mWindowWidth, mWindowHeight);
			}
			else if (mResizing)
			{
				// If user is dragging the resize bars, we do not resize 
				// the buffers here because as the user continuously 
				// drags the resize bars, a stream of WM_SIZE messages are
				// sent to the window, and it would be pointless (and slow)
				// to resize for each WM_SIZE message received from dragging
				// the resize bars.  So instead, we reset after the user is 
				// done resizing the window and releases the resize bars, which 
				// sends a WM_EXITSIZEMOVE message.
			}
			else
			{
				mDXComponent->OnResize(mWindowWidth, mWindowHeight);
			}
		}

		return 0;
	}

	case WM_ENTERSIZEMOVE:
	{
		mAppPaused = true;
		mResizing = true;
		mTimer.Stop();
		return 0;
	}

	case WM_EXITSIZEMOVE:
	{
		mAppPaused = false;
		mResizing = false;
		mTimer.Start();
		mDXComponent->OnResize(mWindowWidth, mWindowHeight);
		return 0;
	}

	case WM_GETMINMAXINFO:
	{
		((MINMAXINFO*)lParam)->ptMinTrackSize.x = mDXComponent->GetMinWidth();
		((MINMAXINFO*)lParam)->ptMinTrackSize.y = mDXComponent->GetMinHeight();
		return 0;
	}

	case WM_LBUTTONDOWN:
	{
		mDXComponent->OnMouseDown(Graphics::DXAppComponent::MouseButtonType::Left, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	}
	case WM_MBUTTONDOWN:
	{
		mDXComponent->OnMouseDown(Graphics::DXAppComponent::MouseButtonType::Middle, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	}
	case WM_RBUTTONDOWN:
	{
		mDXComponent->OnMouseDown(Graphics::DXAppComponent::MouseButtonType::Right, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	}

	case WM_LBUTTONUP:
	{
		mDXComponent->OnMouseUp(Graphics::DXAppComponent::MouseButtonType::Left, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	}
	case WM_MBUTTONUP:
	{
		mDXComponent->OnMouseUp(Graphics::DXAppComponent::MouseButtonType::Middle, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	}
	case WM_RBUTTONUP:
	{
		mDXComponent->OnMouseUp(Graphics::DXAppComponent::MouseButtonType::Right, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	}

	case WM_MOUSEMOVE:
	{
		mDXComponent->OnMouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	}

	case WM_KEYUP:
	{
		if (wParam == VK_ESCAPE) PostQuitMessage(0);
		mDXComponent->OnKeyUp(static_cast<UINT8>(wParam));
		return 0;
	}

	case WM_KEYDOWN:
	{
		mDXComponent->OnKeyDown(static_cast<UINT8>(wParam));
		return 0;
	}

	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}
