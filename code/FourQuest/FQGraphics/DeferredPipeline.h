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

	class DeferredPipeline
	{
	public:
		DeferredPipeline();
		~DeferredPipeline() = default;

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
		void RenderBackBuffer();
		void EndRender();

		void SetSkyBox(const std::wstring& path);
		std::shared_ptr<D3D11ShaderResourceView>& GetBackBufferSRV();

	private:
		std::shared_ptr<D3D11Device> mDevice;
		std::shared_ptr<D3D11ResourceManager> mResourceManager;

		std::vector<std::shared_ptr<class RenderPass>> mPasses;

		std::shared_ptr<class ShadowPass> mShadowPass;
		std::shared_ptr<class DeferredGeometryPass> mGeometryPass;
		std::shared_ptr<class DeferredShadingPass> mShadingPass;
		std::shared_ptr<class FullScreenPass> mFullScreenPass;
		std::shared_ptr<class SkyBoxPass> mSkyBoxPass;

		std::shared_ptr<D3D11RenderTargetView> mSwapChainRTV;
		std::shared_ptr<D3D11RenderTargetView> mBackBufferRTV;
		std::shared_ptr<D3D11ShaderResourceView> mBackBufferSRV;
		std::shared_ptr<D3D11DepthStencilView> mDSV;

		std::shared_ptr<D3D11RenderTargetView> mAlbedoRTV;
		std::shared_ptr<D3D11RenderTargetView> mMetalnessRTV;
		std::shared_ptr<D3D11RenderTargetView> mRoughnessRTV;
		std::shared_ptr<D3D11RenderTargetView> mNormalRTV;
		std::shared_ptr<D3D11RenderTargetView> mEmissiveRTV;
		std::shared_ptr<D3D11RenderTargetView> mPositionRTV;
		std::shared_ptr<D3D11RenderTargetView> mShadowRatioRTV;

		std::shared_ptr<D3D11ShaderResourceView> mAlbedoSRV;
		std::shared_ptr<D3D11ShaderResourceView> mMetalnessSRV;
		std::shared_ptr<D3D11ShaderResourceView> mRoughnessSRV;
		std::shared_ptr<D3D11ShaderResourceView> mNormalSRV;
		std::shared_ptr<D3D11ShaderResourceView> mEmissiveSRV;
		std::shared_ptr<D3D11ShaderResourceView> mPositionSRV;
		std::shared_ptr<D3D11ShaderResourceView> mShadowRatioSRV;
	};
}

