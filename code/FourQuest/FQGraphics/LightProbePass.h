#pragma once

#include "Pass.h"
#include "ConstantBufferStructure.h"

namespace fq::graphics
{
	class LightProbePass : public Pass
	{
	public:
		void Initialize(std::shared_ptr<D3D11Device> device,
			std::shared_ptr<D3D11JobManager> jobManager,
			std::shared_ptr<D3D11CameraManager> cameraManager,
			std::shared_ptr<D3D11ResourceManager> resourceManager,
			std::shared_ptr<D3D11LightProbeManager> lightProbeManager,
			unsigned short width,
			unsigned short height);
		void Finalize() override;
		void OnResize(unsigned short width, unsigned short height) override;
		void Render() override;

	private:
		std::shared_ptr<D3D11Device> mDevice;
		std::shared_ptr<D3D11JobManager> mJobManager;
		std::shared_ptr<D3D11CameraManager> mCameraManager;
		std::shared_ptr<D3D11ResourceManager> mResourceManager;
		std::shared_ptr<D3D11LightProbeManager> mLightProbeManager;

		std::shared_ptr<D3D11RenderTargetView> mLightProbeIrrRTV;
		std::shared_ptr<D3D11DepthStencilView> mDSV;

		std::shared_ptr<D3D11SamplerState> mDefaultSS;
		std::shared_ptr<D3D11DepthStencilState> mLessEqualDS;

		std::unique_ptr<class ShaderProgram> mLightProbePassStaticMeshPassShaderProgram;
		std::unique_ptr<class ShaderProgram> mLightProbePassSkinnedMeshPassShaderProgram;

		std::shared_ptr<D3D11ConstantBuffer<ModelTransform>> mModelTransformCB;
		std::shared_ptr<D3D11ConstantBuffer<SceneTrnasform>> mSceneTransformCB;
		std::shared_ptr<D3D11ConstantBuffer<BoneTransform>> mBoneTransformCB;
		std::shared_ptr<D3D11ConstantBuffer<LightProbeCB>> mLightProbeCB;

		std::shared_ptr<D3D11Texture> mProbeTexture;

		float* r;
		float* g;
		float* b;
	};
}

