#include "SkyBoxPass.h"
#include "ManagementCommon.h"
#include "D3D11Common.h"

namespace fq::graphics
{
	SkyBoxPass::SkyBoxPass()
		:isSetSkyBox(false)
	{
	}

	void SkyBoxPass::Initialize(std::shared_ptr<D3D11Device> device, std::shared_ptr<D3D11CameraManager> cameraManager, std::shared_ptr<D3D11ResourceManager> resourceManager)
	{
		mDevice = device;
		mCameraManager = cameraManager;
		mResourceManager = resourceManager;

		auto skyBoxVS = std::make_shared<D3D11VertexShader>(device, L"./resource/internal/shader/SkyBoxVS.hlsl");
		auto skyboxPS = std::make_shared<D3D11PixelShader>(device, L"./resource/internal/shader/SkyBoxPS.hlsl");
		auto cullFrontRS = mResourceManager->Create<D3D11RasterizerState>(ED3D11RasterizerState::CullFront);
		auto lessEqualDepthStencilDS = mResourceManager->Create<D3D11DepthStencilState>(ED3D11DepthStencilState::LessEqual);
		auto pipelieState = std::make_shared<PipelineState>(cullFrontRS, lessEqualDepthStencilDS, nullptr);
		mSkyBoxShaderProgram = std::make_unique<ShaderProgram>(mDevice, skyBoxVS, nullptr, skyboxPS, pipelieState);

		mDefaultSS = resourceManager->Create<D3D11SamplerState>(ED3D11SamplerState::Default);

		std::vector<DirectX::SimpleMath::Vector3> positions =
		{
			{ DirectX::SimpleMath::Vector3(-1.0f, -1.0f, -1.0f)}, // 0
			{ DirectX::SimpleMath::Vector3(-1.0f, +1.0f, -1.0f)},  // 1
			{ DirectX::SimpleMath::Vector3(+1.0f, +1.0f, -1.0f)},  // 2
			{ DirectX::SimpleMath::Vector3(+1.0f, -1.0f, -1.0f)},  // 3							   					   
			{ DirectX::SimpleMath::Vector3(-1.0f, -1.0f, +1.0f)}, // 4
			{ DirectX::SimpleMath::Vector3(-1.0f, +1.0f, +1.0f)}, // 5
			{ DirectX::SimpleMath::Vector3(+1.0f, +1.0f, +1.0f)}, // 6
			{ DirectX::SimpleMath::Vector3(+1.0f, -1.0f, +1.0f)} // 7
		};

		std::vector<unsigned int> indices =
		{
			0, 1, 2,
			0, 2, 3,

			4, 6, 5,
			4, 7, 6,

			4, 5, 1,
			4, 1, 0,

			3, 2, 6,
			3, 6, 7,

			1, 5, 6,
			1, 6, 2,

			4, 0, 3,
			4, 3, 7
		};

		mSkyBoxVB = std::make_shared<D3D11VertexBuffer>(device, positions);
		mSkyboxIB = std::make_shared<D3D11IndexBuffer>(device, indices);

		mDrawRTV = mResourceManager->Get<D3D11RenderTargetView>(ED3D11RenderTargetViewType::Offscreen);
		mDrawDSV = mResourceManager->Get<D3D11DepthStencilView>(ED3D11DepthStencilViewType::Default);

		mDefaultSS = mResourceManager->Get<D3D11SamplerState>(ED3D11SamplerState::Default);

		mViewProjectionMatrix = mResourceManager->Create<D3D11ConstantBuffer<ViewRotationProjectionMatrix>>(ED3D11ConstantBuffer::ViewRotationProj);
	}

	void SkyBoxPass::Finalize()
	{
		mDevice = nullptr;
		mCameraManager = nullptr;
		mResourceManager = nullptr;

		mSkyBoxShaderProgram = nullptr;

		mSkyBoxVB = nullptr;
		mSkyboxIB = nullptr;

		mDrawRTV = nullptr;
		mDrawDSV = nullptr;
		mSkyBoxTexture = nullptr;

		mDefaultSS = nullptr;

		mViewProjectionMatrix = nullptr;
	}

	void SkyBoxPass::Render()
	{
		if (!isSetSkyBox)
		{
			return;
		}

		mDevice->GetDeviceContext()->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		mDrawRTV->Bind(mDevice, mDrawDSV);

		mSkyBoxShaderProgram->Bind(mDevice);

		mSkyBoxVB->Bind(mDevice);
		mSkyboxIB->Bind(mDevice);

		mDefaultSS->Bind(mDevice, 0, ED3D11ShaderType::Pixelshader);

		mSkyBoxTexture->Bind(mDevice, 0, ED3D11ShaderType::Pixelshader);

		ViewRotationProjectionMatrix viewProjectionMatrix;
		DirectX::SimpleMath::Matrix view = mCameraManager->GetViewMatrix(ECameraType::Player);
		view._41 = 0;
		view._42 = 0;
		view._43 = 0;
		viewProjectionMatrix.ViewProjMat = (view * mCameraManager->GetProjectionMatrix(ECameraType::Player)).Transpose();

		mViewProjectionMatrix->Update(mDevice, viewProjectionMatrix);
		mViewProjectionMatrix->Bind(mDevice, ED3D11ShaderType::VertexShader, 0);

		mDevice->GetDeviceContext()->DrawIndexed(36, 0, 0);
	}

	void SkyBoxPass::SetSkyBox(const std::wstring& path)
	{
		if (path == L"")
		{
			isSetSkyBox = false;
			return;
		}

		isSetSkyBox = true;
		mSkyBoxTexture = mResourceManager->Create<D3D11Texture>(path);
	}
}