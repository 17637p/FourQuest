#include <d3dcompiler.h>
#include <spdlog/spdlog.h>

#include "Material.h"
#include "D3D11Texture.h"

namespace fq::graphics
{
	Material::Material(const std::shared_ptr<D3D11Device>& device, const fq::common::Material& materialData, std::filesystem::path basePath)
		: mMaterialData(materialData)
	{
		if (!materialData.BaseColorFileName.empty()) mBaseColor = std::make_shared<D3D11Texture>(device, basePath / materialData.BaseColorFileName);
		if (!materialData.MetalnessFileName.empty()) mMetalness = std::make_shared<D3D11Texture>(device, basePath / materialData.MetalnessFileName);
		if (!materialData.RoughnessFileName.empty()) mRoughness = std::make_shared<D3D11Texture>(device, basePath / materialData.RoughnessFileName);
		if (!materialData.NormalFileName.empty()) mNormal = std::make_shared<D3D11Texture>(device, basePath / materialData.NormalFileName);
		if (!materialData.EmissiveFileName.empty()) mEmissive = std::make_shared<D3D11Texture>(device, basePath / materialData.EmissiveFileName);
		if (!materialData.OpacityFileName.empty()) mOpacity = std::make_shared<D3D11Texture>(device, basePath / materialData.OpacityFileName);
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
		mAlpha{nullptr}
	{
		mBasePath = basePath;

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

}

