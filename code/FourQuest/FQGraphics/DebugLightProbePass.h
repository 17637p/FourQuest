#pragma once
#include "Pass.h"
#include "ConstantBufferStructure.h"

namespace fq::graphics
{
	class DebugLightProbePass : public Pass
	{
	public:
		void Initialize(std::shared_ptr<D3D11Device> device,
			std::shared_ptr<D3D11ObjectManager> objectManager,
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
		std::shared_ptr<D3D11ObjectManager> mObjectManager;
		std::shared_ptr<D3D11CameraManager> mCameraManager;
		std::shared_ptr<D3D11ResourceManager> mResourceManager;
		std::shared_ptr<D3D11LightProbeManager> mLightProbeManager;

		std::shared_ptr<D3D11DepthStencilState> mDefaultDS;

		std::shared_ptr<D3D11RenderTargetView> mBackBuffer;
		std::shared_ptr<D3D11DepthStencilView> mDSV;

		std::unique_ptr<class ShaderProgram> mDebugLightProbePassPassShaderProgram;

		std::shared_ptr<D3D11ConstantBuffer<ModelTransform>> mModelTransformCB;
		std::shared_ptr<D3D11ConstantBuffer<SceneTrnasform>> mSceneTransformCB;
		std::shared_ptr<D3D11ConstantBuffer<LightProbeCB>> mLightProbeCB;
	};
}

