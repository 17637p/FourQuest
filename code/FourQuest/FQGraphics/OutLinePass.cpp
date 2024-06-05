#include "OutLinePass.h"

#include "D3D11Common.h"
#include "ManagementCommon.h"

void fq::graphics::OutLinePass::Initialize(std::shared_ptr<D3D11Device> device, std::shared_ptr<D3D11CameraManager> cameraManager, std::shared_ptr<D3D11ResourceManager> resourceManager, unsigned short width, unsigned short height)
{
	mDevice = device;
	mCameraManager = cameraManager;
	mResourceManager = resourceManager;

	auto outLineVS = std::make_shared<D3D11VertexShader>(device, L"./resource/internal/shader/OutLineVS.hlsl");
	auto outLinePS = std::make_shared<D3D11PixelShader>(device, L"./resource/internal/shader/OutLinePS.hlsl");
	auto pipelieState = std::make_shared<PipelineState>(nullptr, nullptr, nullptr);
	mOutLinePassShaderProgram = std::make_unique<ShaderProgram>(mDevice, outLineVS, nullptr, outLinePS, pipelieState);

	auto singleColorRTV = mResourceManager->Get<D3D11RenderTargetView>(ED3D11RenderTargetViewType::SingleColor);
	mSingleColorSRV = std::make_shared<D3D11ShaderResourceView>(mDevice, singleColorRTV);

	mOutlineRTV = mResourceManager->Create<fq::graphics::D3D11RenderTargetView>(ED3D11RenderTargetViewType::OutLine, width, height);
	mNoneDSV = mResourceManager->Create<fq::graphics::D3D11DepthStencilView>(ED3D11DepthStencilViewType::None, width, height);

	mDefaultSS = resourceManager->Create<D3D11SamplerState>(ED3D11SamplerState::Default);

	mScreenSizeCB = std::make_shared<D3D11ConstantBuffer<ScreenSize>>(mDevice, ED3D11ConstantBuffer::Transform);

	OnResize(width, height);

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

void fq::graphics::OutLinePass::Finalize()
{

}

void fq::graphics::OutLinePass::Render()
{
	// Update
	{
		mScreenSizeCB->Update(mDevice, mScreenSize);
	}

	// Init
	{
		mOutlineRTV->Clear(mDevice, { 1,1,1,1 });
	}

	// Bind
	{
		mDevice->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		mOutlineRTV->Bind(mDevice, mNoneDSV);

		mOutLinePassShaderProgram->Bind(mDevice);

		mFullScreenVB->Bind(mDevice);
		mFullScreenIB->Bind(mDevice);

		mDefaultSS->Bind(mDevice, 0, ED3D11ShaderType::PixelShader);
		mSingleColorSRV->Bind(mDevice, 0, ED3D11ShaderType::PixelShader);

		mScreenSizeCB->Bind(mDevice, ED3D11ShaderType::PixelShader, 0);
	}

	// draw
	{
		mDevice->GetDeviceContext()->DrawIndexed(6, 0, 0);
	}
}

void fq::graphics::OutLinePass::OnResize(unsigned short width, unsigned short height)
{
	mScreenSize.width = width;
	mScreenSize.height = height;

	auto singleColorRTV = mResourceManager->Get<D3D11RenderTargetView>(ED3D11RenderTargetViewType::SingleColor);
	mSingleColorSRV = std::make_shared<D3D11ShaderResourceView>(mDevice, singleColorRTV);

	mOutlineRTV->OnResize(mDevice, ED3D11RenderTargetViewType::OutLine, width, height);
}
