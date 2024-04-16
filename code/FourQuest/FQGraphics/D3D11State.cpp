#include "D3D11State.h"

#include <d3d11.h>

#include "Define.h"

using namespace fq_graphics;

/*=============================================================================
		SamplerState
=============================================================================*/
D3D11SamplerState::D3D11SamplerState(const std::shared_ptr<D3D11ResourceManager>& resourceManager, const std::shared_ptr<D3D11Device>& d3d11Device, const ED3D11SamplerState eViewType)
	:ResourceBase(resourceManager, ResourceType::SamplerState),
	mState(nullptr)
{
	ID3D11Device* device = d3d11Device->GetDevice().Get();

	D3D11_SAMPLER_DESC clampSamplerDesc;
	ZeroMemory(&clampSamplerDesc, sizeof(clampSamplerDesc));
	clampSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	clampSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	clampSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	clampSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	clampSamplerDesc.MipLODBias = 0.f;
	clampSamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	clampSamplerDesc.MinLOD = 0.f;
	clampSamplerDesc.MaxLOD = 0.f;

	ID3D11SamplerState* tempSamplerState = mState.Get();
	HR(device->CreateSamplerState(&clampSamplerDesc, &mState));
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
			d3d11Device->GetDeviceContext()->VSSetSamplers(startSlot, 1, &mState);
		}
		case ED3D11ShaderType::Pixelshader:
		{
			d3d11Device->GetDeviceContext()->PSSetSamplers(startSlot, 1, &mState);
		}
		case ED3D11ShaderType::GeometryShader:
		{
			d3d11Device->GetDeviceContext()->GSSetSamplers(startSlot, 1, &mState);
		}
		default:
			break;
	}
}

/*=============================================================================
		RasterizerState
=============================================================================*/
D3D11RasterizerState::D3D11RasterizerState(const std::shared_ptr<D3D11ResourceManager>& resourceManager, const std::shared_ptr<D3D11Device>& d3d11Device, const ED3D11RasterizerState eViewType)
	:ResourceBase(resourceManager, ResourceType::RasterizerState),
	mState(nullptr)
{
	ID3D11Device* device = d3d11Device->GetDevice().Get();

	D3D11_RASTERIZER_DESC solidDesc;
	ZeroMemory(&solidDesc, sizeof(D3D11_RASTERIZER_DESC));
	solidDesc.FillMode = D3D11_FILL_SOLID;
	solidDesc.CullMode = D3D11_CULL_BACK;
	solidDesc.FrontCounterClockwise = false;
	solidDesc.DepthClipEnable = true;

	ID3D11RasterizerState* tempRasterizerState = mState.Get();
	HR(device->CreateRasterizerState(&solidDesc, &mState));
}

std::string D3D11RasterizerState::GenerateRID(const ED3D11RasterizerState eStateType)
{
	return typeid(D3D11RasterizerState).name() + std::to_string(static_cast<int>(eStateType));
}

void D3D11RasterizerState::Bind(const std::shared_ptr<D3D11Device>& d3d11Device)
{
	// Todo: ����� �۵��ϴ� �� Ȯ���� �� .Get���� .GetAddressOf����
	d3d11Device->GetDeviceContext()->RSSetState(mState.Get());
}

/*=============================================================================
		DepthStencilState
=============================================================================*/
D3D11DepthStencilState::D3D11DepthStencilState(const std::shared_ptr<D3D11ResourceManager>& resourceManager, const std::shared_ptr<D3D11Device>& d3d11Device, const ED3D11DepthStencilState eStateType)
	:ResourceBase(resourceManager, ResourceType::DepthStencilState),
	mState(nullptr)
{
	ID3D11Device* device = d3d11Device->GetDevice().Get();

	D3D11_DEPTH_STENCIL_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.DepthEnable = false;
	desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	desc.DepthFunc = D3D11_COMPARISON_ALWAYS;
	desc.StencilEnable = false;
	desc.FrontFace.StencilFailOp = desc.FrontFace.StencilDepthFailOp = desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	desc.BackFace = desc.FrontFace;

	ID3D11DepthStencilState* tempRasterizerState = mState.Get();
	HR(device->CreateDepthStencilState(&desc, &tempRasterizerState));
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

D3D11BlendState::D3D11BlendState(const std::shared_ptr<D3D11ResourceManager>& resourceManager, const std::shared_ptr<D3D11Device>& d3d11Device, const ED3D11BlendState eStateType)
	:ResourceBase(resourceManager, ResourceType::BlendState),
	mState(nullptr)
{
	ID3D11Device* device = d3d11Device->GetDevice().Get();

	//D3D11_BLEND_DESC desc;
		//ZeroMemory(&desc, sizeof(desc));
		//desc.AlphaToCoverageEnable = false;
		//desc.RenderTarget[0].BlendEnable = true;
		//desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		//desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		//desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		//desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		//desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
		//desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		//desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		//bd->pd3dDevice->CreateBlendState(&desc, &bd->pBlendState);
}

std::string D3D11BlendState::GenerateRID(const ED3D11BlendState eStateType)
{
	return typeid(D3D11BlendState).name() + std::to_string(static_cast<int>(eStateType));
}

void D3D11BlendState::Bind(const std::shared_ptr<D3D11Device>& d3d11Device)
{
	// Ư���� blend factor ���� ������� �ʴ� �̻� �� ��° ���� ���� �׻� null�� �Ѱܵ� �ȴ�.
	d3d11Device->GetDeviceContext()->OMSetBlendState(mState.Get(), nullptr, 0xFFFFFFFF);
}
