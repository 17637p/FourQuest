#include "D3D11View.h"

#include <assert.h>

#include "Define.h"
#include "D3D11Device.h"
#include "D3D11ResourceManager.h"

using namespace fq_graphics;

/*=============================================================================
		RTV View
=============================================================================*/
D3D11RenderTargetView::D3D11RenderTargetView(const std::shared_ptr<D3D11ResourceManager>& resourceManager, const std::shared_ptr<D3D11Device>& d3d11Device, const ED3D11RenderTargetViewType eViewType, const unsigned short width, const unsigned short height)
	:ResourceBase(resourceManager, ResourceType::RenderTargetView),
	mRTV(nullptr)
{
	switch (eViewType)
	{
	case ED3D11RenderTargetViewType::Default:
	{
		ID3D11Device* device = d3d11Device->GetDevice().Get();
		IDXGISwapChain* swapChain = d3d11Device->GetSwapChain().Get();

		assert(device);
		assert(swapChain);

		// ȭ�鿡 �׷��� ���� ���� Ÿ�� ����
		ID3D11Texture2D* backBuffer = nullptr;

		HR(swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer)));
		if (backBuffer == nullptr)
		{
			MessageBox(NULL, L"backBuffer�� �������� �ʾҽ��ϴ�.", L"����", MB_ICONERROR);
		}
		else
		{
			HR(d3d11Device->GetDevice()->CreateRenderTargetView(backBuffer, 0, &mRTV));
		}
		ReleaseCOM(backBuffer);

		return;
	}
	default:
		break;
	}
}

std::string D3D11RenderTargetView::GenerateRID(const ED3D11RenderTargetViewType eViewType)
{
	return typeid(D3D11RenderTargetView).name() + std::to_string(static_cast<int>(eViewType));
}

void D3D11RenderTargetView::Bind(const std::shared_ptr<D3D11Device>& d3d11Device)
{
	std::shared_ptr<D3D11DepthStencilView> dsv;// = mResourceManager->Get<D3D11DepthStencilView>(ED3D11DepthStencilViewType::Default);

	const float blackBackgroundColor[4] = { 0.2f, 0.7f, 0.2f, 1.0f };
	d3d11Device->GetDeviceContext()->ClearRenderTargetView(mRTV.Get(), blackBackgroundColor);
	//d3d11Device->GetDeviceContext()->OMSetRenderTargets(1, &mRTV, dsv->mDSV.Get());
}

/*=============================================================================
		SRV View
=============================================================================*/
D3D11ShaderResourceView::D3D11ShaderResourceView(const std::shared_ptr<D3D11ResourceManager>& resourceManager, 
	const std::shared_ptr<D3D11Device>& d3d11Device, 
	const std::shared_ptr<D3D11RenderTargetView>& rendertargetView)
	:ResourceBase(resourceManager, ResourceType::ShaderResourceView),
	mSRV(nullptr)
{
	ID3D11Texture2D* rendertargetTexture = nullptr;
	rendertargetView->mRTV->GetResource(reinterpret_cast<ID3D11Resource**>(&rendertargetTexture));
	
	D3D11_TEXTURE2D_DESC textureDesc;
	rendertargetTexture->GetDesc(&textureDesc);

	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc{};
	shaderResourceViewDesc.Format = textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = textureDesc.MipLevels;

	HR(d3d11Device->GetDevice()->CreateShaderResourceView(rendertargetTexture,
		&shaderResourceViewDesc,
		&mSRV));

	ReleaseCOM(rendertargetTexture);
}

std::string D3D11ShaderResourceView::GenerateRID(const ED3D11ShaderResourceViewType eViewType)
{
	return typeid(D3D11RenderTargetView).name() + std::to_string(static_cast<int>(eViewType));
}

void D3D11ShaderResourceView::Bind(const std::shared_ptr<D3D11Device>& d3d11Device, const UINT startSlot, const ED3D11ShaderType eShaderType)
{
	switch (eShaderType)
	{
		case ED3D11ShaderType::VertexShader:
		{
			d3d11Device->GetDeviceContext()->VSSetShaderResources(startSlot, 1, &mSRV);
		}
		case ED3D11ShaderType::Pixelshader:
		{
			d3d11Device->GetDeviceContext()->PSSetShaderResources(startSlot, 1, &mSRV);
		}
		case ED3D11ShaderType::GeometryShader:
		{
			d3d11Device->GetDeviceContext()->GSSetShaderResources(startSlot, 1, &mSRV);
		}
		default:
			break;
	}
}

/*=============================================================================
		DSV View
=============================================================================*/
D3D11DepthStencilView::D3D11DepthStencilView(const std::shared_ptr<D3D11ResourceManager>& resourceManager, const std::shared_ptr<D3D11Device>& d3d11Device, const ED3D11DepthStencilViewType eViewType, const unsigned short width, const unsigned short height)
	:ResourceBase(resourceManager, ResourceType::DepthStencilView),
	mDSV(nullptr)
{
		ID3D11Device* device = d3d11Device->GetDevice().Get();

		// Depth stencil Buffer ����
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
			MessageBox(NULL, L"depthStencilBuffer�� �������� �ʾҽ��ϴ�.", L"����", MB_ICONERROR);
		}
		else
		{
			HR(device->CreateDepthStencilView(depthStencilBuffer, 0, &mDSV));
		}
		ReleaseCOM(depthStencilBuffer);
}

std::string D3D11DepthStencilView::GenerateRID(const ED3D11DepthStencilViewType eViewType)
{
	return typeid(D3D11RenderTargetView).name() + std::to_string(static_cast<int>(eViewType));
}