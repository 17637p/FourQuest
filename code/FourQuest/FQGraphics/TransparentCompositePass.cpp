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
		mDevice = device;
		mResourceManager = resourceManager;

		mBackBufferRTV = mResourceManager->Get<D3D11RenderTargetView>(ED3D11RenderTargetViewType::Offscreen);
		mNullDSV = mResourceManager->Get<D3D11DepthStencilView>(ED3D11DepthStencilViewType::None);
		auto coloraccumulationRTV = mResourceManager->Get<D3D11RenderTargetView>(ED3D11RenderTargetViewType::ColorAcuumulation);
		auto pixelRevealageThresholdRTV = mResourceManager->Get<D3D11RenderTargetView>(ED3D11RenderTargetViewType::PixeldRevealageThreshold);
		mColoraccumulationSRV = std::make_shared<D3D11ShaderResourceView>(mDevice, coloraccumulationRTV);
		mPixelRevealageThresholdSRV = std::make_shared<D3D11ShaderResourceView>(mDevice, pixelRevealageThresholdRTV);

		mOITCompositeState = resourceManager->Create<D3D11BlendState>(ED3D11BlendState::OITComposite);

		D3D_SHADER_MACRO macroComposite[] =
		{
			{"COMPOSITE", ""},
			{ NULL, NULL}
		};

		mFullScreenVS = std::make_shared<D3D11VertexShader>(device, L"./resource/internal/shader/FullScreenVS.hlsl");
		mFullScreenLayout = std::make_shared<D3D11InputLayout>(device, mFullScreenVS->GetBlob().Get());
		mTransparentCompositePS = std::make_shared<D3D11PixelShader>(mDevice, L"./resource/internal/shader/ModelTransparentPS.hlsl", macroComposite);
		mPointClampSamplerState = resourceManager->Create<D3D11SamplerState>(ED3D11SamplerState::PointClamp);

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

		mOITCompositeState = nullptr;

		mFullScreenLayout = nullptr;
		mFullScreenVS = nullptr;
		mTransparentCompositePS = nullptr;
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
		mColoraccumulationSRV->Bind(mDevice, 0, ED3D11ShaderType::Pixelshader);
		mPixelRevealageThresholdSRV->Bind(mDevice, 1, ED3D11ShaderType::Pixelshader);
		mPointClampSamplerState->Bind(mDevice, 0, ED3D11ShaderType::Pixelshader);

		mFullScreenLayout->Bind(mDevice);
		mFullScreenVS->Bind(mDevice);
		mTransparentCompositePS->Bind(mDevice);
		mFullScreenVB->Bind(mDevice);
		mFullScreenIB->Bind(mDevice);
		mOITCompositeState->Bind(mDevice);

		mDevice->GetDeviceContext()->DrawIndexed(6, 0, 0);

		// unbind
		{
			mDevice->GetDeviceContext()->OMSetBlendState(NULL, NULL, 0xFFFFFFFF);
		}
	}
}