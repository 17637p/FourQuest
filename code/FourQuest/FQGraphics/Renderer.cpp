#include "Renderer.h"

#include <directxtk/SimpleMath.h>

#include "D3D11Device.h"
#include "D3D11State.h"
#include "D3D11View.h"
#include "D3D11Buffer.h"
#include "D3D11TextManager.h"

#include "D3D11ResourceManager.h"
#include "Define.h"

using namespace fq::graphics;

Renderer::Renderer()
	:mWindowWidth(0),
	mWindowHeight(0),
	mViewportWidth(0),
	mViewportHeight(0)
{
	mDevice = std::make_shared<D3D11Device>();
}

void Renderer::Initialize(const HWND hWnd, const unsigned short width, const unsigned short height)
{
	// 디바이스를 초기화 한다. (디바이스, 디바이스 컨텍스트)
	mDevice->Initialize(hWnd, width, height);

	mResourceManager = std::make_shared<D3D11ResourceManager>(mDevice);

	mRTVRenderer = mResourceManager->Create<fq::graphics::D3D11RenderTargetView>(ED3D11RenderTargetViewType::Default, width, height);
	mDSV = mResourceManager->Create<fq::graphics::D3D11DepthStencilView>(ED3D11DepthStencilViewType::Default, width, height);
	mDSV = mResourceManager->Create<fq::graphics::D3D11DepthStencilView>(ED3D11DepthStencilViewType::None, width, height);

	D3D11_VIEWPORT screenViewport{};

	/// 뷰포트 변환을 셋팅한다.
	screenViewport.TopLeftX = 0;
	screenViewport.TopLeftY = 0;
	screenViewport.Width = static_cast<float>(width);
	screenViewport.Height = static_cast<float>(height);
	screenViewport.MinDepth = 0.0f;
	screenViewport.MaxDepth = 1.0f;

	mDevice->GetDeviceContext()->RSSetViewports(1, &screenViewport);

	struct Trasform
	{
		int a;
		float b;
		int c;
		float d;
	};

	std::shared_ptr<D3D11ConstantBuffer<Trasform> > testConstantBuffer;

	Trasform tt{};
	tt.a = 1;
	tt.b = 2;
	tt.c = 3;
	tt.d = 4;

	testConstantBuffer = mResourceManager->Create<fq::graphics::D3D11ConstantBuffer<Trasform> >(ED3D11ConstantBuffer::Transform);
	testConstantBuffer->Update(mDevice, tt);
	testConstantBuffer->Bind(mDevice, ED3D11ShaderType::VertexShader, 0);
}

void Renderer::BeginRender()
{
	// 렌더 타겟을 바인딩 한다
	mRTVRenderer->Bind(mDevice, mResourceManager->Get<fq::graphics::D3D11DepthStencilView>(ED3D11DepthStencilViewType::None));

	int a = 1;

	std::shared_ptr<D3D11TextManager> testText = std::make_shared<D3D11TextManager>(mDevice);
	testText->DrawTextColor(500, 500, DirectX::SimpleMath::Color{ 1, 1, 1, 1 }, L"Test %d", a);
}

void Renderer::Render()
{
	// 그린다.
}

void Renderer::EndRender()
{
	// present
	ComPtr<IDXGISwapChain> swapChain = mDevice->GetSwapChain();
	HR(swapChain->Present(0, 0));
}

void Renderer::Finalize()
{

}



