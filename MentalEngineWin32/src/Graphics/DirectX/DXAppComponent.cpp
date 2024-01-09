#include "DXAppComponent.h"

#include <format>
#include <DirectXColors.h>

#include "Exceptions/GraphicsException.h"
#include "App/Win32App.h"
#include "Utils/Macros.h"

Graphics::DXAppComponent::DXAppComponent(
	UINT width,
	UINT height,
	UINT minWidth,
	UINT minHeight,
	std::wstring title,
	UINT msaaSampleCount,
	D3D_FEATURE_LEVEL featureLevel
) :
	DXBaseComponent(),
	mWidth(width),
	mHeight(height),
	mFeatureLevel(featureLevel),
	mTitle(title),
	mMSAASampleCount(msaaSampleCount),
	mMinWidth(minWidth),
	mMinHeight(minHeight),
	mViewport(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height)),
	mScissorRect(0, 0, static_cast<LONG>(width), static_cast<LONG>(height))
{ }

void Graphics::DXAppComponent::OnInit()
{
	DXBaseComponent::OnInit();

	EnableDebugLayer();
	CreateFactory();
	CreateDevice(mFeatureLevel);
	CreateFence();
	GetDescriptorSizes();

	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevel{ mBackBufferFormat, mMSAASampleCount, D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE, 0 };
	if (!CheckMSAASupport(&msQualityLevel)) throw GraphicsException(std::format("MSAA {}x is not supported on this platform", mMSAASampleCount));
	mMSAAQuality = msQualityLevel.NumQualityLevels - 1;

	CreateCommandObjects();
	CreateSwapChain(mWidth, mHeight, mMSAASampleCount, mMSAAQuality, App::Win32App::GetHWND());
	CreateDescriptorHeaps();
	CreateRenderTargetViews();
	CreateDepthStencilBufferAndView(mWidth, mHeight, mMSAASampleCount, mMSAAQuality);
}

void Graphics::DXAppComponent::OnDestroy()
{
	DXBaseComponent::OnDestroy();
}

void Graphics::DXAppComponent::OnResize(UINT newWidth, UINT newHeight)
{
	if (newWidth == mWidth && newHeight == mHeight)
	{
		return;
	}

#if defined(_DEBUG)
	OutputDebugString(std::format(L"New resolution: width={}, height={}\n", newWidth, newHeight).c_str());
#endif

	UpdateWindowSizeDependantFields(newWidth, newHeight);
}

void Graphics::DXAppComponent::Render(const Game::Timer& timer)
{
	PopulateCommandList();
	ExecuteCommandLists();
	SwapBackBuffers(0);
	FlushCommandQueue();
	UpdateBackBufferIndex();
}

void Graphics::DXAppComponent::Update(const Game::Timer& timer)
{
}

void Graphics::DXAppComponent::OnMouseDown(MouseButtonType button, UINT x, UINT y)
{
}

void Graphics::DXAppComponent::OnMouseUp(MouseButtonType button, UINT x, UINT y)
{
}

void Graphics::DXAppComponent::OnMouseMove(UINT x, UINT y)
{

}

void Graphics::DXAppComponent::OnKeyUp(UINT8 key)
{
}

void Graphics::DXAppComponent::OnKeyDown(UINT8 key)
{
}

void Graphics::DXAppComponent::UpdateWindowSizeDependantFields(UINT newWidth, UINT newHeight)
{
	mWidth = newWidth;
	mHeight = newHeight;

	mViewport = CD3DX12_VIEWPORT(0.0f, 0.0f, static_cast<float>(newWidth), static_cast<float>(newHeight));
	mScissorRect = CD3DX12_RECT(0, 0, static_cast<LONG>(newWidth), static_cast<LONG>(newHeight));
}

void Graphics::DXAppComponent::PopulateCommandList()
{
	ThrowIfFailed(mCommandAllocator->Reset());

	ThrowIfFailed(mCommandList->Reset(mCommandAllocator.Get(), mPipelineState.Get()));

	// Indicate that the back buffer will be used as a render target.
	CD3DX12_RESOURCE_BARRIER transition = CD3DX12_RESOURCE_BARRIER::Transition(
		mRenderTargets[mBackBufferIndex].Get(),
		D3D12_RESOURCE_STATE_PRESENT,
		D3D12_RESOURCE_STATE_RENDER_TARGET
	);
	mCommandList->ResourceBarrier(1, &transition);

	D3D12_CPU_DESCRIPTOR_HANDLE currentBackBufferView = GetCurrentBackBufferView();
	D3D12_CPU_DESCRIPTOR_HANDLE currentDepthStencilView = GetCurrentDepthStencilView();

	mCommandList->RSSetViewports(1, &mViewport);
	mCommandList->RSSetScissorRects(1, &mScissorRect);

	mCommandList->OMSetRenderTargets(1, &currentBackBufferView, true, &currentDepthStencilView);

	mCommandList->ClearRenderTargetView(currentBackBufferView, DirectX::Colors::Aquamarine, 0, nullptr);
	mCommandList->ClearDepthStencilView(currentDepthStencilView, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

	// Indicate that the back buffer will now be used to present.
	transition = CD3DX12_RESOURCE_BARRIER::Transition(
		mRenderTargets[mBackBufferIndex].Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PRESENT
	);
	mCommandList->ResourceBarrier(1, &transition);

	ThrowIfFailed(mCommandList->Close());
}
