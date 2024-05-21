#pragma once

#include <directxtk/SimpleMath.h>
#include <vector>
#include <string>

#include "ConstantBufferStructure.h"
#include "RenderJob.h"

namespace fq::graphics
{
	class D3D11Device;

	class D3D11InputLayout;
	class D3D11VertexShader;
	class D3D11PixelShader;

	class D3D11RenderTargetView;
	class D3D11ShaderResourceView;
	class D3D11DepthStencilView;

	template <typename T>
	class D3D11ConstantBuffer;
	class D3D11VertexBuffer;
	class D3D11IndexBuffer;

	class D3D11ResourceManager;
	class D3D11CameraManager;
	class D3D11LightManager;
	class D3D11JobManager;
	class D3D11DebugDrawManager;

	class D3D11RasterizerState;
	class D3D11GeometryShader;
	class D3D11BlendState;
	class D3D11DepthStencilState;
	class D3D11SamplerState;
	class D3D11Texture;

	class RenderPass
	{
	public:
		virtual void Finalize() = 0;
		virtual void OnResize(unsigned short width, unsigned short height) = 0;
		virtual void Render() = 0;
	};

	class SkyBoxPass : public RenderPass
	{
	public:
		SkyBoxPass();

		void Initialize(std::shared_ptr<D3D11Device> device,
			std::shared_ptr<D3D11CameraManager> cameraManager,
			std::shared_ptr<D3D11ResourceManager> resourceManager);
		void Finalize() override;
		void OnResize(unsigned short width, unsigned short height) override {}
		void Render() override;

		void SetSkyBox(const std::wstring& path);

	private:
		bool isSetSkyBox;

		std::shared_ptr<D3D11Device> mDevice;
		std::shared_ptr<D3D11CameraManager> mCameraManager;
		std::shared_ptr<D3D11ResourceManager> mResourceManager;

		std::shared_ptr<D3D11VertexShader> mSkyBoxVS;
		std::shared_ptr<D3D11PixelShader> mSkyboxPS;
		std::shared_ptr<D3D11InputLayout> mSkyBoxLayout;

		std::shared_ptr<D3D11VertexBuffer> mSkyBoxVB;
		std::shared_ptr<D3D11IndexBuffer> mSkyboxIB;

		std::shared_ptr<D3D11RenderTargetView> mDrawRTV;
		std::shared_ptr<D3D11DepthStencilView> mDrawDSV;
		std::shared_ptr<D3D11Texture> mSkyBoxTexture;

		std::shared_ptr<D3D11RasterizerState> mCullFrontRS;
		// Todo: 딱 맞는 샘플러 스테이트가 있는지 고민해보기
		std::shared_ptr<D3D11SamplerState> mDefaultSS;
		std::shared_ptr<D3D11DepthStencilState> mLessEqualDepthStencilDS;

		std::shared_ptr<D3D11ConstantBuffer<ViewRotationProjectionMatrix>> mViewProjectionMatrix;
	};

	class ShadowPass : public RenderPass
	{
	public:
		void Initialize(std::shared_ptr<D3D11Device> device,
			std::shared_ptr<D3D11JobManager> jobManager,
			std::shared_ptr<D3D11CameraManager> cameraManager,
			std::shared_ptr<D3D11ResourceManager> resourceManager,
			std::shared_ptr< D3D11LightManager> lightManager);
		void Finalize() override;
		void OnResize(unsigned short width, unsigned short height) override {}
		void Render() override;

		static std::vector<float> CalculateCascadeEnds(std::vector<float> ratios, float nearZ, float farZ);
		static std::vector<DirectX::SimpleMath::Matrix> CalculateCascadeShadowTransform(std::vector<float> cascadeEnds, DirectX::SimpleMath::Matrix cameraView, DirectX::SimpleMath::Matrix cameraProj, DirectX::SimpleMath::Vector3 lightDirection);

	public:
		enum { SHADOW_SIZE = 1024 };
		enum { Point_LIGHT_SHADOW_SIZE = 256 };

	private:
		std::shared_ptr<D3D11Device> mDevice;
		std::shared_ptr<D3D11JobManager> mJobManager;
		std::shared_ptr<D3D11CameraManager> mCameraManager;
		std::shared_ptr<D3D11ResourceManager> mResourceManager;
		std::shared_ptr< D3D11LightManager> mLightManager;

		D3D11_VIEWPORT mViewport;

		std::shared_ptr<D3D11DepthStencilView> mCascadeShadowDSV;
		std::shared_ptr<D3D11DepthStencilView> mPointLightShadowDSV;

		std::shared_ptr<D3D11VertexShader> mStaticMeshVS;
		std::shared_ptr<D3D11VertexShader> mSkinnedMeshVS;
		std::shared_ptr<D3D11GeometryShader> mShadowGS;
		std::shared_ptr<D3D11InputLayout> mStaticMeshLayout;
		std::shared_ptr<D3D11InputLayout> mSkinnedMeshLayout;

