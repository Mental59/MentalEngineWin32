#pragma once

#include <windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <string>
#include "DXBaseComponent.h"
#include "Game/Timer.h"

namespace Graphics
{
	class DXAppComponent : public DXBaseComponent
	{
	public:
		enum ResizeMode
		{
			Minimized = 0,
			Maximized,
			Restored
		};

		enum MouseButtonType
		{
			Left = 0, Middle, Right
		};

		DXAppComponent(
			UINT width,
			UINT height,
			UINT minWidth,
			UINT minHeight,
			std::wstring title,
			UINT msaaSampleCount = 1,
			D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0);

		inline UINT GetMinWidth() const { return mMinWidth; }
		inline UINT GetMinHeight() const { return mMinHeight; }

		virtual void OnInit() override;
		virtual void OnDestroy() override;

		virtual void OnResize(UINT newWidth, UINT newHeight);
		virtual void OnMouseDown(MouseButtonType button, int x, int y);
		virtual void OnMouseUp(MouseButtonType button, int x, int y);
		virtual void OnMouseMove(int x, int y);
		virtual void OnKeyUp(WPARAM key);

		virtual void Render(const Game::Timer& timer);
		virtual void Update(const Game::Timer& timer);

		const WCHAR* GetTitle() const { return mTitle.c_str(); }

	protected:
		UINT mMinWidth;
		UINT mMinHeight;
		std::wstring mTitle;

		UINT mMSAASampleCount = 1;
		UINT mMSAAQuality = 0;

		CD3DX12_VIEWPORT mViewport;
		CD3DX12_RECT mScissorRect;
	};
}
