#pragma once

#include <memory>
#include <string>

#include "Pass.h"
#include "ConstantBufferStructure.h"

namespace fq::graphics
{
	class SkyBoxPass : public Pass
	{
	public:
		SkyBoxPass();

		void Initialize(std::shared_ptr<D3D11Device> device,
			std::shared_ptr<D3D11CameraManager> cameraManager,
			std::shared_ptr<D3D11ResourceManager> resourceManager,
			std::shared_ptr<D3D11LightManager> lightManager);
		void Finalize() override;
		void OnResize(unsigned short width, unsigned short height) override {}
		void Render() override;

	private:
		bool isSetSkyBox;

		std::shared_ptr<D3D11Device> mDevice;
		std::shared_ptr<D3D11CameraManager> mCameraManager;
		std::shared_ptr<D3D11ResourceManager> mResourceManager;
		std::shared_ptr<D3D11LightManager> mLightManager;

		// 바인딩 대상 shaderProgrma으로 통합
		std::unique_ptr<class ShaderProgram> mSkyBoxShaderProgram;
		// std::shared_ptr<D3D11VertexShader> mSkyBoxVS;
		// std::shared_ptr<D3D11PixelShader> mSkyboxPS;
		// std::shared_ptr<D3D11InputLayout> mSkyBoxLayout;
		//std::shared_ptr<D3D11RasterizerState> mCullFrontRS;
		//std::shared_ptr<D3D11DepthStencilState> mLessEqualDepthStencilDS;

		std::shared_ptr<D3D11VertexBuffer> mSkyBoxVB;
		std::shared_ptr<D3D11IndexBuffer> mSkyboxIB;

		std::shared_ptr<D3D11RenderTargetView> mDrawRTV;
		std::shared_ptr<D3D11DepthStencilView> mDrawDSV;
		std::shared_ptr<D3D11CubeTexture> mSkyBoxTexture;


		//// Todo: 딱 맞는 샘플러 스테이트가 있는지 고민해보기
		std::shared_ptr<D3D11SamplerState> mDefaultSS;

		std::shared_ptr<D3D11ConstantBuffer<ViewRotationProjectionMatrix>> mViewProjectionMatrix;
	};

}