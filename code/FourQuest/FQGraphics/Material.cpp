#include <d3dcompiler.h>
#include <spdlog/spdlog.h>

#include "Material.h"
#include "D3D11Texture.h"
#include "D3D11Device.h"
#include "D3D11ResourceManager.h"

#include <fstream>

namespace fq::graphics
{
	Material::Material(std::shared_ptr<D3D11ResourceManager> resourceManager, const MaterialInfo& materialInfo, const std::string& name)
		: mResourceManager(resourceManager)
		, mInfo(materialInfo)
		, mName(name)
	{
		loadTexture();
	}

	void Material::loadTexture()
	{
		if (!mInfo.BaseColorFileName.empty()) mBaseColor = mResourceManager->Create<D3D11Texture>(mInfo.BaseColorFileName);
		if (!mInfo.MetalnessFileName.empty()) mMetalness = mResourceManager->Create<D3D11Texture>(mInfo.MetalnessFileName);
		if (!mInfo.RoughnessFileName.empty()) mRoughness = mResourceManager->Create<D3D11Texture>(mInfo.RoughnessFileName);
		if (!mInfo.NormalFileName.empty()) mNormal = mResourceManager->Create<D3D11Texture>(mInfo.NormalFileName);
		if (!mInfo.EmissiveFileName.empty()) mEmissive = mResourceManager->Create<D3D11Texture>(mInfo.EmissiveFileName);
	}

	void Material::Bind(const std::shared_ptr<D3D11Device>& d3d11Device)
	{
		if (GetHasBaseColor()) mBaseColor->Bind(d3d11Device, 0, ED3D11ShaderType::PixelShader);
		if (GetHasMetalness()) mMetalness->Bind(d3d11Device, 1, ED3D11ShaderType::PixelShader);
		if (GetHasRoughness()) mRoughness->Bind(d3d11Device, 2, ED3D11ShaderType::PixelShader);
		if (GetHasNormal()) mNormal->Bind(d3d11Device, 3, ED3D11ShaderType::PixelShader);
		if (GetHasEmissive()) mEmissive->Bind(d3d11Device, 4, ED3D11ShaderType::PixelShader);
		if (GetHasOpacity()) mOpacity->Bind(d3d11Device, 5, ED3D11ShaderType::PixelShader);
	}

	TerrainMaterial::TerrainMaterial(const std::shared_ptr<D3D11Device>& device,
		const TerrainMaterialInfo& materialData,
		std::filesystem::path basePath /*= ""*/)
		:mBaseColors{},
		mNormals{},
		mTileSizeXs{},
		mTileSizeYs{},
		mTileOffsetXs{},
		mTileOffsetYs{},
		mMetalics{},
		mRoughnesses{},
		mAlpha{ nullptr },
		mHeightscale(materialData.HeightScale)
	{
		mBasePath = basePath;

		LoadHeight(materialData.HeightFileName);
		SmoothHeightMap();
		BuildHeightMapSRV(device);

		SetMaterial(device, materialData);
	}

	void TerrainMaterial::Bind(const std::shared_ptr<D3D11Device>& d3d11Device)
	{
		if (mBaseColors.size() != 0)
		{
			D3D11Texture::Bind(d3d11Device, mBaseColors, 0, ED3D11ShaderType::PixelShader);
		}
		if (mNormals.size() != 0)
		{
			D3D11Texture::Bind(d3d11Device, mNormals, 12, ED3D11ShaderType::PixelShader);
		}
		if (mAlpha != nullptr)
		{
			mAlpha->Bind(d3d11Device, 16, ED3D11ShaderType::PixelShader);
		}
		if (mHeightMap != nullptr)
		{
			mHeightMap->Bind(d3d11Device, 0, ED3D11ShaderType::VertexShader);
			mHeightMap->Bind(d3d11Device, 17, ED3D11ShaderType::PixelShader);
			mHeightMap->Bind(d3d11Device, 0, ED3D11ShaderType::DomainShader);
		}
	}

	void TerrainMaterial::SetMaterial(const std::shared_ptr<D3D11Device>& device, const TerrainMaterialInfo& materialData)
	{
		numOfTexture = materialData.Layers.size();
		for (unsigned short i = 0; i < numOfTexture; i++)
		{
			if (!materialData.Layers[i].BaseColor.empty())
			{
				mBaseColors.push_back(std::make_shared<D3D11Texture>(device, mBasePath / materialData.Layers[i].BaseColor));
			}
			else
			{
				spdlog::error("Terrain baseColor Texture is not existed");
			}

			if (!materialData.Layers[i].NormalMap.empty())
			{
				mNormals.push_back(std::make_shared<D3D11Texture>(device, mBasePath / materialData.Layers[i].NormalMap));
			}
			else
			{
				spdlog::error("Terrain Normal Texture is not existed");
			}

			mTileSizeXs.push_back(materialData.Layers[i].TileSizeX);
			mTileSizeYs.push_back(materialData.Layers[i].TileSizeY);
			mTileOffsetXs.push_back(materialData.Layers[i].TileOffsetX);
			mTileOffsetYs.push_back(materialData.Layers[i].TileOffsetY);

			mMetalics.push_back(materialData.Layers[i].Metalic);
			mRoughnesses.push_back(materialData.Layers[i].Roughness);

			mAlpha = std::make_shared<D3D11Texture>(device, mBasePath / materialData.AlPhaFileName);
			// �ϴ� BaseColor ��
			//if (!materialData.MetalnessFileNames[i].empty()) mMetalness = std::make_shared<D3D11Texture>(device, basePath / materialData.MetalnessFileName);
			//if (!materialData.RoughnessFileNames[i].empty()) mRoughness = std::make_shared<D3D11Texture>(device, basePath / materialData.RoughnessFileName);
			//if (!materialData.NormalFileNames[i].empty()) mNormal = std::make_shared<D3D11Texture>(device, basePath / materialData.NormalFileName);
		}
	}

