#pragma once

#include <windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <dxgi1_6.h>

namespace Graphics
{
	constexpr D3D_FEATURE_LEVEL GRAPHICS_FEATURE_LEVEL = D3D_FEATURE_LEVEL_11_0;

	class App
	{
	public:
		App(HINSTANCE hInstance);
		virtual ~App();

		App(const App& other) = delete; // delete copy constructor
		App& operator=(const App& other) = delete; // delete copy assignment

		static App* GetApp();

		bool Initialize();
		int Run();

	protected:
		void InitializeGraphics();

	protected:
		static App* mApp;

		HINSTANCE mHAppInst = nullptr; // application instance handle
		HWND mHMainWnd = nullptr; // main window handle

		UINT mMSAASampleCount = 4;
		UINT mMSAAQuality = 0;

		Microsoft::WRL::ComPtr<IDXGIFactory7> mDXGIFactory;
		Microsoft::WRL::ComPtr<ID3D12Device> mDevice;
		Microsoft::WRL::ComPtr<ID3D12Fence> mFence;

		UINT mRtvDescriptorSize = 0;
		UINT mDsvDescriptorSize = 0;
		UINT mCbvSrvUavDescriptorSize = 0;

		DXGI_FORMAT mBackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
		DXGI_FORMAT mDepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	};
}
