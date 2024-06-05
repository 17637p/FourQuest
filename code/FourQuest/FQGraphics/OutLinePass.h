#pragma once

#include "Pass.h"
#include "ConstantBufferStructure.h"

namespace fq::graphics
{
	class OutLinePass : public Pass
	{
	public:
		void Initialize(std::shared_ptr<D3D11Device> device,
			std::shared_ptr<D3D11CameraManager> cameraManager,
			std::shared_ptr<D3D11ResourceManager> resourceManager,
			unsigned short width,
			unsigned short height);
		void Finalize() override;
		void OnResize(unsigned short width, unsigned short height) override;
		void Render() override;

	private:
		ScreenSize mScreenSize;

		std::shared_ptr<D3D11Device> mDevice;
		std::shared_ptr<D3D11CameraManager> mCameraManager;
		std::shared_ptr<D3D11ResourceManager> mResourceManager;

		std::shared_ptr<D3D11ShaderResourceView> mSingleColorSRV;
		std::shared_ptr<D3D11RenderTargetView> mOutlineRTV;

		std::unique_ptr<class ShaderProgram> mOutLinePassShaderProgram;
		std::shared_ptr<D3D11DepthStencilView> mNoneDSV;

		std::shared_ptr<D3D11VertexBuffer> mFullScreenVB;
		std::shared_ptr<D3D11IndexBuffer> mFullScreenIB;

		std::shared_ptr<D3D11SamplerState> mDefaultSS;

		std::shared_ptr<D3D11ConstantBuffer<ScreenSize>> mScreenSizeCB;
	};
}

/*
하고 싶은 것

함수로
SRV와 RTV를 넣으면

SRV로 넣은 텍스처가 RTV에 블러 되어서 그려지기

void Blur(std::shared_ptr<D3D11ShaderResourceView>& srv, std::shared_ptr<D3D11RenderTargetView>& rtv);
*/


