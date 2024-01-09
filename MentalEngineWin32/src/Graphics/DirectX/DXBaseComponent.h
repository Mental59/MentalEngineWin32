#pragma once

#include <windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include "Utils/d3dx12.h"

namespace Graphics
{
	class DXBaseComponent
	{
	public:
		DXBaseComponent();
		virtual ~DXBaseComponent();

		virtual void OnInit();
		virtual void OnDestroy();

		D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentBackBufferView() const;
		D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentDepthStencilView() const;

	protected:
		void EnableDebugLayer();
		void CreateFactory();
		void CreateDevice(D3D_FEATURE_LEVEL featureLevel);
		void CreateFence();
		void GetDescriptorSizes();
		bool CheckMSAASupport(D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS* msQualityLevels);
		void CreateCommandObjects(bool closeCommandList = true);
		void CreateSwapChain(UINT width, UINT height, UINT msaaSampleCount, UINT msaaQuality, HWND outputWindow);
		void FlushCommandQueue();
		void UpdateBackBufferIndex();
		void CreateDescriptorHeaps();
		void CreateRenderTargetViews();
		void CreateDepthStencilBufferAndView(UINT width, UINT height, UINT msaaSampleCount, UINT msaaQuality, D3D12_RESOURCE_STATES initialState);

	protected:
		static const UINT mSwapChainBufferCount = 2;

		Microsoft::WRL::ComPtr<IDXGIFactory7> mDXGIFactory;
		Microsoft::WRL::ComPtr<ID3D12Device> mDevice;
		Microsoft::WRL::ComPtr<ID3D12Fence> mFence;

		Microsoft::WRL::ComPtr<IDXGISwapChain4> mSwapChain;

		Microsoft::WRL::ComPtr<ID3D12CommandQueue> mCommandQueue;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mCommandList;
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> mCommandAllocator;

		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mRTVHeap;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mDSVHeap;

		Microsoft::WRL::ComPtr<ID3D12Resource> mRenderTargets[mSwapChainBufferCount];
		Microsoft::WRL::ComPtr<ID3D12Resource> mDepthStencilBuffer;

		UINT mRtvDescriptorSize = 0;
		UINT mDsvDescriptorSize = 0;
		UINT mCbvSrvUavDescriptorSize = 0;

		UINT mBackBufferIndex = 0;

		DXGI_FORMAT mBackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
		DXGI_FORMAT mDepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

		HANDLE mFenceEvent;
		UINT64 mFenceValue;
	};
}
