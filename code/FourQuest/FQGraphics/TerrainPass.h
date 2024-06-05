#pragma once

#include <memory>
#include <string>

#include "Pass.h"
#include "ConstantBufferStructure.h"

namespace fq::graphics
{
	class TerrainPass : public Pass
	{
	public:
		TerrainPass();

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
		std::shared_ptr<class D3D11DomainShader> mTerrainDS;
		std::shared_ptr<class D3D11HullShader> mTerrainHS;
		std::shared_ptr<D3D11InputLayout> mTerrainLayout;

		std::shared_ptr<D3D11RenderTargetView> mDrawRTV;
		std::shared_ptr<D3D11DepthStencilView> mDrawDSV;
		std::shared_ptr<D3D11ShaderResourceView> mShadowSRV;

		std::shared_ptr<D3D11RasterizerState> mDefaultRS;
		std::shared_ptr<D3D11SamplerState> mAnisotropicWrapSS;
		std::shared_ptr<D3D11SamplerState> mPointClampSS;
		std::shared_ptr<D3D11SamplerState> mShadowSS;
		std::shared_ptr<D3D11DepthStencilState> mDefaultDS;

		std::shared_ptr<D3D11ConstantBuffer<ModelTransform>> mModelTransformCB;
		std::shared_ptr<D3D11ConstantBuffer<SceneTrnasform>> mSceneTransformCB;
		std::shared_ptr<D3D11ConstantBuffer<TerrainHull>> mTerrainHullCB;
		std::shared_ptr<D3D11ConstantBuffer<TerrainTexture>> mTerrainTextureCB;
		std::shared_ptr<D3D11ConstantBuffer<DirectionalShadowInfo>> mDirectioanlShadowInfoCB;
	};
}
