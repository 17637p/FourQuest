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
			std::shared_ptr<D3D11ResourceManager> resourceManager);
		void Finalize() override;
		void OnResize(unsigned short width, unsigned short height) override {}
		void Render() override;

		void SetSkyBox(const std::wstring& path);

	private:
		bool isSetSkyBox;

		std::shared_ptr<D3D11Device> mDevice;
		std::shared_ptr<D3D11CameraManager> mCameraManager;
		std::shared_ptr<D3D11ResourceManager> mResourceManager;

		// ���ε� ��� shaderProgrma���� ����
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
		std::shared_ptr<D3D11Texture> mSkyBoxTexture;
		
		//// Todo: �� �´� ���÷� ������Ʈ�� �ִ��� ����غ���
		std::shared_ptr<D3D11SamplerState> mDefaultSS;
		
		std::shared_ptr<D3D11ConstantBuffer<ViewRotationProjectionMatrix>> mViewProjectionMatrix;
	};

}