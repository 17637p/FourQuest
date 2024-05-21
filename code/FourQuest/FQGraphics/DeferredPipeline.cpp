#include "DeferredPipeline.h"

#include "ManagementCommon.h"
#include "D3D11Common.h"
#include "RenderJob.h"
#include "Material.h"
#include "Mesh.h"
#include "Define.h"
#include "DeferredPass.h"

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

		mDevice = device;
		mResourceManager = resourceManager;

		mShadowPass->Initialize(device, jobManager, cameraManager, resourceManager, lightManager);
		mGeometryPass->Initialize(device, jobManager, cameraManager, resourceManager, lightManager, width, height);
		mShadingPass->Initialize(device, resourceManager, lightManager, cameraManager, width, height);
		mDebugRenderPass->Initialize(device, jobManager, debugDrawManager, cameraManager, resourceManager, width, height);
		mSkyBoxPass->Initialize(device, cameraManager, resourceManager);
		mTransparentRenderPass->Initialize(device, jobManager, cameraManager, lightManager, resourceManager, width, height);
		mTransparentCompositePass->Initialize(device, resourceManager, width, height);
		mTerrainPass->Initialize(device, jobManager, cameraManager, resourceManager, lightManager);
		mFullScreenPass->Initialize(device, resourceManager, width, height);

		mPasses.push_back(mShadowPass);
		mPasses.push_back(mGeometryPass);
		mPasses.push_back(mShadingPass);
		mPasses.push_back(mDebugRenderPass);
		mPasses.push_back(mSkyBoxPass);
		mPasses.push_back(mTransparentRenderPass);
		mPasses.push_back(mTransparentCompositePass);
		mPasses.push_back(mTerrainPass);
		mPasses.push_back(mFullScreenPass);

		mSwapChainRTV = mResourceManager->Create<D3D11RenderTargetView>(ED3D11RenderTargetViewType::Default, width, height);
		mBackBufferRTV = resourceManager->Create<fq::graphics::D3D11RenderTargetView>(ED3D11RenderTargetViewType::Offscreen, width, height);
		mBackBufferSRV = std::make_shared<fq::graphics::D3D11ShaderResourceView>(device, mBackBufferRTV);
		mDSV = resourceManager->Create<fq::graphics::D3D11DepthStencilView>(ED3D11DepthStencilViewType::Default, width, height);

		mAlbedoRTV = std::make_shared<fq::graphics::D3D11RenderTargetView>(device, ED3D11RenderTargetViewType::Offscreen, width, height);
		mMetalnessRTV = std::make_shared<fq::graphics::D3D11RenderTargetView>(device, ED3D11RenderTargetViewType::OffscreenGrayscale, width, height);
		mRoughnessRTV = std::make_shared<fq::graphics::D3D11RenderTargetView>(device, ED3D11RenderTargetViewType::OffscreenGrayscale, width, height);
		mNormalRTV = std::make_shared<fq::graphics::D3D11RenderTargetView>(device, ED3D11RenderTargetViewType::OffscreenHDR, width, height);
		mEmissiveRTV = std::make_shared<fq::graphics::D3D11RenderTargetView>(device, ED3D11RenderTargetViewType::Offscreen, width, height);
		mPositionRTV = std::make_shared<fq::graphics::D3D11RenderTargetView>(device, ED3D11RenderTargetViewType::OffscreenHDR, width, height);
		mGeometryPass->SetRenderTargets(mAlbedoRTV, mMetalnessRTV, mRoughnessRTV, mNormalRTV, mEmissiveRTV, mPositionRTV);

		mAlbedoSRV = std::make_shared<fq::graphics::D3D11ShaderResourceView>(device, mAlbedoRTV);
		mMetalnessSRV = std::make_shared<fq::graphics::D3D11ShaderResourceView>(device, mMetalnessRTV);
		mRoughnessSRV = std::make_shared<fq::graphics::D3D11ShaderResourceView>(device, mRoughnessRTV);
		mNormalSRV = std::make_shared<fq::graphics::D3D11ShaderResourceView>(device, mNormalRTV);
		mEmissiveSRV = std::make_shared<fq::graphics::D3D11ShaderResourceView>(device, mEmissiveRTV);
		mPositionSRV = std::make_shared<fq::graphics::D3D11ShaderResourceView>(device, mPositionRTV);
		mShadingPass->SetShaderResourceViews(mAlbedoSRV, mMetalnessSRV, mRoughnessSRV, mNormalSRV, mEmissiveSRV, mPositionSRV);
	}
	void DeferredPipeline::Finalize()
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

		mAlbedoRTV = nullptr;
		mMetalnessRTV = nullptr;
		mRoughnessRTV = nullptr;
		mNormalRTV = nullptr;
		mEmissiveRTV = nullptr;
		mPositionRTV = nullptr;

		mAlbedoSRV = nullptr;
		mMetalnessSRV = nullptr;
		mRoughnessSRV = nullptr;
		mNormalSRV = nullptr;
		mEmissiveSRV = nullptr;
		mPositionSRV = nullptr;
	}

	void DeferredPipeline::OnResize(unsigned short width, unsigned short height)
	{
		mSwapChainRTV->Release();
		mBackBufferRTV->Release();
		mDSV->Release();

		mDevice->OnResize(width, height);

		mSwapChainRTV->OnResize(mDevice, ED3D11RenderTargetViewType::Default, width, height);
		mBackBufferRTV->OnResize(mDevice, ED3D11RenderTargetViewType::Offscreen, width, height);
		mDSV->OnResize(mDevice, ED3D11DepthStencilViewType::Default, width, height);
		mBackBufferSRV->Init(mDevice, mBackBufferRTV);

		mAlbedoRTV->Release();
		mMetalnessRTV->Release();
		mRoughnessRTV->Release();
		mNormalRTV->Release();
		mEmissiveRTV->Release();
		mPositionRTV->Release();

		mAlbedoRTV->OnResize(mDevice, ED3D11RenderTargetViewType::Offscreen, width, height);
		mMetalnessRTV->OnResize(mDevice, ED3D11RenderTargetViewType::OffscreenGrayscale, width, height);
		mRoughnessRTV->OnResize(mDevice, ED3D11RenderTargetViewType::OffscreenGrayscale, width, height);
		mNormalRTV->OnResize(mDevice, ED3D11RenderTargetViewType::OffscreenHDR, width, height);
		mEmissiveRTV->OnResize(mDevice, ED3D11RenderTargetViewType::Offscreen, width, height);
		mPositionRTV->OnResize(mDevice, ED3D11RenderTargetViewType::OffscreenHDR, width, height);
		mGeometryPass->SetRenderTargets(mAlbedoRTV, mMetalnessRTV, mRoughnessRTV, mNormalRTV, mEmissiveRTV, mPositionRTV);

		mAlbedoSRV->Init(mDevice, mAlbedoRTV);
		mMetalnessSRV->Init(mDevice, mMetalnessRTV);
		mRoughnessSRV->Init(mDevice, mRoughnessRTV);
		mNormalSRV->Init(mDevice, mNormalRTV);
		mEmissiveSRV->Init(mDevice, mEmissiveRTV);
		mPositionSRV->Init(mDevice, mPositionRTV);
		mShadingPass->SetShaderResourceViews(mAlbedoSRV, mMetalnessSRV, mRoughnessSRV, mNormalSRV, mEmissiveSRV, mPositionSRV);

		for (std::shared_ptr<RenderPass> pass : mPasses)
		{
			pass->OnResize(width, height);
		}
	}

	void DeferredPipeline::BeginRender()
	{
		mSwapChainRTV->Clear(mDevice);
	}

	void DeferredPipeline::Render()
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
	void DeferredPipeline::EndRender()
	{
		Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain = mDevice->GetSwapChain();
		HR(swapChain->Present(0, 0));
	}

	std::shared_ptr<D3D11ShaderResourceView>& DeferredPipeline::GetBackBufferSRV()
	{
		return mBackBufferSRV;
	}

	void DeferredPipeline::SetSkyBox(const std::wstring& path)
	{
		mSkyBoxPass->SetSkyBox(path);
	}

	void DeferredPipeline::SetIBLTexture(const std::wstring& diffuse, const std::wstring& specular, const std::wstring& brdfLUT)
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