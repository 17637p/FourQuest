#pragma once

#include <memory>
#include <d3d11.h>
#include "Pass.h"
#include "ConstantBufferStructure.h"

namespace fq::graphics
{
	class TransparentRenderPass : public Pass
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

		// to do : ���̴� �� ȿ�������� �����ϴ� ����� �ʿ���
		std::shared_ptr<D3D11InputLayout> mStaticMeshLayout;
		std::shared_ptr<D3D11InputLayout> mSkinnedMeshLayout;
		std::shared_ptr<D3D11VertexShader> mStaticMeshVS;
		std::shared_ptr<D3D11VertexShader> mSkinnedMeshVS;
		std::shared_ptr<D3D11PixelShader> mTransparentRenderPS;

		std::shared_ptr<D3D11SamplerState> mAnisotropicWrapSamplerState;
		std::shared_ptr<D3D11SamplerState> mShadowSampler;
		std::shared_ptr<D3D11BlendState> mOITRenderState;
		std::shared_ptr<D3D11DepthStencilState> mDisableDepthWriteState;

		// to do : ��� ���� �� ȿ�������� �����ϴ� ����� �ʿ���
		std::shared_ptr<D3D11ConstantBuffer<ModelTransform>> mModelTransformCB;
		std::shared_ptr<D3D11ConstantBuffer<SceneTrnasform>> mSceneTransformCB;
		std::shared_ptr<D3D11ConstantBuffer<BoneTransform>> mBoneTransformCB;
		std::shared_ptr<D3D11ConstantBuffer<ModelTexutre>> mModelTexutreCB;
		std::shared_ptr<D3D11ConstantBuffer<DirectionalShadowInfo>> mDirectioanlShadowInfoCB;
		std::shared_ptr<D3D11ConstantBuffer<AlphaData>> mAlphaDataCB;
	};

}