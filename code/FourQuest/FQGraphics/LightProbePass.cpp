#include "LightProbePass.h"
#include "D3D11Common.h"

#include "RenderObject.h"
#include "Mesh.h"
#include "Material.h"

#include "ManagementCommon.h"

#include <IFQRenderObject.h>

void fq::graphics::LightProbePass::Initialize(std::shared_ptr<D3D11Device> device,
	std::shared_ptr<D3D11JobManager> jobManager,
	std::shared_ptr<D3D11CameraManager> cameraManager,
	std::shared_ptr<D3D11ResourceManager> resourceManager,
	std::shared_ptr<D3D11LightProbeManager> lightProbeManager,
	unsigned short width,
	unsigned short height)
{
	Finalize();

	mDevice = device;
	mJobManager = jobManager;
	mCameraManager = cameraManager;
	mResourceManager = resourceManager;
	mLightProbeManager = lightProbeManager;

	auto LightProbeStaticVS = std::make_shared<D3D11VertexShader>(mDevice, L"LightProbeVS.cso");
	auto LightProbeSkinnedVS = std::make_shared<D3D11VertexShader>(device, L"LightProbeVS_SKINNING.cso");
	auto LightProbePS = std::make_shared<D3D11PixelShader>(device, L"LightProbePS.cso");

	mLessEqualDS = resourceManager->Get<D3D11DepthStencilState>(ED3D11DepthStencilState::LessEqual);

	auto pipelieState = std::make_shared<PipelineState>(nullptr, mLessEqualDS, nullptr);
	mLightProbePassStaticMeshPassShaderProgram = std::make_unique<ShaderProgram>(mDevice, LightProbeStaticVS, nullptr, LightProbePS, pipelieState);
	mLightProbePassSkinnedMeshPassShaderProgram = std::make_unique<ShaderProgram>(mDevice, LightProbeSkinnedVS, nullptr, LightProbePS, pipelieState);

	mModelTransformCB = std::make_shared<D3D11ConstantBuffer<ModelTransform>>(mDevice, ED3D11ConstantBuffer::Transform);
	mSceneTransformCB = std::make_shared<D3D11ConstantBuffer<SceneTrnasform>>(mDevice, ED3D11ConstantBuffer::Transform);
	mBoneTransformCB = std::make_shared<D3D11ConstantBuffer<BoneTransform>>(mDevice, ED3D11ConstantBuffer::Transform);
	mLightProbeCB = std::make_shared<D3D11ConstantBuffer<LightProbeCB>>(mDevice, ED3D11ConstantBuffer::Transform);

	mLightProbeIrrRTV = mResourceManager->Create<fq::graphics::D3D11RenderTargetView>(ED3D11RenderTargetViewType::LightProbeIrr, width, height);

	mDSV = mResourceManager->Get<D3D11DepthStencilView>(ED3D11DepthStencilViewType::Default);
	mDefaultSS = resourceManager->Get<D3D11SamplerState>(ED3D11SamplerState::Default);

	r = new float[9];
	g = new float[9];
	b = new float[9];
}

void fq::graphics::LightProbePass::Finalize()
{
	delete[] r;
	delete[] g;
	delete[] b;

	mDevice = nullptr;
	mJobManager = nullptr;
	mCameraManager = nullptr;
	mResourceManager = nullptr;

	mLightProbeIrrRTV = nullptr;
	mDSV = nullptr;

	mLessEqualDS = nullptr;

	mLightProbePassStaticMeshPassShaderProgram = nullptr;
	mLightProbePassSkinnedMeshPassShaderProgram = nullptr;

	mModelTransformCB = nullptr;
	mSceneTransformCB = nullptr;
	mBoneTransformCB = nullptr;
	mLightProbeCB = nullptr;
}

void fq::graphics::LightProbePass::Render()
{
	// Init
	{
		mLightProbeIrrRTV->Clear(mDevice, { 0,0,0,1 });
	}

	if (!mLightProbeManager->GetIsUsedLightProbe())
	{
		return;
	}

	// Update
	{
		SceneTrnasform sceneTransform;
		sceneTransform.ViewProjMat = mCameraManager->GetViewMatrix(ECameraType::Player) * mCameraManager->GetProjectionMatrix(ECameraType::Player);
		sceneTransform.ViewProjMat = sceneTransform.ViewProjMat.Transpose();
		mSceneTransformCB->Update(mDevice, sceneTransform);
	}

	// Bind
	{
		mDevice->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		mLightProbeIrrRTV->Bind(mDevice, mDSV);

		mModelTransformCB->Bind(mDevice, ED3D11ShaderType::VertexShader);
		mSceneTransformCB->Bind(mDevice, ED3D11ShaderType::VertexShader, 1);

		// pixel shader에 바인드
		mLightProbeCB->Bind(mDevice, ED3D11ShaderType::PixelShader, 0);
		mDefaultSS->Bind(mDevice, 0, ED3D11ShaderType::PixelShader);

		mLightProbePassStaticMeshPassShaderProgram->Bind(mDevice);

		for (const StaticMeshJob& job : mJobManager->GetStaticMeshJobs())
		{
			const MaterialInfo& materialInfo = job.Material->GetInfo();

			bool isUseLightProbe = job.StaticMeshObject->GetMeshObjectInfo().bUseLightProbe;
			if (materialInfo.RenderModeType == MaterialInfo::ERenderMode::Opaque && isUseLightProbe)
			{
				int tetIndex = 0;
				DirectX::SimpleMath::Vector4 weights;
				// position 가져오기
				DirectX::SimpleMath::Matrix matrix = job.StaticMeshObject->GetTransform();
				mLightProbeManager->GetTetIndex(tetIndex, { matrix._41, matrix._42, matrix._43 }, weights);

				mLightProbeManager->GetCoefficientTetrahedronWeight(weights, tetIndex, r, g, b);

				ConstantBufferHelper::UpdateLightProbeCB(mDevice, mLightProbeCB, r, g, b);

				job.StaticMesh->Bind(mDevice);
				job.Material->Bind(mDevice);

				ConstantBufferHelper::UpdateModelTransformCB(mDevice, mModelTransformCB, job.StaticMeshObject->GetTransform());

				job.StaticMesh->Draw(mDevice, job.SubsetIndex);
			}
		}

		mLightProbePassSkinnedMeshPassShaderProgram->Bind(mDevice);
		mBoneTransformCB->Bind(mDevice, ED3D11ShaderType::VertexShader, 2);

		std::vector<DirectX::SimpleMath::Matrix> identityTransforms(BoneTransform::MAX_BOND_COUNT);

		for (const SkinnedMeshJob& job : mJobManager->GetSkinnedMeshJobs())
		{
			job.SkinnedMesh->Bind(mDevice);
			job.Material->Bind(mDevice);

			ConstantBufferHelper::UpdateModelTransformCB(mDevice, mModelTransformCB, job.SkinnedMeshObject->GetTransform());
			ConstantBufferHelper::UpdateBoneTransformCB(mDevice, mBoneTransformCB, identityTransforms);

			job.SkinnedMesh->Draw(mDevice, job.SubsetIndex);
		}
	}

	// UnBind
	{

	}
}

void fq::graphics::LightProbePass::OnResize(unsigned short width, unsigned short height)
{
	mLightProbeIrrRTV->OnResize(mDevice, ED3D11RenderTargetViewType::LightProbeIrr, width, height);
	mDSV = mResourceManager->Get<D3D11DepthStencilView>(ED3D11DepthStencilViewType::Default);
}



