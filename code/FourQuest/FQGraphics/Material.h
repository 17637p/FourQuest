#pragma once

#include <memory>
#include <filesystem>

#include "../FQCommon/FQCommonLoader.h"
#include "../FQCommon/FQCommonGraphics.h"
#include "../FQCommon/IFQRenderResource.h"

namespace fq::graphics
{
	class D3D11Device;
	class D3D11Texture;
	class D3D11ResourceManager;
	// 1. 쉐이더 종류가 많지 않으면 그에 대응되는 재질이랑 1대1로 늘린다.
	// 2. 쉐이더 리플렉션 처리를 해서 재질도 범용적으로 다룬다.?

	class Material : public IMaterial
	{
	public:

	public:
		Material(std::shared_ptr<D3D11ResourceManager> resourceManager, const fq::common::Material& materialData, std::filesystem::path textureBasePath = "");
		~Material() = default;

		void Bind(const std::shared_ptr<D3D11Device>& d3d11Device);

		inline void SetTextureUseFlag(const MaterialTextureUseFlag& materialTextureUseFlag)  override { mMaterialTextureUseFlag = materialTextureUseFlag; }
		void SetMaterialData(const fq::common::Material& materialData) override;
		void SetTextureBasePath(const std::filesystem::path& basePath) override { mTextureBasePath = basePath; }

		inline bool GetHasBaseColor() const;
		inline bool GetHasMetalness() const;
		inline bool GetHasRoughness() const;
		inline bool GetHasNormal() const;
		inline bool GetHasEmissive() const;
		inline bool GetHasOpacity() const;
		inline const MaterialTextureUseFlag& GetTextureUseFlag() const override { return mMaterialTextureUseFlag; }
		const fq::common::Material& GetMaterialData() const override { return mMaterialData; }
		const std::filesystem::path& GetTextureBasePath() const override { return mTextureBasePath; }

		std::shared_ptr<D3D11Texture> GetBaseColor() const { return mBaseColor; }
		std::shared_ptr<D3D11Texture> GetMetalness() const { return mMetalness; }
		std::shared_ptr<D3D11Texture> GetRoughness() const { return mRoughness; }
		std::shared_ptr<D3D11Texture> GetNormal() const { return mNormal; }
		std::shared_ptr<D3D11Texture> GetEmissive() const { return mEmissive; }
		std::shared_ptr<D3D11Texture> GetOpacity() const { return mOpacity; }

	private:
		std::shared_ptr<D3D11ResourceManager> mResourceManager;

		std::filesystem::path mTextureBasePath;
		fq::common::Material mMaterialData;
		MaterialTextureUseFlag mMaterialTextureUseFlag;

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
			const TerrainMaterialInfo& materialData,
			std::filesystem::path basePath = "");
		~TerrainMaterial() = default;

		void Bind(const std::shared_ptr<D3D11Device>& d3d11Device);
		void SetMaterial(const std::shared_ptr<D3D11Device>& device, const TerrainMaterialInfo& materialData);
		inline unsigned short GetNumOfTexture() const;

		float GetTileSizeX(unsigned short index) const { return mTileSizeXs[index]; }
		float GetTileSizeY(unsigned short index) const { return mTileSizeYs[index]; }
		float GetTileOffsetX(unsigned short index) const { return mTileOffsetXs[index]; }
		float GetTileOffsetY(unsigned short index) const { return mTileOffsetYs[index]; }

		float GetMetalic(unsigned short index) const { return mMetalics[index]; }
		float GetRoughness(unsigned short index) const { return mRoughnesses[index]; }

		std::vector<float> GetHeightMapData() const { return mHeightMapData; }

	private:
		void LoadHeight(const std::string& heightMapFilePath);
		bool InBounds(int i, int j);
		float Average(int i, int j);
		void SmoothHeightMap();
		void BuildHeightMapSRV(const std::shared_ptr<D3D11Device>& device);

	private:
		unsigned short numOfTexture;

		std::filesystem::path mBasePath;

		std::vector<std::shared_ptr<D3D11Texture>> mBaseColors;
		std::vector<std::shared_ptr<D3D11Texture>> mNormals;

		std::vector<float> mTileSizeXs;
		std::vector<float> mTileSizeYs;
		std::vector<float> mTileOffsetXs;
		std::vector<float> mTileOffsetYs;

		std::vector<float> mMetalics;
		std::vector<float> mRoughnesses;

		std::shared_ptr<D3D11Texture> mHeightMap;
		std::vector<float> mHeightMapData;
		// temp
		float mHeightscale;

		std::shared_ptr<D3D11Texture> mAlpha;
	};

	inline unsigned short TerrainMaterial::GetNumOfTexture() const { return numOfTexture; }
}
