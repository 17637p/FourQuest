#include <assert.h>
#include "D3D11View.h"
#include "Define.h"
#include "D3D11Device.h"
#include "D3D11ResourceManager.h"
#include "D3D11Buffer.h"

using namespace fq::graphics;

/*=============================================================================
		RTV View
=============================================================================*/
fq::graphics::D3D11RenderTargetView::D3D11RenderTargetView(const std::shared_ptr<D3D11Device>& d3d11Device, const ED3D11RenderTargetViewType eViewType, const unsigned short width, const unsigned short height)
	:ResourceBase(ResourceType::RenderTargetView),
	mRTV(nullptr)
{
	OnResize(d3d11Device, eViewType, width, height);
}

std::string D3D11RenderTargetView::GenerateRID(const ED3D11RenderTargetViewType eViewType)
{
	return typeid(D3D11RenderTargetView).name() + std::to_string(static_cast<int>(eViewType));
}

void  fq::graphics::D3D11RenderTargetView::Clear(const std::shared_ptr<D3D11Device>& d3d11Device, const DirectX::SimpleMath::Color& clearColor /* = { 0.f, 0.f, 0.f, 1.f } */)
{
	d3d11Device->GetDeviceContext()->ClearRenderTargetView(mRTV.Get(), clearColor);
}
void fq::graphics::D3D11RenderTargetView::OnResize(const std::shared_ptr<D3D11Device>& d3d11Device,
	const ED3D11RenderTargetViewType eViewType,
	const unsigned short width, const unsigned short height)
{
	Release();

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
	case ED3D11RenderTargetViewType::Albedo:
		// intentional fall through
	case ED3D11RenderTargetViewType::LightProbeIrr:
	{
		D3D11_TEXTURE2D_DESC textureDesc = {};
		textureDesc.Width = width;
		textureDesc.Height = height;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;
		textureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = 0;

		ComPtr<ID3D11Texture2D> pTexture;
		HR(d3d11Device->GetDevice()->CreateTexture2D(&textureDesc, nullptr, &pTexture));

		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
		rtvDesc.Format = textureDesc.Format;
		rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		rtvDesc.Texture2D = D3D11_TEX2D_RTV{ 0 };
		HR(d3d11Device->GetDevice()->CreateRenderTargetView(pTexture.Get(), &rtvDesc, mRTV.GetAddressOf()));

		break;
	}
	case ED3D11RenderTargetViewType::Emissive:
		// intentional fall through
	case ED3D11RenderTargetViewType::ColorAcuumulation:
		// intentional fall through
	case ED3D11RenderTargetViewType::Normal:
		// intentional fall through
	case ED3D11RenderTargetViewType::SourceNormal:
		// intentional fall through
	case ED3D11RenderTargetViewType::SourceTangent:
		// intentional fall through
	case ED3D11RenderTargetViewType::Offscreen:
		// intentional fall through
	case ED3D11RenderTargetViewType::OffscreenHDR:
		// intentional fall through
	case ED3D11RenderTargetViewType::PreCalculatedLight:
		// intentional fall through
	case ED3D11RenderTargetViewType::SSAODepth:
	{
		D3D11_TEXTURE2D_DESC textureDesc = {};
		textureDesc.Width = width;
		textureDesc.Height = height;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;
		textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = 0;

		ComPtr<ID3D11Texture2D> pTexture;
		HR(d3d11Device->GetDevice()->CreateTexture2D(&textureDesc, nullptr, &pTexture));

		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
		rtvDesc.Format = textureDesc.Format;
		rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		rtvDesc.Texture2D = D3D11_TEX2D_RTV{ 0 };
		HR(d3d11Device->GetDevice()->CreateRenderTargetView(pTexture.Get(), &rtvDesc, mRTV.GetAddressOf()));

		break;
	}
	case ED3D11RenderTargetViewType::PositionWClipZ:
	{
		D3D11_TEXTURE2D_DESC textureDesc = {};
		textureDesc.Width = width;
		textureDesc.Height = height;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;
		textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = 0;

		ComPtr<ID3D11Texture2D> pTexture;
		HR(d3d11Device->GetDevice()->CreateTexture2D(&textureDesc, nullptr, &pTexture));

		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
		rtvDesc.Format = textureDesc.Format;
		rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		rtvDesc.Texture2D = D3D11_TEX2D_RTV{ 0 };
		HR(d3d11Device->GetDevice()->CreateRenderTargetView(pTexture.Get(), &rtvDesc, mRTV.GetAddressOf()));

		break;
	}
	case ED3D11RenderTargetViewType::PixeldRevealageThreshold:
		// intentional fall through
	case ED3D11RenderTargetViewType::OffscreenGrayscale:
	{
		D3D11_TEXTURE2D_DESC textureDesc = {};
		textureDesc.Width = width;
		textureDesc.Height = height;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;
		textureDesc.Format = DXGI_FORMAT_R8_UNORM;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = 0;

		ComPtr<ID3D11Texture2D> pTexture;
		HR(d3d11Device->GetDevice()->CreateTexture2D(&textureDesc, nullptr, &pTexture));

		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
		rtvDesc.Format = textureDesc.Format;
		rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		rtvDesc.Texture2D = D3D11_TEX2D_RTV{ 0 };
		HR(d3d11Device->GetDevice()->CreateRenderTargetView(pTexture.Get(), &rtvDesc, mRTV.GetAddressOf()));

		break;
	}
	case ED3D11RenderTargetViewType::MetalnessRoughness:
	{
		D3D11_TEXTURE2D_DESC textureDesc = {};
		textureDesc.Width = width;
		textureDesc.Height = height;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;
		textureDesc.Format = DXGI_FORMAT_R8G8_UNORM;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = 0;

		ComPtr<ID3D11Texture2D> pTexture;
		HR(d3d11Device->GetDevice()->CreateTexture2D(&textureDesc, nullptr, &pTexture));

		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
		rtvDesc.Format = textureDesc.Format;
		rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		rtvDesc.Texture2D = D3D11_TEX2D_RTV{ 0 };
		HR(d3d11Device->GetDevice()->CreateRenderTargetView(pTexture.Get(), &rtvDesc, mRTV.GetAddressOf()));

		break;
	}
	case ED3D11RenderTargetViewType::OutLineBlur:
	{
		// HDR 수정으로 인한 텍스처 형식 변환
		D3D11_TEXTURE2D_DESC textureDesc = {};
		textureDesc.Width = width;
		textureDesc.Height = height;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;
		textureDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = 0;

		ComPtr<ID3D11Texture2D> pTexture;
		HR(d3d11Device->GetDevice()->CreateTexture2D(&textureDesc, nullptr, &pTexture));

		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
		rtvDesc.Format = textureDesc.Format;
		rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		rtvDesc.Texture2D = D3D11_TEX2D_RTV{ 0 };
		HR(d3d11Device->GetDevice()->CreateRenderTargetView(pTexture.Get(), &rtvDesc, mRTV.GetAddressOf()));

		break;
	}
	case ED3D11RenderTargetViewType::SingleColor:
		// intentional fall through
	case ED3D11RenderTargetViewType::Picking:
	{
		D3D11_TEXTURE2D_DESC textureDesc = {};
		textureDesc.Width = width;
		textureDesc.Height = height;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;
		textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = 0;

		ComPtr<ID3D11Texture2D> pTexture;
		HR(d3d11Device->GetDevice()->CreateTexture2D(&textureDesc, nullptr, &pTexture));

		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
		rtvDesc.Format = textureDesc.Format;
		rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		rtvDesc.Texture2D = D3D11_TEX2D_RTV{ 0 };
		HR(d3d11Device->GetDevice()->CreateRenderTargetView(pTexture.Get(), &rtvDesc, mRTV.GetAddressOf()));

		break;
	}
	case ED3D11RenderTargetViewType::OutLine:
	{
		D3D11_TEXTURE2D_DESC textureDesc = {};
		textureDesc.Width = width;
		textureDesc.Height = height;
		if (width < 64 || height < 64)
		{
			textureDesc.MipLevels = 1;
		}
		else
		{
			textureDesc.MipLevels = 4;
		}
		textureDesc.ArraySize = 1;
		textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

		ComPtr<ID3D11Texture2D> pTexture;
		HR(d3d11Device->GetDevice()->CreateTexture2D(&textureDesc, nullptr, &pTexture));

		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
		rtvDesc.Format = textureDesc.Format;
		rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		rtvDesc.Texture2D = D3D11_TEX2D_RTV{ 0 };
		HR(d3d11Device->GetDevice()->CreateRenderTargetView(pTexture.Get(), &rtvDesc, mRTV.GetAddressOf()));

		break;
	}
	default:
		break;
	}
}
void fq::graphics::D3D11RenderTargetView::Release()
{
	ULONG refCount = mRTV.Reset();
	assert(refCount == 0);
}

