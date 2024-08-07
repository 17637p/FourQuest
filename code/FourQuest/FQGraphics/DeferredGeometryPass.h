#pragma once

#include <memory>
#include <d3d11.h>
#include "Pass.h"
#include "ConstantBufferStructure.h"

namespace fq::graphics
{
	class ShaderProgram;

	class DeferredGeometryPass : public Pass
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

	private:
		std::shared_ptr<D3D11Device> mDevice;
		std::shared_ptr<D3D11JobManager> mJobManager;
		std::shared_ptr<D3D11CameraManager> mCameraManager;
		std::shared_ptr<D3D11ResourceManager> mResourceManager;
		std::shared_ptr<D3D11LightManager> mLightManager;

		D3D11_VIEWPORT mViewport;

		std::shared_ptr<D3D11DepthStencilView> mDSV;

		std::shared_ptr<D3D11RenderTargetView> mAlbedoRTV;
		std::shared_ptr<D3D11RenderTargetView> mMetalnessRoughnessRTV;
		std::shared_ptr<D3D11RenderTargetView> mNormalRTV;
		std::shared_ptr<D3D11RenderTargetView> mEmissiveRTV;
		std::shared_ptr<D3D11RenderTargetView> mPositionRTV;
		std::shared_ptr<D3D11RenderTargetView> mSourceNormalRTV;
		std::shared_ptr<D3D11RenderTargetView> mSourceTangentRTV;
		std::shared_ptr<D3D11RenderTargetView> mPreCalculatedLightRTV;

		std::shared_ptr<D3D11DepthStencilState> mLessEqualStencilReplaceState;

		std::unique_ptr<ShaderProgram> mStaticMeshShaderProgram;
		std::unique_ptr<ShaderProgram> mLightmapStaticMeshShaderProgram;
		std::unique_ptr<ShaderProgram> mVertexColorStaticMeshShaderProgram;
		std::unique_ptr<ShaderProgram> mSkinnedMeshShaderProgram;

		std::shared_ptr<D3D11SamplerState> mAnisotropicWrapSamplerState;
		std::shared_ptr<D3D11SamplerState> mAnisotropicClampSamplerState;
		std::shared_ptr<D3D11SamplerState> mPointWrapSamplerState;
		std::shared_ptr<D3D11SamplerState> mLinearWrapSamplerState;
		std::shared_ptr<D3D11RasterizerState> mDefaultRasterizer;
		std::shared_ptr<D3D11RasterizerState> mCullOffRasterizer;

		std::shared_ptr<D3D11ConstantBuffer<ModelTransform>> mModelTransformCB;
		std::shared_ptr<D3D11ConstantBuffer<LightMapInfomation>> mLightMapInformationCB;
		std::shared_ptr<D3D11ConstantBuffer<SceneTrnasform>> mSceneTransformCB;
		std::shared_ptr<D3D11ConstantBuffer<BoneTransform>> mBoneTransformCB;
		std::shared_ptr<D3D11ConstantBuffer<CBMaterial>> mMaterialCB;
	};
}