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
		Material(std::shared_ptr<D3D11ResourceManager> resourceManager, const MaterialInfo& materialInfo, const std::string& name = "");
		~Material() = default;

		void SetInfo(const MaterialInfo& info) override { mInfo = info; loadTexture(); }
		const MaterialInfo& GetInfo() const override { return mInfo; }

		void Bind(const std::shared_ptr<D3D11Device>& d3d11Device);

		inline bool GetHasBaseColor() const { return mBaseColor != nullptr; }
		inline bool GetHasMetalness() const { return mMetalness != nullptr; }
		inline bool GetHasRoughness() const { return mRoughness != nullptr; }
		inline bool GetHasNormal() const { return mNormal != nullptr; }
		inline bool GetHasEmissive() const { return mEmissive != nullptr; }
		inline bool GetHasMetalnessSmoothness() const { return mMetalnessSmoothness != nullptr; }
		inline bool GetHasNoise() const { return mNoise != nullptr; }
		inline bool GetHasBlend() const { return mBlend != nullptr; }

		std::shared_ptr<D3D11Texture> GetBaseColor() const { return mBaseColor; }
		std::shared_ptr<D3D11Texture> GetMetalness() const { return mMetalness; }
		std::shared_ptr<D3D11Texture> GetRoughness() const { return mRoughness; }
		std::shared_ptr<D3D11Texture> GetNormal() const { return mNormal; }
		std::shared_ptr<D3D11Texture> GetEmissive() const { return mEmissive; }
		std::shared_ptr<D3D11Texture> GetMetalnessSmoothness() const { return mMetalnessSmoothness; }
		std::shared_ptr<D3D11Texture> GetNoise() const { return mNoise; }
		std::shared_ptr<D3D11Texture> GetBlend() const { return mBlend; }

		const std::string& GetName() const override { return mName; }

	private:
		void loadTexture();

	private:
		std::string mName;
		std::shared_ptr<D3D11ResourceManager> mResourceManager;
		MaterialInfo mInfo;

		std::shared_ptr<D3D11Texture> mBaseColor;
		std::shared_ptr<D3D11Texture> mMetalness;
		std::shared_ptr<D3D11Texture> mRoughness;
		std::shared_ptr<D3D11Texture> mNormal;
		std::shared_ptr<D3D11Texture> mEmissive;
		std::shared_ptr<D3D11Texture> mMetalnessSmoothness;
		std::shared_ptr<D3D11Texture> mNoise;
		std::shared_ptr<D3D11Texture> mBlend;
	};

	class TerrainMaterial
	{
	public:
		TerrainMaterial(const std::shared_ptr<D3D11Device>& device,
			const TerrainMaterialInfo& materialData,
			UINT width, UINT height,
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
		int* GetHeightMapRawData() { return mHeightMapRawData.data(); }

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
		std::vector<int> mHeightMapRawData;
		std::vector<float> mHeightMapData;
		// temp
		UINT mWidth;
		UINT mHeight;
		float mHeightscale;

		std::shared_ptr<D3D11Texture> mAlpha;
	};

	inline unsigned short TerrainMaterial::GetNumOfTexture() const { return numOfTexture; }


	class ParticleMaterial : public IParticleMaterial
	{
	public:
		ParticleMaterial(std::shared_ptr<D3D11ResourceManager> resourceManager, const ParticleMaterialInfo& materialInfo, const std::string& name = "");
		~ParticleMaterial() = default;

		void SetInfo(const ParticleMaterialInfo& info) override { mInfo = info; loadTexture(); }
		const ParticleMaterialInfo& GetInfo() const override { return mInfo; }

		void Bind(const std::shared_ptr<D3D11Device>& d3d11Device);

		inline bool GetHasBaseColor() const { return mBaseColor != nullptr; }
		inline bool GetHasEmissive() const { return mEmissive != nullptr; }

		std::shared_ptr<D3D11Texture> GetBaseColor() const { return mBaseColor; }
		std::shared_ptr<D3D11Texture> GetEmissive() const { return mEmissive; }

		const std::string& GetName() const override { return mName; }

	private:
		void loadTexture();

	private:
		std::string mName;
		std::shared_ptr<D3D11ResourceManager> mResourceManager;
		ParticleMaterialInfo mInfo;

		std::shared_ptr<D3D11Texture> mBaseColor;
		std::shared_ptr<D3D11Texture> mEmissive;
	};

	class DecalMaterial : public IDecalMaterial
	{
	public:
		DecalMaterial(std::shared_ptr<D3D11ResourceManager> resourceManager, const DecalMaterialInfo& materialInfo, const std::string& name = "");
		~DecalMaterial() = default;

		void SetInfo(const DecalMaterialInfo& info) override { mInfo = info; loadTexture(); }
		const DecalMaterialInfo& GetInfo() const override { return mInfo; }

		void Bind(const std::shared_ptr<D3D11Device>& d3d11Device);

		inline bool GetHasBaseColor() const { return mBaseColor != nullptr; }
		inline bool GetHasMetalness() const { return mMetalness != nullptr; }
		inline bool GetHasRoughness() const { return mRoughness != nullptr; }
		inline bool GetHasNormal() const { return mNormal != nullptr; }
		inline bool GetHasEmissive() const { return mEmissive != nullptr; }
		inline bool GetHasOpacity() const { return mOpacity != nullptr; }

		std::shared_ptr<D3D11Texture> GetBaseColor() const { return mBaseColor; }
		std::shared_ptr<D3D11Texture> GetMetalness() const { return mMetalness; }
		std::shared_ptr<D3D11Texture> GetRoughness() const { return mRoughness; }
		std::shared_ptr<D3D11Texture> GetNormal() const { return mNormal; }
		std::shared_ptr<D3D11Texture> GetEmissive() const { return mEmissive; }
		std::shared_ptr<D3D11Texture> GetOpacity() const { return mOpacity; }

		const std::string& GetName() const override { return mName; }

	private:
		void loadTexture();

	private:
		std::string mName;
		std::shared_ptr<D3D11ResourceManager> mResourceManager;
		DecalMaterialInfo mInfo;

		std::shared_ptr<D3D11Texture> mBaseColor;
		std::shared_ptr<D3D11Texture> mMetalness;
		std::shared_ptr<D3D11Texture> mRoughness;
		std::shared_ptr<D3D11Texture> mNormal;
		std::shared_ptr<D3D11Texture> mEmissive;
		std::shared_ptr<D3D11Texture> mOpacity;
	};
}
