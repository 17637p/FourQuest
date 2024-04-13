#include "D3D11View.h"

#include <assert.h>

#include "Define.h"
#include "D3D11Device.h"
#include "D3D11ResourceManager.h"

using namespace fq_graphics;

/*=============================================================================
		RTV View
=============================================================================*/
D3D11RenderTargetView::D3D11RenderTargetView(std::shared_ptr<D3D11ResourceManager> resourceManager, const std::shared_ptr<D3D11Device>& d3d11Device, ED3D11RenderTargetViewType eViewType, const unsigned short width, const unsigned short height)
	:ResourceBase(resourceManager, ResourceType::RenderTargetView),
	mRTV(nullptr)
{
	ID3D11Device* device = d3d11Device->GetDevice().Get();
	IDXGISwapChain* swapChain = d3d11Device->GetSwapChain().Get();

	assert(device);
	assert(swapChain);

	// 화면에 그려질 버퍼 렌더 타겟 생성
	ID3D11Texture2D* backBuffer = nullptr;

	//ID3D11RenderTargetView* tempRenderTargetView = defaultRenderTargetView->mRTV.Get();

	HR(swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer)));
	if (backBuffer == nullptr)
	{
		MessageBox(NULL, L"backBuffer가 생성되지 않았습니다.", L"에러", MB_ICONERROR);
	}
	else
	{
		HR(device->CreateRenderTargetView(backBuffer, 0, &mRTV));
	}
	ReleaseCOM(backBuffer);
}

std::string D3D11RenderTargetView::GenerateRID(ED3D11RenderTargetViewType eViewType)
{
	return typeid(D3D11RenderTargetView).name() + std::to_string(static_cast<int>(eViewType));
}

void D3D11RenderTargetView::Bind(std::shared_ptr<D3D11Device>& d3d11Device)
{
	std::shared_ptr<D3D11DepthStencilView> dsv; //= mResourceManager->Get<D3D11DepthStencilView>(ED3D11DepthStencilViewType::Default);

	const float blackBackgroundColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	d3d11Device->GetDeviceContext()->ClearRenderTargetView(mRTV.Get(), blackBackgroundColor);
	//d3d11Device->GetDeviceContext()->OMSetRenderTargets(1, &mRTV, dsv->mDSV.Get());
}

/*=============================================================================
		SRV View
=============================================================================*/
D3D11ShaderResourceView::D3D11ShaderResourceView(std::shared_ptr<D3D11ResourceManager> resourceManager, const std::shared_ptr<D3D11Device>& d3d11Device, const std::shared_ptr<D3D11RenderTargetView>& rendertargetView)
	:ResourceBase(resourceManager, ResourceType::ShaderResourceView),
	mSRV(nullptr)
{
}

std::string D3D11ShaderResourceView::GenerateRID(ED3D11ShaderResourceViewType eViewType)
{
	return typeid(D3D11RenderTargetView).name() + std::to_string(static_cast<int>(eViewType));
}

/*=============================================================================
		DSV View
=============================================================================*/
D3D11DepthStencilView::D3D11DepthStencilView(std::shared_ptr<D3D11ResourceManager> resourceManager, const std::shared_ptr<D3D11Device>& d3d11Device, ED3D11DepthStencilViewType eViewType, const unsigned short width, const unsigned short height)
	:ResourceBase(resourceManager, ResourceType::DepthStencilView),
	mDSV(nullptr)
{
		ID3D11Device* device = d3d11Device->GetDevice().Get();

		// Depth stencil Buffer 생성
		D3D11_TEXTURE2D_DESC depthStencilDesc{};

		depthStencilDesc.Width = width;
		depthStencilDesc.Height = height;
		depthStencilDesc.MipLevels = 1;
		depthStencilDesc.ArraySize = 1;
		depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

		depthStencilDesc.SampleDesc.Count = 1;
		depthStencilDesc.SampleDesc.Quality = 0;

		depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
		depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthStencilDesc.CPUAccessFlags = 0;
		depthStencilDesc.MiscFlags = 0;

		ID3D11Texture2D* depthStencilBuffer;

		HR(device->CreateTexture2D(&depthStencilDesc, 0, &depthStencilBuffer));
		if (depthStencilBuffer == nullptr)
		{
			MessageBox(NULL, L"depthStencilBuffer가 생성되지 않았습니다.", L"에러", MB_ICONERROR);
		}
		else
		{
			HR(device->CreateDepthStencilView(depthStencilBuffer, 0, &mDSV));
		}
		ReleaseCOM(depthStencilBuffer);
}

std::string D3D11DepthStencilView::GenerateRID(ED3D11DepthStencilViewType eViewType)
{
	return typeid(D3D11RenderTargetView).name() + std::to_string(static_cast<int>(eViewType));
}
