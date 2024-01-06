#include "Initialization.h"
#include <cassert>
#include "../Utils/Macros.h"

void Graphics::EnableDebugLayer()
{
#if defined(_DEBUG)
	{
		Microsoft::WRL::ComPtr<ID3D12Debug> debugController;
		ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)));
		debugController->EnableDebugLayer();
	}
#endif
}

void Graphics::CreateFactory(Microsoft::WRL::ComPtr<IDXGIFactory7>& factory)
{
	ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&factory)));
}

void Graphics::CreateDevice(
	Microsoft::WRL::ComPtr<IDXGIFactory7>& factory,
	D3D_FEATURE_LEVEL featureLevel,
	Microsoft::WRL::ComPtr<ID3D12Device>& device)
{
	HRESULT hardwareResult = D3D12CreateDevice(
		nullptr, // default adapter
		featureLevel,
		IID_PPV_ARGS(&device));

	// Fallback to WARP device.
	if (FAILED(hardwareResult))
	{
		Microsoft::WRL::ComPtr<IDXGIAdapter> pWarpAdapter;
		ThrowIfFailed(factory->EnumWarpAdapter(IID_PPV_ARGS(&pWarpAdapter)));
		ThrowIfFailed(D3D12CreateDevice(pWarpAdapter.Get(), featureLevel, IID_PPV_ARGS(&device)));
	}
}

void Graphics::CreateFence(
	Microsoft::WRL::ComPtr<ID3D12Device>& device,
	Microsoft::WRL::ComPtr<ID3D12Fence>& fence)
{
	ThrowIfFailed(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)));
}

void Graphics::GetDescriptorSizes(
	Microsoft::WRL::ComPtr<ID3D12Device>& device,
	UINT* rtvDescriptorSize,
	UINT* dsvDescriptorSize,
	UINT* cbvSrvUavDescriptorSize
)
{
	*rtvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	*dsvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	*cbvSrvUavDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void Graphics::CheckMSAASupport(
	Microsoft::WRL::ComPtr<ID3D12Device>& device,
	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS* msQualityLevels)
{
	ThrowIfFailed(device->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, msQualityLevels, sizeof(*msQualityLevels)));
	assert(msQualityLevels->NumQualityLevels > 0);
}
