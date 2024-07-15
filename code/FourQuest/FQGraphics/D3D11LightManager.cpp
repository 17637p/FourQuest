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

	mResizeCubemapCS = std::make_shared<D3D11ComputeShader>(d3d11Device, L"ResizeCubemapCS.cso");
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
	lightData.isUseIBL = mIBLTexture.DiffuseIrradiance != nullptr && mIBLTexture.SpecularBRDF != nullptr && mIBLTexture.SpecularIBL != nullptr;

	mLightConstantBuffer->Update(d3d11Device, lightData);
}

fq::graphics::IBLTexture fq::graphics::D3D11LightManager::CreateIBLTexture(const std::shared_ptr<D3D11Device>& d3d11Device, std::shared_ptr<D3D11CubeTexture> cubemapTexture, EEnvironmentFormat envFormat, EEnvironmentResoulution specularResolution, EEnvironmentResoulution diffuseResolution, float envScale)
{
	ID3D11UnorderedAccessView* const nullUAV[] = { nullptr };
	ID3D11Buffer* const nullBuffer[] = { nullptr };

	DXGI_FORMAT format;

	switch (envFormat)
	{
	case EEnvironmentFormat::RGBA16:
		format = DXGI_FORMAT_R16G16B16A16_FLOAT;
		break;
	case EEnvironmentFormat::RGBA32:
		format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		break;
	defualt:
		assert(false);
	}

	UINT specularSize;

	switch (specularResolution)
	{
	case EEnvironmentResoulution::Size32x32: specularSize = 32; break;
	case EEnvironmentResoulution::Size64x64: specularSize = 64; break;
	case EEnvironmentResoulution::Size128x128: specularSize = 128; break;
	case EEnvironmentResoulution::Size256x256: specularSize = 256; break;
	case EEnvironmentResoulution::Size512x512: specularSize = 512; break;
	case EEnvironmentResoulution::Size1024x1024: specularSize = 1024; break;
	default: assert(false); break;
	}

	UINT diffuseSize;

	switch (diffuseResolution)
	{
	case EEnvironmentResoulution::Size32x32: diffuseSize = 32; break;
	case EEnvironmentResoulution::Size64x64: diffuseSize = 64; break;
	case EEnvironmentResoulution::Size128x128: diffuseSize = 128; break;
	case EEnvironmentResoulution::Size256x256: diffuseSize = 256; break;
	case EEnvironmentResoulution::Size512x512: diffuseSize = 512; break;
	case EEnvironmentResoulution::Size1024x1024: diffuseSize = 1024; break;
	default: assert(false); break;
	}

	fq::graphics::IBLTexture result;

	auto envTexture = std::make_shared<D3D11CubeTexture>(d3d11Device, specularSize, specularSize, format);
	result.SpecularIBL = std::make_shared<D3D11CubeTexture>(d3d11Device, specularSize, specularSize, format);
	result.DiffuseIrradiance = std::make_shared<D3D11CubeTexture>(d3d11Device, diffuseSize, diffuseSize, format, 1);

	if (envTexture != nullptr)
	{
		// copy env
		mResizeCubemapCS->Bind(d3d11Device);

		d3d11Device->GetDeviceContext()->CSSetShaderResources(0, 1, cubemapTexture->GetSRV().GetAddressOf());
		envTexture->CreateUAV(d3d11Device, 0);
		d3d11Device->GetDeviceContext()->CSSetUnorderedAccessViews(0, 1, envTexture->GetUAV().GetAddressOf(), nullptr);
		mLinearWrapSampler->Bind(d3d11Device, 0, ED3D11ShaderType::ComputeShader);

		d3d11Device->GetDeviceContext()->CSSetConstantBuffers(0, 1, nullBuffer);
		d3d11Device->GetDeviceContext()->CSSetUnorderedAccessViews(0, 1, nullUAV, nullptr);

		for (UINT level = 0, size = specularSize; level < envTexture->GetLevel(); ++level, size /= 2) {
			const UINT numGroups = std::max<UINT>(1, size / 32);
			envTexture->CreateUAV(d3d11Device, level);

			d3d11Device->GetDeviceContext()->CSSetUnorderedAccessViews(0, 1, envTexture->GetUAV().GetAddressOf(), nullptr);
			d3d11Device->GetDeviceContext()->Dispatch(numGroups, numGroups, 6);
		}

		d3d11Device->GetDeviceContext()->CSSetConstantBuffers(0, 1, nullBuffer);
		d3d11Device->GetDeviceContext()->CSSetUnorderedAccessViews(0, 1, nullUAV, nullptr);
	}

	// Compute pre-filtered specular environment map.
	if (result.SpecularIBL != nullptr)
	{
		// Copy 0th mipmap level into destination environment map.
		for (int arraySlice = 0; arraySlice < 6; ++arraySlice) {
			const UINT subresourceIndex = D3D11CalcSubresource(0, arraySlice, envTexture->GetLevel());
			const UINT sourceSubresourceIndex = D3D11CalcSubresource(0, arraySlice, result.SpecularIBL->GetLevel());
			d3d11Device->GetDeviceContext()->CopySubresourceRegion(result.SpecularIBL->GetTexture().Get(), sourceSubresourceIndex, 0, 0, 0, envTexture->GetTexture().Get(), subresourceIndex, nullptr);
		}

		d3d11Device->GetDeviceContext()->CSSetShaderResources(0, 1, envTexture->GetSRV().GetAddressOf());
		mLinearWrapSampler->Bind(d3d11Device, 0, ED3D11ShaderType::ComputeShader);
		mSpecularIBLCS->Bind(d3d11Device);

		// Pre-filter rest of the mip chain.
		const float deltaRoughness = 1.0f / std::max<float>(float(envTexture->GetLevel() - 1), 1.0f);
		for (UINT level = 1, size = specularSize / 2; level < envTexture->GetLevel(); ++level, size /= 2) {
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
		mSpecularMapFilterSettingsCB->Bind(d3d11Device, ED3D11ShaderType::ComputeShader);

		d3d11Device->GetDeviceContext()->Dispatch(result.DiffuseIrradiance->GetWidth() / 32, result.DiffuseIrradiance->GetHeight() / 32, 6);
		d3d11Device->GetDeviceContext()->CSSetUnorderedAccessViews(0, 1, nullUAV, nullptr);
		d3d11Device->GetDeviceContext()->CSSetConstantBuffers(0, 1, nullBuffer);
	}

	// Compute Cook-Torrance BRDF 2D LUT for split-sum approximation.
	if (mSpecularBRDF == nullptr)
	{
		mSpecularBRDF = std::make_shared<D3D11Texture>(d3d11Device, 256, 256, format, 1);

		mSpecularBRDFCS->Bind(d3d11Device);
		mSpecularBRDF->CreateUAV(d3d11Device, 0);

		d3d11Device->GetDeviceContext()->CSSetUnorderedAccessViews(0, 1, mSpecularBRDF->GetUAV().GetAddressOf(), nullptr);
		d3d11Device->GetDeviceContext()->Dispatch(mSpecularBRDF->GetWidth() / 32, mSpecularBRDF->GetHeight() / 32, 1);
		d3d11Device->GetDeviceContext()->CSSetUnorderedAccessViews(0, 1, nullUAV, nullptr);
	}

	result.SpecularBRDF = mSpecularBRDF;

	return result;
}
