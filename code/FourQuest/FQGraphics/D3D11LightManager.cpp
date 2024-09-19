#include <cmath>
#include "D3D11LightManager.h"
#include "D3D11Texture.h"
#include "D3D11Shader.h"
#include "D3D11State.h"
#include "FQCommon.h"
#include "D3D11CameraManager.h"

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
	mDirectioanlShadowInfoCB = std::make_shared<D3D11ConstantBuffer<DirectionalShadowInfo>>(d3d11Device, ED3D11ConstantBuffer::Transform);
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
		directLight.lightMode = static_cast<unsigned int>(lightInfo.mode);

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
		pointLight.lightMode = static_cast<unsigned int>(lightInfo.mode);

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
		spotLight.lightMode = static_cast<unsigned int>(lightInfo.mode);

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
		if (mDirectionalShadows.size() < DirectionalShadowInfo::MAX_SHADOW_COUNT)
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
	for (const auto& [key, directionalLight] : mDirectionalShadows)
	{
		if (ARRAYSIZE(lightData.directionalLight) <= count)
		{
			break;
		}

		idSet.insert(key);

		lightData.directionalLight[count] = directionalLight->GetData();
		count++;
	}
	for (const auto& directionalLight : mDirectionalLights)
	{
		DirectionalLight directionalLightData = directionalLight.second->GetData();

		if (directionalLightData.lightMode == static_cast<unsigned int>(ELightMode::Baked))
		{
			continue;
		}
		if (ARRAYSIZE(lightData.directionalLight) <= count)
		{
			break;
		}

		auto find = idSet.find(directionalLight.first);

		if (find == idSet.end())
		{
			lightData.directionalLight[count] = directionalLightData;
			count++;
		}
	}
	lightData.numOfDirectionalLight = count;

	// Point Light
	count = 0;
	for (const auto& pointLight : mPointLights)
	{
		PointLight pointLightData = pointLight.second->GetData();

		if (pointLightData.lightMode == static_cast<unsigned int>(ELightMode::Baked))
		{
			continue;
		}
		if (ARRAYSIZE(lightData.pointLight) <= count)
		{
			break;
		}

		lightData.pointLight[count] = pointLightData;
		count++;
	}
	lightData.numOfPointLight = count;

	// Spot Light
	count = 0;
	for (const auto& spotLight : mSpotLight)
	{
		SpotLight spotLightData = spotLight.second->GetData();

		if (spotLightData.lightMode == static_cast<unsigned int>(ELightMode::Baked))
		{
			continue;
		}
		if (ARRAYSIZE(lightData.spotLight) <= count)
		{
			break;
		}

		lightData.spotLight[count] = spotLightData;
		count++;
	}
	lightData.numOfSpotLight = count;

	lightData.eyePosition = eyePosition;
	lightData.isUseIBL = mIBLTexture.DiffuseIrradiance != nullptr && mIBLTexture.SpecularBRDF != nullptr && mIBLTexture.SpecularIBL != nullptr;

	mLightConstantBuffer->Update(d3d11Device, lightData);
}

