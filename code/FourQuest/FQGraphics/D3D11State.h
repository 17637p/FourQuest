#pragma once

#include <wrl.h>
#include <unordered_map>
#include <memory>

#include "D3D11ResourceType.h"
#include "D3D11Device.h"
#include "ResourceBase.h"

using namespace Microsoft::WRL;

namespace fq_graphics
{
	/*=============================================================================
		SamplerState
	=============================================================================*/
	struct D3D11SamplerState : public ResourceBase
	{
	public:
		D3D11SamplerState(std::shared_ptr<D3D11ResourceManager> resourceManager, 
			const std::shared_ptr<D3D11Device>& d3d11Device,
			ED3D11SamplerState eStateType);

	private:
		ComPtr<ID3D11SamplerState> mState;
	};

	/*=============================================================================
		RasterizerState
	=============================================================================*/
	class D3D11RasterizerState : public ResourceBase
	{
	public:
		D3D11RasterizerState(std::shared_ptr<D3D11ResourceManager> resourceManager,
			const std::shared_ptr<D3D11Device>& d3d11Device,
			ED3D11SamplerState eStateType);

	private:
		ComPtr<ID3D11RasterizerState> mState;
	};

	/*=============================================================================
		DepthStencilState
	=============================================================================*/
	class D3D11DepthStencilState : public ResourceBase
	{
	public:
		D3D11DepthStencilState(std::shared_ptr<D3D11ResourceManager> resourceManager,
			const std::shared_ptr<D3D11Device>& d3d11Device,
			ED3D11DepthStencilState eStateType);

	private:
		ComPtr<ID3D11DepthStencilState> mState;
	};

	/*=============================================================================
		BlendState
	=============================================================================*/
	class D3D11BlendState : public ResourceBase
	{
	public:
		D3D11BlendState(std::shared_ptr<D3D11ResourceManager> resourceManager,
			const std::shared_ptr<D3D11Device>& d3d11Device,
			ED3D11BlendState eStateType);

	private:
		ComPtr<ID3D11BlendState> mState;
	};
}