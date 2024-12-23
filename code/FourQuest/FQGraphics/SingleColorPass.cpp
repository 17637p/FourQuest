#include "SingleColorPass.h"
#include "D3D11Common.h"
#include "Mesh.h"
#include "Material.h"
#include "RenderObject.h"
#include "ManagementCommon.h"

#include <IFQRenderObject.h>

void fq::graphics::SingleColorPass::Initialize(std::shared_ptr<D3D11Device> device,
	std::shared_ptr<D3D11JobManager> jobManager,
	std::shared_ptr<D3D11CameraManager> cameraManager,
	std::shared_ptr<D3D11ResourceManager> resourceManager,
	unsigned short width,
	unsigned short height)
{
	Finalize();

	mDevice = device;
	mJobManager = jobManager;
	mCameraManager = cameraManager;
	mResourceManager = resourceManager;

	auto singleColorStaticVS = std::make_shared<D3D11VertexShader>(mDevice, L"SingleColorVS.cso");
	auto singleColorSkinnedVS = std::make_shared<D3D11VertexShader>(device, L"SingleColorVS_SKINNING.cso");
	auto singleColorPS = std::make_shared<D3D11PixelShader>(device, L"SingleColorPS.cso");

	mLessEqualDS = resourceManager->Get<D3D11DepthStencilState>(ED3D11DepthStencilState::LessEqual);

	auto pipelieState = std::make_shared<PipelineState>(nullptr, mLessEqualDS, nullptr);
	mSingleColorStaticMeshPassShaderProgram = std::make_unique<ShaderProgram>(mDevice, singleColorStaticVS, nullptr, singleColorPS, pipelieState);
	mSingleColorSkinnedMeshPassShaderProgram = std::make_unique<ShaderProgram>(mDevice, singleColorSkinnedVS, nullptr, singleColorPS, pipelieState);

	mModelTransformCB = std::make_shared<D3D11ConstantBuffer<ModelTransform>>(mDevice, ED3D11ConstantBuffer::Transform);
	mSceneTransformCB = std::make_shared<D3D11ConstantBuffer<SceneTrnasform>>(mDevice, ED3D11ConstantBuffer::Transform);
	mBoneTransformCB = std::make_shared<D3D11ConstantBuffer<BoneTransform>>(mDevice, ED3D11ConstantBuffer::Transform);
	mOutlineColorCB = std::make_shared<D3D11ConstantBuffer<OutLineColor>>(mDevice, ED3D11ConstantBuffer::Transform);

	mSingleColorRTV = mResourceManager->Create<fq::graphics::D3D11RenderTargetView>(ED3D11RenderTargetViewType::SingleColor, width, height);
	mDSV = mResourceManager->Get<D3D11DepthStencilView>(ED3D11DepthStencilViewType::Default);
}

void fq::graphics::SingleColorPass::Finalize()
{
	mDevice = nullptr;
	mJobManager = nullptr;
	mCameraManager = nullptr;
	mResourceManager = nullptr;

	mSingleColorRTV = nullptr;
	mDSV = nullptr;

	mLessEqualDS = nullptr;

	mSingleColorStaticMeshPassShaderProgram = nullptr;
	mSingleColorSkinnedMeshPassShaderProgram = nullptr;

	mModelTransformCB = nullptr;
	mSceneTransformCB = nullptr;
	mBoneTransformCB = nullptr;
	mOutlineColorCB = nullptr;
}

void fq::graphics::SingleColorPass::Render()
{
	// Update
	{
		SceneTrnasform sceneTransform;
		sceneTransform.ViewProjMat = mCameraManager->GetViewMatrix(ECameraType::Player) * mCameraManager->GetProjectionMatrix(ECameraType::Player);
		sceneTransform.ViewProjMat = sceneTransform.ViewProjMat.Transpose();
		mSceneTransformCB->Update(mDevice, sceneTransform);
	}

	// Init
	{
		mSingleColorRTV->Clear(mDevice, { 0,0,0,1 });
	}

	// Bind
	{
		mDevice->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		mSingleColorRTV->Bind(mDevice, mDSV);

		mModelTransformCB->Bind(mDevice, ED3D11ShaderType::VertexShader);
		mSceneTransformCB->Bind(mDevice, ED3D11ShaderType::VertexShader, 1);
		mOutlineColorCB->Bind(mDevice, ED3D11ShaderType::VertexShader, 3);

		mSingleColorStaticMeshPassShaderProgram->Bind(mDevice);

		for (const StaticMeshJob& job : mJobManager->GetStaticMeshJobs())
		{
			const MaterialInfo& materialInfo = job.Material->GetInfo();

			if (materialInfo.RenderModeType == MaterialInfo::ERenderMode::Opaque)
			{
				const MeshObjectInfo& meshObjectInfo = job.StaticMeshObject->GetMeshObjectInfo();

				OutLineColor outlineColor;
				outlineColor.color = meshObjectInfo.OutlineColor;
				if (outlineColor.color.R() < 0 ||
					outlineColor.color.G() < 0 ||
					outlineColor.color.B() < 0 ||
					(outlineColor.color.R() == 0 && outlineColor.color.G() == 0 && outlineColor.color.B() == 0))
				{
					continue;
				}

				mOutlineColorCB->Update(mDevice, outlineColor);

				job.StaticMesh->Bind(mDevice);
				job.Material->Bind(mDevice);

				ConstantBufferHelper::UpdateModelTransformCB(mDevice, mModelTransformCB, job.StaticMeshObject->GetTransform());

				job.StaticMesh->Draw(mDevice, job.SubsetIndex);
			}
		}

		mSingleColorSkinnedMeshPassShaderProgram->Bind(mDevice);
		mBoneTransformCB->Bind(mDevice, ED3D11ShaderType::VertexShader, 2);
		std::vector<DirectX::SimpleMath::Matrix> identityTransform(BoneTransform::MAX_BOND_COUNT);

		// Color 값 있는 친구들 (-1, -1, -1 이 아닌 object 들만 출력)
		for (const SkinnedMeshJob& job : mJobManager->GetSkinnedMeshJobs())
		{
			OutLineColor outlineColor;
			outlineColor.color = job.SkinnedMeshObject->GetMeshObjectInfo().OutlineColor;
			if (outlineColor.color.R() == -1)
			{
				continue;
			}

			mOutlineColorCB->Update(mDevice, outlineColor);

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

void fq::graphics::SingleColorPass::OnResize(unsigned short width, unsigned short height)
{
	mSingleColorRTV->OnResize(mDevice, ED3D11RenderTargetViewType::SingleColor, width, height);
}
