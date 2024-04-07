#include "FQGameEnginePCH.h"
#include "Editor.h"

#include "GameProcess.h"
#include "WindowSystem.h"

fq::game_engine::Editor::Editor()
	:mGameProcess(nullptr)
{
}

fq::game_engine::Editor::~Editor()
{

}

void fq::game_engine::Editor::Initialize(GameProcess* process)
{
	mGameProcess = process;

	//tmp
	createDeviceD3D();

	initializeImGui();
}

void fq::game_engine::Editor::Finalize()
{

}

void fq::game_engine::Editor::initializeImGui()
{
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(mGameProcess->mWindowSystem->GetHWND());
	ImGui_ImplDX11_Init(mDevice, mDeviceContext);
}

void fq::game_engine::Editor::createDeviceD3D()
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

void fq::game_engine::Editor::createRenderTarget()
{
	ID3D11Texture2D* pBackBuffer;
	mSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
	mDevice->CreateRenderTargetView(pBackBuffer, nullptr, &mRenderTargetView);
	pBackBuffer->Release();
}

void fq::game_engine::Editor::cleanupRenderTarget()
{
	if (mRenderTargetView) { mRenderTargetView->Release(); mRenderTargetView = nullptr; }
}

void fq::game_engine::Editor::NewFrame()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void fq::game_engine::Editor::Render()
{
	ImGui::Render();
	const float clear_color_with_alpha[4] = { 1.f,1.f,0.f,1.f  };
	mDeviceContext->OMSetRenderTargets(1, &mRenderTargetView, nullptr);
	mDeviceContext->ClearRenderTargetView(mRenderTargetView, clear_color_with_alpha);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	mSwapChain->Present(1, 0); // Present with vsync
}

