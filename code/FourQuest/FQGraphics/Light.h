#pragma once

#include <memory>
#include <directxtk/SimpleMath.h>

#include <FQCommonGraphics.h>

namespace fq::graphics
{
	struct DirectionalLight
	{
		DirectX::SimpleMath::Vector3 color;
		float intensity;
		DirectX::SimpleMath::Vector3 direction;
		float pad;
	};

	struct PointLight
	{
		DirectX::SimpleMath::Vector3 color;
		float intensity;
		DirectX::SimpleMath::Vector3 position;
		float range;
		DirectX::SimpleMath::Vector3 attenuation;
		float pad;
	};

	struct SpotLight
	{
		DirectX::SimpleMath::Vector3 color;
		float intensity;
		DirectX::SimpleMath::Vector3 position;
		float range;
		DirectX::SimpleMath::Vector3 direction;
		float spot;
		DirectX::SimpleMath::Vector3 attenuation;
		float pad;
	};

	template<typename LightData>
	class Light
	{
	public:
		Light();

		LightData GetData() const;
		void SetData(const LightData& lightData);

	private:
		LightData mData;
	};

	template<typename LightData>
	LightData fq::graphics::Light<LightData>::GetData() const
	{
		return mData;
	}

	template<typename LightData>
	fq::graphics::Light<LightData>::Light()
		:mData()
	{
	}

	template<typename LightData>
	void fq::graphics::Light<LightData>::SetData(const LightData& lightData)
	{
		mData = lightData;
	}
}

