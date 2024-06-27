#pragma once

#include "ResourceBase.h"
#include "D3D11ResourceType.h"

#include <memory>
#include <wrl.h>
#include <string>
#include <d3d11.h>
#include <vector>

namespace fq::graphics
{
	class D3D11Device;

	class D3D11Texture : public ResourceBase
	{
		enum class TextureType
		{
			Default,
			CubeMap
		};

	public:
		D3D11Texture(const std::shared_ptr<D3D11Device>& d3d11Device,
			const std::wstring& texturePath);
		D3D11Texture(const std::shared_ptr<D3D11Device>& d3d11Device,
			const std::vector<float>& rawArray,
			const UINT width, const UINT height);
		// Cube Map
		D3D11Texture(const std::shared_ptr<D3D11Device>& d3d11Device,
			const std::vector<std::wstring>& texturePaths);

		static std::string GenerateRID(const std::wstring& texturePath);

		static void Bind(const std::shared_ptr<D3D11Device>& d3d11Device, std::vector<std::shared_ptr<D3D11Texture>> textures, const UINT startSlot, const ED3D11ShaderType eShaderType);
		void Bind(const std::shared_ptr<D3D11Device>& d3d11Device, const UINT startSlot, const ED3D11ShaderType eShaderType);

		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetSRV() const { return mTextureSRV; }

		void Save(const std::shared_ptr<D3D11Device>& d3d11Device, const std::wstring& savePath);

	private:
		TextureType type;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mTextureSRV;
	};
}

