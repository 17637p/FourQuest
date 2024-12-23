#pragma once

#include <memory>
#include <vector>
#include <d3d11.h>
#include <directxtk/SimpleMath.h>
#include "Pass.h"
#include "ConstantBufferStructure.h"

namespace fq::graphics
{
	class ShadowPass : public Pass
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

		std::unique_ptr<class ShaderProgram> mStaticMeshShaderProgram;
		std::unique_ptr<class ShaderProgram> mSkinnedMeshShaderProgram;

		std::shared_ptr<D3D11ConstantBuffer<ModelTransform>> mModelTransformCB;
		std::shared_ptr<D3D11ConstantBuffer<SceneTrnasform>> mSceneTransformCB;
		std::shared_ptr<D3D11ConstantBuffer<BoneTransform>> mBoneTransformCB;

		struct cbLightCondition
		{
			int bIsStatic;
			float unused[3];
		};

		std::shared_ptr<D3D11ConstantBuffer<cbLightCondition>> mLightConditionCB;
	};
}