void fq::graphics::D3D11RenderTargetView::Bind(const std::shared_ptr<D3D11Device>& d3d11Device, std::vector<std::shared_ptr<D3D11RenderTargetView>> renderTargetViews, const std::shared_ptr<D3D11DepthStencilView>& depthStencilView)
{
	assert(renderTargetViews.size() > 0);

	UINT numViews = (UINT)renderTargetViews.size();
	std::vector<ID3D11RenderTargetView*> RTVs;
	RTVs.reserve(renderTargetViews.size());

	D3D11_TEXTURE2D_DESC textureDesc;
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetDesc;

	for (std::shared_ptr<D3D11RenderTargetView>& rtv : renderTargetViews)
	{
		if (rtv != nullptr)
		{
			RTVs.push_back(rtv->mRTV.Get());
		}
		else
		{
			RTVs.push_back(nullptr);
		}
	}

	if (depthStencilView->mDSV != nullptr)
	{
		d3d11Device->GetDeviceContext()->OMSetRenderTargets(numViews, RTVs.data(), depthStencilView->mDSV.Get());
	}
	else
	{
		d3d11Device->GetDeviceContext()->OMSetRenderTargets(numViews, RTVs.data(), nullptr);
	}
}

void fq::graphics::D3D11RenderTargetView::Bind(const std::shared_ptr<D3D11Device>& d3d11Device, const std::shared_ptr<D3D11DepthStencilView>& depthStencilView)
{
	// Todo: 일단은 none 을 넘기면 mDSV가 nullptr이라고 가정하고 한다
	if (depthStencilView->mDSV != nullptr)
	{
		d3d11Device->GetDeviceContext()->OMSetRenderTargets(1, mRTV.GetAddressOf(), depthStencilView->mDSV.Get());
	}
	else
	{
		d3d11Device->GetDeviceContext()->OMSetRenderTargets(1, mRTV.GetAddressOf(), nullptr);
	}
}

