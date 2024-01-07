#pragma once

#include "Graphics/DirectX/DXAppComponent.h"
#include "Game/Timer.h"

namespace App
{
	class Win32App
	{
	public:
		static int Run(Graphics::DXAppComponent* dxComponent, HINSTANCE hInstance, int cmdShow);
		static inline HWND GetHWND() { return mHWND; }
		static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	private:
		static HWND mHWND;
		static Graphics::DXAppComponent* mDXComponent;
		static Game::Timer mTimer;

		static UINT mWindowWidth;
		static UINT mWindowHeight;

		static bool mResizing;
		static bool mAppPaused;
		static bool mMaximized;
		static bool mMinimized;
		static bool mFullscreenEnabled;
	};
}
