#pragma once

#include <memory>
#include <filesystem>

#include "../FQCommon/FQCommonLoader.h"

namespace fq::graphics
{
	class D3D11Device;
	class D3D11Texture;

	// 1. ���̴� ������ ���� ������ �׿� �����Ǵ� �����̶� 1��1�� �ø���.
	// 2. ���̴� ���÷��� ó���� �ؼ� ������ ���������� �ٷ��.?

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
}