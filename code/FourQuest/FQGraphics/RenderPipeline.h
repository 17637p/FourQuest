#pragma once

#include "CommonHeader.h"

namespace fq::graphics
{
	class D3D11Device;
	class D3D11ResourceManager;
	class D3D11ShaderResourceView;
	class D3D11RenderTargetView;
	class D3D11DepthStencilView;
	class Pass;
	class D3D11ParticleManager;
	class D3D11ObjectManager;
	class D3D11ObjectManager;

	class RenderPipeline
	{
	public:
		RenderPipeline() = default;
		virtual ~RenderPipeline() = 0 {}

		void Initialize(std::shared_ptr<D3D11Device>& device,
			std::shared_ptr<D3D11ResourceManager>& resourceManager,
			unsigned short width,
			unsigned short height);
		virtual	void Finalize();
		virtual void OnResize(unsigned short width, unsigned short height);

		virtual void BeginRender();
		virtual void Render();
		virtual void RenderFullScreen();
		virtual void EndRender();

		inline std::shared_ptr<D3D11ShaderResourceView>& GetBackBufferSRV();

	protected:
		std::shared_ptr<D3D11Device> mDevice;
		std::shared_ptr<D3D11ResourceManager> mResourceManager;
		std::shared_ptr<Pass> mFullScreenLastPass;
		std::vector<std::shared_ptr<Pass>> mPasses;

		std::shared_ptr<D3D11RenderTargetView> mSwapChainRTV;
		std::shared_ptr<D3D11RenderTargetView> mBackBufferRTV;
		std::shared_ptr<D3D11ShaderResourceView> mBackBufferSRV;
		std::shared_ptr<D3D11DepthStencilView> mDSV;
		std::shared_ptr<D3D11DepthStencilView> mNoneDSV;
	};

	inline std::shared_ptr<D3D11ShaderResourceView>& RenderPipeline::GetBackBufferSRV()
	{
		return mBackBufferSRV;
	}
}