#include "OutLineBlurPass.h"

#include "D3D11Common.h"
#include "ManagementCommon.h"

void fq::graphics::OutLineBlurPass::Initialize(std::shared_ptr<D3D11Device> device, std::shared_ptr<D3D11ResourceManager> resourceManager, unsigned short width, unsigned short height)
{
	mDevice = device;
	mResourceManager = resourceManager;

	auto outLineBlurVS = std::make_shared<D3D11VertexShader>(device, L"./resource/internal/shader/OutLineBlurVS.hlsl");
	auto outLineBlurPS = std::make_shared<D3D11PixelShader>(device, L"./resource/internal/shader/OutLineBlurPS.hlsl");

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

void fq::graphics::OutLineBlurPass::Finalize()
{

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

		mDefaultSS->Bind(mDevice, 0, ED3D11ShaderType::Pixelshader);
		mOutLineSRV->Bind(mDevice, 0, ED3D11ShaderType::Pixelshader);
		mOffScreenSRV->Bind(mDevice, 1, ED3D11ShaderType::Pixelshader);

		mDevice->GetDeviceContext().Get()->GenerateMips(mOutLineSRV->GetSRV().Get()); //mip에 아무것도 안 그려져 있음 
	}

	// draw
	{
		mDevice->GetDeviceContext()->DrawIndexed(6, 0, 0);
	}

	// UnBind
	{
		mOffScreenSRV->UnBind(mDevice, 1, ED3D11ShaderType::Pixelshader);
	}
}