#include <d3dcompiler.h>

#include "Material.h"
#include "D3D11Texture.h"

namespace fq::graphics
{
	Material::Material(const std::shared_ptr<D3D11Device>& device, const fq::common::Material& materialData, std::filesystem::path basePath)
		: mMaterialData(materialData)
	{
		mBaseColor = std::make_shared<D3D11Texture>(device, basePath / materialData.BaseColorFileName);
		mMetalness = std::make_shared<D3D11Texture>(device, basePath / materialData.MetalnessFileName);
		mRoughness = std::make_shared<D3D11Texture>(device, basePath / materialData.RoughnessFileName);
		mNormal = std::make_shared<D3D11Texture>(device, basePath / materialData.NormalFileName);
		mEmissive = std::make_shared<D3D11Texture>(device, basePath / materialData.EmissiveFileName);
		mOpacity = std::make_shared<D3D11Texture>(device, basePath / materialData.OpacityFileName);
	}

	void Material::Bind(const std::shared_ptr<D3D11Device>& d3d11Device)
	{
		mBaseColor->Bind(d3d11Device, 0, ED3D11ShaderType::Pixelshader);
		mMetalness->Bind(d3d11Device, 1, ED3D11ShaderType::Pixelshader);
		mRoughness->Bind(d3d11Device, 2, ED3D11ShaderType::Pixelshader);
		mNormal->Bind(d3d11Device, 3, ED3D11ShaderType::Pixelshader);
		mEmissive->Bind(d3d11Device, 4, ED3D11ShaderType::Pixelshader);
		mOpacity->Bind(d3d11Device, 5, ED3D11ShaderType::Pixelshader);
	}
}