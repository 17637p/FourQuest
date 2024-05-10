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

		inline std::shared_ptr<D3D11ConstantBuffer<cbLight>> GetLightConstnatBuffer();

		// 임시 추가
		inline std::vector<std::shared_ptr<Light<DirectionalLight>>> GetDirectionalShadows() const;
		cbLight GetLightData() const;

	private:
		std::shared_ptr<D3D11ConstantBuffer<cbLight>> mLightConstantBuffer;

		std::unordered_map<unsigned int, std::shared_ptr<Light<DirectionalLight>>>	mDirectionalLights;
		std::unordered_map<unsigned int, std::shared_ptr<Light<PointLight>>>		mPointLights;
		std::unordered_map<unsigned int, std::shared_ptr<Light<SpotLight>>>			mSpotLight;

		std::unordered_map<unsigned int, std::shared_ptr<Light<DirectionalLight>>>	mDirectionalShadows;
	};

	inline std::shared_ptr<D3D11ConstantBuffer<cbLight>> D3D11LightManager::GetLightConstnatBuffer()
	{
		return mLightConstantBuffer;
	}

	inline std::vector<std::shared_ptr<Light<DirectionalLight>>> D3D11LightManager::GetDirectionalShadows() const
	{
		assert(mDirectionalShadows.size() <= cbShadowTransform::MAX_SHADOW_COUNT);

		std::vector<std::shared_ptr<Light<DirectionalLight>>> lights;
		lights.reserve(mDirectionalShadows.size());

		for (auto [id, light] : mDirectionalShadows)
		{
			lights.push_back(light);
		}

		return lights;
	}
}

