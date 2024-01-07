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
	DXObject(width, height, featureLevel),
	mTitle(title),
	mMSAASampleCount(msaaSampleCount),
	mMinWidth(minWidth),
	mMinHeight(minHeight)
{ }

void Graphics::DXAppComponent::OnInit()
{
	LoadPipeline();
	isInitialized = true;
}

void Graphics::DXAppComponent::OnDestroy()
{
	DXObject::OnDestroy();
}

void Graphics::DXAppComponent::OnResize(UINT newWidth, UINT newHeight)
{

}

void Graphics::DXAppComponent::OnMouseDown(MouseButtonType button, int x, int y)
{
}

void Graphics::DXAppComponent::OnMouseUp(MouseButtonType button, int x, int y)
{
}

void Graphics::DXAppComponent::OnMouseMove(int x, int y)
{

}

void Graphics::DXAppComponent::OnKeyUp(WPARAM key)
{
}

void Graphics::DXAppComponent::Render(const Game::Timer& timer)
{
}

void Graphics::DXAppComponent::Update(const Game::Timer& timer)
{
}

void Graphics::DXAppComponent::LoadPipeline()
{
	EnableDebugLayer();
	CreateFactory();
	CreateDevice();
	CreateFence();
	GetDescriptorSizes();

	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevel{ mBackBufferFormat, mMSAASampleCount, D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE, 0 };
	if (!CheckMSAASupport(&msQualityLevel)) throw GraphicsException(std::format("MSAA {}x is not supported on this platform", mMSAASampleCount));
	mMSAAQuality = msQualityLevel.NumQualityLevels - 1;

	CreateCommandObjects();
	CreateSwapChain(mMSAASampleCount, mMSAAQuality, mSwapChainBufferCount, App::Win32App::GetHWND());
}