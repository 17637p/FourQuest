#pragma once

#include <wrl.h>
#include <unordered_map>
#include <memory>

#include "D3D11Device.h"
#include "ResourceBase.h"

using namespace Microsoft::WRL;

namespace fq_graphics
{
	/*class D3D11State
	{
	public:
		D3D11State();
		~D3D11State();

		void Initilize(const std::shared_ptr<D3D11Device>& d3d11Device);

	private:
		std::unordered_map<ED3D11SamplerState, std::shared_ptr<D3D11SamplerState> >	mSamplerStates;
		std::unordered_map<ED3D11RasterizerState, std::shared_ptr<D3D11RasterizerState> > mRasterizerStates;
		std::unordered_map<ED3D11DepthStencilState, std::shared_ptr<D3D11DepthStencilState> > mDepthStencilStates;
		std::unordered_map<ED3D11BlendState, std::shared_ptr<D3D11BlendState> >	mBlendStates;
	};*/

	/*=============================================================================
		SamplerState
	=============================================================================*/
	enum class ED3D11SamplerState
	{
		Default,
	};

	struct D3D11SamplerState
	{
	public:
		ComPtr<ID3D11SamplerState> Create(const std::shared_ptr<D3D11Device>& d3d11Device);

	private:
		ComPtr<ID3D11SamplerState> mState;
	};

	/*=============================================================================
		RasterizerState
	=============================================================================*/
	enum class ED3D11RasterizerState
	{
		Default,
	};

	class D3D11RasterizerState : public ResourceBase
	{
	public:
		ComPtr<ID3D11RasterizerState> Create(const std::shared_ptr<D3D11Device>& d3d11Device);

	private:
		ComPtr<ID3D11RasterizerState> mState;
	};

	/*=============================================================================
		DepthStencilState
	=============================================================================*/
	enum class ED3D11DepthStencilState
	{
		Default,
	};

	class D3D11DepthStencilState : public ResourceBase
	{
	public:
		ComPtr<ID3D11DepthStencilState> Create(const std::shared_ptr<D3D11Device>& d3d11Device);

	private:
		ComPtr<ID3D11DepthStencilState> mState;
	};

	/*=============================================================================
		BlendState
	=============================================================================*/
	enum class ED3D11BlendState
	{
		Default,
	};

	class D3D11BlendState : public ResourceBase
	{
	public:
		ComPtr<ID3D11BlendState> Create(const std::shared_ptr<D3D11Device>& d3d11Device);

	private:
		ComPtr<ID3D11BlendState> mState;
	};
}