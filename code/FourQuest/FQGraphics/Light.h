#pragma once

#include <memory>
#include <directxtk/SimpleMath.h>

#include <FQCommonGraphics.h>

#include "D3D11Device.h"
#include "D3D11Buffer.h"

namespace fq::graphics
{
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

	//template<typename ConstantBuffer>
	//class D3D11ConstantBuffer;
	//
	//class D3D11Device;

	template<typename LightData>
	class Light
	{
	public:
		Light();
		virtual ~Light();

		void Update(const std::shared_ptr<D3D11Device>& d3d11Device, const LightData& lightData);

	private:
		std::shared_ptr<LightData> mData;
	};

	template<typename LightData>
	fq::graphics::Light<LightData>::Light()
		:mData(nullptr)
	{

	}

	template<typename LightData>
	void fq::graphics::Light<LightData>::Update(const std::shared_ptr<D3D11Device>& d3d11Device, const LightData& lightData)
	{
		*mData = lightData;
	}
}

