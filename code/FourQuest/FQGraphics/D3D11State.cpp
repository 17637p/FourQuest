#include "D3D11State.h"

#include <d3d11.h>

#include "Define.h"

using namespace fq_graphics;

/*
D3D11State::D3D11State()
	:mSamplerStates(),
	mRasterizerStates(),
	mDepthStencilStates(),
	mBlendStates()
{

}

D3D11State::~D3D11State()
{
	mSamplerStates.clear();
	mRasterizerStates.clear();
	mDepthStencilStates.clear();
	mBlendStates.clear();
}

void D3D11State::Initilize(const std::shared_ptr<D3D11Device>& d3d11Device)
{
	ID3D11Device* device = d3d11Device->GetDevice().Get();

	// SamplerState
	{
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

		std::shared_ptr<D3D11SamplerState> defaultSamplerState = std::make_shared<D3D11SamplerState>();
		ID3D11SamplerState* tempSamplerState = defaultSamplerState->mState.Get();
		HR(device->CreateSamplerState(&clampSamplerDesc, &tempSamplerState));
		mSamplerStates[ED3D11SamplerState::Default] = defaultSamplerState;
	}

	// RasterizerState
	{
		D3D11_RASTERIZER_DESC solidDesc;
		ZeroMemory(&solidDesc, sizeof(D3D11_RASTERIZER_DESC));
		solidDesc.FillMode = D3D11_FILL_SOLID;
		solidDesc.CullMode = D3D11_CULL_BACK;
		solidDesc.FrontCounterClockwise = false;
		solidDesc.DepthClipEnable = true;

		std::shared_ptr<D3D11RasterizerState> defaultRasterizerState = std::make_shared<D3D11RasterizerState>();
		ID3D11RasterizerState* tempRasterizerState = defaultRasterizerState->mState.Get();
		HR(device->CreateRasterizerState(&solidDesc, &tempRasterizerState));
		mRasterizerStates[ED3D11RasterizerState::Default] = defaultRasterizerState;
	}

	// DepthStencilState
	{
		D3D11_DEPTH_STENCIL_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.DepthEnable = false;
		desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		desc.DepthFunc = D3D11_COMPARISON_ALWAYS;
		desc.StencilEnable = false;
		desc.FrontFace.StencilFailOp = desc.FrontFace.StencilDepthFailOp = desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		desc.BackFace = desc.FrontFace;

		std::shared_ptr<D3D11DepthStencilState> defaultDepthStencilState = std::make_shared<D3D11DepthStencilState>();
		ID3D11DepthStencilState* tempRasterizerState = defaultDepthStencilState->mState.Get();
		HR(device->CreateDepthStencilState(&desc, &tempRasterizerState));
		mDepthStencilStates[ED3D11DepthStencilState::Default] = defaultDepthStencilState;
	}

	// BlendState
	{
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
}*/

ComPtr<ID3D11SamplerState> D3D11SamplerState::Create(const std::shared_ptr<D3D11Device>& d3d11Device)
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

	return mState;
}

ComPtr<ID3D11RasterizerState> D3D11RasterizerState::Create(const std::shared_ptr<D3D11Device>& d3d11Device)
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

	return mState;
}

ComPtr<ID3D11DepthStencilState> D3D11DepthStencilState::Create(const std::shared_ptr<D3D11Device>& d3d11Device)
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

	return mState;
}

ComPtr<ID3D11BlendState> D3D11BlendState::Create(const std::shared_ptr<D3D11Device>& d3d11Device)
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

	return mState;
}
