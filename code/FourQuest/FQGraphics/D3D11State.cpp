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
		default:
			break;
	}

	HR(device->CreateRasterizerState(&rasterizerDesc, mState.GetAddressOf()));
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
		default:
			break;
	}

	HR(device->CreateDepthStencilState(&depthStencilDesc, mState.GetAddressOf()));
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
