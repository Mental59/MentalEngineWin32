#include "App.h"

#include <cassert>
#include <format>

#include "Initialization.h"
#include "../Exceptions/GraphicsException.h"

Graphics::App::App(HINSTANCE hInstance) : mHAppInst(hInstance)
{
	assert(mApp == nullptr);
	mApp = this;
}

Graphics::App* Graphics::App::mApp = nullptr;

Graphics::App::~App()
{

}

Graphics::App* Graphics::App::GetApp()
{
	return mApp;
}

bool Graphics::App::Initialize()
{
	InitializeGraphics();

	return true;
}

int Graphics::App::Run()
{
	MessageBox(nullptr, L"App successfully ran", L"App", MB_OK);
	return 0;
}

void Graphics::App::InitializeGraphics()
{
	Graphics::EnableDebugLayer();
	Graphics::CreateFactory(mDXGIFactory);
	Graphics::CreateDevice(mDXGIFactory, Graphics::GRAPHICS_FEATURE_LEVEL, mDevice);
	Graphics::CreateFence(mDevice, mFence);
	Graphics::GetDescriptorSizes(mDevice, &mRtvDescriptorSize, &mDsvDescriptorSize, &mCbvSrvUavDescriptorSize);

	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevel{ mBackBufferFormat, mMSAASampleCount, D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE, 0 };
	if (!Graphics::CheckMSAASupport(mDevice, &msQualityLevel)) throw GraphicsException(std::format("MSAA {}x is not supported on this platform", mMSAASampleCount));
	mMSAAQuality = msQualityLevel.NumQualityLevels - 1;
}