	void TerrainMaterial::LoadHeight(const std::string& heightMapFilePath)
	{
		// temp
		const UINT width = 513;
		const UINT height = 513;

		std::vector<unsigned char> in(width * height);

		std::ifstream inFile;
		inFile.open(heightMapFilePath.c_str(), std::ios_base::binary);

		if (inFile)
		{
			inFile.read((char*)&in[0], (std::streamsize)in.size());

			inFile.close();
		}

		mHeightMapData.resize(width * height, 0);
		for (UINT i = 0; i < width * height; i++)
		{
			mHeightMapData[i] = (in[i] / 255.0f) * mHeightscale;
		}
	}

	bool TerrainMaterial::InBounds(int i, int j)
	{
		// temp
		const UINT width = 513;
		const UINT height = 513;

		return
			j >= 0 &&
			i < (int)height &&
			i >= 0 &&
			j < (int)width;
	}

	float TerrainMaterial::Average(int i, int j)
	{
		// temp
		const UINT width = 513;
		const UINT height = 513;

		float average = 0.0f;
		float num = 0.0f;

		for (int m = i - 1; m <= i + 1; m++)
		{
			for (int n = j - 1; n <= j + 1; n++)
			{
				if (InBounds(m, n))
				{
					average += mHeightMapData[m * width + n];
					num += 1.0f;
				}
			}
		}

		if (average != 0)
		{
			int a = 3;
		}
		return average / num;
	}

	void TerrainMaterial::SmoothHeightMap()
	{
		// temp
		const UINT width = 513;
		const UINT height = 513;

		std::vector<float> dest(mHeightMapData.size());

		for (UINT i = 0; i < height; i++)
		{
			for (UINT j = 0; j < width; j++)
			{
				dest[i * width + j] = Average(i, j);
			}
		}

		mHeightMapData = dest;
	}

	void TerrainMaterial::BuildHeightMapSRV(const std::shared_ptr<D3D11Device>& device)
	{
		// temp
		const UINT width = 513;
		const UINT height = 513;

		mHeightMap = std::make_shared<D3D11Texture>(device, mHeightMapData, width, height);
	}

	ParticleMaterial::ParticleMaterial(std::shared_ptr<D3D11ResourceManager> resourceManager, const ParticleMaterialInfo& materialInfo, const std::string& name)
		: mResourceManager(resourceManager)
		, mInfo(materialInfo)
		, mName(name)
	{
		loadTexture();
	}
	void ParticleMaterial::Bind(const std::shared_ptr<D3D11Device>& d3d11Device) 
	{
		if (GetHasBaseColor()) mBaseColor->Bind(d3d11Device, 0, ED3D11ShaderType::PixelShader);
		if (GetHasEmissive()) mEmissive->Bind(d3d11Device, 1, ED3D11ShaderType::PixelShader);
	}
	void ParticleMaterial::loadTexture() 
	{
		if (!mInfo.BaseColorFileName.empty()) mBaseColor = mResourceManager->Create<D3D11Texture>(mInfo.BaseColorFileName);
		if (!mInfo.EmissiveFileName.empty()) mEmissive = mResourceManager->Create<D3D11Texture>(mInfo.EmissiveFileName);
	}

	DecalMaterial::DecalMaterial(std::shared_ptr<D3D11ResourceManager> resourceManager, const DecalMaterialInfo& materialInfo, const std::string& name)
		: mResourceManager(resourceManager)
		, mInfo(materialInfo)
		, mName(name)
	{
		loadTexture();
	}
	void DecalMaterial::Bind(const std::shared_ptr<D3D11Device>& d3d11Device)
	{
		if (GetHasBaseColor()) mBaseColor->Bind(d3d11Device, 0, ED3D11ShaderType::PixelShader);
		if (GetHasMetalness()) mMetalness->Bind(d3d11Device, 1, ED3D11ShaderType::PixelShader);
		if (GetHasRoughness()) mRoughness->Bind(d3d11Device, 2, ED3D11ShaderType::PixelShader);
		if (GetHasNormal()) mNormal->Bind(d3d11Device, 3, ED3D11ShaderType::PixelShader);
		if (GetHasEmissive()) mEmissive->Bind(d3d11Device, 4, ED3D11ShaderType::PixelShader);
	}
	void DecalMaterial::loadTexture()
	{
		if (!mInfo.BaseColorFileName.empty()) mBaseColor = mResourceManager->Create<D3D11Texture>(mInfo.BaseColorFileName);
		if (!mInfo.MetalnessFileName.empty()) mMetalness = mResourceManager->Create<D3D11Texture>(mInfo.MetalnessFileName);
		if (!mInfo.RoughnessFileName.empty()) mRoughness = mResourceManager->Create<D3D11Texture>(mInfo.RoughnessFileName);
		if (!mInfo.NormalFileName.empty()) mNormal = mResourceManager->Create<D3D11Texture>(mInfo.NormalFileName);
		if (!mInfo.EmissiveFileName.empty()) mEmissive = mResourceManager->Create<D3D11Texture>(mInfo.EmissiveFileName);
	}
}

