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
		, mFullScreenPass(std::make_shared<FullScreenPass>())
	{
	}

	void DeferredPipeline::Initialize(std::shared_ptr<D3D11Device>& device,
		std::shared_ptr<D3D11JobManager>& jobManager,
		std::shared_ptr<D3D11CameraManager>& cameraManager,
		std::shared_ptr< D3D11LightManager>& lightManager,
		std::shared_ptr<D3D11ResourceManager>& resourceManager,
		std::shared_ptr<D3D11DebugDrawManager> debugDrawManager,
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
		mTransparentRenderPass->Initialize(device, jobManager, cameraManager, lightManager, resourceManager, width, height);
		mTransparentCompositePass->Initialize(device, resourceManager, width, height);
		mDebugRenderPass->Initialize(device, jobManager, debugDrawManager, cameraManager, resourceManager, width, height);
		mSkyBoxPass->Initialize(device, cameraManager, resourceManager);
		mFullScreenPass->Initialize(device, resourceManager, width, height);

		mPasses.push_back(mShadowPass);
		mPasses.push_back(mGeometryPass);
		mPasses.push_back(mShadingPass);
		mPasses.push_back(mTransparentRenderPass);
		mPasses.push_back(mTransparentCompositePass);
		mPasses.push_back(mDebugRenderPass);
		mPasses.push_back(mSkyBoxPass);
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

		RenderPipeline::OnResize(width, height);
	}

	void DeferredPipeline::SetSkyBox(const std::wstring& path)
	{
		mSkyBoxPass->SetSkyBox(path);
	}

}