#pragma once

#include "CommonHeader.h"
#include "ConstantBufferStructure.h"
#include "RenderJob.h"

namespace fq::graphics
{
	class D3D11CameraManager;
	class D3D11Device;
	class D3D11ResourceManager;
	class D3D11ShaderResourceView;
	class D3D11LightManager;
	class D3D11JobManager;
	class D3D11RenderTargetView;
	class D3D11DepthStencilView;

	class ForwardPipeline
	{
	public:
		ForwardPipeline();
		~ForwardPipeline() = default;

		void Initialize(std::shared_ptr<D3D11Device>& device,
			std::shared_ptr<D3D11JobManager>& jobManager,
			std::shared_ptr<D3D11CameraManager>& cameraManager,
			std::shared_ptr< D3D11LightManager>& lightManager,
			std::shared_ptr<D3D11ResourceManager>& resourceManager,
			unsigned short width,
			unsigned short height);
		void Finalize();

		void OnResize(unsigned short width, unsigned short height);

		void BeginRender();
		void Render();
		void EndRender();

		std::shared_ptr<D3D11ShaderResourceView>& GetBackBufferSRV();

	private:
		std::shared_ptr<D3D11Device> mDevice;
		std::shared_ptr<D3D11ResourceManager> mResourceManager;

		std::vector<std::shared_ptr<class RenderPass>> mPasses;

		std::shared_ptr<class ShadowPass> mShadowPass;
		std::shared_ptr<class ForwardRenderPass> mRenderPass;
		std::shared_ptr<class FullScreenPass> mFullScreenPass;

		std::shared_ptr<D3D11RenderTargetView> mSwapChainRTV;
		std::shared_ptr<D3D11RenderTargetView> mBackBufferRTV;
		std::shared_ptr<D3D11ShaderResourceView> mBackBufferSRV;
		std::shared_ptr<D3D11DepthStencilView> mDSV;
	};
}

