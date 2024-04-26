#pragma once

#include <unordered_map>
#include <memory>

#include "Light.h"
#include "D3D11Buffer.h"

namespace fq::common { struct Transform; }

namespace fq::graphics
{
	struct LightInfo;

	class D3D11LightManager
	{
		// Todo: 확인해보기. 클래스 안 쪽에 넣어도 잘 돌아 간다면 넣는 게 좋을지도? 여기서만 쓸 거 같은데
		struct LightData
		{
			DirectionalLight directionalLight[3];
			PointLight pointLight[10];
			SpotLight spotLight[5];

			unsigned int numOfDirectionalLight;
			unsigned int numOfPointLight;
			unsigned int numOfSpotLight;
			unsigned int isUseIBL;

			DirectX::SimpleMath::Vector3 eyePosition;
			float pad2;
		};
	public:
		D3D11LightManager();
		
		void Initialize(const std::shared_ptr<D3D11Device>& d3d11Device);

		void AddLight(const unsigned int id, const LightInfo& lightInfo);
		void SetLight(const unsigned int id, const LightInfo& lightInfo);
		void DeleteLight(const unsigned int id);

		void UpdateConstantBuffer(const std::shared_ptr<D3D11Device>& d3d11Device, 
			const DirectX::SimpleMath::Vector3& eyePosition,
			const unsigned int isUseIBL);
		void BindConstantBuffer(
			const std::shared_ptr<D3D11Device>& d3d11Device, 
			const ED3D11ShaderType eShaderType, 
			const UINT startSlot = 0);

	private:
		std::shared_ptr<D3D11ConstantBuffer<LightData>> mLightConstantBuffer;

		std::unordered_map<unsigned int, std::shared_ptr<Light<DirectionalLight>>>	mDirectionalLights;
		std::unordered_map<unsigned int, std::shared_ptr<Light<PointLight>>>		mPointLights;
		std::unordered_map<unsigned int, std::shared_ptr<Light<SpotLight>>>			mSpotLight;
	};
}

