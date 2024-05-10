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

		std::shared_ptr<D3D11ConstantBuffer<cbModelTransform>> mModelTransformCB;
		std::shared_ptr<D3D11ConstantBuffer<cbSceneTransform>> mSceneTransformCB;
		std::shared_ptr<D3D11ConstantBuffer<cbBoneTransform>> mBoneTransformCB;
		std::shared_ptr<D3D11ConstantBuffer<cbShadowTransform>> mDirectionalShadowTransformCB;
	};
}