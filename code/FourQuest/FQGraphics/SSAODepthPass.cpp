#include "SSAODepthPass.h"

#include "ManagementCommon.h"
#include "D3D11Common.h"
#include "RenderJob.h"
#include "Mesh.h"
#include "Material.h"
#include "NodeHierarchy.h"
#include "RenderObject.h"

#include <random>

void fq::graphics::SSAODepthPass::Initialize(std::shared_ptr<D3D11Device> device,
	std::shared_ptr<D3D11JobManager> jobManager,
	std::shared_ptr<D3D11CameraManager> cameraManager,
	std::shared_ptr<D3D11ResourceManager> resourceManager,
	unsigned short width, unsigned short height)
{
	Finalize();

	mDevice = device;
	mJobManager = jobManager;
	mCameraManager = cameraManager;
	mResourceManager = resourceManager;

	mViewDepthRTV = mResourceManager->Create<D3D11RenderTargetView>(ED3D11RenderTargetViewType::SSAODepth, width, height);
	mDSV = mResourceManager->Get<D3D11DepthStencilView>(ED3D11DepthStencilViewType::Default);

	auto staticMeshVS = std::make_shared<D3D11VertexShader>(mDevice, L"SSAODepthVS.cso");
	auto skinnedMeshVS = std::make_shared<D3D11VertexShader>(mDevice, L"SSAODepthVS_SKINNING.cso");
	auto SSAODepthPS = std::make_shared<D3D11PixelShader>(mDevice, L"SSAODepthPS.cso");
	auto pipelieState = std::make_shared<PipelineState>(nullptr, nullptr, nullptr);

	mSSAOViewDepthStaticMeshPassShaderProgram = std::make_unique<ShaderProgram>(mDevice, staticMeshVS, nullptr, SSAODepthPS, pipelieState);
	mSSAOViewDepthskinnedMeshPassShaderProgram = std::make_unique<ShaderProgram>(mDevice, skinnedMeshVS, nullptr, SSAODepthPS, pipelieState);

	mModelTransformCB = std::make_shared<D3D11ConstantBuffer<ModelTransform>>(mDevice, ED3D11ConstantBuffer::Transform);
	mViewProjectionMatrix = std::make_shared<D3D11ConstantBuffer<ViewProjectionMatrix>>(mDevice, ED3D11ConstantBuffer::Transform);
	mBoneTransformCB = std::make_shared<D3D11ConstantBuffer<BoneTransform>>(mDevice, ED3D11ConstantBuffer::Transform);
}

void fq::graphics::SSAODepthPass::Finalize()
{
	mDevice = nullptr;
	mJobManager = nullptr;
	mCameraManager = nullptr;
	mResourceManager = nullptr;

	mViewDepthRTV = nullptr;
	mDSV = nullptr;

	mSSAOViewDepthStaticMeshPassShaderProgram = nullptr;
	mSSAOViewDepthskinnedMeshPassShaderProgram = nullptr;

	mModelTransformCB = nullptr;
	mViewProjectionMatrix = nullptr;
	mBoneTransformCB = nullptr;
}

void fq::graphics::SSAODepthPass::OnResize(unsigned short width, unsigned short height)
{
	mViewDepthRTV = mResourceManager->Create<D3D11RenderTargetView>(ED3D11RenderTargetViewType::SSAODepth, width, height);
}

