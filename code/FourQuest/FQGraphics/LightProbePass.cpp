#include "LightProbePass.h"
#include "D3D11Common.h"
#include "Mesh.h"
#include "Material.h"

#include "ManagementCommon.h"

#include <IFQRenderObject.h>

void fq::graphics::LightProbePass::Initialize(std::shared_ptr<D3D11Device> device,
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

	r = new float[9];
	g = new float[9];
	b = new float[9];
}

void fq::graphics::LightProbePass::Finalize()
{
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
	if (!isSetProbe)
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

	// Init
	{
		mLightProbeIrrRTV->Clear(mDevice, { 0,0,0,1 });
	}

	// Bind
	{
		mDevice->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		mLightProbeIrrRTV->Bind(mDevice, mDSV);

		mModelTransformCB->Bind(mDevice, ED3D11ShaderType::VertexShader);
		mSceneTransformCB->Bind(mDevice, ED3D11ShaderType::VertexShader, 1);

		// pixel shader�� ���ε�
		mLightProbeCB->Bind(mDevice, ED3D11ShaderType::PixelShader, 0);

		mLightProbePassStaticMeshPassShaderProgram->Bind(mDevice);

		for (const StaticMeshJob& job : mJobManager->GetStaticMeshJobs())
		{
			if (job.ObjectRenderType == EObjectRenderType::Opaque)
			{
				ConstantBufferHelper::UpdateLightProbeCB(mDevice, mLightProbeCB, r, g, b);

				job.StaticMesh->Bind(mDevice);
				job.Material->Bind(mDevice);

				ConstantBufferHelper::UpdateModelTransformCB(mDevice, mModelTransformCB, *job.TransformPtr);

				job.StaticMesh->Draw(mDevice, job.SubsetIndex);
			}
		}

		mLightProbePassSkinnedMeshPassShaderProgram->Bind(mDevice);
		mBoneTransformCB->Bind(mDevice, ED3D11ShaderType::VertexShader, 2);

		for (const SkinnedMeshJob& job : mJobManager->GetSkinnedMeshJobs())
		{
			ConstantBufferHelper::UpdateLightProbeCB(mDevice, mLightProbeCB, r, g, b);

			job.SkinnedMesh->Bind(mDevice);
			job.Material->Bind(mDevice);

			ConstantBufferHelper::UpdateModelTransformCB(mDevice, mModelTransformCB, *job.TransformPtr);
			ConstantBufferHelper::UpdateBoneTransformCB(mDevice, mBoneTransformCB, *job.BoneMatricesPtr);

			job.SkinnedMesh->Draw(mDevice, job.SubsetIndex);
		}
	}
}

void fq::graphics::LightProbePass::OnResize(unsigned short width, unsigned short height)
{
	mLightProbeIrrRTV->OnResize(mDevice, ED3D11RenderTargetViewType::LightProbeIrr, width, height);
}

void fq::graphics::LightProbePass::SetLightProbe(const std::wstring& texturePaths)
{
	if (texturePaths.size() == 0)
	{
		isSetProbe = false;
		return;
	}

	isSetProbe = true;
	mProbeTexture = mResourceManager->Create<D3D11Texture>(texturePaths);

	mProbeTexture->GetSHCoefficientRGB(mDevice, r, g, b);

	int a = 3;
}