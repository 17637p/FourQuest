#include "D3D11State.h"

#include <d3d11.h>

#include "Define.h"

using namespace fq::graphics;

/*=============================================================================
		SamplerState
=============================================================================*/
fq::graphics::D3D11SamplerState::D3D11SamplerState(const std::shared_ptr<D3D11Device>& d3d11Device, const ED3D11SamplerState eStateType)
	:ResourceBase(ResourceType::SamplerState),
	mState(nullptr)
{
	ID3D11Device* device = d3d11Device->GetDevice().Get();

	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(samplerDesc));

	switch (eStateType)
	{
	case ED3D11SamplerState::Default:
	{
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.MinLOD = -FLT_MAX;
		samplerDesc.MaxLOD = FLT_MAX;
		samplerDesc.MipLODBias = 0.f;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;

		break;
	}
	case ED3D11SamplerState::AnisotropicWrap:
	{
		samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.MinLOD = -FLT_MAX;
		samplerDesc.MaxLOD = FLT_MAX;
		samplerDesc.MipLODBias = 0.f;
		samplerDesc.MaxAnisotropy = D3D11_REQ_MAXANISOTROPY;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;

		break;
	}
	case ED3D11SamplerState::PointClamp:
	{
		samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.MinLOD = -FLT_MAX;
		samplerDesc.MaxLOD = FLT_MAX;
		samplerDesc.MipLODBias = 0.f;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;

		break;
	}
	case ED3D11SamplerState::Shadow:
	{
		samplerDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
		samplerDesc.MaxAnisotropy = 1u;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_LESS;
		samplerDesc.MinLOD = 0;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

		break;
	}
	default:
		break;
	}

	HR(device->CreateSamplerState(&samplerDesc, mState.GetAddressOf()));
}

std::string D3D11SamplerState::GenerateRID(const ED3D11SamplerState eStateType)
{
	return typeid(D3D11SamplerState).name() + std::to_string(static_cast<int>(eStateType));
}

void D3D11SamplerState::Bind(const std::shared_ptr<D3D11Device>& d3d11Device, const UINT startSlot, const ED3D11ShaderType eShaderType)
{
	switch (eShaderType)
	{
	case ED3D11ShaderType::VertexShader:
	{
		d3d11Device->GetDeviceContext()->VSSetSamplers(startSlot, 1, mState.GetAddressOf());
		break;
	}
	case ED3D11ShaderType::Pixelshader:
	{
		d3d11Device->GetDeviceContext()->PSSetSamplers(startSlot, 1, mState.GetAddressOf());
		break;
	}
	case ED3D11ShaderType::GeometryShader:
	{
		d3d11Device->GetDeviceContext()->GSSetSamplers(startSlot, 1, mState.GetAddressOf());
		break;
	}
	default:
		break;
	}
}

/*=============================================================================
		RasterizerState
=============================================================================*/
fq::graphics::D3D11RasterizerState::D3D11RasterizerState(const std::shared_ptr<D3D11Device>& d3d11Device, const ED3D11RasterizerState eStateType)
	:ResourceBase(ResourceType::RasterizerState),
	mState(nullptr)
{
	ID3D11Device* device = d3d11Device->GetDevice().Get();

	D3D11_RASTERIZER_DESC rasterizerDesc;
	ZeroMemory(&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));

	switch (eStateType)
	{
	case ED3D11RasterizerState::Default:
	{
		rasterizerDesc.FillMode = D3D11_FILL_SOLID;
		rasterizerDesc.CullMode = D3D11_CULL_BACK;
		rasterizerDesc.FrontCounterClockwise = false;
		rasterizerDesc.DepthClipEnable = true;

		break;
	}
	case ED3D11RasterizerState::Shadow:
	{
		// 물방울 책 참고
		// 광원 기준으로 기울기에 따라 더 큰 편향치를 적용할 수 있다고 함
		// bias = (float)depthBias * r + slopeScaledDepthBias * maxDepthSlope
		// 24비트 깊이 버퍼의 경우 1 / 2^24
		rasterizerDesc = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT{});
		rasterizerDesc.DepthBias = 100000; // 고정된 편향치
		rasterizerDesc.SlopeScaledDepthBias = 1.f; // 기울기 편형치 비례계수
		rasterizerDesc.DepthBiasClamp = 0.0f; // 허용되는 최대 깊이 편향치
		break;
	}
	case ED3D11RasterizerState::CullFront:
	{
		rasterizerDesc.FillMode = D3D11_FILL_SOLID;
		rasterizerDesc.CullMode = D3D11_CULL_FRONT;

		break;
	}
	default:
		break;
	}

	HR(device->CreateRasterizerState(&rasterizerDesc, mState.GetAddressOf()));
}

