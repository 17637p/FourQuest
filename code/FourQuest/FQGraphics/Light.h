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

	template<typename cbLight>
	class Light
	{
	public:
		Light();

		cbLight GetData() const;
		void SetData(const cbLight& lightData);

	private:
		cbLight mData;
	};

	template<typename cbLight>
	cbLight fq::graphics::Light<cbLight>::GetData() const
	{
		return mData;
	}

	template<typename cbLight>
	fq::graphics::Light<cbLight>::Light()
		:mData()
	{
	}

	template<typename cbLight>
	void fq::graphics::Light<cbLight>::SetData(const cbLight& lightData)
	{
		mData = lightData;
	}
}

