#include "D3D11LightManager.h"

#include <FQCommon.h>

fq::graphics::D3D11LightManager::D3D11LightManager()
	:mDirectionalLights{},
	mPointLights{},
	mSpotLight{}
{
}

void fq::graphics::D3D11LightManager::Initialize(const std::shared_ptr<D3D11Device>& d3d11Device)
{
	mLightConstantBuffer = std::make_shared<D3D11ConstantBuffer<cbLight>>(d3d11Device, ED3D11ConstantBuffer::Light);
}

void fq::graphics::D3D11LightManager::AddLight(const unsigned int id, const LightInfo& lightInfo)
{
	switch (lightInfo.type)
	{
	case ELightType::Directional:
	{
		mDirectionalLights[id] = std::make_shared<Light<DirectionalLight>>();
		break;
	}
	case ELightType::Point:
	{
		mPointLights[id] = std::make_shared<Light<PointLight>>();
		break;
	}
	case ELightType::Spot:
	{
		mSpotLight[id] = std::make_shared<Light<SpotLight>>();
		break;
	}
	default:
	{
		MessageBox(NULL, L"정의되지 않은 Light Type을 추가하려고 시도했습니다.", L"에러", MB_ICONERROR);

		return;
	}
	}

	SetLight(id, lightInfo);
}

void fq::graphics::D3D11LightManager::SetLight(const unsigned int id, const LightInfo& lightInfo)
{
	switch (lightInfo.type)
	{
	case ELightType::Directional:
	{
		DirectionalLight directLight;
		directLight.color.x = lightInfo.color.x;
		directLight.color.y = lightInfo.color.y;
		directLight.color.z = lightInfo.color.z;
		directLight.intensity = lightInfo.intensity;
		directLight.direction = lightInfo.direction;

		mDirectionalLights[id]->SetData(directLight);
		break;
	}
	case ELightType::Point:
	{
		PointLight pointLight;
		pointLight.color.x = lightInfo.color.x;
		pointLight.color.y = lightInfo.color.y;
		pointLight.color.z = lightInfo.color.z;
		pointLight.intensity = lightInfo.intensity;
		pointLight.position = lightInfo.position;
		pointLight.range = lightInfo.range;
		pointLight.attenuation = lightInfo.attenuation;

		mPointLights[id]->SetData(pointLight);
		break;
	}
	case ELightType::Spot:
	{
		SpotLight spotLight;
		spotLight.color.x = lightInfo.color.x;
		spotLight.color.y = lightInfo.color.y;
		spotLight.color.z = lightInfo.color.z;
		spotLight.intensity = lightInfo.intensity;
		spotLight.position = lightInfo.position;
		spotLight.range = lightInfo.range;
		spotLight.direction = lightInfo.direction;
		spotLight.spot = lightInfo.spot;
		spotLight.attenuation = lightInfo.attenuation;

		mSpotLight[id]->SetData(spotLight);
		break;
	}
	default:
		break;
	}
}

void fq::graphics::D3D11LightManager::DeleteLight(const unsigned int id)
{
	{
		auto iter = mDirectionalLights.find(id);
		if (iter != mDirectionalLights.end())
		{
			mDirectionalLights.erase(id);
			UseShadow(id, false);
			return;
		}
	}

	{
		auto iter = mPointLights.find(id);
		if (iter != mPointLights.end())
		{
			mPointLights.erase(id);
			return;
		}
	}

	{
		auto iter = mSpotLight.find(id);
		if (iter != mSpotLight.end())
		{
			mSpotLight.erase(id);
			return;
		}
	}

	MessageBox(NULL, L"지우려는 ID를 가진 라이트가 존재하지 않습니다! 안돼 돌아가~", L"에러", MB_ICONERROR);
}

void fq::graphics::D3D11LightManager::UseShadow(const unsigned int id, bool bUseShadow)
{
	if (bUseShadow)
	{
		if (mDirectionalShadows.size() < cbShadowTransform::MAX_SHADOW_COUNT)
		{
			auto find = mDirectionalLights.find(id);

			if (find != mDirectionalLights.end())
			{
				mDirectionalShadows.insert({ find->first, find->second });
			}
		}
	}
	else
	{
		mDirectionalShadows.erase(id);
	}
}

void fq::graphics::D3D11LightManager::UpdateConstantBuffer(
	const std::shared_ptr<D3D11Device>& d3d11Device,
	const DirectX::SimpleMath::Vector3& eyePosition,
	const unsigned int isUseIBL)
{
	cbLight lightData;

	/// Todo: 일단은 다 올리는 데 컬링을 넣기 시작하면 포인트 라이트와 스팟 라이트의 영역을 검사해서 컬링을 해야한다.

	// Directional Light
	// 임의로 쉐도우를 사용하는 라이트가 먼저 추가되도록 함_홍지환
	unsigned short count = 0;
	std::set<unsigned int> idSet;
	for (const auto& shadowDirectionalLight : mDirectionalShadows)
	{
		idSet.insert(shadowDirectionalLight.first);

		lightData.directionalLight[count] = shadowDirectionalLight.second->GetData();
		count++;
	}
	for (const auto& directionalLight : mDirectionalLights)
	{
		auto find = idSet.find(directionalLight.first);

		if (find == idSet.end())
		{
			lightData.directionalLight[count] = directionalLight.second->GetData();
			count++;
		}
	}

	lightData.numOfDirectionalLight = count;
	// Point Light
	count = 0;
	for (const auto& pointLight : mPointLights)
	{
		lightData.pointLight[count] = pointLight.second->GetData();
		count++;
	}
	lightData.numOfPointLight = count;
	// Spot Light
	count = 0;
	for (const auto& spotLight : mSpotLight)
	{
		lightData.spotLight[count] = spotLight.second->GetData();
		count++;
	}
	lightData.numOfSpotLight = count;

	lightData.eyePosition = eyePosition;
	lightData.isUseIBL = isUseIBL;

	mLightConstantBuffer->Update(d3d11Device, lightData);
}

fq::graphics::cbLight fq::graphics::D3D11LightManager::GetLightData() const
{
	cbLight lightData;
	/// Todo: 일단은 다 올리는 데 컬링을 넣기 시작하면 포인트 라이트와 스팟 라이트의 영역을 검사해서 컬링을 해야한다.

	// Directional Light
	// 임의로 쉐도우를 사용하는 라이트가 먼저 추가되도록 함_홍지환
	unsigned short count = 0;
	std::set<unsigned int> idSet;
	for (const auto& shadowDirectionalLight : mDirectionalShadows)
	{
		idSet.insert(shadowDirectionalLight.first);

		lightData.directionalLight[count] = shadowDirectionalLight.second->GetData();
		count++;
	}
	for (const auto& directionalLight : mDirectionalLights)
	{
		auto find = idSet.find(directionalLight.first);

		if (find == idSet.end())
		{
			lightData.directionalLight[count] = directionalLight.second->GetData();
			count++;
		}
	}

	lightData.numOfDirectionalLight = count;
	// Point Light
	count = 0;
	for (const auto& pointLight : mPointLights)
	{
		lightData.pointLight[count] = pointLight.second->GetData();
		count++;
	}
	lightData.numOfPointLight = count;
	// Spot Light
	count = 0;
	for (const auto& spotLight : mSpotLight)
	{
		lightData.spotLight[count] = spotLight.second->GetData();
		count++;
	}
	lightData.numOfSpotLight = count;

	return lightData;
}

