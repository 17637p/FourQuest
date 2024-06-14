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

		OnResize(width, height);
	}

	void FullScreenPass::Finalize()
	{
		mDevice = nullptr;
		mResourceManager = nullptr;

		mSwapChainRTV = nullptr;
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
		mSwapChainRTV->Bind(mDevice, mDSV);
	}
}