#pragma once

#include <memory>
#include <filesystem>

#include "../FQCommon/FQCommonLoader.h"
#include "../FQCommon/FQCommonGraphics.h"

namespace fq::graphics
{
	class D3D11Device;
	class D3D11Texture;

	// 1. 쉐이더 종류가 많지 않으면 그에 대응되는 재질이랑 1대1로 늘린다.
	// 2. 쉐이더 리플렉션 처리를 해서 재질도 범용적으로 다룬다.?

	class Material
	{
	public:
		Material(const std::shared_ptr<D3D11Device>& device, const fq::common::Material& materialData, std::filesystem::path basePath = "");
		~Material() = default;

		void Bind(const std::shared_ptr<D3D11Device>& d3d11Device);

		inline bool GetHasBaseColor() const;
		inline bool GetHasMetalness() const;
		inline bool GetHasRoughness() const;
		inline bool GetHasNormal() const;
		inline bool GetHasEmissive() const;
		inline bool GetHasOpacity() const;

	private:
		fq::common::Material mMaterialData;

		std::shared_ptr<D3D11Texture> mBaseColor;
		std::shared_ptr<D3D11Texture> mMetalness;
		std::shared_ptr<D3D11Texture> mRoughness;
		std::shared_ptr<D3D11Texture> mNormal;
		std::shared_ptr<D3D11Texture> mEmissive;
		std::shared_ptr<D3D11Texture> mOpacity;
	};

	inline bool Material::GetHasBaseColor() const { return mBaseColor != nullptr; }
	inline bool Material::GetHasMetalness() const { return mMetalness != nullptr; }
	inline bool Material::GetHasRoughness() const { return mRoughness != nullptr; }
	inline bool Material::GetHasNormal() const { return mNormal != nullptr; }
	inline bool Material::GetHasEmissive() const { return mEmissive != nullptr; }
	inline bool Material::GetHasOpacity() const { return mOpacity != nullptr; }

	class TerrainMaterial
	{
	public:
		TerrainMaterial(const std::shared_ptr<D3D11Device>& device, 
			const fq::common::TerrainMaterial& materialData, 
			std::filesystem::path basePath = "");
		~TerrainMaterial() = default;

		void Bind(const std::shared_ptr<D3D11Device>& d3d11Device);
		void SetMaterial(const std::shared_ptr<D3D11Device>& device, const fq::common::TerrainMaterial& materialData);
		inline unsigned short GetNumOfTexture() const;

	private:
		fq::common::TerrainMaterial mMaterialData;

		std::filesystem::path mBasePath;

		std::vector<std::shared_ptr<D3D11Texture>> mBaseColors;
		std::vector<std::shared_ptr<D3D11Texture>> mMetalnesses;
		std::vector<std::shared_ptr<D3D11Texture>> mRoughnesses;
		std::vector<std::shared_ptr<D3D11Texture>> mNormals;
		std::shared_ptr<D3D11Texture> mAlpha;
	};

	inline unsigned short TerrainMaterial::GetNumOfTexture() const { return mMaterialData.NumOfTexture; }
}