void fq::graphics::SSAODepthPass::Render()
{
	// update
	{
		// View Matrix를 따로 전달해줘야 함!
		ViewProjectionMatrix viewProjectionMatrix;
		viewProjectionMatrix.ViewMatrix = mCameraManager->GetViewMatrix(ECameraType::Player);
		viewProjectionMatrix.ViewMatrix = viewProjectionMatrix.ViewMatrix.Transpose();
		viewProjectionMatrix.ViewProjMat = mCameraManager->GetViewMatrix(ECameraType::Player) * mCameraManager->GetProjectionMatrix(ECameraType::Player);
		viewProjectionMatrix.ViewProjMat = viewProjectionMatrix.ViewProjMat.Transpose();
		mViewProjectionMatrix->Update(mDevice, viewProjectionMatrix);
	}

	// Init
	{
		ID3D11ShaderResourceView* NullSRVs[10] = { NULL, };
		mDevice->GetDeviceContext()->PSSetShaderResources(0, ARRAYSIZE(NullSRVs), NullSRVs);

		mDSV->Clear(mDevice);
		mViewDepthRTV->Clear(mDevice, { 0.f, 0.f, 0.f, 0.f });
	}

	// Bind
	{
		mViewDepthRTV->Bind(mDevice, mDSV);

		mDevice->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		mModelTransformCB->Bind(mDevice, ED3D11ShaderType::VertexShader);
		mViewProjectionMatrix->Bind(mDevice, ED3D11ShaderType::VertexShader, 1);
	}

	// Draw
	{
		mSSAOViewDepthStaticMeshPassShaderProgram->Bind(mDevice);

		for (const StaticMeshJob& job : mJobManager->GetStaticMeshJobs())
		{
			const MaterialInfo& materialInfo = job.Material->GetInfo();

			if (materialInfo.RenderModeType == MaterialInfo::ERenderMode::Opaque)
			{
				job.StaticMesh->Bind(mDevice);
				job.Material->Bind(mDevice);

				ConstantBufferHelper::UpdateModelTransformCB(mDevice, mModelTransformCB, job.StaticMeshObject->GetTransform());

				job.StaticMesh->Draw(mDevice, job.SubsetIndex);
			}
		}

		mSSAOViewDepthskinnedMeshPassShaderProgram->Bind(mDevice);
		mBoneTransformCB->Bind(mDevice, ED3D11ShaderType::VertexShader, 2);
		std::vector<DirectX::SimpleMath::Matrix> identityTransform(BoneTransform::MAX_BOND_COUNT);

		for (const SkinnedMeshJob& job : mJobManager->GetSkinnedMeshJobs())
		{
			const MaterialInfo& materialInfo = job.Material->GetInfo();

			if (materialInfo.RenderModeType == MaterialInfo::ERenderMode::Opaque)
			{
				job.SkinnedMesh->Bind(mDevice);
				job.Material->Bind(mDevice);

				ConstantBufferHelper::UpdateModelTransformCB(mDevice, mModelTransformCB, job.SkinnedMeshObject->GetTransform());

				if (job.NodeHierarchyInstnace != nullptr)
				{
					ConstantBufferHelper::UpdateBoneTransformCB(mDevice, mBoneTransformCB, job.NodeHierarchyInstnace->GetTransposedFinalTransforms());
				}
				else
				{
					ConstantBufferHelper::UpdateBoneTransformCB(mDevice, mBoneTransformCB, identityTransform);
				}

				job.SkinnedMesh->Draw(mDevice, job.SubsetIndex);
			}
		}
	}
}

void fq::graphics::SSAODepthPass::BuildOffsetVectors()
{
	// 입방체 꼭짓점 여덟 개
	mOffsets.push_back(DirectX::SimpleMath::Vector4{1.0f, 1.0f, 1.0f, 0.0f});
	mOffsets.push_back(DirectX::SimpleMath::Vector4{-1.0f, -1.0f, -1.0f, 0.0f});

	mOffsets.push_back(DirectX::SimpleMath::Vector4{ -1.0f, 1.0f, 1.0f, 0.0f });
	mOffsets.push_back(DirectX::SimpleMath::Vector4{ 1.0f, -1.0f, -1.0f, 0.0f });

	mOffsets.push_back(DirectX::SimpleMath::Vector4{ 1.0f, 1.0f, -1.0f, 0.0f });
	mOffsets.push_back(DirectX::SimpleMath::Vector4{ -1.0f, -1.0f, 1.0f, 0.0f });

	mOffsets.push_back(DirectX::SimpleMath::Vector4{ -1.0f, 1.0f, -1.0f, 0.0f });
	mOffsets.push_back(DirectX::SimpleMath::Vector4{ 1.0f, -1.0f, 1.0f, 0.0f });

	// 입방체 면 중점 여섯 개
	mOffsets.push_back(DirectX::SimpleMath::Vector4{ -1.0f, 0.0f, 0.0f, 0.0f });
	mOffsets.push_back(DirectX::SimpleMath::Vector4{ 1.0f, 0.0f, 0.0f, 0.0f });

	mOffsets.push_back(DirectX::SimpleMath::Vector4{ 0.0f, -1.0f, 0.0f, 0.0f });
	mOffsets.push_back(DirectX::SimpleMath::Vector4{ 0.0f, 1.0f, 0.0f, 0.0f });

	mOffsets.push_back(DirectX::SimpleMath::Vector4{ 0.0f, 0.0f, -1.0f, 0.0f });
	mOffsets.push_back(DirectX::SimpleMath::Vector4{ 0.0f, 0.0f, 1.0f, 0.0f });

	// 랜덤 값 생성
	std::random_device rd;  // 시드 값을 얻기 위한 random_device
	std::mt19937 gen(rd()); // Mersenne Twister 엔진을 시드로 초기화

	// 0.0에서 1.0 사이의 float 값을 생성하기 위한 균등 분포 정의
	std::uniform_real_distribution<> dis(0.25f, 1.0f);

	for (int i = 0; i < 14; i++)
	{
		float s = dis(gen);
		
		mOffsets[i].Normalize();
		mOffsets[i] = s * mOffsets[i];
	}
}
