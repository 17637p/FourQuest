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
		D3D11Texture(const std::shared_ptr<D3D11Device>& d3d11Device, UINT width, UINT height, DXGI_FORMAT format, UINT levels = 0);
		// Cube Map
		D3D11Texture(const std::shared_ptr<D3D11Device>& d3d11Device,
			const std::vector<std::wstring>& texturePaths);

		static std::string GenerateRID(const std::wstring& texturePath);

		void CreateUAV(const std::shared_ptr<D3D11Device>& d3d11Device, UINT mipSlice);

		static void Bind(const std::shared_ptr<D3D11Device>& d3d11Device, std::vector<std::shared_ptr<D3D11Texture>> textures, const UINT startSlot, const ED3D11ShaderType eShaderType);
		void Bind(const std::shared_ptr<D3D11Device>& d3d11Device, const UINT startSlot, const ED3D11ShaderType eShaderType);

		UINT GetWidth() const { return mWidth; }
		UINT GetHeight() const { return mHeight; }
		UINT GetLevel() const { return mLevels; }
		Microsoft::WRL::ComPtr<ID3D11Texture2D> GetTexture() const { return mTexture; }
		Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> GetUAV() const { return mUAV; }
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetSRV() const { return mSRV; }

		void Save(const std::shared_ptr<D3D11Device>& d3d11Device, const std::wstring& savePath);
		void GetSHCoefficientRGB(const std::shared_ptr<D3D11Device>& d3d11Device, float* R, float* G, float* B);

	private:
		UINT mWidth;
		UINT mHeight;
		UINT mLevels; // ¹Ó¸Ê ·¹º§
		Microsoft::WRL::ComPtr<ID3D11Texture2D> mTexture;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mSRV;
		Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> mUAV;
		TextureType type;
	};

	class D3D11CubeTexture : public ResourceBase
	{
	public:
		D3D11CubeTexture(const std::shared_ptr<D3D11Device>& d3d11Device, const std::wstring& texturePath);
		D3D11CubeTexture(const std::shared_ptr<D3D11Device>& d3d11Device, UINT width, UINT height, DXGI_FORMAT format, UINT level = 0);

		static std::string GenerateRID(const std::wstring& texturePath);

		void CreateUAV(const std::shared_ptr<D3D11Device>& d3d11Device, UINT mipSlice);

		UINT GetWidth() const { return mWidth; }
		UINT GetHeight() const { return mHeight; }
		UINT GetLevel() const { return mLevels; }
		Microsoft::WRL::ComPtr<ID3D11Texture2D> GetTexture() const { return mTexture; }
		Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> GetUAV() const { return mUAV; }
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetSRV() const { return mSRV; }

	private:
		UINT mWidth;
		UINT mHeight;
		UINT mLevels; // ¹Ó¸Ê ·¹º§
		Microsoft::WRL::ComPtr<ID3D11Texture2D> mTexture;
		Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> mUAV;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mSRV;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mTextureSRV;
	};
}

