#pragma once

#include <wrl.h>
#include <unordered_map>
#include <string>
#include <memory>
#include <directxtk/SimpleMath.h>
#include <d3d11.h>

#include "D3D11ResourceType.h"
#include "ResourceBase.h"

struct ID3D11RenderTargetView;
struct ID3D11ShaderResourceView;
struct ID3D11DepthStencilView;

using namespace Microsoft::WRL;

namespace fq::graphics
{
	class D3D11Device;
	class D3D11DepthStencilView;

	/*=============================================================================
		RTV View
	=============================================================================*/
	class D3D11RenderTargetView : public ResourceBase
	{
	public:
		D3D11RenderTargetView
		(const std::shared_ptr<D3D11Device>& d3d11Device,
			const ED3D11RenderTargetViewType eViewType,
			const unsigned short width, const unsigned short height);

		static std::string GenerateRID(const ED3D11RenderTargetViewType eViewType);

		static void Bind(const std::shared_ptr<D3D11Device>& d3d11Device, std::vector<std::shared_ptr<D3D11RenderTargetView>> renderTargetViews, const std::shared_ptr<D3D11DepthStencilView>& depthStencilView);
		void Bind(const std::shared_ptr<D3D11Device>& d3d11Device, const std::shared_ptr<D3D11DepthStencilView>& depthStencilView);
		void UnBind(const std::shared_ptr<D3D11Device>& d3d11Device);

		ComPtr<ID3D11RenderTargetView> GetRTV();

		void Clear(const std::shared_ptr<D3D11Device>& d3d11Device, const DirectX::SimpleMath::Color& clearColor = { 0.f, 0.f, 0.f, 1.f });
		void OnResize(const std::shared_ptr<D3D11Device>& d3d11Device,
			const ED3D11RenderTargetViewType eViewType,
			const unsigned short width, const unsigned short height);
		void Release();

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
		D3D11ShaderResourceView(const std::shared_ptr<D3D11Device>& d3d11Device, const std::shared_ptr<D3D11RenderTargetView>& rendertargetView);
		D3D11ShaderResourceView(const std::shared_ptr<D3D11Device>& d3d11Device, const std::shared_ptr<D3D11DepthStencilView>& depthStencilView);
		D3D11ShaderResourceView(const std::shared_ptr<D3D11Device>& d3d11Device, const std::shared_ptr<D3D11DepthStencilView>& depthStencilView, DXGI_FORMAT format);
		D3D11ShaderResourceView(const std::shared_ptr<D3D11Device>& d3d11Device, const std::shared_ptr<class D3D11UnorderedAccessView>& unorderedAccessView, bool bIsTexture = false);

		static std::string GenerateRID(const ED3D11ShaderResourceViewType eViewType);

		void Init(const std::shared_ptr<D3D11Device>& d3d11Device, const std::shared_ptr<D3D11RenderTargetView>& rendertargetView);
		void Bind(const std::shared_ptr<D3D11Device>& d3d11Device, const UINT startSlot, const ED3D11ShaderType eShaderType);
		void UnBind(const std::shared_ptr<D3D11Device>& d3d11Device, const UINT startSlot, const ED3D11ShaderType eShaderType);

		inline ComPtr<ID3D11ShaderResourceView> GetSRV() const;

	private:
		ComPtr<ID3D11ShaderResourceView> mSRV;
	};

	inline ComPtr<ID3D11ShaderResourceView> D3D11ShaderResourceView::GetSRV() const
	{
		return mSRV;
	}

	/*=============================================================================
		DSV View
	=============================================================================*/
	class D3D11DepthStencilView : public ResourceBase
	{
		friend class D3D11RenderTargetView;
		friend class D3D11ShaderResourceView;

	public:
		D3D11DepthStencilView(const std::shared_ptr<D3D11Device>& d3d11Device,
			const ED3D11DepthStencilViewType eViewType,
			const unsigned short width, const unsigned short height);

		static std::string GenerateRID(const ED3D11DepthStencilViewType eViewType);

		void Clear(const std::shared_ptr<D3D11Device>& d3d11Device);
		void OnResize(const std::shared_ptr<D3D11Device>& d3d11Device,
			const ED3D11DepthStencilViewType eViewType,
			const unsigned short width, const unsigned short height);
		void Release();

		inline ComPtr<ID3D11DepthStencilView> GetDSV() const;

	private:
		ComPtr<ID3D11DepthStencilView> mDSV;
	};

	inline ComPtr<ID3D11DepthStencilView> D3D11DepthStencilView::GetDSV() const
	{
		return mDSV;
	}

	/*=============================================================================
		SRV View
	=============================================================================*/

	class D3D11StructuredBuffer;

	enum class eUAVType
	{
		Default,
		ComsumeAppend
	};

	class D3D11UnorderedAccessView
	{
		friend class D3D11ShaderResourceView;

	public:
		D3D11UnorderedAccessView(const std::shared_ptr<D3D11Device>& d3d11Device, const unsigned short width, const unsigned short height);
		D3D11UnorderedAccessView(const std::shared_ptr<D3D11Device>& d3d11Device, std::shared_ptr<D3D11StructuredBuffer> buffer, eUAVType type);
		~D3D11UnorderedAccessView() = default;

		void Bind(const std::shared_ptr<D3D11Device>& d3d11Device, UINT startSlot, UINT initialCount = (UINT)-1);
		void UnBind(const std::shared_ptr<D3D11Device>& d3d11Device, UINT startSlot, UINT initialCount = (UINT)-1);
		void Clear(const std::shared_ptr<D3D11Device>& d3d11Device);

		Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> GetView() const { return mView; }

	private:
		Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> mView;
	};
}

