#include "Renderer.h"

#include "D3D11Device.h"
#include "D3D11State.h"
#include "D3D11View.h"

#include "D3D11ResourceManager.h"
#include "Define.h"

using namespace fq_graphics;

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
	// ����̽��� �ʱ�ȭ �Ѵ�. (����̽�, ����̽� ���ؽ�Ʈ)
	mDevice->Initialize(hWnd, width, height);

	mResourceManager = std::make_shared<D3D11ResourceManager>(mDevice);

	mRTVRenderer = mResourceManager->Create<fq_graphics::D3D11RenderTargetView>(ED3D11RenderTargetViewType::Default, width, height);
	mDSV = mResourceManager->Create<fq_graphics::D3D11DepthStencilView>(ED3D11DepthStencilViewType::Default, width, height);
}

void Renderer::BeginRender()
{
	// ���� Ÿ���� ���ε� �Ѵ�
	mRTVRenderer->Bind(mDevice, ED3D11DepthStencilViewType::None);
}

void Renderer::Render()
{
	// �׸���.
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



