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
		auto createTextureOrNull = [](std::wstring& filePath, const std::shared_ptr<D3D11ResourceManager>& resourceManager) -> std::shared_ptr<D3D11Texture>
			{
				if (filePath.empty())
				{
					return nullptr;
				}
				if (!std::filesystem::exists(filePath))
				{
					spdlog::warn("[Material] \"{}\" not exist", std::filesystem::path(filePath).string());
					filePath = L"";
					return nullptr;
				}

				return resourceManager->Create<D3D11Texture>(filePath);
			};

		mBaseColor = createTextureOrNull(mInfo.BaseColorFileName, mResourceManager);
		mMetalness = createTextureOrNull(mInfo.MetalnessFileName, mResourceManager);
		mRoughness = createTextureOrNull(mInfo.RoughnessFileName, mResourceManager);
		mNormal = createTextureOrNull(mInfo.NormalFileName, mResourceManager);
		mEmissive = createTextureOrNull(mInfo.EmissiveFileName, mResourceManager);
		mMetalnessSmoothness = createTextureOrNull(mInfo.MetalnessSmoothnessFileName, mResourceManager);
		mNoise = createTextureOrNull(mInfo.NoiseFileName, mResourceManager);
	}

	void Material::Bind(const std::shared_ptr<D3D11Device>& d3d11Device)
	{
		if (GetHasBaseColor()) mBaseColor->Bind(d3d11Device, 0, ED3D11ShaderType::PixelShader);
		if (GetHasMetalness()) mMetalness->Bind(d3d11Device, 1, ED3D11ShaderType::PixelShader);
		if (GetHasRoughness()) mRoughness->Bind(d3d11Device, 2, ED3D11ShaderType::PixelShader);
		if (GetHasNormal()) mNormal->Bind(d3d11Device, 3, ED3D11ShaderType::PixelShader);
		if (GetHasEmissive()) mEmissive->Bind(d3d11Device, 4, ED3D11ShaderType::PixelShader);
		if (GetHasMetalnessSmoothness()) mMetalnessSmoothness->Bind(d3d11Device, 5, ED3D11ShaderType::PixelShader);
		if (GetHasNoise()) mNoise->Bind(d3d11Device, 10, ED3D11ShaderType::PixelShader);
	}

	TerrainMaterial::TerrainMaterial(const std::shared_ptr<D3D11Device>& device,
		const TerrainMaterialInfo& materialData,
		UINT width, UINT height,
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
		mWidth{ width },
		mHeight{ height },
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
			// 일단 BaseColor 만
			//if (!materialData.MetalnessFileNames[i].empty()) mMetalness = std::make_shared<D3D11Texture>(device, basePath / materialData.MetalnessFileName);
			//if (!materialData.RoughnessFileNames[i].empty()) mRoughness = std::make_shared<D3D11Texture>(device, basePath / materialData.RoughnessFileName);
			//if (!materialData.NormalFileNames[i].empty()) mNormal = std::make_shared<D3D11Texture>(device, basePath / materialData.NormalFileName);
		}
	}

	void TerrainMaterial::LoadHeight(const std::string& heightMapFilePath)
	{
		std::vector<unsigned char> in(mWidth * mHeight);

		std::ifstream inFile;
		inFile.open(heightMapFilePath.c_str(), std::ios_base::binary);

		if (inFile)
		{
			inFile.read((char*)&in[0], (std::streamsize)in.size());

			inFile.close();
		}

		mHeightMapData.resize(mWidth * mHeight, 0);
		mHeightMapRawData.resize(mWidth * mHeight, 0);
		for (UINT i = 0; i < mWidth * mHeight; i++)
		{
			mHeightMapRawData[i] = in[i];
			mHeightMapData[i] = (in[i] / 255.0f) * mHeightscale;
		}

		// 유니티에서 읽어온 하이트맵 텍스처의 y축이 대칭된 모습을 보여서 임시로 수정_홍지환
		// flip y
		for (UINT i = 0; i < mHeight / 2; ++i)
		{
			for (UINT j = 0; j < mWidth; ++j)
			{
				UINT front = i * mWidth + j;
				UINT back = (mHeight - i - 1) * mWidth + j;
				std::swap(mHeightMapRawData[front], mHeightMapRawData[back]);
				std::swap(mHeightMapData[front], mHeightMapData[back]);
			}
		}
	}

	bool TerrainMaterial::InBounds(int i, int j)
	{
		return
			j >= 0 &&
			i < (int)mHeight &&
			i >= 0 &&
			j < (int)mWidth;
	}

	float TerrainMaterial::Average(int i, int j)
	{
		float average = 0.0f;
		float num = 0.0f;

		for (int m = i - 1; m <= i + 1; m++)
		{
			for (int n = j - 1; n <= j + 1; n++)
			{
				if (InBounds(m, n))
				{
					average += mHeightMapData[m * mWidth + n];
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
		std::vector<float> dest(mHeightMapData.size());

		for (UINT i = 0; i < mHeight; i++)
		{
			for (UINT j = 0; j < mWidth; j++)
			{
				dest[i * mWidth + j] = Average(i, j);
			}
		}

		mHeightMapData = dest;
	}

	void TerrainMaterial::BuildHeightMapSRV(const std::shared_ptr<D3D11Device>& device)
	{
		mHeightMap = std::make_shared<D3D11Texture>(device, mHeightMapData, mWidth, mHeight);
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
		if (GetHasNormal()) mNormal->Bind(d3d11Device, 1, ED3D11ShaderType::PixelShader);
		if (GetHasEmissive()) mEmissive->Bind(d3d11Device, 2, ED3D11ShaderType::PixelShader);
	}
	void DecalMaterial::loadTexture()
	{
		if (!mInfo.BaseColorFileName.empty()) mBaseColor = mResourceManager->Create<D3D11Texture>(mInfo.BaseColorFileName);
		if (!mInfo.NormalFileName.empty()) mNormal = mResourceManager->Create<D3D11Texture>(mInfo.NormalFileName);
		if (!mInfo.EmissiveFileName.empty()) mEmissive = mResourceManager->Create<D3D11Texture>(mInfo.EmissiveFileName);
	}
}