void D3D11RenderTargetView::UnBind(const std::shared_ptr<D3D11Device>& d3d11Device)
{
	ID3D11RenderTargetView* tempRTV = nullptr;
	ID3D11DepthStencilView* tempDSV = nullptr;
	d3d11Device->GetDeviceContext()->OMSetRenderTargets(1, &tempRTV, tempDSV);
}

Microsoft::WRL::ComPtr<ID3D11RenderTargetView> D3D11RenderTargetView::GetRTV()
{
	return mRTV;
}

/*=============================================================================
		SRV View
=============================================================================*/
fq::graphics::D3D11ShaderResourceView::D3D11ShaderResourceView(const std::shared_ptr<D3D11Device>& d3d11Device,
	const std::shared_ptr<D3D11RenderTargetView>& rendertargetView)
	:ResourceBase(ResourceType::ShaderResourceView),
	mSRV(nullptr)
{
	Init(d3d11Device, rendertargetView);
}

fq::graphics::D3D11ShaderResourceView::D3D11ShaderResourceView(const std::shared_ptr<D3D11Device>& d3d11Device, const std::shared_ptr<D3D11DepthStencilView>& depthStencilView)
	:ResourceBase(ResourceType::ShaderResourceView),
	mSRV(nullptr)
{
	ID3D11Texture2D* texture = nullptr;
	depthStencilView->mDSV->GetResource(reinterpret_cast<ID3D11Resource**>(&texture));

	D3D11_TEXTURE2D_DESC textureDesc;
	texture->GetDesc(&textureDesc);

	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc{};

	switch (textureDesc.Format)
	{
	case DXGI_FORMAT_R32_TYPELESS:
		shaderResourceViewDesc.Format = DXGI_FORMAT_R32_FLOAT;
		break;
	case DXGI_FORMAT_R24G8_TYPELESS:
		shaderResourceViewDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		break;
	default:
		assert(false);
	}

	// if arraysize == 6 create cubeTexture
	if (textureDesc.ArraySize > 1)
	{
		shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
		shaderResourceViewDesc.Texture2DArray.MostDetailedMip = 0;
		shaderResourceViewDesc.Texture2DArray.FirstArraySlice = 0;
		shaderResourceViewDesc.Texture2DArray.MipLevels = textureDesc.MipLevels;
		shaderResourceViewDesc.Texture2DArray.ArraySize = textureDesc.ArraySize;
	}
	else
	{
		shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
		shaderResourceViewDesc.Texture2D.MipLevels = textureDesc.MipLevels;
	}

	HR(d3d11Device->GetDevice()->CreateShaderResourceView(texture,
		&shaderResourceViewDesc,
		&mSRV));

	ReleaseCOM(texture);
}

