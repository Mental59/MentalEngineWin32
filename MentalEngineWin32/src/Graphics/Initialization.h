#pragma once

#include <windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <dxgi1_6.h>

namespace Graphics
{
	void EnableDebugLayer();

	void CreateFactory(Microsoft::WRL::ComPtr<IDXGIFactory7>& factory);

	void CreateDevice(
		Microsoft::WRL::ComPtr<IDXGIFactory7>& factory,
		D3D_FEATURE_LEVEL featureLevel,
		Microsoft::WRL::ComPtr<ID3D12Device>& device);

	void CreateFence(
		Microsoft::WRL::ComPtr<ID3D12Device>& device,
		Microsoft::WRL::ComPtr<ID3D12Fence>& fence);

	void GetDescriptorSizes(
		Microsoft::WRL::ComPtr<ID3D12Device>& device,
		UINT* rtvDescriptorSize,
		UINT* dsvDescriptorSize,
		UINT* cbvSrvDescriptorSize
	);

	bool CheckMSAASupport(
		Microsoft::WRL::ComPtr<ID3D12Device>& device,
		D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS* msQualityLevels);
}
