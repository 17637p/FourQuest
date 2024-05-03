#include "ForwardPipeline.h"

#include "D3D11ResourceManager.h"
#include "D3D11CameraManager.h"
#include "D3D11LightManager.h"
#include "D3D11Common.h"
#include "RenderJob.h"
#include "Material.h"
#include "Mesh.h"
#include "Define.h"
#include "FowardPass.h"
#include "D3D11View.h"

namespace fq::graphics
{
	ForwardPipeline::ForwardPipeline()
		: mShadowPass(std::make_shared<ShadowPass>())
		, mRenderPass(std::make_shared<ForwardRenderPass>())
		, mFullScreenPass(std::make_shared<FullScreenPass>())
	{
	}

	void ForwardPipeline::Initialize(std::shared_ptr<D3D11Device>& device,
		std::shared_ptr<D3D11JobManager>& jobManager,
		std::shared_ptr<D3D11CameraManager>& cameraManager,
		std::shared_ptr< D3D11LightManager>& lightManager,
		std::shared_ptr<D3D11ResourceManager>& resourceManager,
		unsigned short width,
		unsigned short height)
	{
		Finalize();

		mDevice = device;
		mResourceManager = resourceManager;

		mShadowPass->Initialize(device, jobManager, cameraManager, resourceManager, lightManager);
		mRenderPass->Initialize(device, jobManager, cameraManager, lightManager, resourceManager, width, height);
		mFullScreenPass->Initialize(device, resourceManager, width, height);

		// 삽입 순서가 처리되는 순서
		mPasses.push_back(mShadowPass);
		mPasses.push_back(mRenderPass);

		mSwapChainRTV = mResourceManager->Create<D3D11RenderTargetView>(ED3D11RenderTargetViewType::Default, width, height);
		mBackBufferRTV = mResourceManager->Create<D3D11RenderTargetView>(ED3D11RenderTargetViewType::Offscreen, width, height);
		mDSV = mResourceManager->Create<D3D11DepthStencilView>(ED3D11DepthStencilViewType::Default, width, height);
		mBackBufferSRV = std::make_shared<D3D11ShaderResourceView>(mDevice, mBackBufferRTV);
	}
	void ForwardPipeline::Finalize()
	{
		for (std::shared_ptr<RenderPass> pass : mPasses)
		{
			pass->Finalize();
		}
		mPasses.clear();

		mDevice = nullptr;
		mResourceManager = nullptr;

		mSwapChainRTV = nullptr;
		mBackBufferRTV = nullptr;
		mBackBufferSRV = nullptr;
		mDSV = nullptr;

	}

	void ForwardPipeline::OnResize(unsigned short width, unsigned short height)
	{
		mSwapChainRTV->Release();
		mBackBufferRTV->Release();
		mDSV->Release();

		mDevice->OnResize(width, height);

		mSwapChainRTV->OnResize(mDevice, ED3D11RenderTargetViewType::Default, width, height);
		mBackBufferRTV->OnResize(mDevice, ED3D11RenderTargetViewType::Offscreen, width, height);
		mDSV->OnResize(mDevice, ED3D11DepthStencilViewType::Default, width, height);

		mBackBufferSRV->Init(mDevice, mBackBufferRTV);

		for (std::shared_ptr<RenderPass> pass : mPasses)
		{
			pass->OnResize(width, height);
		}
	}

	void ForwardPipeline::BeginRender()
	{
		mSwapChainRTV->Clear(mDevice);
	}

	void ForwardPipeline::Render()
	{
		for (std::shared_ptr<RenderPass> pass : mPasses)
		{
			pass->Render();
		}
	}

	void ForwardPipeline::RenderBackBuffer()
	{
		mFullScreenPass->Render();
	}

	void ForwardPipeline::EndRender()
	{
		Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain = mDevice->GetSwapChain();
		HR(swapChain->Present(0, 0));
	}

	std::shared_ptr<D3D11ShaderResourceView>& ForwardPipeline::GetBackBufferSRV()
	{
		return mBackBufferSRV;
	}
}