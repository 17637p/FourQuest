#include "ForwardPipeline.h"
#include "ManagementCommon.h"
#include "D3D11Common.h"
#include "PassCommon.h"

namespace fq::graphics
{
	ForwardPipeline::ForwardPipeline()
		: mShadowPass(std::make_shared<ShadowPass>())
		, mRenderPass(std::make_shared<ForwardRenderPass>())
		, mTransparentRenderPass(std::make_shared<TransparentRenderPass>())
		, mTransparentCompositePass(std::make_shared<TransparentCompositePass>())
		, mDebugRenderPass(std::make_shared<DebugRenderPass>())
		, mSkyBoxPass(std::make_shared<SkyBoxPass>())
		, mTerrainPass(std::make_shared<TerrainPass>())
		, mFullScreenPass(std::make_shared<FullScreenPass>())
		, mParticlePass(std::make_shared<ParticlePass>())
		, mSingleColorPass(std::make_shared<SingleColorPass>())
		, mOutLinePass(std::make_shared<OutLinePass>())
		, mOutLineBlurPass(std::make_shared<OutLineBlurPass>())
		, mOutLineAddPass(std::make_shared<OutLineAddPass>())
		, mLightProbePass(std::make_shared<LightProbePass>())
	{
	}

	void ForwardPipeline::Initialize(std::shared_ptr<D3D11Device>& device,
		std::shared_ptr<D3D11JobManager>& jobManager,
		std::shared_ptr<D3D11CameraManager>& cameraManager,
		std::shared_ptr< D3D11LightManager>& lightManager,
		std::shared_ptr<D3D11ResourceManager>& resourceManager,
		std::shared_ptr<D3D11DebugDrawManager> debugDrawManager,
		std::shared_ptr<D3D11ParticleManager> particleManager,
		std::shared_ptr<D3D11DecalManager> decalManager,
		unsigned short width,
		unsigned short height)
	{
		Finalize();

		RenderPipeline::Initialize(device, resourceManager, width, height);

		mShadowPass->Initialize(device, jobManager, cameraManager, resourceManager, lightManager);
		mRenderPass->Initialize(device, jobManager, cameraManager, lightManager, resourceManager, width, height);
		mDebugRenderPass->Initialize(device, jobManager, debugDrawManager, cameraManager, resourceManager, particleManager, width, height);
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
		mLightProbePass->Initialize(device, jobManager, cameraManager, resourceManager, width, height);

		// 삽입 순서가 처리되는 순서
		mPasses.push_back(mShadowPass);
		mPasses.push_back(mRenderPass);
		mPasses.push_back(mLightProbePass);
		mPasses.push_back(mDebugRenderPass);
		mPasses.push_back(mSkyBoxPass);
		mPasses.push_back(mTerrainPass);
		mPasses.push_back(mTransparentRenderPass);
		mPasses.push_back(mTransparentCompositePass);
		mPasses.push_back(mParticlePass);
		mPasses.push_back(mSingleColorPass);
		mPasses.push_back(mOutLinePass);
		mPasses.push_back(mOutLineBlurPass);
		mPasses.push_back(mOutLineAddPass);

		mFullScreenLastPass = mFullScreenPass;
	}

	void ForwardPipeline::SetSkyBox(const std::wstring& path)
	{
		mSkyBoxPass->SetSkyBox(path);
		mLightProbePass->SetLightProbe(L"./resource/example/texture/custom1.dds");
		//mLightProbePass->SetLightProbe(L"./resource/example/texture/custom1.dds");
	}
}