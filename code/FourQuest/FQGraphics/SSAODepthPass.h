#pragma once
#include "Pass.h"
#include "ConstantBufferStructure.h"

namespace fq::graphics
{
	class SSAODepthPass : public Pass
	{
	public:
		void Initialize(std::shared_ptr<D3D11Device> device,
			std::shared_ptr<D3D11JobManager> jobManager,
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
		std::shared_ptr<D3D11CameraManager> mCameraManager;
		std::shared_ptr<D3D11ResourceManager> mResourceManager;

		std::shared_ptr<D3D11RenderTargetView> mViewDepthRTV; // Position, Depth View 이지만 나중에 https://mynameismjp.wordpress.com/2010/09/05/position-from-depth-3/
		std::shared_ptr<D3D11DepthStencilView> mDSV;

		std::unique_ptr<class ShaderProgram> mSSAOViewDepthStaticMeshPassShaderProgram;
		std::unique_ptr<class ShaderProgram> mSSAOViewDepthskinnedMeshPassShaderProgram;

		std::shared_ptr<D3D11ConstantBuffer<ModelTransform>> mModelTransformCB;
		std::shared_ptr<D3D11ConstantBuffer<ViewProjectionMatrix>> mViewProjectionMatrix;
		std::shared_ptr<D3D11ConstantBuffer<BoneTransform>> mBoneTransformCB;
	};
}

