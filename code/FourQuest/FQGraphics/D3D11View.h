#pragma once

#include <wrl.h>
#include <unordered_map>
#include <string>
#include <memory>

#include "D3D11ResourceType.h"
#include "ResourceBase.h"

struct ID3D11RenderTargetView;
struct ID3D11ShaderResourceView;
struct ID3D11DepthStencilView;

using namespace Microsoft::WRL;

namespace fq_graphics
{
	class D3D11Device;

	/*=============================================================================
		RTV View 
	=============================================================================*/
	class D3D11RenderTargetView : public ResourceBase
	{
	public:
		D3D11RenderTargetView(const std::shared_ptr<D3D11ResourceManager>& resourceManager, 
			const std::shared_ptr<D3D11Device>& d3d11Device, 
			const ED3D11RenderTargetViewType eViewType, 
			const unsigned short width, const unsigned short height);

		static std::string GenerateRID(const ED3D11RenderTargetViewType eViewType);

		void Bind(const std::shared_ptr<D3D11Device>& d3d11Device);

	private:
		ComPtr<ID3D11RenderTargetView> mRTV;

		friend class D3D11ShaderResourceView;
	};

	/*=============================================================================
		SRV View
	=============================================================================*/
	class D3D11ShaderResourceView : public ResourceBase
	{
	public:
		D3D11ShaderResourceView(const std::shared_ptr<D3D11ResourceManager>& resourceManager, 
			const std::shared_ptr<D3D11Device>& d3d11Device, const std::shared_ptr<D3D11RenderTargetView>& rendertargetView);

		static std::string GenerateRID(const ED3D11ShaderResourceViewType eViewType);

		void Bind(const std::shared_ptr<D3D11Device>& d3d11Device, const UINT startSlot, const ED3D11ShaderType eShaderType);

	private:
		ComPtr<ID3D11ShaderResourceView> mSRV;
	};

	/*=============================================================================
		DSV View
	=============================================================================*/
	class D3D11DepthStencilView : public ResourceBase
	{
	public:
		D3D11DepthStencilView(const std::shared_ptr<D3D11ResourceManager>& resourceManager, 
			const std::shared_ptr<D3D11Device>& d3d11Device, 
			const ED3D11DepthStencilViewType eViewType, 
			const unsigned short width, const unsigned short height);

		static std::string GenerateRID(const ED3D11DepthStencilViewType eViewType);

	private:
		ComPtr<ID3D11DepthStencilView> mDSV;
	};
}

