#include "D3D11RenderManager.h"

#include "ManagementCommon.h"
#include "D3D11Common.h"
#include "D3D11Util.h"
#include "RenderJob.h"
#include "Mesh.h"
#include "Material.h"
#include "ForwardPipeline.h"
#include "DeferredPipeline.h"

namespace fq::graphics
{
	D3D11RenderManager::D3D11RenderManager()
		: mForwardPipeline(std::make_unique<ForwardPipeline>())
		, mDeferredPipeline(std::make_unique<DeferredPipeline>())
		, mPipelineType(EPipelineType::None)
	{
	}

	D3D11RenderManager::~D3D11RenderManager()
	{
		Finalize();
	}

	void D3D11RenderManager::Initialize(std::shared_ptr<D3D11Device> device,
		std::shared_ptr<D3D11JobManager> jobManager,
		std::shared_ptr<D3D11CameraManager> cameraManager,
		std::shared_ptr< D3D11LightManager> lightManager,
		std::shared_ptr<D3D11ResourceManager> resourceManager,
		std::shared_ptr<D3D11DebugDrawManager> debugDrawManager,
		std::shared_ptr<D3D11ParticleManager> particleManager,
		std::shared_ptr<D3D11ObjectManager> objectManager,
		std::shared_ptr<D3D11LightProbeManager> lightProbeManager,
		unsigned short width,
		unsigned short height,
		EPipelineType pipelineType)
	{
		Finalize();

		mPipelineType = pipelineType;

		switch (mPipelineType)
		{
		case fq::graphics::EPipelineType::Forward:
			mForwardPipeline->Initialize(device, jobManager, cameraManager, lightManager, resourceManager, debugDrawManager, particleManager, objectManager, lightProbeManager, width, height);
			break;
		case fq::graphics::EPipelineType::Deferred:
			mDeferredPipeline->Initialize(device, jobManager, cameraManager, lightManager, resourceManager, debugDrawManager, particleManager, objectManager, lightProbeManager, width, height);
			break;
		default:
			assert(false);
			break;
		}
	}

	void D3D11RenderManager::Finalize()
	{
		switch (mPipelineType)
		{
		case fq::graphics::EPipelineType::Forward:
			mForwardPipeline->Finalize();
			break;
		case fq::graphics::EPipelineType::Deferred:
			mDeferredPipeline->Finalize();
			break;
		case fq::graphics::EPipelineType::None:
			break;
		default:
			assert(false);
			break;
		}
	}

	void D3D11RenderManager::OnResize(unsigned short width, unsigned short height, unsigned short oriWidth, unsigned short oriHeight)
	{
		switch (mPipelineType)
		{
		case fq::graphics::EPipelineType::Forward:
			mForwardPipeline->OnResize(width, height, oriWidth, oriHeight);
			break;
		case fq::graphics::EPipelineType::Deferred:
			mDeferredPipeline->OnResize(width, height, oriWidth, oriHeight);
			break;
		default:
			assert(false);
			break;
		}
	}

	void D3D11RenderManager::BeginRender()
	{
		switch (mPipelineType)
		{
		case fq::graphics::EPipelineType::Forward:
			mForwardPipeline->BeginRender();
			break;
		case fq::graphics::EPipelineType::Deferred:
			mDeferredPipeline->BeginRender();
			break;
		default:
			assert(false);
			break;
		}
	}

	void D3D11RenderManager::Render()
	{
		switch (mPipelineType)
		{
		case fq::graphics::EPipelineType::Forward:
			mForwardPipeline->Render();
			break;
		case fq::graphics::EPipelineType::Deferred:
			mDeferredPipeline->Render();
			break;
		default:
			assert(false);
			break;
		}
	}

	void D3D11RenderManager::EndRender(bool bUseVSync)
	{
		switch (mPipelineType)
		{
		case fq::graphics::EPipelineType::Forward:
			mForwardPipeline->EndRender(bUseVSync);
			break;
		case fq::graphics::EPipelineType::Deferred:
			mDeferredPipeline->EndRender(bUseVSync);
			break;
		default:
			assert(false);
			break;
		}
	}

	ID3D11ShaderResourceView* D3D11RenderManager::GetBackBufferSRV() const
	{
		switch (mPipelineType)
		{
		case fq::graphics::EPipelineType::Forward:
			return mForwardPipeline->GetBackBufferSRV()->GetSRV().Get();
			break;
		case fq::graphics::EPipelineType::Deferred:
			return mDeferredPipeline->GetBackBufferSRV()->GetSRV().Get();
			break;
		default:
			assert(false);
			break;
		}

		return nullptr;
	}

	void D3D11RenderManager::RenderFullScreen()
	{
		switch (mPipelineType)
		{
		case fq::graphics::EPipelineType::Forward:
			return mForwardPipeline->RenderFullScreen();
			break;
		case fq::graphics::EPipelineType::Deferred:
			return mDeferredPipeline->RenderFullScreen();
			break;
		default:
			assert(false);
			break;
		}
	}
}

