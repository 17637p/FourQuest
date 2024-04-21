#pragma once

#include <wrl.h>
#include <unordered_map>
#include <memory>
#include <string>

#include "D3D11ResourceType.h"
#include "D3D11Device.h"
#include "ResourceBase.h"

using namespace Microsoft::WRL;

namespace fq::graphics
{
	// to do : 구조체인 이유 묻기
	/*=============================================================================
		SamplerState
	=============================================================================*/
	struct D3D11SamplerState : public ResourceBase
	{
	public:
		D3D11SamplerState(const std::shared_ptr<D3D11Device>& d3d11Device,
			const ED3D11SamplerState eStateType);

		static std::string GenerateRID(const ED3D11SamplerState eStateType);

		void Bind(const std::shared_ptr<D3D11Device>& d3d11Device, const UINT startSlot, const ED3D11ShaderType eShaderType);

	private:
		ComPtr<ID3D11SamplerState> mState;
	};

	/*=============================================================================
		RasterizerState
	=============================================================================*/
	class D3D11RasterizerState : public ResourceBase
	{
	public:
		D3D11RasterizerState(const std::shared_ptr<D3D11Device>& d3d11Device,
			const ED3D11RasterizerState eStateType);

		static std::string GenerateRID(const ED3D11RasterizerState eStateType);

		void Bind(const std::shared_ptr<D3D11Device>& d3d11Device);

	private:
		ComPtr<ID3D11RasterizerState> mState;
	};

	/*=============================================================================
		DepthStencilState
	=============================================================================*/
	class D3D11DepthStencilState : public ResourceBase
	{
	public:
		D3D11DepthStencilState(const std::shared_ptr<D3D11Device>& d3d11Device,
			const ED3D11DepthStencilState eStateType);

		static std::string GenerateRID(const ED3D11DepthStencilState eStateType);

		void Bind(const std::shared_ptr<D3D11Device>& d3d11Device);

	private:
		ComPtr<ID3D11DepthStencilState> mState;
	};

	/*=============================================================================
		BlendState
	=============================================================================*/
	class D3D11BlendState : public ResourceBase
	{
	public:
		D3D11BlendState(const std::shared_ptr<D3D11Device>& d3d11Device,
			const ED3D11BlendState eStateType);

		static std::string GenerateRID(const ED3D11BlendState eStateType);

		void Bind(const std::shared_ptr<D3D11Device>& d3d11Device);

	private:
		ComPtr<ID3D11BlendState> mState;
	};
}