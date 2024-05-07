#pragma once

#include "CommonHeader.h"
#include "CommonPass.h"

namespace fq::graphics
{
	template <typename T>
	class D3D11ConstantBuffer;
	class D3D11InputLayout;
	class D3D11VertexShader;
	class D3D11PixelShader;
	class D3D11Device;
	class D3D11SamplerState;
	class D3D11ResourceManager;
	class D3D11RenderTargetView;
	class D3D11ShaderResourceView;
	class D3D11DepthStencilView;
	class D3D11VertexBuffer;
	class D3D11IndexBuffer;
	class D3D11CameraManager;
	class D3D11LightManager;
	class D3D11JobManager;
	class D3D11RasterizerState;
	class D3D11DebugDrawManager;

	class ForwardRenderPass : public RenderPass
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

		std::shared_ptr<D3D11RenderTargetView> mBackBufferRTV;
		std::shared_ptr<D3D11DepthStencilView> mDSV;
		std::shared_ptr<D3D11ShaderResourceView> mShadowSRV;
		std::shared_ptr<D3D11ShaderResourceView> mPointLightShadowSRV;

		std::shared_ptr<D3D11InputLayout> mStaticMeshLayout;
		std::shared_ptr<D3D11InputLayout> mSkinnedMeshLayout;
		std::shared_ptr<D3D11VertexShader> mStaticMeshVS;
		std::shared_ptr<D3D11VertexShader> mSkinnedMeshVS;
		std::shared_ptr<D3D11PixelShader> mMeshPS;

		std::shared_ptr<D3D11RasterizerState> mDefaultRS;
		std::shared_ptr<D3D11SamplerState> mAnisotropicWrapSamplerState;
		std::shared_ptr<D3D11SamplerState> mLinearClampSamplerState;
		std::shared_ptr<D3D11SamplerState> mPointClampSamplerState;
		std::shared_ptr<D3D11SamplerState> mShadowSampler;

		std::shared_ptr<D3D11ConstantBuffer<ModelTransform>> mModelTransformCB;
		std::shared_ptr<D3D11ConstantBuffer<SceneTrnasform>> mSceneTransformCB;
		std::shared_ptr<D3D11ConstantBuffer<BoneTransform>> mBoneTransformCB;
		std::shared_ptr<D3D11ConstantBuffer<ShadowTransform>> mShadowViewProjTexCB;
		std::shared_ptr<D3D11ConstantBuffer<CascadeEnd>> mCascadeEndCB;

		std::shared_ptr<D3D11ConstantBuffer<ModelTexutre>> mModelTexutreCB;
	};
}