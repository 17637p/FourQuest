#pragma once

#include <unordered_map>
#include <memory>
#include <vector>

#include "Light.h"
#include "D3D11Buffer.h"
#include "ConstantBufferStructure.h"

namespace fq::common { struct Transform; }

namespace fq::graphics
{
	struct LightInfo;
	class D3D11Texture;
	class D3D11CubeTexture;
	template <typename T>
	class D3D11ConstantBuffer;

	struct IBLTexture
	{
		std::shared_ptr<D3D11CubeTexture> DiffuseIrradiance;
		std::shared_ptr<D3D11CubeTexture> SpecularIBL;
		std::shared_ptr<D3D11Texture> SpecularBRDF;
	};

	class D3D11LightManager
	{
	public:
		D3D11LightManager();

		void Initialize(const std::shared_ptr<D3D11Device>& d3d11Device);

		void AddLight(const unsigned int id, const LightInfo& lightInfo);
		void SetLight(const unsigned int id, const LightInfo& lightInfo);
		void DeleteLight(const unsigned int id);

		void UseShadow(const unsigned int id, bool bUseShadow);

		void UpdateConstantBuffer(const std::shared_ptr<D3D11Device>& d3d11Device,
			const DirectX::SimpleMath::Vector3& eyePosition,
			const unsigned int isUseIBL);

		inline std::shared_ptr<D3D11ConstantBuffer<LightData>> GetLightConstnatBuffer();

		// 임시 추가
		inline std::vector<std::shared_ptr<Light<DirectionalLight>>> GetDirectionalShadows() const;

		void SetIBLTexture(IBLTexture IBLTexture) { mIBLTexture = IBLTexture; }
		const IBLTexture& GetIBLTexture() { return mIBLTexture; }

		IBLTexture CreateIBLTexture(const std::shared_ptr<D3D11Device>& d3d11Device, std::shared_ptr<D3D11CubeTexture> cubemapTexture, float envScale = 1.f);

	private:
		std::shared_ptr<D3D11ConstantBuffer<LightData>> mLightConstantBuffer;

		std::unordered_map<unsigned int, std::shared_ptr<Light<DirectionalLight>>>	mDirectionalLights;
		std::unordered_map<unsigned int, std::shared_ptr<Light<PointLight>>>		mPointLights;
		std::unordered_map<unsigned int, std::shared_ptr<Light<SpotLight>>>			mSpotLight;

		struct SpecularMapFilterSettingsCB
		{
			float roughness;
			float EnvScale;
			float padding[2];
		};

		std::unordered_map<unsigned int, std::shared_ptr<Light<DirectionalLight>>>	mDirectionalShadows;
		std::shared_ptr<class D3D11ComputeShader> mDiffuseIrradianceCS;
		std::shared_ptr<class D3D11ComputeShader> mSpecularIBLCS;
		std::shared_ptr<class D3D11ComputeShader> mSpecularBRDFCS;
		std::shared_ptr<class D3D11SamplerState> mLinearWrapSampler;
		std::shared_ptr<D3D11ConstantBuffer<SpecularMapFilterSettingsCB>> mSpecularMapFilterSettingsCB;

		IBLTexture mIBLTexture;
	};

	inline std::shared_ptr<D3D11ConstantBuffer<LightData>> D3D11LightManager::GetLightConstnatBuffer()
	{
		return mLightConstantBuffer;
	}

	inline std::vector<std::shared_ptr<Light<DirectionalLight>>> D3D11LightManager::GetDirectionalShadows() const
	{
		assert(mDirectionalShadows.size() <= DirectionalShadowTransform::MAX_SHADOW_COUNT);

		std::vector<std::shared_ptr<Light<DirectionalLight>>> lights;
		lights.reserve(mDirectionalShadows.size());

		for (auto [id, light] : mDirectionalShadows)
		{
			lights.push_back(light);
		}

		return lights;
	}
}

