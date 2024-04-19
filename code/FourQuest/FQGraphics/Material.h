#pragma once

#include <memory>
#include <filesystem>

#include "../FQCommon/FQCommonLoader.h"

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

	private:
		fq::common::Material mMaterialData;

		std::shared_ptr<D3D11Texture> mBaseColor;
		std::shared_ptr<D3D11Texture> mMetalness;
		std::shared_ptr<D3D11Texture> mRoughness;
		std::shared_ptr<D3D11Texture> mNormal;
		std::shared_ptr<D3D11Texture> mEmissive;
		std::shared_ptr<D3D11Texture> mOpacity;
	};
}
