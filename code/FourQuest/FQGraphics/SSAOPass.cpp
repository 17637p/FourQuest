#include "SSAOPass.h"

#include "ManagementCommon.h"
#include "D3D11Common.h"

void fq::graphics::SSAOPass::Initialize(std::shared_ptr<D3D11Device> device, 
	std::shared_ptr<D3D11JobManager> jobManager, 
	std::shared_ptr<D3D11CameraManager> cameraManager, 
	std::shared_ptr<D3D11ResourceManager> resourceManager, 
	unsigned short width, unsigned short height)
{
	mDevice = device;
	mJobManager = jobManager;
	mCameraManager = cameraManager;
	mResourceManager = resourceManager;

	OnResize(width, height);

	auto SSAOVS = std::make_shared<D3D11VertexShader>(mDevice, L"./resource/internal/shader/SSAOVS.hlsl");
	auto SSAOPS = std::make_shared<D3D11PixelShader>(mDevice, L"./resource/internal/shader/SSAOPS.hlsl");
	auto pipelieState = std::make_shared<PipelineState>(nullptr, nullptr, nullptr);

	mSSAOPassShaderProgram = std::make_unique<ShaderProgram>(mDevice, SSAOVS, nullptr, SSAOPS, pipelieState);

	struct Vertex
	{
		DirectX::SimpleMath::Vector3 Position;
		DirectX::SimpleMath::Vector3 ToFarPlane;
	};

	std::vector<Vertex> positions =
	{
		{{ -1, 1, 0 }, {1, 0, 0}},
		{{ 1, 1, 0 }, {2, 0, 0}},
		{{ -1, -1, 0 }, {0, 0, 0}},
		{{ 1, -1, 0 }, {3, 0, 0}}
	};

	std::vector<unsigned int> indices =
	{
		0,1,2,
		1,3,2
	};

	mFullScreenVB = std::make_shared<D3D11VertexBuffer>(device, positions);
	mFullScreenIB = std::make_shared<D3D11IndexBuffer>(device, indices);

	mFrustumCornersCB = std::make_shared<D3D11ConstantBuffer<FrustumCorners>>(mDevice, ED3D11ConstantBuffer::Transform);

	mCameraManager->RegisterOnSetCamera(
		[this]() 
		{
			float fovY = mCameraManager->GetFovY(ECameraType::Player);
			float farZ = mCameraManager->GetFarPlane(ECameraType::Player);
			this->BuildFrustumFarCorners(fovY, farZ);
		});
}

void fq::graphics::SSAOPass::Finalize()
{

}

void fq::graphics::SSAOPass::Render()
{
	// update
	{
		// View Matrix를 따로 전달해줘야 함!
		//ViewProjectionMatrix viewProjectionMatrix;
		//viewProjectionMatrix.ViewMatrix = mCameraManager->GetViewMatrix(ECameraType::Player);
		//viewProjectionMatrix.ViewMatrix = viewProjectionMatrix.ViewMatrix.Transpose();
		//viewProjectionMatrix.ViewProjMat = mCameraManager->GetViewMatrix(ECameraType::Player) * mCameraManager->GetProjectionMatrix(ECameraType::Player);
		//viewProjectionMatrix.ViewProjMat = viewProjectionMatrix.ViewProjMat.Transpose();
		//mViewProjectionMatrix->Update(mDevice, viewProjectionMatrix);
	}

	// Init
	{
		ID3D11ShaderResourceView* NullSRVs[10] = { NULL, };
		mDevice->GetDeviceContext()->PSSetShaderResources(0, ARRAYSIZE(NullSRVs), NullSRVs);

		mSSAORTV->Clear(mDevice, { 0.f, 0.f, 0.f, 0.f });
	}

	// Bind
	{
		mSSAORTV->Bind(mDevice, mNoneDSV);

		mDevice->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		mFullScreenVB->Bind(mDevice);
		mFullScreenIB->Bind(mDevice);

		//mSingleColorSRV->Bind(mDevice, 0, ED3D11ShaderType::PixelShader);
		//mModelTransformCB->Bind(mDevice, ED3D11ShaderType::VertexShader);
		//mViewProjectionMatrix->Bind(mDevice, ED3D11ShaderType::VertexShader, 1);
	}

	// Draw
	{
		mDevice->GetDeviceContext()->DrawIndexed(6, 0, 0);
	}
}

void fq::graphics::SSAOPass::OnResize(unsigned short width, unsigned short height)
{
	mWidth = width / 4;
	mHeight = height / 4;

	// 크기 바뀌었을 때도 해야하지만 카메라 바뀌었을 때도 해야됨 
	BuildFrustumFarCorners(mCameraManager->GetFovY(ECameraType::Player), mCameraManager->GetFarPlane(ECameraType::Player));

	mSSAORTV = mResourceManager->Create<D3D11RenderTargetView>(ED3D11RenderTargetViewType::SSAO, mWidth, mHeight);
	mNoneDSV = mResourceManager->Get<D3D11DepthStencilView>(ED3D11DepthStencilViewType::None);
	auto SSAODepthRTV = mResourceManager->Get<D3D11RenderTargetView>(ED3D11RenderTargetViewType::SSAODepth);
	mSSAODepthSRV = std::make_shared<D3D11ShaderResourceView>(mDevice, SSAODepthRTV);
}

void fq::graphics::SSAOPass::BuildFrustumFarCorners(float fovy, float farZ)
{
	float aspect = (float)mWidth / (float)mHeight;

	float halfHeight = farZ * tanf(0.f * fovy);
	float halfWidth = aspect * halfHeight;

	mFrustumCorner.FrustumFarCorners[0] = DirectX::SimpleMath::Vector4{ -halfWidth, -halfHeight, farZ, 0.0f };
	mFrustumCorner.FrustumFarCorners[1] = DirectX::SimpleMath::Vector4{ -halfWidth, +halfHeight, farZ, 0.0f };
	mFrustumCorner.FrustumFarCorners[2] = DirectX::SimpleMath::Vector4{ +halfWidth, +halfHeight, farZ, 0.0f };
	mFrustumCorner.FrustumFarCorners[3] = DirectX::SimpleMath::Vector4{ +halfWidth, -halfHeight, farZ, 0.0f };
}
