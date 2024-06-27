#include <cmath>
#include "D3D11LightManager.h"
#include "D3D11Texture.h"
#include "D3D11Shader.h"
#include "D3D11State.h"
#include "FQCommon.h"

fq::graphics::D3D11LightManager::D3D11LightManager()
	:mDirectionalLights{},
	mPointLights{},
	mSpotLight{}
{
}

void fq::graphics::D3D11LightManager::Initialize(const std::shared_ptr<D3D11Device>& d3d11Device)
{
	mLightConstantBuffer = std::make_shared<D3D11ConstantBuffer<LightData>>(d3d11Device, ED3D11ConstantBuffer::Light);

	mSpecularIBLCS = std::make_shared<D3D11ComputeShader>(d3d11Device, L"SpecularIBLCS.cso");
	mDiffuseIrradianceCS = std::make_shared<D3D11ComputeShader>(d3d11Device, L"DiffuseIrradianceCS.cso");
	mSpecularBRDFCS = std::make_shared<D3D11ComputeShader>(d3d11Device, L"SpecularBRDFCS.cso");
	mSpecularMapFilterSettingsCB = std::make_shared<D3D11ConstantBuffer<SpecularMapFilterSettingsCB>>(d3d11Device, ED3D11ConstantBuffer::Transform);
	mLinearWrapSampler = std::make_shared<D3D11SamplerState>(d3d11Device, ED3D11SamplerState::LinearWrap);
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

	MessageBox(NULL, L"������� ID�� ���� ����Ʈ�� �������� �ʽ��ϴ�! �ȵ� ���ư�~", L"����", MB_ICONERROR);
}

void fq::graphics::D3D11LightManager::UseShadow(const unsigned int id, bool bUseShadow)
{
	if (bUseShadow)
	{
		if (mDirectionalShadows.size() < DirectionalShadowTransform::MAX_SHADOW_COUNT)
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
	LightData lightData;

	/// Todo: �ϴ��� �� �ø��� �� �ø��� �ֱ� �����ϸ� ����Ʈ ����Ʈ�� ���� ����Ʈ�� ������ �˻��ؼ� �ø��� �ؾ��Ѵ�.

	// Directional Light
	// ���Ƿ� �����츦 ����ϴ� ����Ʈ�� ���� �߰��ǵ��� ��_ȫ��ȯ
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

fq::graphics::IBLTexture fq::graphics::D3D11LightManager::CreateIBLTexture(const std::shared_ptr<D3D11Device>& d3d11Device, std::shared_ptr<D3D11CubeTexture> cubemapTexture, float envScale)
{
	ID3D11UnorderedAccessView* const nullUAV[] = { nullptr };
	ID3D11Buffer* const nullBuffer[] = { nullptr };

	fq::graphics::IBLTexture result;
	result.SpecularIBL = std::make_shared<D3D11CubeTexture>(d3d11Device, 1024, 1024, DXGI_FORMAT_R32G32B32A32_FLOAT);
	result.DiffuseIrradiance = std::make_shared<D3D11CubeTexture>(d3d11Device, 256, 256, DXGI_FORMAT_R32G32B32A32_FLOAT, 1);
	result.SpecularBRDF = std::make_shared<D3D11Texture>(d3d11Device, 256, 256, DXGI_FORMAT_R16G16_FLOAT, 1);

	// Compute pre-filtered specular environment map.
	if (result.SpecularIBL != nullptr)
	{
		// Copy 0th mipmap level into destination environment map.
		for (int arraySlice = 0; arraySlice < 6; ++arraySlice) {
			const UINT subresourceIndex = D3D11CalcSubresource(0, arraySlice, cubemapTexture->GetLevel());
			const UINT sourceSubresourceIndex = D3D11CalcSubresource(0, arraySlice, result.SpecularIBL->GetLevel());
			d3d11Device->GetDeviceContext()->CopySubresourceRegion(result.SpecularIBL->GetTexture().Get(), sourceSubresourceIndex, 0, 0, 0, cubemapTexture->GetTexture().Get(), subresourceIndex, nullptr);
		}

		d3d11Device->GetDeviceContext()->CSSetShaderResources(0, 1, cubemapTexture->GetSRV().GetAddressOf());
		mLinearWrapSampler->Bind(d3d11Device, 0, ED3D11ShaderType::ComputeShader);
		mSpecularIBLCS->Bind(d3d11Device);

		// Pre-filter rest of the mip chain.
		const float deltaRoughness = 1.0f / std::max<float>(float(cubemapTexture->GetLevel() - 1), 1.0f);
		for (UINT level = 1, size = 512; level < cubemapTexture->GetLevel(); ++level, size /= 2) {
			const UINT numGroups = std::max<UINT>(1, size / 32);
			result.SpecularIBL->CreateUAV(d3d11Device, level);

			const SpecularMapFilterSettingsCB spmapConstants = { level * deltaRoughness, envScale };
			mSpecularMapFilterSettingsCB->Update(d3d11Device, spmapConstants);
			mSpecularMapFilterSettingsCB->Bind(d3d11Device, ED3D11ShaderType::ComputeShader);

			d3d11Device->GetDeviceContext()->CSSetUnorderedAccessViews(0, 1, result.SpecularIBL->GetUAV().GetAddressOf(), nullptr);
			d3d11Device->GetDeviceContext()->Dispatch(numGroups, numGroups, 6);
		}
		d3d11Device->GetDeviceContext()->CSSetConstantBuffers(0, 1, nullBuffer);
		d3d11Device->GetDeviceContext()->CSSetUnorderedAccessViews(0, 1, nullUAV, nullptr);
	}

	// Compute diffuse irradiance cubemap.
	if (result.DiffuseIrradiance != nullptr)
	{
		result.DiffuseIrradiance->CreateUAV(d3d11Device, 0);

		d3d11Device->GetDeviceContext()->CSSetShaderResources(0, 1, result.SpecularIBL->GetSRV().GetAddressOf());
		mLinearWrapSampler->Bind(d3d11Device, 0, ED3D11ShaderType::ComputeShader);
		d3d11Device->GetDeviceContext()->CSSetUnorderedAccessViews(0, 1, result.DiffuseIrradiance->GetUAV().GetAddressOf(), nullptr);
		mDiffuseIrradianceCS->Bind(d3d11Device);
		d3d11Device->GetDeviceContext()->Dispatch(result.DiffuseIrradiance->GetWidth() / 32, result.DiffuseIrradiance->GetHeight() / 32, 6);
		d3d11Device->GetDeviceContext()->CSSetUnorderedAccessViews(0, 1, nullUAV, nullptr);
	}

	// Compute Cook-Torrance BRDF 2D LUT for split-sum approximation.
	if (result.SpecularBRDF != nullptr)
	{
		mSpecularBRDFCS->Bind(d3d11Device);
		result.SpecularBRDF->CreateUAV(d3d11Device, 0);

		d3d11Device->GetDeviceContext()->CSSetUnorderedAccessViews(0, 1, result.SpecularBRDF->GetUAV().GetAddressOf(), nullptr);
		d3d11Device->GetDeviceContext()->Dispatch(result.SpecularBRDF->GetWidth() / 32, result.SpecularBRDF->GetHeight() / 32, 1);
		d3d11Device->GetDeviceContext()->CSSetUnorderedAccessViews(0, 1, nullUAV, nullptr);
	}

	return result;
}
