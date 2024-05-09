#pragma once

#include "CommonHeader.h"
#include "CommonPass.h"

namespace fq::graphics
{
	class DeferredGeometryPass : public RenderPass
	{
	public:
		void Initialize(std::shared_ptr<D3D11Device> device,
			std::shared_ptr<D3D11JobManager> jobManager,
			std::shared_ptr<D3D11CameraManager> cameraManager,
			std::shared_ptr<D3D11ResourceManager> resourceManager,
			std::shared_ptr< D3D11LightManager> lightManager,
			unsigned short width,
			unsigned short height);
		void Finalize() override;
		void OnResize(unsigned short width, unsigned short height) override;
		void Render() override;

		inline void SetRenderTargets(std::shared_ptr<D3D11RenderTargetView> mAlbedoRTV,
			std::shared_ptr<D3D11RenderTargetView> mMetalnessRTV,
			std::shared_ptr<D3D11RenderTargetView> mRoughnessRTV,
			std::shared_ptr<D3D11RenderTargetView> mNormalRTV,
			std::shared_ptr<D3D11RenderTargetView> mEmissiveRTV,
			std::shared_ptr<D3D11RenderTargetView> mPositionRTV);

	private:
		std::shared_ptr<D3D11Device> mDevice;
		std::shared_ptr<D3D11JobManager> mJobManager;
		std::shared_ptr<D3D11CameraManager> mCameraManager;
		std::shared_ptr<D3D11ResourceManager> mResourceManager;
		std::shared_ptr<D3D11LightManager> mLightManager;

		D3D11_VIEWPORT mViewport;

		std::shared_ptr<D3D11DepthStencilView> mDSV;

		std::shared_ptr<D3D11RenderTargetView> mAlbedoRTV;
		std::shared_ptr<D3D11RenderTargetView> mMetalnessRTV;
		std::shared_ptr<D3D11RenderTargetView> mRoughnessRTV;
		std::shared_ptr<D3D11RenderTargetView> mNormalRTV;
		std::shared_ptr<D3D11RenderTargetView> mEmissiveRTV;
		std::shared_ptr<D3D11RenderTargetView> mPositionRTV;

		std::shared_ptr<D3D11InputLayout> mStaticMeshLayout;
		std::shared_ptr<D3D11InputLayout> mSkinnedMeshLayout;

		std::shared_ptr<D3D11VertexShader> mStaticMeshVS;
		std::shared_ptr<D3D11VertexShader> mSkinnedMeshVS;

		std::shared_ptr<D3D11PixelShader> mGeometryPS;

		std::shared_ptr<D3D11SamplerState> mAnisotropicWrapSamplerState;

		std::shared_ptr<D3D11ConstantBuffer<ModelTransform>> mModelTransformCB;
		std::shared_ptr<D3D11ConstantBuffer<SceneTrnasform>> mSceneTransformCB;
		std::shared_ptr<D3D11ConstantBuffer<BoneTransform>> mBoneTransformCB;
		std::shared_ptr<D3D11ConstantBuffer<ModelTexutre>> mModelTexutreCB;
	};

	void DeferredGeometryPass::SetRenderTargets(std::shared_ptr<D3D11RenderTargetView> albedoRTV,
		std::shared_ptr<D3D11RenderTargetView> metalnessRTV,
		std::shared_ptr<D3D11RenderTargetView> roughnessRTV,
		std::shared_ptr<D3D11RenderTargetView> normalRTV,
		std::shared_ptr<D3D11RenderTargetView> emissiveRTV,
		std::shared_ptr<D3D11RenderTargetView> positionRTV)
	{
		mAlbedoRTV = albedoRTV;
		mMetalnessRTV = metalnessRTV;
		mRoughnessRTV = roughnessRTV;
		mNormalRTV = normalRTV;
		mEmissiveRTV = emissiveRTV;
		mPositionRTV = positionRTV;
	}

	class DeferredShadingPass : public RenderPass
	{
	public:
		void Initialize(std::shared_ptr<D3D11Device> device,
			std::shared_ptr<D3D11ResourceManager> resourceManager,
			std::shared_ptr< D3D11LightManager> lightManager,
			std::shared_ptr<D3D11CameraManager> cameraManager,
			unsigned short width,
			unsigned short height);
		void Finalize() override;
		void OnResize(unsigned short width, unsigned short height) override;
		void Render() override;

		inline void SetShaderResourceViews(std::shared_ptr<D3D11ShaderResourceView> albedoSRV,
			std::shared_ptr<D3D11ShaderResourceView> metalnessSRV,
			std::shared_ptr<D3D11ShaderResourceView> roughnessSRV,
			std::shared_ptr<D3D11ShaderResourceView> normalSRV,
			std::shared_ptr<D3D11ShaderResourceView> emissiveSRV,
			std::shared_ptr<D3D11ShaderResourceView> positionSRV);

	private:
		std::shared_ptr<D3D11Device> mDevice;
		std::shared_ptr<D3D11ResourceManager> mResourceManager;
		std::shared_ptr< D3D11LightManager> mLightManager;
		std::shared_ptr<D3D11CameraManager> mCameraManager;

		D3D11_VIEWPORT mViewport;
		
		std::shared_ptr<D3D11ShaderResourceView> mShadowSRV;
		std::shared_ptr<D3D11DepthStencilView> mNullDSV;

		std::shared_ptr<D3D11ShaderResourceView> mAlbedoSRV;
		std::shared_ptr<D3D11ShaderResourceView> mMetalnessSRV;
		std::shared_ptr<D3D11ShaderResourceView> mRoughnessSRV;
		std::shared_ptr<D3D11ShaderResourceView> mNormalSRV;
		std::shared_ptr<D3D11ShaderResourceView> mEmissiveSRV;
		std::shared_ptr<D3D11ShaderResourceView> mPositionSRV;

		std::shared_ptr<D3D11RenderTargetView> mBackBufferRTV;

		std::shared_ptr<D3D11SamplerState> mAnisotropicWrapSamplerState;
		std::shared_ptr<D3D11SamplerState> mLinearClampSamplerState;
		std::shared_ptr<D3D11SamplerState> mPointClampSamplerState;
		std::shared_ptr<D3D11SamplerState> mShadowSampler;

		std::shared_ptr<D3D11InputLayout> mFullScreenLayout;
		std::shared_ptr<D3D11VertexShader> mFullScreenVS;
		std::shared_ptr<D3D11VertexBuffer> mFullScreenVB;
		std::shared_ptr<D3D11IndexBuffer> mFullScreenIB;
		std::shared_ptr<D3D11PixelShader> mShadingPS;

		std::shared_ptr<D3D11ConstantBuffer<DirectionalShadowInfo>> mDirectioanlShadowInfoCB;
	};

	void DeferredShadingPass::SetShaderResourceViews(std::shared_ptr<D3D11ShaderResourceView> albedoSRV,
		std::shared_ptr<D3D11ShaderResourceView> metalnessSRV,
		std::shared_ptr<D3D11ShaderResourceView> roughnessSRV,
		std::shared_ptr<D3D11ShaderResourceView> normalSRV,
		std::shared_ptr<D3D11ShaderResourceView> emissiveSRV,
		std::shared_ptr<D3D11ShaderResourceView> positionSRV)
	{
		mAlbedoSRV = albedoSRV;
		mMetalnessSRV = metalnessSRV;
		mRoughnessSRV = roughnessSRV;
		mNormalSRV = normalSRV;
		mEmissiveSRV = emissiveSRV;
		mPositionSRV = positionSRV;
	}
}
