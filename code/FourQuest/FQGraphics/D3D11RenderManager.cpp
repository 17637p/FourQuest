#include "D3D11RenderManager.h"

#include "D3D11CameraManager.h"
#include "D3D11ResourceManager.h"
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

	void D3D11RenderManager::Initialize(const std::shared_ptr<D3D11Device>& device,
		std::shared_ptr<class D3D11ResourceManager>& resourceManager,
		unsigned short width,
		unsigned short height,
		EPipelineType pipelineType)
	{
		Finalize();

		mPipelineType = pipelineType;
		mSwapChainRTV = resourceManager->Create<fq::graphics::D3D11RenderTargetView>(ED3D11RenderTargetViewType::Default, width, height);
		mNullDSV = resourceManager->Create<fq::graphics::D3D11DepthStencilView>(ED3D11DepthStencilViewType::None, width, height);

		mFullScreenVS = std::make_shared<D3D11VertexShader>(device, L"./resource/internal/shader/FullScreenVS.hlsl");
		mFullScreenLayout = std::make_shared<D3D11InputLayout>(device, mFullScreenVS->GetBlob().Get());
		mFullScreenPS = std::make_shared<D3D11PixelShader>(device, L"./resource/internal/shader/FullScreenPS.hlsl");

		mPointClampSamplerState = resourceManager->Create<D3D11SamplerState>(ED3D11SamplerState::PointClamp);

		createFullScreenBuffer(device);

		switch (mPipelineType)
		{
		case fq::graphics::EPipelineType::Forward:
			mForwardPipeline->Initialize(device, resourceManager, width, height);
			break;
		case fq::graphics::EPipelineType::Deferred:
			mDeferredPipeline->Initialize(device, resourceManager, width, height);
			mDeferredPipeline->SetFullScreenBindable(mFullScreenLayout, mFullScreenVS, mFullScreenVB, mFullScreenIB);
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

	void D3D11RenderManager::OnResize(const std::shared_ptr<D3D11Device>& device,
		std::shared_ptr<D3D11ResourceManager>& resourceManager,
		unsigned short width,
		unsigned short height)
	{
		mSwapChainRTV->Release();
		mNullDSV->Release();

		device->OnResize(width, height);
		mSwapChainRTV->OnResize(device, ED3D11RenderTargetViewType::Default, width, height);
		mNullDSV->OnResize(device, ED3D11DepthStencilViewType::None, width, height);

		switch (mPipelineType)
		{
		case fq::graphics::EPipelineType::Forward:
			mForwardPipeline->OnResize(device, resourceManager, width, height);
			break;
		case fq::graphics::EPipelineType::Deferred:
			mDeferredPipeline->OnResize(device, resourceManager, width, height);
			break;
		default:
			assert(false);
			break;
		}
	}

	void D3D11RenderManager::BeginRender(const std::shared_ptr<D3D11Device>& device, const std::shared_ptr<class D3D11CameraManager>& cameraManager)
	{
		switch (mPipelineType)
		{
		case fq::graphics::EPipelineType::Forward:
			mForwardPipeline->BeginRender(device, cameraManager);
			break;
		case fq::graphics::EPipelineType::Deferred:
			mDeferredPipeline->BeginRender(device, cameraManager);
			break;
		default:
			assert(false);
			break;
		}
	}

	void D3D11RenderManager::EndRender(const std::shared_ptr<D3D11Device>& device)
	{
		switch (mPipelineType)
		{
		case fq::graphics::EPipelineType::Forward:
			mForwardPipeline->EndRender(device);
			break;
		case fq::graphics::EPipelineType::Deferred:
			mDeferredPipeline->EndRender(device);
			break;
		default:
			assert(false);
			break;
		}
	}

	void D3D11RenderManager::Render(const std::shared_ptr<D3D11Device>& device, const std::vector<StaticMeshJob>& staticMeshJobs)
	{
		switch (mPipelineType)
		{
		case fq::graphics::EPipelineType::Forward:
			mForwardPipeline->Render(device, staticMeshJobs);
			break;
		case fq::graphics::EPipelineType::Deferred:
			mDeferredPipeline->Render(device, staticMeshJobs);
			break;
		default:
			assert(false);
			break;
		}
	}

	void D3D11RenderManager::Render(const std::shared_ptr<D3D11Device>& device, const std::vector<SkinnedMeshJob>& skinnedMeshJobs)
	{
		switch (mPipelineType)
		{
		case fq::graphics::EPipelineType::Forward:
			mForwardPipeline->Render(device, skinnedMeshJobs);
			break;
		case fq::graphics::EPipelineType::Deferred:
			mDeferredPipeline->Render(device, skinnedMeshJobs);
			break;
		default:
			assert(false);
			break;
		}
	}

	void D3D11RenderManager::RenderBackBuffer(const std::shared_ptr<D3D11Device>& device)
	{

		switch (mPipelineType)
		{
		case fq::graphics::EPipelineType::Forward:
			mSwapChainRTV->Bind(device, mNullDSV);
			mForwardPipeline->GetBackBufferSRV()->Bind(device, 0, ED3D11ShaderType::Pixelshader);
			break;
		case fq::graphics::EPipelineType::Deferred:
			mDeferredPipeline->Shading(device);
			mSwapChainRTV->Bind(device, mNullDSV);
			mDeferredPipeline->GetBackBufferSRV()->Bind(device, 0, ED3D11ShaderType::Pixelshader);
			break;
		default:
			assert(false);
			break;
		}

		mFullScreenVB->Bind(device);
		mFullScreenIB->Bind(device);

		mFullScreenLayout->Bind(device);
		mFullScreenVS->Bind(device);
		mFullScreenPS->Bind(device);

		mPointClampSamplerState->Bind(device, 0, ED3D11ShaderType::Pixelshader);

		device->GetDeviceContext()->DrawIndexed(6u, 0, 0);
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
	}

	void D3D11RenderManager::createFullScreenBuffer(const std::shared_ptr<D3D11Device>& device)
	{
		std::vector<DirectX::SimpleMath::Vector2> positions =
		{
			{ -1, 1 },
			{ 1, 1 },
			{ -1, -1 },
			{ 1, -1 }
		};

		std::vector<unsigned int> indices =
		{
			0,1,2,
			1,3,2
		};

		mFullScreenVB = std::make_shared<D3D11VertexBuffer>(device, positions);
		mFullScreenIB = std::make_shared<D3D11IndexBuffer>(device, indices);
	}
}