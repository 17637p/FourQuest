#pragma once

#include "CommonHeader.h"
#include "ConstantBufferStructure.h"
#include "RenderJob.h"
#include "RenderPipeline.h"

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
	class D3D11DebugDrawManager;
	class D3D11ParticleManager;

	class ForwardPipeline : public RenderPipeline
	{
	public:
		ForwardPipeline();
		virtual ~ForwardPipeline() override = default;

		void Initialize(std::shared_ptr<D3D11Device>& device,
			std::shared_ptr<D3D11JobManager>& jobManager,
			std::shared_ptr<D3D11CameraManager>& cameraManager,
			std::shared_ptr< D3D11LightManager>& lightManager,
			std::shared_ptr<D3D11ResourceManager>& resourceManager,
			std::shared_ptr<D3D11DebugDrawManager> debugDrawManager,
			std::shared_ptr<D3D11ParticleManager> particleManager,
			unsigned short width,
			unsigned short height);

		void SetSkyBox(const std::wstring& path);

	private:
		std::shared_ptr<class ShadowPass> mShadowPass;
		std::shared_ptr<class ForwardRenderPass> mRenderPass;
		std::shared_ptr<class TransparentRenderPass> mTransparentRenderPass;
		std::shared_ptr<class TransparentCompositePass> mTransparentCompositePass;
		std::shared_ptr<class DebugRenderPass> mDebugRenderPass;
		std::shared_ptr<class SkyBoxPass> mSkyBoxPass;
		std::shared_ptr<class FullScreenPass> mFullScreenPass;
		std::shared_ptr<class TerrainPass> mTerrainPass;
		std::shared_ptr<class SingleColorPass> mSingleColorPass;
		std::shared_ptr<class OutLinePass> mOutLinePass;
		std::shared_ptr<class OutLineBlurPass> mOutLineBlurPass;
		std::shared_ptr<class OutLineAddPass> mOutLineAddPass;
		std::shared_ptr<class ParticlePass> mParticlePass;

		std::shared_ptr<D3D11RenderTargetView> mSwapChainRTV;
		std::shared_ptr<D3D11RenderTargetView> mBackBufferRTV;
		std::shared_ptr<D3D11ShaderResourceView> mBackBufferSRV;
		std::shared_ptr<D3D11DepthStencilView> mDSV;

		// IBL юс╫ц
		std::shared_ptr<class D3D11Texture> mDiffuseCubeMap;
		std::shared_ptr<class D3D11Texture> mSpecularCubeMap;
		std::shared_ptr<class D3D11Texture> mBRDFLUT;
	};
}

