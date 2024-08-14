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
		, mTerrainPass(std::make_shared<TerrainPass>(EPipelineType::Deferred))
		, mFullScreenPass(std::make_shared<FullScreenPass>())
		, mSingleColorPass(std::make_shared<SingleColorPass>())
		, mOutLinePass(std::make_shared<OutLinePass>())
		, mOutLineBlurPass(std::make_shared<OutLineBlurPass>())
		, mOutLineAddPass(std::make_shared<OutLineAddPass>())
		, mParticlePass(std::make_shared<ParticlePass>())
		, mDecalPass(std::make_shared<DeferredDecalPass>())
		, mTrailRenderPass(std::make_shared<TrailRenderPass>())
		, mLightProbePass(std::make_shared<LightProbePass>())
		, mLightProbeAddPass(std::make_shared<LightProbeAddPass>())
		, mDebugLightProbePass(std::make_shared<DebugLightProbePass>())
	{
	}

	void DeferredPipeline::Initialize(std::shared_ptr<D3D11Device>& device,
		std::shared_ptr<D3D11JobManager>& jobManager,
		std::shared_ptr<D3D11CameraManager>& cameraManager,
		std::shared_ptr< D3D11LightManager>& lightManager,
		std::shared_ptr<D3D11ResourceManager>& resourceManager,
		std::shared_ptr<D3D11DebugDrawManager> debugDrawManager,
		std::shared_ptr<D3D11ParticleManager> particleManager,
		std::shared_ptr<D3D11ObjectManager> objectManager,
		std::shared_ptr<D3D11LightProbeManager> lightProbeManager,
		unsigned short width,
		unsigned short height)
	{
		Finalize();

		RenderPipeline::Initialize(device, resourceManager, width, height);

		mAlbedoRTV = mResourceManager->Create<D3D11RenderTargetView>(ED3D11RenderTargetViewType::Albedo, width, height);
		mMetalnessRoughnessRTV = mResourceManager->Create<D3D11RenderTargetView>(ED3D11RenderTargetViewType::MetalnessRoughness, width, height);
		mNormalRTV = mResourceManager->Create<D3D11RenderTargetView>(ED3D11RenderTargetViewType::Normal, width, height);
		mEmissiveRTV = mResourceManager->Create<D3D11RenderTargetView>(ED3D11RenderTargetViewType::Emissive, width, height);
		mPositionRTV = mResourceManager->Create<D3D11RenderTargetView>(ED3D11RenderTargetViewType::PositionWClipZ, width, height);
		mSourceNormalRTV = mResourceManager->Create<D3D11RenderTargetView>(ED3D11RenderTargetViewType::SourceNormal, width, height);
		mSourceTangentRTV = mResourceManager->Create<D3D11RenderTargetView>(ED3D11RenderTargetViewType::SourceTangent, width, height);
		mPreCalculatedLightRTV = mResourceManager->Create<D3D11RenderTargetView>(ED3D11RenderTargetViewType::PreCalculatedLight, width, height);

		mShadowPass->Initialize(device, jobManager, cameraManager, resourceManager, lightManager);
		mGeometryPass->Initialize(device, jobManager, cameraManager, resourceManager, lightManager, width, height);
		mShadingPass->Initialize(device, resourceManager, lightManager, cameraManager, width, height);
		mDebugRenderPass->Initialize(device, jobManager, objectManager, debugDrawManager, cameraManager, resourceManager, particleManager, width, height);
		mSkyBoxPass->Initialize(device, cameraManager, resourceManager, lightManager);
		mTransparentRenderPass->Initialize(device, jobManager, cameraManager, lightManager, resourceManager, width, height);
		mTransparentCompositePass->Initialize(device, resourceManager, width, height);
		mTerrainPass->Initialize(device, jobManager, cameraManager, resourceManager, lightManager);
		mSingleColorPass->Initialize(device, jobManager, cameraManager, resourceManager, width, height);
		mOutLinePass->Initialize(device, cameraManager, resourceManager, width, height);
		mOutLineBlurPass->Initialize(device, resourceManager, width, height);
		mOutLineAddPass->Initialize(device, resourceManager);
		mFullScreenPass->Initialize(device, resourceManager, width, height);
		mParticlePass->Initialize(device, particleManager, objectManager, cameraManager, resourceManager, lightManager, width, height);
		mDecalPass->Initialize(device, resourceManager, cameraManager, objectManager, debugDrawManager, width, height);
		mTrailRenderPass->Initialize(device, objectManager, cameraManager, resourceManager, lightManager, width, height);
		mLightProbePass->Initialize(device, jobManager, cameraManager, resourceManager, lightProbeManager, width, height);
		mLightProbeAddPass->Initialize(device, resourceManager, width, height);
		mDebugLightProbePass->Initialize(device, objectManager, cameraManager, resourceManager, lightProbeManager, width, height);

		// 삽입 순서가 처리되는 순서
		mPasses.push_back(mShadowPass);
		mPasses.push_back(mGeometryPass);
		mPasses.push_back(mTerrainPass);
		mPasses.push_back(mDecalPass);
		mPasses.push_back(mShadingPass);
		mPasses.push_back(mDebugRenderPass);
		mPasses.push_back(mSkyBoxPass);
		mPasses.push_back(mLightProbePass);
		mPasses.push_back(mLightProbeAddPass);
		mPasses.push_back(mOutLineAddPass);
		mPasses.push_back(mTransparentRenderPass);
		mPasses.push_back(mTransparentCompositePass);
		mPasses.push_back(mTrailRenderPass);
		mPasses.push_back(mParticlePass);
		mPasses.push_back(mSingleColorPass);
		mPasses.push_back(mOutLinePass);
		mPasses.push_back(mOutLineBlurPass);
		mPasses.push_back(mOutLineAddPass);
		mPasses.push_back(mDebugLightProbePass);

		mFullScreenLastPass = mFullScreenPass;
		//mPasses.push_back(mFullScreenPass);
	}

	void DeferredPipeline::Finalize()
	{
		RenderPipeline::Finalize();

		mAlbedoRTV = nullptr;
		mMetalnessRoughnessRTV = nullptr;
		mNormalRTV = nullptr;
		mEmissiveRTV = nullptr;
		mPositionRTV = nullptr;
		mSourceNormalRTV = nullptr;
		mSourceTangentRTV = nullptr;
		mPreCalculatedLightRTV = nullptr;
	}

	void DeferredPipeline::OnResize(unsigned short width, unsigned short height, unsigned short oriWidth, unsigned short oriHeight)
	{
		mAlbedoRTV->OnResize(mDevice, ED3D11RenderTargetViewType::Albedo, width, height);
		mMetalnessRoughnessRTV->OnResize(mDevice, ED3D11RenderTargetViewType::MetalnessRoughness, width, height);
		mNormalRTV->OnResize(mDevice, ED3D11RenderTargetViewType::Normal, width, height);
		mEmissiveRTV->OnResize(mDevice, ED3D11RenderTargetViewType::Emissive, width, height);
		mPositionRTV->OnResize(mDevice, ED3D11RenderTargetViewType::PositionWClipZ, width, height);
		mSourceNormalRTV->OnResize(mDevice, ED3D11RenderTargetViewType::PositionWClipZ, width, height);
		mSourceTangentRTV->OnResize(mDevice, ED3D11RenderTargetViewType::PositionWClipZ, width, height);
		mPreCalculatedLightRTV->OnResize(mDevice, ED3D11RenderTargetViewType::PreCalculatedLight, width, height);

		RenderPipeline::OnResize(width, height, oriWidth, oriHeight);
	}
}