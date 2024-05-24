#pragma once

#include "Pass.h"
#include "ConstantBufferStructure.h"

namespace fq::graphics
{
	class OutLineAddPass : public Pass
	{
	public:
		void Initialize(std::shared_ptr<D3D11Device> device,
			std::shared_ptr<D3D11ResourceManager> resourceManager);
		void Finalize() override;
		void OnResize(unsigned short width, unsigned short height) override {}
		void Render() override;

	private:
		std::shared_ptr<D3D11Device> mDevice;
		std::shared_ptr<D3D11ResourceManager> mResourceManager;

		std::shared_ptr<D3D11ShaderResourceView> mOutLineBlurSRV;

		std::shared_ptr<D3D11RenderTargetView> mOffScreenRTV;
		std::shared_ptr<D3D11DepthStencilView> mDSV;

		std::unique_ptr<class ShaderProgram> mOutLineAddShaderProgram;

		std::shared_ptr<D3D11VertexBuffer> mFullScreenVB;
		std::shared_ptr<D3D11IndexBuffer> mFullScreenIB;

		std::shared_ptr<D3D11SamplerState> mDefaultSS;
	};
}
