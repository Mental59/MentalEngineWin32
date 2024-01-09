#include "DXBaseComponent.h"
#include "Utils/Macros.h"

Graphics::DXBaseComponent::DXBaseComponent() :
	mFenceEvent(nullptr),
	mFenceValue(0)
{

}

Graphics::DXBaseComponent::~DXBaseComponent()
{
}

void Graphics::DXBaseComponent::OnInit()
{

}

void Graphics::DXBaseComponent::OnDestroy()
{
	CloseHandle(mFenceEvent);
}

D3D12_CPU_DESCRIPTOR_HANDLE Graphics::DXBaseComponent::GetCurrentBackBufferView() const
{
	return CD3DX12_CPU_DESCRIPTOR_HANDLE(mRTVHeap->GetCPUDescriptorHandleForHeapStart(), mBackBufferIndex, mRtvDescriptorSize);
}

D3D12_CPU_DESCRIPTOR_HANDLE Graphics::DXBaseComponent::GetCurrentDepthStencilView() const
{
	return mDSVHeap->GetCPUDescriptorHandleForHeapStart();
}

void Graphics::DXBaseComponent::EnableDebugLayer()
{
#if defined(_DEBUG)
	Microsoft::WRL::ComPtr<ID3D12Debug> debugController;
	ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)));
	debugController->EnableDebugLayer();
#endif
}

void Graphics::DXBaseComponent::CreateFactory()
{
	UINT dxgiFactoryFlags = 0;

#if defined(_DEBUG)
	dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
#endif

	ThrowIfFailed(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&mDXGIFactory)));
}

void Graphics::DXBaseComponent::CreateDevice(D3D_FEATURE_LEVEL featureLevel)
{
	HRESULT hardwareResult = D3D12CreateDevice(
		nullptr, // default adapter
		featureLevel,
		IID_PPV_ARGS(&mDevice));

	// Fallback to WARP device.
	if (FAILED(hardwareResult))
	{
		Microsoft::WRL::ComPtr<IDXGIAdapter> pWarpAdapter;
		ThrowIfFailed(mDXGIFactory->EnumWarpAdapter(IID_PPV_ARGS(&pWarpAdapter)));
		ThrowIfFailed(D3D12CreateDevice(pWarpAdapter.Get(), featureLevel, IID_PPV_ARGS(&mDevice)));
	}
}

void Graphics::DXBaseComponent::CreateFence()
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

void Graphics::DXBaseComponent::GetDescriptorSizes()
{
	mRtvDescriptorSize = mDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	mDsvDescriptorSize = mDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	mCbvSrvUavDescriptorSize = mDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

bool Graphics::DXBaseComponent::CheckMSAASupport(D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS* msQualityLevels)
{
	ThrowIfFailed(mDevice->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, msQualityLevels, sizeof(*msQualityLevels)));
	return msQualityLevels->NumQualityLevels > 0;
}

void Graphics::DXBaseComponent::CreateCommandObjects(bool closeCommandList)
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

void Graphics::DXBaseComponent::CreateSwapChain(
	UINT width,
	UINT height,
	UINT msaaSampleCount,
	UINT msaaQuality,
	HWND outputWindow)
{
	mSwapChain.Reset();

	Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChain;

	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
	swapChainDesc.Width = width;
	swapChainDesc.Height = height;
	swapChainDesc.Format = mBackBufferFormat;
	swapChainDesc.SampleDesc.Count = msaaSampleCount;
	swapChainDesc.SampleDesc.Quality = msaaQuality;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = mSwapChainBufferCount;
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
	UpdateBackBufferIndex();
}

void Graphics::DXBaseComponent::FlushCommandQueue()
{
	// Wait for GPU to finish executing all commands
	const UINT64 fence = mFenceValue;
	ThrowIfFailed(mCommandQueue->Signal(mFence.Get(), fence));
	mFenceValue++;

	if (mFence->GetCompletedValue() < fence)
	{
		ThrowIfFailed(mFence->SetEventOnCompletion(fence, mFenceEvent));
		WaitForSingleObject(mFenceEvent, INFINITE);
	}
}

void Graphics::DXBaseComponent::UpdateBackBufferIndex()
{
	mBackBufferIndex = mSwapChain->GetCurrentBackBufferIndex();
}

void Graphics::DXBaseComponent::CreateDescriptorHeaps()
{
	// Describe and create a render target view (RTV) descriptor heap
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc{};
	rtvHeapDesc.NumDescriptors = mSwapChainBufferCount;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	ThrowIfFailed(mDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&mRTVHeap)));

	// Describe and create a depth stencil view (DSV) descriptor heap
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{};
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	ThrowIfFailed(mDevice->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&mDSVHeap)));
}

void Graphics::DXBaseComponent::CreateRenderTargetViews()
{
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(mRTVHeap->GetCPUDescriptorHandleForHeapStart());

	// Create a RTV for each back buffer
	for (UINT i = 0; i < mSwapChainBufferCount; i++)
	{
		ThrowIfFailed(mSwapChain->GetBuffer(i, IID_PPV_ARGS(&mRenderTargets[i])));
		mDevice->CreateRenderTargetView(mRenderTargets[i].Get(), nullptr, rtvHandle);
		rtvHandle.Offset(1, mRtvDescriptorSize);
	}
}

void Graphics::DXBaseComponent::CreateDepthStencilBufferAndView(
	UINT width,
	UINT height,
	UINT msaaSampleCount,
	UINT msaaQuality,
	D3D12_RESOURCE_STATES initialState
)
{
	CD3DX12_RESOURCE_DESC depthStencilDesc(
		D3D12_RESOURCE_DIMENSION_TEXTURE2D,
		0,
		width,
		height,
		1,
		1,
		mDepthStencilFormat,
		msaaSampleCount,
		msaaQuality,
		D3D12_TEXTURE_LAYOUT_UNKNOWN,
		D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL
	);
	CD3DX12_CLEAR_VALUE clearValue(mDepthStencilFormat, 1.0f, 0);
	CD3DX12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_DEFAULT);

	ThrowIfFailed(mDevice->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&depthStencilDesc,
		initialState,
		&clearValue,
		IID_PPV_ARGS(&mDepthStencilBuffer)
	));

	// Create a depth stencil view (DSV).
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	dsvDesc.Format = mDepthStencilFormat;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;
	mDevice->CreateDepthStencilView(mDepthStencilBuffer.Get(), &dsvDesc, GetCurrentDepthStencilView());
}

void Graphics::DXBaseComponent::ExecuteCommandLists()
{
	ID3D12CommandList* ppCommandLists[] = { mCommandList.Get() };
	mCommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
}

void Graphics::DXBaseComponent::SwapBackAndFrontBuffers(UINT syncInterval)
{
	ThrowIfFailed(mSwapChain->Present(syncInterval, 0));
	UpdateBackBufferIndex();
}
