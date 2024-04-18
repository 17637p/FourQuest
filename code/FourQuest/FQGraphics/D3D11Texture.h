#pragma once

#include "ResourceBase.h"
#include "D3D11ResourceType.h"

#include <memory>
#include <wrl.h>
#include <string>
#include <d3d11.h>

namespace fq::graphics
{
	class D3D11Device;

	class D3D11Texture : public ResourceBase
	{
	public:
		D3D11Texture(const std::shared_ptr<D3D11ResourceManager>& resourceManager,
			const std::shared_ptr<D3D11Device>& d3d11Device,
			const std::wstring& texturePath);

		static std::string GenerateRID(const std::wstring& texturePath);

		void Bind(const std::shared_ptr<D3D11Device>& d3d11Device, const UINT startSlot, const ED3D11ShaderType eShaderType);

	private:
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mTextureSRV;
	};
}