		std::shared_ptr<D3D11RasterizerState> mShadowRS;
		std::shared_ptr<D3D11RasterizerState> mDefaultRS;

		std::shared_ptr<D3D11ConstantBuffer<ModelTransform>> mModelTransformCB;
		std::shared_ptr<D3D11ConstantBuffer<SceneTrnasform>> mSceneTransformCB;
		std::shared_ptr<D3D11ConstantBuffer<BoneTransform>> mBoneTransformCB;
		std::shared_ptr<D3D11ConstantBuffer<DirectionalShadowTransform>> mDirectionalShadowTransformCB;
	};

	class FullScreenPass : public RenderPass
	{
	public:
		void Initialize(std::shared_ptr<D3D11Device> device,
			std::shared_ptr<D3D11ResourceManager> resourceManager,
			unsigned short width,
			unsigned short height);
		void Finalize() override;
		void OnResize(unsigned short width, unsigned short height) override;
		void Render() override;

	private:
		std::shared_ptr<D3D11Device> mDevice;
		std::shared_ptr<D3D11ResourceManager> mResourceManager;

		D3D11_VIEWPORT mViewport;

		std::shared_ptr<D3D11RenderTargetView> mSwapChainRTV;
		std::shared_ptr<D3D11RenderTargetView> mBackBufferRTV;
		std::shared_ptr<D3D11ShaderResourceView> mBackBufferSRV;
		std::shared_ptr<D3D11DepthStencilView> mNoneDSV;
		std::shared_ptr<D3D11DepthStencilView> mDSV;

		std::shared_ptr<D3D11InputLayout> mFullScreenLayout;
		std::shared_ptr<D3D11VertexShader> mFullScreenVS;
		std::shared_ptr<D3D11PixelShader> mFullScreenPS;
		std::shared_ptr<D3D11VertexBuffer> mFullScreenVB;
		std::shared_ptr<D3D11IndexBuffer> mFullScreenIB;
		std::shared_ptr<D3D11SamplerState> mPointClampSamplerState;
	};

	class TransparentRenderPass : public RenderPass
	{
	public:
		void Initialize(std::shared_ptr<D3D11Device> device,
			std::shared_ptr<D3D11JobManager> jobManager,
			std::shared_ptr<D3D11CameraManager> cameraManager,
			std::shared_ptr< D3D11LightManager> lightManager,
			std::shared_ptr<D3D11ResourceManager> resourceManager,
			unsigned short width,
			unsigned short height);
		void Finalize() override;
		void OnResize(unsigned short width, unsigned short height) override;
		void Render() override;

	private:
		std::shared_ptr<D3D11Device> mDevice;
		std::shared_ptr<D3D11JobManager> mJobManager;
		std::shared_ptr<D3D11CameraManager> mCameraManager;
		std::shared_ptr< D3D11LightManager> mLightManager;
		std::shared_ptr<D3D11ResourceManager> mResourceManager;

		D3D11_VIEWPORT mViewport;

		std::shared_ptr<D3D11RenderTargetView> mColoraccumulationRTV;
		std::shared_ptr<D3D11RenderTargetView> mPixelRevealageThresholdRTV;
		std::shared_ptr<D3D11DepthStencilView> mDefaultDSV;
		std::shared_ptr<D3D11ShaderResourceView> mShadowSRV;

		// to do : 쉐이더 더 효율적으로 관리하는 기법이 필요함
		std::shared_ptr<D3D11InputLayout> mStaticMeshLayout;
		std::shared_ptr<D3D11InputLayout> mSkinnedMeshLayout;
		std::shared_ptr<D3D11VertexShader> mStaticMeshVS;
		std::shared_ptr<D3D11VertexShader> mSkinnedMeshVS;
		std::shared_ptr<D3D11PixelShader> mTransparentRenderPS;

		std::shared_ptr<D3D11SamplerState> mAnisotropicWrapSamplerState;
		std::shared_ptr<D3D11SamplerState> mShadowSampler;
		std::shared_ptr<D3D11SamplerState> mLinearSampler;
		std::shared_ptr<D3D11BlendState> mOITRenderState;
		std::shared_ptr<D3D11DepthStencilState> mDisableDepthWriteState;

		// to do : 상수 버퍼 더 효율적으로 관리하는 기법이 필요함
		std::shared_ptr<D3D11ConstantBuffer<ModelTransform>> mModelTransformCB;
		std::shared_ptr<D3D11ConstantBuffer<SceneTrnasform>> mSceneTransformCB;
		std::shared_ptr<D3D11ConstantBuffer<BoneTransform>> mBoneTransformCB;
		std::shared_ptr<D3D11ConstantBuffer<ModelTexutre>> mModelTexutreCB;
		std::shared_ptr<D3D11ConstantBuffer<DirectionalShadowInfo>> mDirectioanlShadowInfoCB;
		std::shared_ptr<D3D11ConstantBuffer<AlphaData>> mAlphaDataCB;
	};