void D3D11RasterizerState::UnBind(const std::shared_ptr<D3D11Device>& d3d11Device)
{
	d3d11Device->GetDeviceContext()->RSSetState(nullptr);
}

std::string D3D11RasterizerState::GenerateRID(const ED3D11RasterizerState eStateType)
{
	return typeid(D3D11RasterizerState).name() + std::to_string(static_cast<int>(eStateType));
}

void D3D11RasterizerState::Bind(const std::shared_ptr<D3D11Device>& d3d11Device)
{
	// Todo: 제대로 작동하는 지 확인할 것 .Get인지 .GetAddressOf인지
	d3d11Device->GetDeviceContext()->RSSetState(mState.Get());
}

/*=============================================================================
		DepthStencilState
=============================================================================*/
fq::graphics::D3D11DepthStencilState::D3D11DepthStencilState(const std::shared_ptr<D3D11Device>& d3d11Device, const ED3D11DepthStencilState eStateType)
	:ResourceBase(ResourceType::DepthStencilState),
	mState(nullptr)
{
	ID3D11Device* device = d3d11Device->GetDevice().Get();

	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	switch (eStateType)
	{
	case ED3D11DepthStencilState::Default:
	{
		depthStencilDesc.DepthEnable = true;
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
		depthStencilDesc.StencilEnable = false;
		depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		depthStencilDesc.FrontFace.StencilFailOp = depthStencilDesc.FrontFace.StencilDepthFailOp = depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace = depthStencilDesc.FrontFace;

		break;
	}
	case ED3D11DepthStencilState::DisableDepthWirte:
	{
		depthStencilDesc = CD3D11_DEPTH_STENCIL_DESC{ D3D11_DEFAULT };
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;

		break;
	}
	case ED3D11DepthStencilState::LessEqual:
	{
		depthStencilDesc.DepthEnable = true;
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		depthStencilDesc.StencilEnable = false;
		depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		depthStencilDesc.FrontFace.StencilFailOp = depthStencilDesc.FrontFace.StencilDepthFailOp = depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace = depthStencilDesc.FrontFace;

		break;
	}
	default:
		break;
	}

	HR(device->CreateDepthStencilState(&depthStencilDesc, mState.GetAddressOf()));
}

void D3D11DepthStencilState::UnBind(const std::shared_ptr<D3D11Device>& d3d11Device)
{
	d3d11Device->GetDeviceContext()->OMSetDepthStencilState(nullptr, 0);
}

std::string D3D11DepthStencilState::GenerateRID(const ED3D11DepthStencilState eStateType)
{
	return typeid(D3D11DepthStencilState).name() + std::to_string(static_cast<int>(eStateType));
}

void D3D11DepthStencilState::Bind(const std::shared_ptr<D3D11Device>& d3d11Device)
{
	d3d11Device->GetDeviceContext()->OMSetDepthStencilState(mState.Get(), 0);
}

/*=============================================================================
		BlendState
=============================================================================*/