void fq::graphics::D3D11LightManager::UpdateShadowConstantBuffer(const std::shared_ptr<D3D11Device>& d3d11Device, const std::shared_ptr<D3D11CameraManager>& cameraManager)
{
	using namespace DirectX::SimpleMath;

	const std::vector<std::shared_ptr<Light<DirectionalLight>>>& directioanlShadows = GetDirectionalShadows();
	size_t currentDirectionaShadowCount = directioanlShadows.size();
	DirectionalShadowInfo directionalShadowData;
	directionalShadowData.ShadowCount = currentDirectionaShadowCount;

	if (currentDirectionaShadowCount > 0)
	{
		for (size_t i = 0; i < currentDirectionaShadowCount; ++i)
		{
			std::vector<float> cascadeEnds = calculateCascadeEnds({ 0.33, 0.5 },
				cameraManager->GetNearPlane(ECameraType::Player),
				cameraManager->GetFarPlane(ECameraType::Player));

			std::vector<DirectX::SimpleMath::Matrix> shadowTransforms = calculateCascadeShadowTransform(cascadeEnds,
				cameraManager->GetViewMatrix(ECameraType::Player),
				cameraManager->GetProjectionMatrix(ECameraType::Player),
				directioanlShadows[i]->GetData().direction);
			assert(shadowTransforms.size() == 3);

			auto cameraProj = cameraManager->GetProjectionMatrix(ECameraType::Player);
			size_t shaodwIndex = i * DirectionalShadowInfo::MAX_SHADOW_COUNT;

			directionalShadowData.ShadowViewProj[shaodwIndex] = (shadowTransforms[0]).Transpose();
			directionalShadowData.CascadeEnds[i].x = Vector4::Transform({ 0, 0, cascadeEnds[1], 1.f }, cameraProj).z;
			directionalShadowData.ShadowViewProj[shaodwIndex + 1] = (shadowTransforms[1]).Transpose();
			directionalShadowData.CascadeEnds[i].y = Vector4::Transform({ 0, 0, cascadeEnds[2], 1.f }, cameraProj).z;
			directionalShadowData.ShadowViewProj[shaodwIndex + 2] = (shadowTransforms[2]).Transpose();
			directionalShadowData.CascadeEnds[i].z = Vector4::Transform({ 0, 0, cascadeEnds[3], 1.f }, cameraProj).z;

			directionalShadowData.LightModes[i] = directioanlShadows[i]->GetData().lightMode;
		}
	}

	mDirectioanlShadowInfoCB->Update(d3d11Device, directionalShadowData);
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

std::vector<float> fq::graphics::D3D11LightManager::calculateCascadeEnds(std::vector<float> ratios, float nearZ, float farZ)
{
	std::vector<float> cascadeEnds;
	cascadeEnds.reserve(ratios.size() + 2);

	cascadeEnds.push_back(nearZ);

	float distanceZ = farZ - nearZ;

	for (float ratio : ratios)
	{
		cascadeEnds.push_back(ratio * distanceZ);
	}

	cascadeEnds.push_back(farZ);

	return cascadeEnds;
}

std::vector<DirectX::SimpleMath::Matrix> fq::graphics::D3D11LightManager::calculateCascadeShadowTransform(std::vector<float> cascadeEnds, DirectX::SimpleMath::Matrix cameraView, DirectX::SimpleMath::Matrix cameraProj, DirectX::SimpleMath::Vector3 lightDirection)
{
	using namespace DirectX::SimpleMath;

	const size_t CASCADE_COUNT = 3u;
	assert(cascadeEnds.size() >= 4);

	DirectX::BoundingFrustum frustum(cameraProj);

	float frustumDistnace = (frustum.Far - frustum.Near);
	Matrix viewInverse = cameraView.Invert();

	DirectX::SimpleMath::Vector3 forwardVec;
	cameraView.Forward(forwardVec);
	DirectX::SimpleMath::Vector3 adjustTranslate = forwardVec * frustumDistnace;

	std::array<std::array<Vector3, 8>, CASCADE_COUNT> sliceFrustums;

	for (size_t i = 0; i < sliceFrustums.size(); ++i)
	{
		std::array<Vector3, 8>& sliceFrustum = sliceFrustums[i];
		float curEnd = cascadeEnds[i];
		float nextEnd = cascadeEnds[i + 1];

		sliceFrustum[0] = Vector3::Transform({ frustum.RightSlope * curEnd, frustum.TopSlope * curEnd, curEnd }, viewInverse) + adjustTranslate;
		sliceFrustum[1] = Vector3::Transform({ frustum.RightSlope * curEnd, frustum.BottomSlope * curEnd, curEnd }, viewInverse) + adjustTranslate;
		sliceFrustum[2] = Vector3::Transform({ frustum.LeftSlope * curEnd, frustum.TopSlope * curEnd, curEnd }, viewInverse) + adjustTranslate;
		sliceFrustum[3] = Vector3::Transform({ frustum.LeftSlope * curEnd, frustum.BottomSlope * curEnd, curEnd }, viewInverse) + adjustTranslate;

		sliceFrustum[4] = Vector3::Transform({ frustum.RightSlope * nextEnd, frustum.TopSlope * nextEnd, nextEnd }, viewInverse) + adjustTranslate;
		sliceFrustum[5] = Vector3::Transform({ frustum.RightSlope * nextEnd, frustum.BottomSlope * nextEnd, nextEnd }, viewInverse) + adjustTranslate;
		sliceFrustum[6] = Vector3::Transform({ frustum.LeftSlope * nextEnd, frustum.TopSlope * nextEnd, nextEnd }, viewInverse) + adjustTranslate;
		sliceFrustum[7] = Vector3::Transform({ frustum.LeftSlope * nextEnd, frustum.BottomSlope * nextEnd, nextEnd }, viewInverse) + adjustTranslate;
	}

	std::vector<DirectX::SimpleMath::Matrix> result;
	result.reserve(CASCADE_COUNT);

	for (size_t i = 0; i < CASCADE_COUNT; ++i)
	{
		const std::array<Vector3, 8>& sliceFrustum = sliceFrustums[i];

		DirectX::SimpleMath::Vector3 centerPos = { 0.f, 0.f, 0.f };
		for (const DirectX::SimpleMath::Vector3& pos : sliceFrustum)
		{
			centerPos += pos;
		}
		centerPos /= 8.f;

		float radius = 0.f;
		for (const DirectX::SimpleMath::Vector3& pos : sliceFrustum)
		{
			float distance = DirectX::SimpleMath::Vector3::Distance(centerPos, pos);
			radius = std::max<float>(radius, distance);
		}

		radius = std::ceil(radius * 16.f) / 16.f;

		DirectX::SimpleMath::Vector3 maxExtents = { radius, radius, radius };
		DirectX::SimpleMath::Vector3 minExtents = -maxExtents;
		DirectX::SimpleMath::Vector3 shadowPos = centerPos + lightDirection * minExtents.z;

		DirectX::SimpleMath::Vector3 cascadeExtents = maxExtents - minExtents;
		DirectX::SimpleMath::Matrix lightView = DirectX::XMMatrixLookAtLH(shadowPos, centerPos, { 0, 1, 0 });
		DirectX::SimpleMath::Matrix lightProj = DirectX::XMMatrixOrthographicOffCenterLH(minExtents.x, maxExtents.x, minExtents.y, maxExtents.y, 0.f, cascadeExtents.z);

		result.push_back(lightView * lightProj);
	}

	return result;
}

void fq::graphics::D3D11LightManager::CreateLightMapTextureArray(const std::shared_ptr<D3D11Device>& d3d11Device, const std::vector<std::filesystem::path>& paths)
{
	if (paths.empty())
	{
		mLightMapTextureArray = nullptr;
		return;
	}

	mLightMapTextureArray = std::make_shared<D3D11TextureArray>(d3d11Device, paths);
}

void fq::graphics::D3D11LightManager::CreateLightMapDirectionTextureArray(const std::shared_ptr<D3D11Device>& d3d11Device, const std::vector<std::filesystem::path>& paths)
{
	if (paths.empty())
	{
		mLightMapDirectionTextureArray = nullptr;
		return;
	}

	mLightMapDirectionTextureArray = std::make_shared<D3D11TextureArray>(d3d11Device, paths);
}
