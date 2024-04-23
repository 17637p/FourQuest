#pragma once

#include <memory>
#include <directxtk/SimpleMath.h>

#include <FQCommonGraphics.h>

struct DirectionalLight
{
	DirectX::SimpleMath::Vector4 color;
	DirectX::SimpleMath::Vector3 direction;
	float intensity;
};

struct PointLight
{
	DirectX::SimpleMath::Vector4 color;
	DirectX::SimpleMath::Vector3 position;
	float intensity;
	DirectX::SimpleMath::Vector3 attenuation;
	float range;
};

struct SpotLight
{
	DirectX::SimpleMath::Vector4 color;
	DirectX::SimpleMath::Vector3 position;
	float intensity;
	DirectX::SimpleMath::Vector3 direction;
	float range;
	DirectX::SimpleMath::Vector3 attenuation;
	float spot;
};

namespace fq::graphics
{
	template<typename LightData>
	class Light
	{
	public:
		Light();
		virtual ~Light();

		ELightType GetLightType() const { return mLightType; };

	private:
		ELightType mLightType;
		std::shared_ptr<LightData> mData;
	};
}