	class TransparentCompositePass : public RenderPass
	{
	public:
		void Initialize(std::shared_ptr<D3D11Device> device,
			std::shared_ptr<D3D11ResourceManager> resourceManager,
			unsigned short width,
			unsigned short height);
		void Finalize() override;
		void OnResize(unsigned short width, unsigned short height) override;
		void Render() override;

	private:
		std::shared_ptr<D3D11Device> mDevice;
		std::shared_ptr<D3D11ResourceManager> mResourceManager;

		D3D11_VIEWPORT mViewport;

		std::shared_ptr<D3D11RenderTargetView> mBackBufferRTV;
		std::shared_ptr<D3D11DepthStencilView> mNullDSV;
		std::shared_ptr<D3D11ShaderResourceView> mColoraccumulationSRV;
		std::shared_ptr<D3D11ShaderResourceView> mPixelRevealageThresholdSRV;

		std::shared_ptr<D3D11BlendState> mOITCompositeState;

		std::shared_ptr<D3D11InputLayout> mFullScreenLayout;
		std::shared_ptr<D3D11VertexShader> mFullScreenVS;
		std::shared_ptr<D3D11PixelShader> mTransparentCompositePS;
		std::shared_ptr<D3D11VertexBuffer> mFullScreenVB;
		std::shared_ptr<D3D11IndexBuffer> mFullScreenIB;
		std::shared_ptr<D3D11SamplerState> mPointClampSamplerState;
	};

	class DebugRenderPass : public RenderPass
	{
	public:
		void Initialize(std::shared_ptr<D3D11Device> device,
			std::shared_ptr<D3D11JobManager> jobManager,
			std::shared_ptr<D3D11DebugDrawManager> dbugDrawManager,
			std::shared_ptr<D3D11CameraManager> cameraManager,
			std::shared_ptr<D3D11ResourceManager> resourceManager,
			unsigned short width,
			unsigned short height);
		void Finalize() override;
		void OnResize(unsigned short width, unsigned short height) override;
		void Render() override;

	private:
		std::shared_ptr<D3D11Device> mDevice;
		std::shared_ptr<D3D11JobManager> mJobManager;
		std::shared_ptr<D3D11DebugDrawManager> mDebugDrawManager;
		std::shared_ptr<D3D11CameraManager> mCameraManager;
		std::shared_ptr<D3D11ResourceManager> mResourceManager;
		
		D3D11_VIEWPORT mViewport;

		std::shared_ptr<D3D11RenderTargetView> mBackBufferRTV;
		std::shared_ptr<D3D11DepthStencilView> mDefaultDSV;
	};

	class TerrainPass : public RenderPass
	{
	public:
		TerrainPass();

		// 일단 텍스처를 섞어서 띄워 주기 위해 해야하는 것
		// 1. 플레인 위치를 받아서 1개 그리고 
		// 2. 그 위에 텍스처를 입힌다. 
		// 3. 텍스처는 알파 맵, BaseColor만 적용 해보자. 그리고 normalMap, pbr 넣기
		// 4. 쉐이더를 하나 간단하게 만들자

		// 오브젝트 매니저에 터레인을 추가해야할 것만 같다 
		void Initialize(std::shared_ptr<D3D11Device> device,
			std::shared_ptr<D3D11JobManager> jobManager,
			std::shared_ptr<D3D11CameraManager> cameraManager,
			std::shared_ptr<D3D11ResourceManager> resourceManager,
			std::shared_ptr< D3D11LightManager> lightManager);
		void Finalize() override;
		void OnResize(unsigned short width, unsigned short height) override {}
		void Render() override;

	private:
		std::shared_ptr<D3D11Device> mDevice;
		std::shared_ptr<D3D11JobManager> mJobManager;
		std::shared_ptr<D3D11CameraManager> mCameraManager;
		std::shared_ptr<D3D11ResourceManager> mResourceManager;
		std::shared_ptr<D3D11LightManager> mLightManager;

		std::shared_ptr<D3D11VertexShader> mTerrainVS;
		std::shared_ptr<D3D11PixelShader> mTerrainPS;
		std::shared_ptr<D3D11InputLayout> mTerrainLayout;

		// 앞에거 다 그리고 나서 마지막에 지형ㅇ르 그려야 하나?
		std::shared_ptr<D3D11RenderTargetView> mDrawRTV;
		std::shared_ptr<D3D11DepthStencilView> mDrawDSV;

		std::shared_ptr<D3D11RasterizerState> mDefaultRS;
		std::shared_ptr<D3D11SamplerState> mAnisotropicWrapSS;
		std::shared_ptr<D3D11DepthStencilState> mDefaultDS;

		std::shared_ptr<D3D11ConstantBuffer<ModelTransform>> mModelTransformCB;
		std::shared_ptr<D3D11ConstantBuffer<SceneTrnasform>> mSceneTransformCB;
		std::shared_ptr<D3D11ConstantBuffer<TerrainTexture>> mTerrainTextureCB;
	};
}