D3D11ShaderResourceView::D3D11ShaderResourceView(const std::shared_ptr<D3D11Device>& d3d11Device, const std::shared_ptr<class D3D11DepthStencilView>& depthStencilView, DXGI_FORMAT format)
	:ResourceBase(ResourceType::ShaderResourceView),
	mSRV(nullptr)
{
	ID3D11Texture2D* texture = nullptr;
	depthStencilView->mDSV->GetResource(reinterpret_cast<ID3D11Resource**>(&texture));

	D3D11_TEXTURE2D_DESC textureDesc;
	texture->GetDesc(&textureDesc);

	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;

	shaderResourceViewDesc.Format = format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = textureDesc.MipLevels;

	HR(d3d11Device->GetDevice()->CreateShaderResourceView(texture,
		&shaderResourceViewDesc,
		&mSRV));

	ReleaseCOM(texture);
}

fq::graphics::D3D11ShaderResourceView::D3D11ShaderResourceView(const std::shared_ptr<D3D11Device>& d3d11Device, const std::shared_ptr<class D3D11UnorderedAccessView>& unorderedAccessView, bool bIsTexture)
	: ResourceBase(ResourceType::ShaderResourceView)
	, mSRV(nullptr)
{
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc = {};

	if (!bIsTexture)
	{
		ComPtr<ID3D11Buffer> buffer;
		unorderedAccessView->mView->GetResource(reinterpret_cast<ID3D11Resource**>(buffer.GetAddressOf()));

		D3D11_BUFFER_DESC bufferDesc;
		buffer->GetDesc(&bufferDesc);

		shaderResourceViewDesc.Format = DXGI_FORMAT_UNKNOWN;
		shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		shaderResourceViewDesc.Buffer.ElementWidth = bufferDesc.StructureByteStride;
		shaderResourceViewDesc.Buffer.FirstElement = 0;
		shaderResourceViewDesc.Buffer.NumElements = bufferDesc.ByteWidth / bufferDesc.StructureByteStride;
		assert(bufferDesc.ByteWidth % bufferDesc.StructureByteStride == 0);

		HR(d3d11Device->GetDevice()->CreateShaderResourceView(buffer.Get(), &shaderResourceViewDesc, mSRV.GetAddressOf()));
	}
	else
	{
		ComPtr<ID3D11Texture2D> texture;
		unorderedAccessView->mView->GetResource(reinterpret_cast<ID3D11Resource**>(texture.GetAddressOf()));

		D3D11_TEXTURE2D_DESC textureDesc;
		texture->GetDesc(&textureDesc);

		shaderResourceViewDesc.Format = textureDesc.Format;
		shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
		shaderResourceViewDesc.Texture2D.MipLevels = textureDesc.MipLevels;

		ULONG refCount = mSRV.Reset();
		assert(refCount == 0);

		HR(d3d11Device->GetDevice()->CreateShaderResourceView(texture.Get(),
			nullptr,
			mSRV.GetAddressOf()));
	}
}

void D3D11ShaderResourceView::UnBind(const std::shared_ptr<D3D11Device>& d3d11Device, const UINT startSlot, const ED3D11ShaderType eShaderType)
{
	ID3D11ShaderResourceView* tempShaderResource = nullptr;
	switch (eShaderType)
	{
	case ED3D11ShaderType::VertexShader:
	{
		d3d11Device->GetDeviceContext()->VSSetShaderResources(startSlot, 1, &tempShaderResource);
		break;
	}
	case ED3D11ShaderType::PixelShader:
	{
		d3d11Device->GetDeviceContext()->PSSetShaderResources(startSlot, 1, &tempShaderResource);
		break;
	}
	case ED3D11ShaderType::ComputeShader:
	{
		d3d11Device->GetDeviceContext()->CSSetShaderResources(startSlot, 1, &tempShaderResource);
		break;
	}
	case ED3D11ShaderType::GeometryShader:
	{
		d3d11Device->GetDeviceContext()->GSSetShaderResources(startSlot, 1, &tempShaderResource);
		break;
	}
	default:
		break;
	}
}

