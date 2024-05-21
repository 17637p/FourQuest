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

		// 일단 텍스처를 섞어서 띄워 주기 위해 해야하는 것
		// 1. 플레인 위치를 받아서 1개 그리고 
		// 2. 그 위에 텍스처를 입힌다. 
		// 3. 텍스처는 알파 맵, BaseColor만 적용 해보자. 그리고 normalMap, pbr 넣기
		// 4. 쉐이더를 하나 간단하게 만들자

		// 오브젝트 매니저에 터레인을 추가해야할 것만 같다 
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
		std::shared_ptr<D3D11InputLayout> mTerrainLayout;

		std::shared_ptr<D3D11RenderTargetView> mDrawRTV;
		std::shared_ptr<D3D11DepthStencilView> mDrawDSV;
		std::shared_ptr<D3D11ShaderResourceView> mShadowSRV;

		std::shared_ptr<D3D11RasterizerState> mDefaultRS;
		std::shared_ptr<D3D11SamplerState> mAnisotropicWrapSS;
		std::shared_ptr<D3D11SamplerState> mShadowSS;
		std::shared_ptr<D3D11DepthStencilState> mDefaultDS;

		std::shared_ptr<D3D11ConstantBuffer<ModelTransform>> mModelTransformCB;
		std::shared_ptr<D3D11ConstantBuffer<SceneTrnasform>> mSceneTransformCB;
		std::shared_ptr<D3D11ConstantBuffer<TerrainTexture>> mTerrainTextureCB;
		std::shared_ptr<D3D11ConstantBuffer<DirectionalShadowInfo>> mDirectioanlShadowInfoCB;
	};
}
