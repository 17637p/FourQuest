#include "OutLineBlurPass.h"

#include "D3D11Common.h"
#include "ManagementCommon.h"

void fq::graphics::OutLineBlurPass::Initialize(std::shared_ptr<D3D11Device> device, std::shared_ptr<D3D11ResourceManager> resourceManager, unsigned short width, unsigned short height)
{
	Finalize();
	
	mDevice = device;
	mResourceManager = resourceManager;

	auto outLineBlurVS = std::make_shared<D3D11VertexShader>(device, L"OutLineBlurVS.cso");
	auto outLineBlurPS = std::make_shared<D3D11PixelShader>(device, L"OutLineBlurPS.cso");

	mLessEqualDS = resourceManager->Get<D3D11DepthStencilState>(ED3D11DepthStencilState::LessEqual);

	auto pipelieState = std::make_shared<PipelineState>(nullptr, mLessEqualDS, nullptr);

	mOutLineBlurPassShaderProgram = std::make_unique<ShaderProgram>(mDevice, outLineBlurVS, nullptr, outLineBlurPS, pipelieState);

	auto outLineRTV = mResourceManager->Get<D3D11RenderTargetView>(ED3D11RenderTargetViewType::OutLine);
	mOutLineSRV = std::make_shared<D3D11ShaderResourceView>(mDevice, outLineRTV);

	auto offScreenRTV = mResourceManager->Get<D3D11RenderTargetView>(ED3D11RenderTargetViewType::Offscreen);
	mOffScreenSRV = std::make_shared<D3D11ShaderResourceView>(mDevice, offScreenRTV);

	mOutLineBlurRTV = mResourceManager->Create<fq::graphics::D3D11RenderTargetView>(ED3D11RenderTargetViewType::OutLineBlur, width, height);
	mNoneDSV = mResourceManager->Create<fq::graphics::D3D11DepthStencilView>(ED3D11DepthStencilViewType::None, width, height);

	mDefaultSS = resourceManager->Get<D3D11SamplerState>(ED3D11SamplerState::Default);

	mFullScreenVB = std::make_shared<D3D11VertexBuffer>(D3D11VertexBuffer::CreateFullScreenVertexBuffer(device));
	mFullScreenIB = std::make_shared<D3D11IndexBuffer>(D3D11IndexBuffer::CreateFullScreenIndexBuffer(device));
}

void fq::graphics::OutLineBlurPass::Finalize()
{
	mDevice = nullptr;
	mResourceManager = nullptr;

	mOutLineSRV = nullptr;
	mOffScreenSRV = nullptr;
	mOutLineBlurRTV = nullptr;
	mNoneDSV = nullptr;

	mLessEqualDS = nullptr;

	mOutLineBlurPassShaderProgram = nullptr;

	mFullScreenVB = nullptr;
	mFullScreenIB = nullptr;

	mDefaultSS = nullptr;
}

void fq::graphics::OutLineBlurPass::Render()
{
	// Init
	{
		mOutLineBlurRTV->Clear(mDevice, { 0,0,0,0 });
	}

	// Bind
	{
		mDevice->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		mOutLineBlurRTV->Bind(mDevice, mNoneDSV);

		mOutLineBlurPassShaderProgram->Bind(mDevice);

		mFullScreenVB->Bind(mDevice);
		mFullScreenIB->Bind(mDevice);

		mDefaultSS->Bind(mDevice, 0, ED3D11ShaderType::PixelShader);
		mOutLineSRV->Bind(mDevice, 0, ED3D11ShaderType::PixelShader);
		mOffScreenSRV->Bind(mDevice, 1, ED3D11ShaderType::PixelShader);

		mDevice->GetDeviceContext().Get()->GenerateMips(mOutLineSRV->GetSRV().Get()); //mip에 아무것도 안 그려져 있음 
	}

	// draw
	{
		mDevice->GetDeviceContext()->DrawIndexed(6, 0, 0);
	}

	// UnBind
	{
		mOffScreenSRV->UnBind(mDevice, 1, ED3D11ShaderType::PixelShader);
	}
}

void fq::graphics::OutLineBlurPass::OnResize(unsigned short width, unsigned short height)
{
	auto outLineRTV = mResourceManager->Get<D3D11RenderTargetView>(ED3D11RenderTargetViewType::OutLine);
	mOutLineSRV = std::make_shared<D3D11ShaderResourceView>(mDevice, outLineRTV);

	auto offScreenRTV = mResourceManager->Get<D3D11RenderTargetView>(ED3D11RenderTargetViewType::Offscreen);
	mOffScreenSRV = std::make_shared<D3D11ShaderResourceView>(mDevice, offScreenRTV);

	mOutLineBlurRTV->OnResize(mDevice, ED3D11RenderTargetViewType::OutLineBlur, width, height);
	mNoneDSV->OnResize(mDevice, ED3D11DepthStencilViewType::None, width, height);
}