std::string D3D11ShaderResourceView::GenerateRID(const ED3D11ShaderResourceViewType eViewType)
{
	return typeid(D3D11ShaderResourceView).name() + std::to_string(static_cast<int>(eViewType));
}

void D3D11ShaderResourceView::Init(const std::shared_ptr<D3D11Device>& d3d11Device, const std::shared_ptr<D3D11RenderTargetView>& rendertargetView)
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

	ULONG refCount = mSRV.Reset();
	assert(refCount == 0);

	HR(d3d11Device->GetDevice()->CreateShaderResourceView(rendertargetTexture,
		&shaderResourceViewDesc,
		mSRV.GetAddressOf()));

	ReleaseCOM(rendertargetTexture);
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
	case ED3D11ShaderType::PixelShader:
	{
		d3d11Device->GetDeviceContext()->PSSetShaderResources(startSlot, 1, mSRV.GetAddressOf());
		break;
	}
	case ED3D11ShaderType::ComputeShader:
	{
		d3d11Device->GetDeviceContext()->CSSetShaderResources(startSlot, 1, mSRV.GetAddressOf());
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
	OnResize(d3d11Device, eViewType, width, height);
}

void D3D11DepthStencilView::Clear(const std::shared_ptr<D3D11Device>& d3d11Device)
{
	d3d11Device->GetDeviceContext()->ClearDepthStencilView(mDSV.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
}
void D3D11DepthStencilView::OnResize(const std::shared_ptr<D3D11Device>& d3d11Device,
	const ED3D11DepthStencilViewType eViewType,
	const unsigned short width, const unsigned short height)
{
	Release();

	ID3D11Device* device = d3d11Device->GetDevice().Get();

	if (eViewType == ED3D11DepthStencilViewType::None)
	{
		return;
	}

	D3D11_TEXTURE2D_DESC depthStencilDesc{};
	depthStencilDesc.Width = width;
	depthStencilDesc.Height = height;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	D3D11_DEPTH_STENCIL_VIEW_DESC descView = {};
	descView.Flags = 0;
	descView.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descView.Texture2D.MipSlice = 0;

	switch (eViewType)
	{
	case ED3D11DepthStencilViewType::Default:
		depthStencilDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
		depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;

		descView.Format = DXGI_FORMAT::DXGI_FORMAT_D24_UNORM_S8_UINT;
		break;
	case ED3D11DepthStencilViewType::ShaderInputDepthStencil:
		depthStencilDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R32_TYPELESS;
		depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;

		descView.Format = DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT;

		break;
	case ED3D11DepthStencilViewType::CascadeShadow1:
		depthStencilDesc.ArraySize = 3;

		depthStencilDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R32_TYPELESS;
		depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;

		descView.Format = DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT;
		descView.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
		descView.Texture2DArray.MipSlice = 0;
		descView.Texture2DArray.FirstArraySlice = 0;
		descView.Texture2DArray.ArraySize = 3;

		break;
	case ED3D11DepthStencilViewType::CascadeShadow2:
		depthStencilDesc.ArraySize = 6;

		depthStencilDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R32_TYPELESS;
		depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;

		descView.Format = DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT;
		descView.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
		descView.Texture2DArray.MipSlice = 0;
		descView.Texture2DArray.FirstArraySlice = 0;
		descView.Texture2DArray.ArraySize = 6;

		break;
	case ED3D11DepthStencilViewType::CascadeShadow3:
		depthStencilDesc.ArraySize = 9;

		depthStencilDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R32_TYPELESS;
		depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;

		descView.Format = DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT;
		descView.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
		descView.Texture2DArray.MipSlice = 0;
		descView.Texture2DArray.FirstArraySlice = 0;
		descView.Texture2DArray.ArraySize = 9;

		break;
	case ED3D11DepthStencilViewType::PointLightShadow:
		depthStencilDesc.ArraySize = 6;

		depthStencilDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R32_TYPELESS;
		depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;

		descView.Format = DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT;
		descView.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
		descView.Texture2DArray.MipSlice = 0;
		descView.Texture2DArray.FirstArraySlice = 0;
		descView.Texture2DArray.ArraySize = 6;
		break;
	case ED3D11DepthStencilViewType::Picking:
		depthStencilDesc.Format = DXGI_FORMAT::DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		descView.Format = DXGI_FORMAT::DXGI_FORMAT_D24_UNORM_S8_UINT;

		break;
	case ED3D11DepthStencilViewType::None:
		// intentional fall through
	default:
		assert(false);
		break;
	}
	// Depth stencil Buffer 생성
	ID3D11Texture2D* depthStencilBuffer;
	HR(device->CreateTexture2D(&depthStencilDesc, 0, &depthStencilBuffer));

	if (depthStencilBuffer == nullptr)
	{
		MessageBox(NULL, L"depthStencilBuffer가 생성되지 않았습니다.", L"에러", MB_ICONERROR);
	}
	else
	{
		HR(device->CreateDepthStencilView(depthStencilBuffer, &descView, mDSV.GetAddressOf()));
	}
	ReleaseCOM(depthStencilBuffer);
}
void D3D11DepthStencilView::Release()
{
	ULONG refCount = mDSV.Reset();
	assert(refCount == 0);
}

std::string D3D11DepthStencilView::GenerateRID(const ED3D11DepthStencilViewType eViewType)
{
	return typeid(D3D11DepthStencilView).name() + std::to_string(static_cast<int>(eViewType));
}

fq::graphics::D3D11UnorderedAccessView::D3D11UnorderedAccessView(const std::shared_ptr<D3D11Device>& d3d11Device, const unsigned short width, const unsigned short height)
{
	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	ComPtr<ID3D11Texture2D> pTexture;
	HR(d3d11Device->GetDevice()->CreateTexture2D(&textureDesc, nullptr, &pTexture));

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
	uavDesc.Format = textureDesc.Format;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;

	HR(d3d11Device->GetDevice()->CreateUnorderedAccessView(pTexture.Get(), &uavDesc, mView.GetAddressOf()));
}

D3D11UnorderedAccessView::D3D11UnorderedAccessView(const std::shared_ptr<D3D11Device>& d3d11Device, std::shared_ptr<D3D11StructuredBuffer> buffer, eUAVType type)
{
	Microsoft::WRL::ComPtr<ID3D11Buffer> d3dBuffer = buffer->GetBuffer();

	D3D11_BUFFER_DESC bufferDesc;
	d3dBuffer->GetDesc(&bufferDesc);

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.FirstElement = 0;
	uavDesc.Buffer.NumElements = bufferDesc.ByteWidth / bufferDesc.StructureByteStride;
	assert(bufferDesc.ByteWidth % bufferDesc.StructureByteStride == 0);

	switch (type)
	{
	case fq::graphics::eUAVType::Default:
		uavDesc.Buffer.Flags = 0;
		break;
	case fq::graphics::eUAVType::ComsumeAppend:
		uavDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_APPEND;
		break;
	default:
		break;
	}

	uavDesc.Buffer.NumElements = buffer->GetMaxCount();

	HR(d3d11Device->GetDevice()->CreateUnorderedAccessView(d3dBuffer.Get(), &uavDesc, mView.GetAddressOf()));
}

void D3D11UnorderedAccessView::Bind(const std::shared_ptr<D3D11Device>& d3d11Device, const UINT startSlot, UINT initialCount)
{
	d3d11Device->GetDeviceContext()->CSSetUnorderedAccessViews(startSlot, 1, mView.GetAddressOf(), &initialCount);
}

void fq::graphics::D3D11UnorderedAccessView::UnBind(const std::shared_ptr<D3D11Device>& d3d11Device, UINT startSlot, UINT initialCount)
{
	ID3D11UnorderedAccessView* tempView = nullptr;

	d3d11Device->GetDeviceContext()->CSSetUnorderedAccessViews(startSlot, 1, &tempView, &initialCount);
}

void D3D11UnorderedAccessView::Clear(const std::shared_ptr<D3D11Device>& d3d11Device)
{
	UINT initValue = 0;
	d3d11Device->GetDeviceContext()->ClearUnorderedAccessViewUint(mView.Get(), &initValue);
}