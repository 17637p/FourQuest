#pragma once

#include "Pass.h"
#include "ConstantBufferStructure.h"

namespace fq::graphics
{
	class OutLineBlurPass : public Pass
	{
	public:
		void Initialize(std::shared_ptr<D3D11Device> device,
			std::shared_ptr<D3D11ResourceManager> resourceManager,
			unsigned short width,
			unsigned short height);
		void Finalize() override;
		void OnResize(unsigned short width, unsigned short height) override {}
		void Render() override;

	private:
		std::shared_ptr<D3D11Device> mDevice;
		std::shared_ptr<D3D11ResourceManager> mResourceManager;

		std::shared_ptr<D3D11ShaderResourceView> mOutLineSRV;
		std::shared_ptr<D3D11ShaderResourceView> mOffScreenSRV;
		std::shared_ptr<D3D11RenderTargetView> mOutLineBlurRTV;
		std::shared_ptr<D3D11DepthStencilView> mNoneDSV;

		std::shared_ptr<class D3D11DepthStencilState> mLessEqualDS;

		std::unique_ptr<class ShaderProgram> mOutLineBlurPassShaderProgram;

		std::shared_ptr<D3D11VertexBuffer> mFullScreenVB;
		std::shared_ptr<D3D11IndexBuffer> mFullScreenIB;

		std::shared_ptr<D3D11SamplerState> mDefaultSS;

		// 나중에 블러 정도를 제어하고 싶어지면...
		//std::shared_ptr<D3D11ConstantBuffer<ModelTransform>> mModelTransformCB;
	};
}

