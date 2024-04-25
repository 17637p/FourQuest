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
	mLightConstantBuffer = std::make_shared<D3D11ConstantBuffer<LightData>>(d3d11Device, ED3D11ConstantBuffer::Light);
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
			MessageBox(NULL, L"���ǵ��� ���� Light Type�� �߰��Ϸ��� �õ��߽��ϴ�.", L"����", MB_ICONERROR);

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

	MessageBox(NULL, L"������� ID�� ���� ����Ʈ�� �������� �ʽ��ϴ�! �ȵ� ���ư�~", L"����", MB_ICONERROR);
}

void fq::graphics::D3D11LightManager::BindConstantBuffer(const std::shared_ptr<D3D11Device>& d3d11Device, const ED3D11ShaderType eShaderType, const UINT startSlot /*= 0*/)
{
	mLightConstantBuffer->Bind(d3d11Device, eShaderType, startSlot);
}

void fq::graphics::D3D11LightManager::UpdateConstantBuffer(const std::shared_ptr<D3D11Device>& d3d11Device, const DirectX::SimpleMath::Vector3& eyePosition)
{
	LightData lightData;

	/// Todo: �ϴ��� �� �ø��� �� �ø��� �ֱ� �����ϸ� ����Ʈ ����Ʈ�� ���� ����Ʈ�� ������ �˻��ؼ� �ø��� �ؾ��Ѵ�.

	// Directional Light
	unsigned short count = 0;
	for (const auto& directionalLight : mDirectionalLights)
	{
		lightData.directionalLight[count] = directionalLight.second->GetData();
		count++;
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

	mLightConstantBuffer->Update(d3d11Device, lightData);
}
