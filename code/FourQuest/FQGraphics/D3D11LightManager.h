#pragma once

#include <unordered_map>
#include <memory>
#include <vector>
#include <cassert>

#include "Light.h"
#include "D3D11Buffer.h"
#include "ConstantBufferStructure.h"

namespace fq::common { struct Transform; }

namespace fq::graphics
{
	struct LightInfo;
	class D3D11Texture;
	class D3D11CubeTexture;
	class D3D11TextureArray;
	template <typename T>
	class D3D11ConstantBuffer;
	class D3D11CameraManager;

	struct IBLTexture
	{
		std::shared_ptr<D3D11CubeTexture> DiffuseIrradiance;
		std::shared_ptr<D3D11CubeTexture> SpecularIBL;
		std::shared_ptr<D3D11Texture> SpecularBRDF;
	};

	enum class EEnvironmentFormat
	{
		RGBA16,
		RGBA32,
	};
	enum class EEnvironmentResoulution
	{
		Size32x32,
		Size64x64,
		Size128x128,
		Size256x256,
		Size512x512,
		Size1024x1024,
	};

	class D3D11LightManager
	{
	public:
		D3D11LightManager();

		void Initialize(const std::shared_ptr<D3D11Device>& d3d11Device);

		void AddLight(const unsigned int id, const LightInfo& lightInfo);
		void SetLight(const unsigned int id, const LightInfo& lightInfo);
		void DeleteLight(const unsigned int id);

		void UseShadow(const unsigned int id, bool bUseShadow);

		void UpdateConstantBuffer(const std::shared_ptr<D3D11Device>& d3d11Device,
			const DirectX::SimpleMath::Vector3& eyePosition,
			const unsigned int isUseIBL);
		void UpdateShadowConstantBuffer(const std::shared_ptr<D3D11Device>& d3d11Device,
			const std::shared_ptr<D3D11CameraManager>& cameraManager);

		inline std::shared_ptr<D3D11ConstantBuffer<LightData>> GetLightConstnatBuffer();
		inline std::shared_ptr<D3D11ConstantBuffer<DirectionalShadowInfo>> GetShadowConstnatBuffer();

		inline std::vector<std::shared_ptr<Light<DirectionalLight>>> GetDirectionalShadows() const;

		IBLTexture CreateIBLTexture(const std::shared_ptr<D3D11Device>& d3d11Device, std::shared_ptr<D3D11CubeTexture> cubemapTexture, EEnvironmentFormat envFormat = EEnvironmentFormat::RGBA32, EEnvironmentResoulution specularResolution = EEnvironmentResoulution::Size1024x1024, EEnvironmentResoulution diffuseResolution = EEnvironmentResoulution::Size32x32, float envScale = 1.f);
		void SetIBLTexture(IBLTexture IBLTexture) { mIBLTexture = IBLTexture; }
		const IBLTexture& GetIBLTexture() const { return mIBLTexture; }

		void SetSkybox(std::shared_ptr<D3D11CubeTexture> skyBox) { mSkyBox = skyBox; }
		const std::shared_ptr<D3D11CubeTexture>& GetSkybox() const { return mSkyBox; }

		void CreateLightMapTextureArray(const std::shared_ptr<D3D11Device>& d3d11Device, const std::vector<std::filesystem::path>& paths);
		const std::shared_ptr<D3D11TextureArray>& GetLightMapTextureArray() const { return mLightMapTextureArray; }

		void CreateLightMapDirectionTextureArray(const std::shared_ptr<D3D11Device>& d3d11Device, const std::vector<std::filesystem::path>& paths);
		const std::shared_ptr<D3D11TextureArray>& GetLightMapDirectionTextureArray() const { return mLightMapDirectionTextureArray; }

		bool GetIsLightmapOnly() const { return mbUseLightmapOnly; }
		void SetIsLightmapOnly(bool bUseLightmapOnly) { mbUseLightmapOnly = bUseLightmapOnly; }

	private:
		std::vector<float> calculateCascadeEnds(std::vector<float> ratios, float nearZ, float farZ);
		std::vector<DirectX::SimpleMath::Matrix> calculateCascadeShadowTransform(std::vector<float> cascadeEnds, DirectX::SimpleMath::Matrix cameraView, DirectX::SimpleMath::Matrix cameraProj, DirectX::SimpleMath::Vector3 lightDirection);

	private:
		std::shared_ptr<D3D11ConstantBuffer<LightData>> mLightConstantBuffer;
		std::unordered_map<unsigned int, std::shared_ptr<Light<DirectionalLight>>>	mDirectionalLights;
		std::unordered_map<unsigned int, std::shared_ptr<Light<PointLight>>>		mPointLights;
		std::unordered_map<unsigned int, std::shared_ptr<Light<SpotLight>>>			mSpotLight;
		std::unordered_map<unsigned int, std::shared_ptr<Light<DirectionalLight>>> mDirectionalShadows;

		std::shared_ptr<class D3D11ComputeShader> mResizeCubemapCS;
		std::shared_ptr<class D3D11ComputeShader> mDiffuseIrradianceCS;
		std::shared_ptr<class D3D11ComputeShader> mSpecularIBLCS;
		std::shared_ptr<class D3D11ComputeShader> mSpecularBRDFCS;
		std::shared_ptr<class D3D11SamplerState> mLinearWrapSampler;
		std::shared_ptr<D3D11ConstantBuffer<SpecularMapFilterSettingsCB>> mSpecularMapFilterSettingsCB;

		IBLTexture mIBLTexture;
		std::shared_ptr<D3D11Texture> mSpecularBRDF;
		std::shared_ptr<D3D11CubeTexture> mSkyBox;
		std::shared_ptr<D3D11ConstantBuffer<DirectionalShadowInfo>> mDirectioanlShadowInfoCB;
		std::shared_ptr<D3D11TextureArray> mLightMapTextureArray;
		std::shared_ptr<D3D11TextureArray> mLightMapDirectionTextureArray;
		bool mbUseLightmapOnly;
	};

	inline std::shared_ptr<D3D11ConstantBuffer<LightData>> D3D11LightManager::GetLightConstnatBuffer()
	{
		return mLightConstantBuffer;
	}

	inline std::shared_ptr<D3D11ConstantBuffer<DirectionalShadowInfo>> D3D11LightManager::GetShadowConstnatBuffer()
	{
		return mDirectioanlShadowInfoCB;
	}

	inline std::vector<std::shared_ptr<Light<DirectionalLight>>> D3D11LightManager::GetDirectionalShadows() const
	{
		assert(mDirectionalShadows.size() <= DirectionalShadowInfo::MAX_SHADOW_COUNT);

		std::vector<std::shared_ptr<Light<DirectionalLight>>> lights;
		lights.reserve(mDirectionalShadows.size());

		for (auto [id, light] : mDirectionalShadows)
		{
			lights.push_back(light);
		}

		return lights;
	}
}

