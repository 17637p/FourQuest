#include "RenderPipeline.h"

#include "ManagementCommon.h"
#include "D3D11Common.h"
#include "Pass.h"
#include "Define.h"

namespace fq::graphics
{
	void RenderPipeline::Initialize(std::shared_ptr<D3D11Device>& device,
		std::shared_ptr<D3D11ResourceManager>& resourceManager,
		unsigned short width,
		unsigned short height)
	{
		mDevice = device;
		mResourceManager = resourceManager;

		mSwapChainRTV = mResourceManager->Create<D3D11RenderTargetView>(ED3D11RenderTargetViewType::Default, width, height);
		mBackBufferRTV = mResourceManager->Create<D3D11RenderTargetView>(ED3D11RenderTargetViewType::Offscreen, width, height);
		mDSV = mResourceManager->Create<D3D11DepthStencilView>(ED3D11DepthStencilViewType::Default, width, height);
		mNoneDSV = mResourceManager->Create<D3D11DepthStencilView>(ED3D11DepthStencilViewType::None, width, height);

		mBackBufferSRV = std::make_shared<D3D11ShaderResourceView>(mDevice, mBackBufferRTV);

		// 자동화하고 싶다 요기 부분을!
		std::map<std::string, ED3D11SamplerState> samplerStateMap;
		samplerStateMap.insert({ "gSamplerAnisotropic", ED3D11SamplerState::AnisotropicWrap });
		samplerStateMap.insert({ "gLinearClamp", ED3D11SamplerState::Default });
		samplerStateMap.insert({ "gShadowSampler", ED3D11SamplerState::Shadow });
		D3D11ShaderProgram::SetSamplerStateMap(samplerStateMap);

		std::map<std::string, ED3D11ConstantBufferNT> constnatBufferMap;
		constnatBufferMap.insert({ "cbModelTransform", ED3D11ConstantBufferNT::cbModelTransform });
		constnatBufferMap.insert({ "cbSceneTransform", ED3D11ConstantBufferNT::cbSceneTransform });
		constnatBufferMap.insert({ "cbBoneTransform", ED3D11ConstantBufferNT::cbBoneTransform });
		constnatBufferMap.insert({ "cbModelTexture", ED3D11ConstantBufferNT::cbModelTexture });
		constnatBufferMap.insert({ "cbLight", ED3D11ConstantBufferNT::cbLight });
		constnatBufferMap.insert({ "cbShadowTransformCascaseEnd", ED3D11ConstantBufferNT::cbShadowTransformCascaseEnd });
		D3D11ShaderProgram::SetConstantBufferMap(constnatBufferMap);
	}

	void RenderPipeline::Finalize()
	{
		for (std::shared_ptr<Pass> pass : mPasses)
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
		mNoneDSV = nullptr;
	}

	void RenderPipeline::OnResize(unsigned short width, unsigned short height)
	{
		mSwapChainRTV->Release();
		mBackBufferRTV->Release();
		mDSV->Release();
		mNoneDSV->Release();

		mDevice->OnResize(width, height);

		mSwapChainRTV->OnResize(mDevice, ED3D11RenderTargetViewType::Default, width, height);
		mBackBufferRTV->OnResize(mDevice, ED3D11RenderTargetViewType::Offscreen, width, height);
		mDSV->OnResize(mDevice, ED3D11DepthStencilViewType::Default, width, height);
		mNoneDSV->OnResize(mDevice, ED3D11DepthStencilViewType::None, width, height);

		mBackBufferSRV->Init(mDevice, mBackBufferRTV);

		for (std::shared_ptr<Pass> pass : mPasses)
		{
			pass->OnResize(width, height);
		}
	}

	void RenderPipeline::BeginRender()
	{
		mSwapChainRTV->Clear(mDevice);
	}

	void RenderPipeline::Render()
	{
		for (std::shared_ptr<Pass> pass : mPasses)
		{
			pass->Render();
		}
	}

	void RenderPipeline::EndRender()
	{
		Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain = mDevice->GetSwapChain();
		HR(swapChain->Present(0, 0));
	}
}