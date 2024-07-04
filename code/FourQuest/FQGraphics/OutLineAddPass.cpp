#include "OutLineAddPass.h"

#include "D3D11Common.h"

#include "ManagementCommon.h"

void fq::graphics::OutLineAddPass::Initialize(std::shared_ptr<D3D11Device> device, std::shared_ptr<D3D11ResourceManager> resourceManager)
{
	Finalize();

	mDevice = device;
	mResourceManager = resourceManager;

	auto outLineAddVS = std::make_shared<D3D11VertexShader>(device, L"OutLineAddVS.cso");
	auto outLineAddPS = std::make_shared<D3D11PixelShader>(device, L"OutLineAddPS.cso");

	auto pipelieState = std::make_shared<PipelineState>(nullptr, nullptr, nullptr);
	mOutLineAddShaderProgram = std::make_unique<ShaderProgram>(mDevice, outLineAddVS, nullptr, outLineAddPS, pipelieState);

	auto outLineBlurRTV = mResourceManager->Get<D3D11RenderTargetView>(ED3D11RenderTargetViewType::OutLineBlur);
	mOutLineBlurSRV = std::make_shared<D3D11ShaderResourceView>(mDevice, outLineBlurRTV);

	mOffScreenRTV = mResourceManager->Get<fq::graphics::D3D11RenderTargetView>(ED3D11RenderTargetViewType::Offscreen);
	mDSV = mResourceManager->Get<D3D11DepthStencilView>(ED3D11DepthStencilViewType::None);

	mDefaultSS = resourceManager->Get<D3D11SamplerState>(ED3D11SamplerState::Default);

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

void fq::graphics::OutLineAddPass::Finalize()
{
	mDevice = nullptr;
	mResourceManager = nullptr;

	mOutLineBlurSRV = nullptr;

	mOffScreenRTV = nullptr;
	mDSV = nullptr;

	mOutLineAddShaderProgram = nullptr;

	mFullScreenVB = nullptr;
	mFullScreenIB = nullptr;

	mDefaultSS = nullptr;
}

void fq::graphics::OutLineAddPass::Render()
{
	// Bind
	{
		mDevice->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		mOffScreenRTV->Bind(mDevice, mDSV);

		mOutLineAddShaderProgram->Bind(mDevice);

		mFullScreenVB->Bind(mDevice);
		mFullScreenIB->Bind(mDevice);

		mDefaultSS->Bind(mDevice, 0, ED3D11ShaderType::PixelShader);
		mOutLineBlurSRV->Bind(mDevice, 0, ED3D11ShaderType::PixelShader);
	}

	// draw
	{
		mDevice->GetDeviceContext()->DrawIndexed(6, 0, 0);
	}

	// UnBind
	{
		mOutLineBlurSRV->UnBind(mDevice, 0, ED3D11ShaderType::PixelShader);
	}
}

void fq::graphics::OutLineAddPass::OnResize(unsigned short width, unsigned short height)
{
	auto outLineBlurRTV = mResourceManager->Get<D3D11RenderTargetView>(ED3D11RenderTargetViewType::OutLineBlur);
	mOutLineBlurSRV = std::make_shared<D3D11ShaderResourceView>(mDevice, outLineBlurRTV);
}