fq::graphics::D3D11BlendState::D3D11BlendState(const std::shared_ptr<D3D11Device>& d3d11Device, const ED3D11BlendState eStateType)
	:ResourceBase(ResourceType::BlendState),
	mState(nullptr)
{
	ID3D11Device* device = d3d11Device->GetDevice().Get();

	D3D11_BLEND_DESC blendDesc;
	ZeroMemory(&blendDesc, sizeof(blendDesc));

	switch (eStateType)
	{
	case ED3D11BlendState::Default:
	{
		blendDesc.AlphaToCoverageEnable = false;
		blendDesc.RenderTarget[0].BlendEnable = false;
		blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
		blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		break;
	}
	case ED3D11BlendState::OITRender:
	{
		blendDesc = CD3D11_BLEND_DESC{ CD3D11_DEFAULT{} };
		blendDesc.AlphaToCoverageEnable = FALSE;
		blendDesc.IndependentBlendEnable = true;

		auto& acuumRT = blendDesc.RenderTarget[0];
		acuumRT.BlendEnable = true;
		acuumRT.SrcBlend = D3D11_BLEND_ONE;
		acuumRT.DestBlend = D3D11_BLEND_ONE;
		acuumRT.BlendOp = D3D11_BLEND_OP_ADD;
		acuumRT.SrcBlendAlpha = D3D11_BLEND_ONE;
		acuumRT.DestBlendAlpha = D3D11_BLEND_ONE;
		acuumRT.BlendOpAlpha = D3D11_BLEND_OP_ADD;
		acuumRT.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		auto& revealRT = blendDesc.RenderTarget[1];
		revealRT.BlendEnable = true;
		revealRT.SrcBlend = D3D11_BLEND_ZERO;
		revealRT.DestBlend = D3D11_BLEND_INV_SRC_COLOR;
		revealRT.BlendOp = D3D11_BLEND_OP_ADD;
		// alpha 사용 안함
		revealRT.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_RED;

		break;
	}
	case ED3D11BlendState::OITComposite:
	{
		blendDesc = CD3D11_BLEND_DESC{ CD3D11_DEFAULT{} };
		blendDesc.AlphaToCoverageEnable = FALSE;
		blendDesc.IndependentBlendEnable = FALSE;
		blendDesc.RenderTarget[0].BlendEnable = TRUE;
		blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
		blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		break;
	}
	default:
		break;
	}

	HR(device->CreateBlendState(&blendDesc, mState.GetAddressOf()));
}

std::string D3D11BlendState::GenerateRID(const ED3D11BlendState eStateType)
{
	return typeid(D3D11BlendState).name() + std::to_string(static_cast<int>(eStateType));
}

void D3D11BlendState::Bind(const std::shared_ptr<D3D11Device>& d3d11Device)
{
	// 특별히 blend factor 값을 사용하지 않는 이상 두 번째 인자 값은 항상 null로 넘겨도 된다.
	d3d11Device->GetDeviceContext()->OMSetBlendState(mState.Get(), nullptr, 0xFFFFFFFF);
}

PipelineState::PipelineState(std::shared_ptr<D3D11RasterizerState> rasterizerOrNull, std::shared_ptr<D3D11DepthStencilState> depthStencilOrNull, std::shared_ptr<D3D11BlendState> blendStateOrNull)
	: mRasterizerOrNull(rasterizerOrNull)
	, mDepthStencilOrNull(depthStencilOrNull)
	, mBlendStateOrNull(blendStateOrNull)
{
}

void PipelineState::Bind(const std::shared_ptr<D3D11Device>& d3d11Device)
{
	if (mRasterizerOrNull != nullptr)
	{
		mRasterizerOrNull->Bind(d3d11Device);
	}
	else
	{
		d3d11Device->GetDeviceContext()->RSSetState(NULL);
	}

	if (mDepthStencilOrNull != nullptr)
	{
		mDepthStencilOrNull->Bind(d3d11Device);
	}
	else
	{
		d3d11Device->GetDeviceContext()->OMSetDepthStencilState(NULL, 0);
	}

	if (mBlendStateOrNull != nullptr)
	{
		mBlendStateOrNull->Bind(d3d11Device);
	}
	else
	{
		d3d11Device->GetDeviceContext()->OMSetBlendState(NULL, NULL, 0xFFFFFFFF);
	}
}