#include "LightProbeAddPass.h"

#include "D3D11Common.h"

#include "ManagementCommon.h"

void fq::graphics::LightProbeAddPass::Initialize(std::shared_ptr<D3D11Device> device, std::shared_ptr<D3D11ResourceManager> resourceManager, unsigned short width, unsigned short height)
{
	Finalize();

	mDevice = device;
	mResourceManager = resourceManager;

	auto outLineAddVS = std::make_shared<D3D11VertexShader>(device, L"LightProbeAddVS.cso");
	auto outLineAddPS = std::make_shared<D3D11PixelShader>(device, L"LightProbeAddPS.cso");

	auto pipelieState = std::make_shared<PipelineState>(nullptr, nullptr, nullptr);
	mLightProbeAddShaderProgram = std::make_unique<ShaderProgram>(mDevice, outLineAddVS, nullptr, outLineAddPS, pipelieState);

	auto LightProbeRTV = mResourceManager->Get<D3D11RenderTargetView>(ED3D11RenderTargetViewType::LightProbeIrr);
	mLightProbeSRV = std::make_shared<D3D11ShaderResourceView>(mDevice, LightProbeRTV);

	auto offScreenRTV = mResourceManager->Get<D3D11RenderTargetView>(ED3D11RenderTargetViewType::Offscreen);
	mOffScreenSRV = std::make_shared<D3D11ShaderResourceView>(mDevice, offScreenRTV);

	mOutLineBlurRTV = mResourceManager->Create<fq::graphics::D3D11RenderTargetView>(ED3D11RenderTargetViewType::OutLineBlur, width, height);
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

void fq::graphics::LightProbeAddPass::Finalize()
{
	mDevice = nullptr;
	mResourceManager = nullptr;

	mLightProbeSRV = nullptr;
	mOffScreenSRV = nullptr;

	mOutLineBlurRTV = nullptr;
	mDSV = nullptr;

	mLightProbeAddShaderProgram = nullptr;

	mFullScreenVB = nullptr;
	mFullScreenIB = nullptr;

	mDefaultSS = nullptr;
}

void fq::graphics::LightProbeAddPass::Render()
{
	// Bind
	{
		mDevice->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		mOutLineBlurRTV->Bind(mDevice, mDSV);

		mLightProbeAddShaderProgram->Bind(mDevice);

		mFullScreenVB->Bind(mDevice);
		mFullScreenIB->Bind(mDevice);

		mDefaultSS->Bind(mDevice, 0, ED3D11ShaderType::PixelShader);
		mOffScreenSRV->Bind(mDevice, 0, ED3D11ShaderType::PixelShader);
		mLightProbeSRV->Bind(mDevice, 1, ED3D11ShaderType::PixelShader);
	}

	// draw
	{
		mDevice->GetDeviceContext()->DrawIndexed(6, 0, 0);
	}

	// UnBind
	{
		mOffScreenSRV->UnBind(mDevice, 0, ED3D11ShaderType::PixelShader);
		mLightProbeSRV->UnBind(mDevice, 0, ED3D11ShaderType::PixelShader);
		mOutLineBlurRTV->UnBind(mDevice);
	}
}

void fq::graphics::LightProbeAddPass::OnResize(unsigned short width, unsigned short height)
{
	mOutLineBlurRTV = mResourceManager->Create<fq::graphics::D3D11RenderTargetView>(ED3D11RenderTargetViewType::OutLineBlur, width, height);

	auto LightProbeRTV = mResourceManager->Get<D3D11RenderTargetView>(ED3D11RenderTargetViewType::LightProbeIrr);
	mLightProbeSRV = std::make_shared<D3D11ShaderResourceView>(mDevice, LightProbeRTV);

	auto offScreenRTV = mResourceManager->Get<D3D11RenderTargetView>(ED3D11RenderTargetViewType::Offscreen);
	mOffScreenSRV = std::make_shared<D3D11ShaderResourceView>(mDevice, offScreenRTV);
}
