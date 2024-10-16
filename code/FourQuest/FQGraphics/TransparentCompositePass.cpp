#include "TransparentCompositePass.h"
#include "ManagementCommon.h"
#include "D3D11Common.h"

namespace fq::graphics
{
	void TransparentCompositePass::Initialize(std::shared_ptr<D3D11Device> device,
		std::shared_ptr<D3D11ResourceManager> resourceManager,
		unsigned short width,
		unsigned short height)
	{
		Finalize();

		mDevice = device;
		mResourceManager = resourceManager;

		mBackBufferRTV = mResourceManager->Get<D3D11RenderTargetView>(ED3D11RenderTargetViewType::Offscreen);
		mNullDSV = mResourceManager->Get<D3D11DepthStencilView>(ED3D11DepthStencilViewType::None);
		auto coloraccumulationRTV = mResourceManager->Get<D3D11RenderTargetView>(ED3D11RenderTargetViewType::ColorAcuumulation);
		auto pixelRevealageThresholdRTV = mResourceManager->Get<D3D11RenderTargetView>(ED3D11RenderTargetViewType::PixeldRevealageThreshold);
		mColoraccumulationSRV = std::make_shared<D3D11ShaderResourceView>(mDevice, coloraccumulationRTV);
		mPixelRevealageThresholdSRV = std::make_shared<D3D11ShaderResourceView>(mDevice, pixelRevealageThresholdRTV);

		auto fullScreenVS = std::make_shared<D3D11VertexShader>(device, L"FullScreenVS.cso");
		auto transparentCompositePS = std::make_shared<D3D11PixelShader>(mDevice, L"ModelTransparentPS_COMPOSITE.cso");
		auto OITCompositeState = resourceManager->Create<D3D11BlendState>(ED3D11BlendState::OITComposite);
		auto pipelieState = std::make_shared<PipelineState>(nullptr, nullptr, OITCompositeState);
		mShaderProgram = std::make_unique<ShaderProgram>(mDevice, fullScreenVS, nullptr, transparentCompositePS, pipelieState);

		mPointClampSamplerState = resourceManager->Create<D3D11SamplerState>(ED3D11SamplerState::AnisotropicClamp);

		mFullScreenVB = std::make_shared<D3D11VertexBuffer>(D3D11VertexBuffer::CreateFullScreenVertexBuffer(device));
		mFullScreenIB = std::make_shared<D3D11IndexBuffer>(D3D11IndexBuffer::CreateFullScreenIndexBuffer(device));

		mViewport.Width = (float)width;
		mViewport.Height = (float)height;
		mViewport.MinDepth = 0.f;
		mViewport.MaxDepth = 1.f;
		mViewport.TopLeftX = 0.f;
		mViewport.TopLeftY = 0.f;
	}

	void TransparentCompositePass::Finalize()
	{
		mDevice = nullptr;
		mResourceManager = nullptr;

		mBackBufferRTV = nullptr;
		mNullDSV = nullptr;
		mColoraccumulationSRV = nullptr;
		mPixelRevealageThresholdSRV = nullptr;

		mShaderProgram = nullptr;
		mFullScreenVB = nullptr;
		mFullScreenIB = nullptr;
		mPointClampSamplerState = nullptr;
	}

	void TransparentCompositePass::OnResize(unsigned short width, unsigned short height)
	{
		mViewport.Width = (float)width;
		mViewport.Height = (float)height;
		mViewport.MinDepth = 0.f;
		mViewport.MaxDepth = 1.f;
		mViewport.TopLeftX = 0.f;
		mViewport.TopLeftY = 0.f;

		auto coloraccumulationRTV = mResourceManager->Get<D3D11RenderTargetView>(ED3D11RenderTargetViewType::ColorAcuumulation);
		auto pixelRevealageThresholdRTV = mResourceManager->Get<D3D11RenderTargetView>(ED3D11RenderTargetViewType::PixeldRevealageThreshold);
		mColoraccumulationSRV = std::make_shared<D3D11ShaderResourceView>(mDevice, coloraccumulationRTV);
		mPixelRevealageThresholdSRV = std::make_shared<D3D11ShaderResourceView>(mDevice, pixelRevealageThresholdRTV);
	}

	void TransparentCompositePass::Render()
	{
		ID3D11ShaderResourceView* SRVs[10] = { NULL };
		mDevice->GetDeviceContext()->PSSetShaderResources(0, 10, SRVs);

		mDevice->GetDeviceContext()->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		mDevice->GetDeviceContext()->RSSetViewports(1, &mViewport);

		mBackBufferRTV->Bind(mDevice, mNullDSV);
		mColoraccumulationSRV->Bind(mDevice, 0, ED3D11ShaderType::PixelShader);
		mPixelRevealageThresholdSRV->Bind(mDevice, 1, ED3D11ShaderType::PixelShader);
		mPointClampSamplerState->Bind(mDevice, 0, ED3D11ShaderType::PixelShader);

		mShaderProgram->Bind(mDevice);
		mFullScreenVB->Bind(mDevice);
		mFullScreenIB->Bind(mDevice);

		mDevice->GetDeviceContext()->DrawIndexed(6, 0, 0);
	}
}