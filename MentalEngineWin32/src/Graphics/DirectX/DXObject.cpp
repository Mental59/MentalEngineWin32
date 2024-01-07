#include "DXObject.h"
#include "Utils/Macros.h"

Graphics::DXObject::DXObject(UINT width, UINT height, D3D_FEATURE_LEVEL featureLevel) :
	mFeatureLevel(featureLevel),
	mWidth(width),
	mHeight(height),
	mFenceEvent(nullptr),
	mFenceValue(0)
{

}

Graphics::DXObject::~DXObject()
{
}

void Graphics::DXObject::OnDestroy()
{
	CloseHandle(mFenceEvent);
}

void Graphics::DXObject::EnableDebugLayer()
{
#if defined(_DEBUG)
	Microsoft::WRL::ComPtr<ID3D12Debug> debugController;
	ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)));
	debugController->EnableDebugLayer();
#endif
}

void Graphics::DXObject::CreateFactory()
{
	UINT dxgiFactoryFlags = 0;

#if defined(_DEBUG)
	dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
#endif

	ThrowIfFailed(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&mDXGIFactory)));
}

void Graphics::DXObject::CreateDevice()
{
	HRESULT hardwareResult = D3D12CreateDevice(
		nullptr, // default adapter
		mFeatureLevel,
		IID_PPV_ARGS(&mDevice));

	// Fallback to WARP device.
	if (FAILED(hardwareResult))
	{
		Microsoft::WRL::ComPtr<IDXGIAdapter> pWarpAdapter;
		ThrowIfFailed(mDXGIFactory->EnumWarpAdapter(IID_PPV_ARGS(&pWarpAdapter)));
		ThrowIfFailed(D3D12CreateDevice(pWarpAdapter.Get(), mFeatureLevel, IID_PPV_ARGS(&mDevice)));
	}
}

void Graphics::DXObject::CreateFence()
{
	ThrowIfFailed(mDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&mFence)));
	mFenceValue = 1;

	// Create an event handle to use for frame synchronization.
	mFenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	if (mFenceEvent == nullptr)
	{
		ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));
	}
}

void Graphics::DXObject::GetDescriptorSizes()
{
	mRtvDescriptorSize = mDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	mDsvDescriptorSize = mDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	mCbvSrvUavDescriptorSize = mDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

bool Graphics::DXObject::CheckMSAASupport(D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS* msQualityLevels)
{
	ThrowIfFailed(mDevice->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, msQualityLevels, sizeof(*msQualityLevels)));
	return msQualityLevels->NumQualityLevels > 0;
}

void Graphics::DXObject::CreateCommandObjects(bool closeCommandList)
{
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

	ThrowIfFailed(mDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&mCommandQueue)));
	ThrowIfFailed(mDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&mCommandAllocator)));
	ThrowIfFailed(mDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, mCommandAllocator.Get(), nullptr, IID_PPV_ARGS(&mCommandList)));

	if (closeCommandList)
	{
		ThrowIfFailed(mCommandList->Close());
	}
}

void Graphics::DXObject::CreateSwapChain(
	UINT msaaSampleCount,
	UINT msaaQuality,
	UINT swapChainBufferCount,
	HWND outputWindow)
{
	mSwapChain.Reset();

	Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChain;

	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
	swapChainDesc.Width = mWidth;
	swapChainDesc.Height = mHeight;
	swapChainDesc.Format = mBackBufferFormat;
	swapChainDesc.SampleDesc.Count = msaaSampleCount;
	swapChainDesc.SampleDesc.Quality = msaaQuality;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = swapChainBufferCount;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	ThrowIfFailed(mDXGIFactory->CreateSwapChainForHwnd(
		mCommandQueue.Get(),
		outputWindow,
		&swapChainDesc,
		nullptr,
		nullptr,
		&swapChain
	));

	ThrowIfFailed(mDXGIFactory->MakeWindowAssociation(outputWindow, DXGI_MWA_NO_ALT_ENTER));

	ThrowIfFailed(swapChain.As(&mSwapChain));
}

void Graphics::DXObject::FlushCommandQueue()
{
	const UINT64 fence = mFenceValue;
	ThrowIfFailed(mCommandQueue->Signal(mFence.Get(), fence));
	mFenceValue++;

	if (mFence->GetCompletedValue() < fence)
	{
		ThrowIfFailed(mFence->SetEventOnCompletion(fence, mFenceEvent));
		WaitForSingleObject(mFenceEvent, INFINITE);
	}
}
