#include "FullScreenPass.h"

#include "ManagementCommon.h"
#include "D3D11Common.h"

namespace fq::graphics
{
	void FullScreenPass::Initialize(std::shared_ptr<D3D11Device> device,
		std::shared_ptr<D3D11ResourceManager> resourceManager,
		unsigned short width,
		unsigned short height)
	{
		Finalize();

		mDevice = device;
		mResourceManager = resourceManager;

		mSwapChainRTV = mResourceManager->Get<D3D11RenderTargetView>(ED3D11RenderTargetViewType::Default);
		mBackBufferRTV = mResourceManager->Get<D3D11RenderTargetView>(ED3D11RenderTargetViewType::Offscreen);
		mBackBufferSRV = std::make_shared<D3D11ShaderResourceView>(mDevice, mBackBufferRTV);
		mNoneDSV = mResourceManager->Get<D3D11DepthStencilView>(ED3D11DepthStencilViewType::None);
		mDSV = mResourceManager->Get<D3D11DepthStencilView>(ED3D11DepthStencilViewType::Default);

		auto fullScreenVS = std::make_shared<D3D11VertexShader>(device, L"FullScreenVS.cso");
		auto fullScreenPS = std::make_shared<D3D11PixelShader>(device, L"FullScreenPS.cso");
		auto pipelieState = std::make_shared<PipelineState>(nullptr, nullptr, nullptr);
		mFullScreenShaderProgram = std::make_unique<ShaderProgram>(mDevice, fullScreenVS, nullptr, fullScreenPS, pipelieState);
		mPointClampSamplerState = resourceManager->Create<D3D11SamplerState>(ED3D11SamplerState::AnisotropicClamp);

		mFullScreenVB = std::make_shared<D3D11VertexBuffer>(D3D11VertexBuffer::CreateFullScreenVertexBuffer(device));
		mFullScreenIB = std::make_shared<D3D11IndexBuffer>(D3D11IndexBuffer::CreateFullScreenIndexBuffer(device));

		OnResize(width, height);
	}

	void FullScreenPass::Finalize()
	{
		mDevice = nullptr;
		mResourceManager = nullptr;

		mSwapChainRTV = nullptr;
		mBackBufferRTV = nullptr;
		mBackBufferSRV = nullptr;
		mNoneDSV = nullptr;
		mDSV = nullptr;

		mFullScreenShaderProgram = nullptr;

		mFullScreenVB = nullptr;
		mFullScreenIB = nullptr;
		mPointClampSamplerState = nullptr;
	}
	void FullScreenPass::OnResize(unsigned short width, unsigned short height)
	{
		mViewport.Width = (float)width;
		mViewport.Height = (float)height;
		mViewport.MinDepth = 0.f;
		mViewport.MaxDepth = 1.f;
		mViewport.TopLeftX = 0.f;
		mViewport.TopLeftY = 0.f;

		mBackBufferSRV->Init(mDevice, mBackBufferRTV);
	}

	void FullScreenPass::OnResize(unsigned short width, unsigned short height, unsigned short oriWidth, unsigned short oriHeight)
	{
		// 16:9 비율 유지하기 위한 스케일링 계산
		float renderAspect = 16.0f / 9.0f;
		float windowAspect = (float)oriWidth / oriHeight;

		D3D11_VIEWPORT viewport = {};
		if (windowAspect > renderAspect)
		{
			// 창이 더 넓은 경우
			viewport.Width = oriHeight * renderAspect;
			viewport.Height = oriHeight;
			viewport.TopLeftX = (oriWidth - viewport.Width) / 2.0f;
			viewport.TopLeftY = 0.0f;
		}
		else
		{
			// 창이 더 높은 경우
			viewport.Width = oriWidth;
			viewport.Height = oriWidth / renderAspect;
			viewport.TopLeftX = 0.0f;
			viewport.TopLeftY = (oriHeight - viewport.Height) / 2.0f;
		}
		mViewport = viewport;

		mBackBufferSRV->Init(mDevice, mBackBufferRTV);
	}

	void FullScreenPass::Render()
	{
		ID3D11ShaderResourceView* SRVs[10] = { NULL };
		mDevice->GetDeviceContext()->PSSetShaderResources(0, 10, SRVs);

		mDevice->GetDeviceContext()->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		mDevice->GetDeviceContext()->RSSetViewports(1, &mViewport);

		mSwapChainRTV->Bind(mDevice, mNoneDSV);
		mBackBufferSRV->Bind(mDevice, 0, ED3D11ShaderType::PixelShader);

		mFullScreenShaderProgram->Bind(mDevice);
		mFullScreenVB->Bind(mDevice);
		mFullScreenIB->Bind(mDevice);
		mPointClampSamplerState->Bind(mDevice, 1, ED3D11ShaderType::PixelShader);

		mDevice->GetDeviceContext()->DrawIndexed(6, 0, 0);
		//mSwapChainRTV->Bind(mDevice, mDSV);
	}
}