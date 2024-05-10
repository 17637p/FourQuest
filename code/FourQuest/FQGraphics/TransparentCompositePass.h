#pragma once

#include <memory>
#include <d3d11.h>

#include "Pass.h"

namespace fq::graphics
{
	class TransparentCompositePass : public Pass
	{
	public:
		void Initialize(std::shared_ptr<D3D11Device> device,
			std::shared_ptr<D3D11ResourceManager> resourceManager,
			unsigned short width,
			unsigned short height);
		void Finalize() override;
		void OnResize(unsigned short width, unsigned short height) override;
		void Render() override;

	private:
		std::shared_ptr<D3D11Device> mDevice;
		std::shared_ptr<D3D11ResourceManager> mResourceManager;

		D3D11_VIEWPORT mViewport;

		std::shared_ptr<D3D11RenderTargetView> mBackBufferRTV;
		std::shared_ptr<D3D11DepthStencilView> mNullDSV;
		std::shared_ptr<D3D11ShaderResourceView> mColoraccumulationSRV;
		std::shared_ptr<D3D11ShaderResourceView> mPixelRevealageThresholdSRV;

		std::unique_ptr<class ShaderProgram> mShaderProgram;
		std::shared_ptr<D3D11VertexBuffer> mFullScreenVB;
		std::shared_ptr<D3D11IndexBuffer> mFullScreenIB;
		std::shared_ptr<D3D11SamplerState> mPointClampSamplerState;
	};
}