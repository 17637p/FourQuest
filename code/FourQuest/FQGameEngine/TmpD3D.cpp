
#include "TmpD3D.h"

#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include <imgui_internal.h>

#include "../FQGameModule/GameModule.h"
#include "../FQCommon/FQPath.h"

#include "GameProcess.h"
#include "WindowSystem.h"

fq::game_engine::TmpD3D::TmpD3D()
	:mGameProcess(nullptr)
{}

fq::game_engine::TmpD3D::~TmpD3D()
{

}

void fq::game_engine::TmpD3D::Initialize(GameProcess* process)
{
	mGameProcess = process;
	//tmp
	createDeviceD3D();
	clearGarbage();
}

void fq::game_engine::TmpD3D::createDeviceD3D()
{
	// Setup swap chain
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 2;
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = mGameProcess->mWindowSystem->GetHWND();
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	UINT createDeviceFlags = 0;
	//createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
	D3D_FEATURE_LEVEL featureLevel;
	const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
	HRESULT res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &mSwapChain, &mDevice, &featureLevel, &mDeviceContext);
	if (res == DXGI_ERROR_UNSUPPORTED) // Try high-performance WARP software driver if hardware is not available.
		res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &mSwapChain, &mDevice, &featureLevel, &mDeviceContext);

	createRenderTarget();
}

void fq::game_engine::TmpD3D::createRenderTarget()
{
	ID3D11Texture2D* pBackBuffer;
	mSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
	mDevice->CreateRenderTargetView(pBackBuffer, nullptr, &mRenderTargetView);
	pBackBuffer->Release();
}

void fq::game_engine::TmpD3D::cleanupRenderTarget()
{
	if (mRenderTargetView) { mRenderTargetView->Release(); mRenderTargetView = nullptr; }
}

void fq::game_engine::TmpD3D::Clear()
{
	const float clear_color_with_alpha[4] = { 1.f,1.f,0.f,1.f };
	mDeviceContext->OMSetRenderTargets(1, &mRenderTargetView, nullptr);
	mDeviceContext->ClearRenderTargetView(mRenderTargetView, clear_color_with_alpha);
}

void fq::game_engine::TmpD3D::Present()
{
	mSwapChain->Present(1, 0); // Present with vsync
}


void fq::game_engine::TmpD3D::clearGarbage()
{
	auto gargabePath = fq::path::GetGarbagePath();
	auto fileList = fq::path::GetFileList(gargabePath);

	for (const auto& file : fileList)
	{
		std::filesystem::remove_all(file);
	}
}

