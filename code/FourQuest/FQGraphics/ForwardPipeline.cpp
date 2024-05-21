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
		, mTransparentRenderPass(std::make_shared<TransparentRenderPass>())
		, mTransparentCompositePass(std::make_shared<TransparentCompositePass>())
		, mDebugRenderPass(std::make_shared<DebugRenderPass>())
		, mSkyBoxPass(std::make_shared<SkyBoxPass>())
		, mTerrainPass(std::make_shared<TerrainPass>())
		, mFullScreenPass(std::make_shared<FullScreenPass>())
	{
	}

	void ForwardPipeline::Initialize(std::shared_ptr<D3D11Device>& device,
		std::shared_ptr<D3D11JobManager>& jobManager,
		std::shared_ptr<D3D11CameraManager>& cameraManager,
		std::shared_ptr< D3D11LightManager>& lightManager,
		std::shared_ptr<D3D11ResourceManager>& resourceManager,
		std::shared_ptr<D3D11DebugDrawManager> debugDrawManager,
		unsigned short width,
		unsigned short height)
	{
		Finalize();

		mDevice = device;
		mResourceManager = resourceManager;

		mShadowPass->Initialize(device, jobManager, cameraManager, resourceManager, lightManager);
		mRenderPass->Initialize(device, jobManager, cameraManager, lightManager, resourceManager, width, height);
		mDebugRenderPass->Initialize(device, jobManager, debugDrawManager, cameraManager, resourceManager, width, height);
		mSkyBoxPass->Initialize(device, cameraManager, resourceManager);
		mTransparentRenderPass->Initialize(device, jobManager, cameraManager, lightManager, resourceManager, width, height);
		mTransparentCompositePass->Initialize(device, resourceManager, width, height);
		mTerrainPass->Initialize(device, jobManager, cameraManager, resourceManager, lightManager);
		mFullScreenPass->Initialize(device, resourceManager, width, height);

		// 삽입 순서가 처리되는 순서
		mPasses.push_back(mShadowPass);
		mPasses.push_back(mRenderPass);
		mPasses.push_back(mDebugRenderPass);
		mPasses.push_back(mSkyBoxPass);
		mPasses.push_back(mTransparentRenderPass);
		mPasses.push_back(mTransparentCompositePass);
		mPasses.push_back(mTerrainPass);
		mPasses.push_back(mFullScreenPass);

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
			if (mDiffuseCubeMap != nullptr)
			{
				mDiffuseCubeMap->Bind(mDevice, 6, ED3D11ShaderType::Pixelshader);
				mSpecularCubeMap->Bind(mDevice, 7, ED3D11ShaderType::Pixelshader);
				mBRDFLUT->Bind(mDevice, 8, ED3D11ShaderType::Pixelshader);
			}

			pass->Render();
		}
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

	void ForwardPipeline::SetSkyBox(const std::wstring& path)
	{
		mSkyBoxPass->SetSkyBox(path);
	}

	void ForwardPipeline::SetIBLTexture(const std::wstring& diffuse, const std::wstring& specular, const std::wstring& brdfLUT)
	{
		if (diffuse == L"" || specular == L"" || brdfLUT == L"")
		{
			return;
		}

		mDiffuseCubeMap = mResourceManager->Create<D3D11Texture>(diffuse);
		mSpecularCubeMap = mResourceManager->Create<D3D11Texture>(specular);
		mBRDFLUT = mResourceManager->Create<D3D11Texture>(brdfLUT);
	}
}