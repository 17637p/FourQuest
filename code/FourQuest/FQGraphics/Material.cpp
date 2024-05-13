#include <d3dcompiler.h>

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
		if (GetHasBaseColor()) mBaseColor->Bind(d3d11Device, 0, ED3D11ShaderType::Pixelshader);
		if (GetHasMetalness()) mMetalness->Bind(d3d11Device, 1, ED3D11ShaderType::Pixelshader);
		if (GetHasRoughness()) mRoughness->Bind(d3d11Device, 2, ED3D11ShaderType::Pixelshader);
		if (GetHasNormal()) mNormal->Bind(d3d11Device, 3, ED3D11ShaderType::Pixelshader);
		if (GetHasEmissive()) mEmissive->Bind(d3d11Device, 4, ED3D11ShaderType::Pixelshader);
		if (GetHasOpacity()) mOpacity->Bind(d3d11Device, 5, ED3D11ShaderType::Pixelshader);
	}

	TerrainMaterial::TerrainMaterial(const std::shared_ptr<D3D11Device>& device, 
		const fq::common::TerrainMaterial& materialData,
		std::filesystem::path basePath /*= ""*/)
	{
		mBasePath = basePath;
		mMaterialData = materialData;

		SetMaterial(device, materialData);
	}

	void TerrainMaterial::Bind(const std::shared_ptr<D3D11Device>& d3d11Device)
	{
		for (unsigned short i = 0; i < mMaterialData.NumOfTexture; i++)
		{
			mBaseColors[i]->Bind(d3d11Device, i, ED3D11ShaderType::Pixelshader);
		}

		mAlpha->Bind(d3d11Device, 16, ED3D11ShaderType::Pixelshader);
	}

	void TerrainMaterial::SetMaterial(const std::shared_ptr<D3D11Device>& device, const fq::common::TerrainMaterial& materialData)
	{
		for (unsigned short i = 0; i < materialData.NumOfTexture; i++)
		{
			if (!materialData.BaseColorFileNames[i].empty())
			{
				mBaseColors.push_back(std::make_shared<D3D11Texture>(device, mBasePath / materialData.BaseColorFileNames[i]));
			}

			mAlpha = std::make_shared<D3D11Texture>(device, mBasePath / materialData.AlPhaFileName);
			// ¿œ¥‹ BaseColor ∏∏
			//if (!materialData.MetalnessFileNames[i].empty()) mMetalness = std::make_shared<D3D11Texture>(device, basePath / materialData.MetalnessFileName);
			//if (!materialData.RoughnessFileNames[i].empty()) mRoughness = std::make_shared<D3D11Texture>(device, basePath / materialData.RoughnessFileName);
			//if (!materialData.NormalFileNames[i].empty()) mNormal = std::make_shared<D3D11Texture>(device, basePath / materialData.NormalFileName);
		}
	}

}

