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

		void UpdateConstantBuffer(const std::shared_ptr<D3D11Device>& d3d11Device,
			const DirectX::SimpleMath::Vector3& eyePosition,
			const unsigned int isUseIBL);

		inline std::shared_ptr<D3D11ConstantBuffer<LightData>> GetLightConstnatBuffer();

		// 임시 추가
		inline std::vector<std::shared_ptr<Light<DirectionalLight>>> GetDirectionalLights() const;

	private:
		std::shared_ptr<D3D11ConstantBuffer<LightData>> mLightConstantBuffer;

		std::unordered_map<unsigned int, std::shared_ptr<Light<DirectionalLight>>>	mDirectionalLights;
		std::unordered_map<unsigned int, std::shared_ptr<Light<PointLight>>>		mPointLights;
		std::unordered_map<unsigned int, std::shared_ptr<Light<SpotLight>>>			mSpotLight;
	};

	inline std::shared_ptr<D3D11ConstantBuffer<LightData>> D3D11LightManager::GetLightConstnatBuffer()
	{
		return mLightConstantBuffer;
	}

	inline std::vector<std::shared_ptr<Light<DirectionalLight>>> D3D11LightManager::GetDirectionalLights() const
	{
		std::vector<std::shared_ptr<Light<DirectionalLight>>> lights;
		lights.reserve(mDirectionalLights.size());

		for (auto [id, light] : mDirectionalLights)
		{
			lights.push_back(light);
		}

		return lights;
	}
}

