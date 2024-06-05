#include "DeferredPipeline.h"
#include "ManagementCommon.h"
#include "D3D11Common.h"
#include "PassCommon.h"

namespace fq::graphics
{
	DeferredPipeline::DeferredPipeline()
		: mShadowPass(std::make_shared<ShadowPass>())
		, mGeometryPass(std::make_shared<DeferredGeometryPass>())
		, mShadingPass(std::make_shared<DeferredShadingPass>())
		, mTransparentRenderPass(std::make_shared<TransparentRenderPass>())
		, mTransparentCompositePass(std::make_shared<TransparentCompositePass>())
		, mDebugRenderPass(std::make_shared<DebugRenderPass>())
		, mSkyBoxPass(std::make_shared<SkyBoxPass>())
		, mTerrainPass(std::make_shared<TerrainPass>())
		, mFullScreenPass(std::make_shared<FullScreenPass>())
		, mSingleColorPass(std::make_shared<SingleColorPass>())
		, mOutLinePass(std::make_shared<OutLinePass>())
		, mOutLineBlurPass(std::make_shared<OutLineBlurPass>())
		, mOutLineAddPass(std::make_shared<OutLineAddPass>())
		, mParticlePass(std::make_shared<ParticlePass>())
	{
	}

	void DeferredPipeline::Initialize(std::shared_ptr<D3D11Device>& device,
		std::shared_ptr<D3D11JobManager>& jobManager,
		std::shared_ptr<D3D11CameraManager>& cameraManager,
		std::shared_ptr< D3D11LightManager>& lightManager,
		std::shared_ptr<D3D11ResourceManager>& resourceManager,
		std::shared_ptr<D3D11DebugDrawManager> debugDrawManager,
		std::shared_ptr<D3D11ParticleManager> particleManager,
		unsigned short width,
		unsigned short height)
	{
		Finalize();

		RenderPipeline::Initialize(device, resourceManager, width, height);

		mAlbedoRTV = mResourceManager->Create<D3D11RenderTargetView>(ED3D11RenderTargetViewType::Albedo, width, height);
		mMetalnessRTV = mResourceManager->Create<D3D11RenderTargetView>(ED3D11RenderTargetViewType::Metalness, width, height);
		mRoughnessRTV = mResourceManager->Create<D3D11RenderTargetView>(ED3D11RenderTargetViewType::Roughness, width, height);
		mNormalRTV = mResourceManager->Create<D3D11RenderTargetView>(ED3D11RenderTargetViewType::Normal, width, height);
		mEmissiveRTV = mResourceManager->Create<D3D11RenderTargetView>(ED3D11RenderTargetViewType::Emissive, width, height);
		mPositionRTV = mResourceManager->Create<D3D11RenderTargetView>(ED3D11RenderTargetViewType::PositionWClipZ, width, height);

		mShadowPass->Initialize(device, jobManager, cameraManager, resourceManager, lightManager);
		mGeometryPass->Initialize(device, jobManager, cameraManager, resourceManager, lightManager, width, height);
		mShadingPass->Initialize(device, resourceManager, lightManager, cameraManager, width, height);
		mDebugRenderPass->Initialize(device, jobManager, debugDrawManager, cameraManager, resourceManager, width, height);
		mSkyBoxPass->Initialize(device, cameraManager, resourceManager);
		mTransparentRenderPass->Initialize(device, jobManager, cameraManager, lightManager, resourceManager, width, height);
		mTransparentCompositePass->Initialize(device, resourceManager, width, height);
		mTerrainPass->Initialize(device, jobManager, cameraManager, resourceManager, lightManager);
		mSingleColorPass->Initialize(device, jobManager, cameraManager, resourceManager, width, height);
		mOutLinePass->Initialize(device, cameraManager, resourceManager, width, height);
		mOutLineBlurPass->Initialize(device, resourceManager, width, height);
		mOutLineAddPass->Initialize(device, resourceManager);
		mFullScreenPass->Initialize(device, resourceManager, width, height);
		mParticlePass->Initialize(device, particleManager, cameraManager, resourceManager, lightManager, width, height);

		mPasses.push_back(mShadowPass);
		mPasses.push_back(mGeometryPass);
		mPasses.push_back(mShadingPass);
		mPasses.push_back(mDebugRenderPass);
		mPasses.push_back(mSkyBoxPass);
		mPasses.push_back(mParticlePass);
		mPasses.push_back(mTerrainPass);
		mPasses.push_back(mTransparentRenderPass);
		mPasses.push_back(mTransparentCompositePass);
		mPasses.push_back(mSingleColorPass);
		mPasses.push_back(mOutLinePass);
		mPasses.push_back(mOutLineBlurPass);
		mPasses.push_back(mOutLineAddPass);
		mPasses.push_back(mFullScreenPass);
	}

	void DeferredPipeline::Finalize()
	{
		RenderPipeline::Finalize();

		mAlbedoRTV = nullptr;
		mMetalnessRTV = nullptr;
		mRoughnessRTV = nullptr;
		mNormalRTV = nullptr;
		mEmissiveRTV = nullptr;
		mPositionRTV = nullptr;
	}

	void DeferredPipeline::OnResize(unsigned short width, unsigned short height)
	{
		mAlbedoRTV->OnResize(mDevice, ED3D11RenderTargetViewType::Albedo, width, height);
		mMetalnessRTV->OnResize(mDevice, ED3D11RenderTargetViewType::Metalness, width, height);
		mRoughnessRTV->OnResize(mDevice, ED3D11RenderTargetViewType::Roughness, width, height);
		mNormalRTV->OnResize(mDevice, ED3D11RenderTargetViewType::Normal, width, height);
		mEmissiveRTV->OnResize(mDevice, ED3D11RenderTargetViewType::Emissive, width, height);
		mPositionRTV->OnResize(mDevice, ED3D11RenderTargetViewType::PositionWClipZ, width, height);

		//mAlbedoRTV->Release();
		//mMetalnessRTV->Release();
		//mRoughnessRTV->Release();
		//mNormalRTV->Release();
		//mEmissiveRTV->Release();
		//mPositionRTV->Release();
		//
		//mAlbedoRTV->OnResize(mDevice, ED3D11RenderTargetViewType::Offscreen, width, height);
		//mMetalnessRTV->OnResize(mDevice, ED3D11RenderTargetViewType::OffscreenGrayscale, width, height);
		//mRoughnessRTV->OnResize(mDevice, ED3D11RenderTargetViewType::OffscreenGrayscale, width, height);
		//mNormalRTV->OnResize(mDevice, ED3D11RenderTargetViewType::OffscreenHDR, width, height);
		//mEmissiveRTV->OnResize(mDevice, ED3D11RenderTargetViewType::Offscreen, width, height);
		//mPositionRTV->OnResize(mDevice, ED3D11RenderTargetViewType::OffscreenHDR, width, height);

		RenderPipeline::OnResize(width, height);
	}

	void DeferredPipeline::SetSkyBox(const std::wstring& path)
	{
		mSkyBoxPass->SetSkyBox(path);
	}
}