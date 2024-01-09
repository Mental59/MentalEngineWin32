#include "DXAppComponent.h"

#include <format>

#include "Exceptions/GraphicsException.h"
#include "App/Win32App.h"

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
	CreateDepthStencilBufferAndView(mWidth, mHeight, mMSAASampleCount, mMSAAQuality, D3D12_RESOURCE_STATE_DEPTH_WRITE);
}

void Graphics::DXAppComponent::OnDestroy()
{
	DXBaseComponent::OnDestroy();
}

void Graphics::DXAppComponent::OnResize(UINT newWidth, UINT newHeight)
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

void Graphics::DXAppComponent::Render(const Game::Timer& timer)
{
}

void Graphics::DXAppComponent::Update(const Game::Timer& timer)
{
}
