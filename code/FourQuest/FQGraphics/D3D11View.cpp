#include "D3D11View.h"

#include <assert.h>

#include "Define.h"
#include "D3D11Device.h"
#include "D3D11ResourceManager.h"

using namespace fq::graphics;

/*=============================================================================
		RTV View
=============================================================================*/
fq::graphics::D3D11RenderTargetView::D3D11RenderTargetView(const std::shared_ptr<D3D11Device>& d3d11Device, const ED3D11RenderTargetViewType eViewType, const unsigned short width, const unsigned short height) 
	:ResourceBase(ResourceType::RenderTargetView),
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

		// 화면에 그려질 버퍼 렌더 타겟 생성
		ID3D11Texture2D* backBuffer = nullptr;

		HR(swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer)));
		if (backBuffer == nullptr)
		{
			MessageBox(NULL, L"backBuffer가 생성되지 않았습니다.", L"에러", MB_ICONERROR);
		}
		else
		{
			HR(d3d11Device->GetDevice()->CreateRenderTargetView(backBuffer, 0, mRTV.GetAddressOf()));
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

void fq::graphics::D3D11RenderTargetView::Bind(const std::shared_ptr<D3D11Device>& d3d11Device, const std::shared_ptr<D3D11DepthStencilView>& depthStencilView)
{
	std::shared_ptr<D3D11DepthStencilView> dsv = nullptr; 

	const float blackBackgroundColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	d3d11Device->GetDeviceContext()->ClearRenderTargetView(mRTV.Get(), blackBackgroundColor);

	// Todo: 일단은 none 을 넘기면 mDSV가 nullptr이라고 가정하고 한다
	if (depthStencilView->mDSV != nullptr)
	{
		d3d11Device->GetDeviceContext()->OMSetRenderTargets(1, mRTV.GetAddressOf(), dsv->mDSV.Get());
	}
	else
	{
		d3d11Device->GetDeviceContext()->OMSetRenderTargets(1, mRTV.GetAddressOf(), nullptr);
	}
}

/*=============================================================================
		SRV View
=============================================================================*/
fq::graphics::D3D11ShaderResourceView::D3D11ShaderResourceView(const std::shared_ptr<D3D11Device>& d3d11Device, 
	const std::shared_ptr<D3D11RenderTargetView>& rendertargetView) 
	:ResourceBase(ResourceType::ShaderResourceView),
	mSRV(nullptr)
{
	// 렌더 타겟에서 텍스처를 가져와서 SRV를 만든다.
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
	return typeid(D3D11ShaderResourceView).name() + std::to_string(static_cast<int>(eViewType));
}

void D3D11ShaderResourceView::Bind(const std::shared_ptr<D3D11Device>& d3d11Device, const UINT startSlot, const ED3D11ShaderType eShaderType)
{
	switch (eShaderType)
	{
		case ED3D11ShaderType::VertexShader:
		{
			d3d11Device->GetDeviceContext()->VSSetShaderResources(startSlot, 1, mSRV.GetAddressOf());
			break;
		}
		case ED3D11ShaderType::Pixelshader:
		{
			d3d11Device->GetDeviceContext()->PSSetShaderResources(startSlot, 1, mSRV.GetAddressOf());
			break;
		}
		case ED3D11ShaderType::GeometryShader:
		{
			d3d11Device->GetDeviceContext()->GSSetShaderResources(startSlot, 1, mSRV.GetAddressOf());
			break;
		}
		default:
			break;
	}
}

/*=============================================================================
		DSV View
=============================================================================*/
fq::graphics::D3D11DepthStencilView::D3D11DepthStencilView(const std::shared_ptr<D3D11Device>& d3d11Device, const ED3D11DepthStencilViewType eViewType, const unsigned short width, const unsigned short height) 
	:ResourceBase(ResourceType::DepthStencilView),
	mDSV(nullptr)
{
		ID3D11Device* device = d3d11Device->GetDevice().Get();

		if (eViewType == ED3D11DepthStencilViewType::None)
		{
			return;
		}

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
			HR(device->CreateDepthStencilView(depthStencilBuffer, 0, mDSV.GetAddressOf()));
		}
		ReleaseCOM(depthStencilBuffer);
}

std::string D3D11DepthStencilView::GenerateRID(const ED3D11DepthStencilViewType eViewType)
{
	return typeid(D3D11DepthStencilView).name() + std::to_string(static_cast<int>(eViewType));
}
