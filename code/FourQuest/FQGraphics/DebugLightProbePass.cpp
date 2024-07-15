#include "DebugLightProbePass.h"

#include "D3D11Common.h"
#include "ManagementCommon.h"

#include "RenderObject.h"

void fq::graphics::DebugLightProbePass::Initialize(std::shared_ptr<D3D11Device> device, std::shared_ptr<D3D11ObjectManager> objectManager, std::shared_ptr<D3D11CameraManager> cameraManager, std::shared_ptr<D3D11ResourceManager> resourceManager, std::shared_ptr<D3D11LightProbeManager> lightProbeManager, unsigned short width, unsigned short height)
{
	Finalize();

	mDevice = device;
	mObjectManager = objectManager;
	mCameraManager = cameraManager;
	mResourceManager = resourceManager;
	mLightProbeManager = lightProbeManager;

	auto debugLightProbeVS = std::make_shared<D3D11VertexShader>(mDevice, L"DebugLightProbeVS.cso");
	auto debugLightProbePS = std::make_shared<D3D11PixelShader>(device, L"DebugLightProbePS.cso");

	mDefaultDS = resourceManager->Create<D3D11DepthStencilState>(ED3D11DepthStencilState::Default);
	auto pipelieState = std::make_shared<PipelineState>(nullptr, mDefaultDS, nullptr);
	mDebugLightProbePassPassShaderProgram = std::make_unique<ShaderProgram>(mDevice, debugLightProbeVS, nullptr, debugLightProbePS, pipelieState);

	mModelTransformCB = std::make_shared<D3D11ConstantBuffer<ModelTransform>>(mDevice, ED3D11ConstantBuffer::Transform);
	mSceneTransformCB = std::make_shared<D3D11ConstantBuffer<SceneTrnasform>>(mDevice, ED3D11ConstantBuffer::Transform);
	mLightProbeCB = std::make_shared<D3D11ConstantBuffer<LightProbeCB>>(mDevice, ED3D11ConstantBuffer::Transform);

	mBackBuffer = mResourceManager->Get<fq::graphics::D3D11RenderTargetView>(ED3D11RenderTargetViewType::Offscreen);

	mDSV = mResourceManager->Get<D3D11DepthStencilView>(ED3D11DepthStencilViewType::Default); // object 그린거 받아와야 함
}

void fq::graphics::DebugLightProbePass::Finalize()
{

}

void fq::graphics::DebugLightProbePass::OnResize(unsigned short width, unsigned short height)
{
	mBackBuffer = mResourceManager->Get<fq::graphics::D3D11RenderTargetView>(ED3D11RenderTargetViewType::Offscreen);
	mDSV = mResourceManager->Get<D3D11DepthStencilView>(ED3D11DepthStencilViewType::Default); // object 그린거 받아와야 함
}

void fq::graphics::DebugLightProbePass::Render()
{
	if (!mLightProbeManager->GetIsDrawDebugLightProbe())
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

		mBackBuffer->Bind(mDevice, mDSV);

		mModelTransformCB->Bind(mDevice, ED3D11ShaderType::VertexShader);
		mSceneTransformCB->Bind(mDevice, ED3D11ShaderType::VertexShader, 1);

		// pixel shader에 바인드
		mLightProbeCB->Bind(mDevice, ED3D11ShaderType::PixelShader, 0);

		mDebugLightProbePassPassShaderProgram->Bind(mDevice);

		for (IProbeObject* probeObject : mObjectManager->GetProbeObjects())
		{
			float* coefficient = mLightProbeManager->GetLightProbeCoefficient(probeObject->GetIndex());

			float* g = coefficient + 9;
			float* b = coefficient + 18;

			ConstantBufferHelper::UpdateLightProbeCB(mDevice, mLightProbeCB, coefficient, g, b, 1);

			std::shared_ptr<graphics::IStaticMesh> iStaticMesh = probeObject->GetStaticMesh();
			std::shared_ptr<graphics::StaticMesh> staticMesh = std::static_pointer_cast<StaticMesh>(iStaticMesh);

			ConstantBufferHelper::UpdateModelTransformCB(mDevice, mModelTransformCB, probeObject->GetTransform());

			const size_t JOB_COUNT = probeObject->GetStaticMesh()->GetMeshData().Subsets.size();

			staticMesh->Bind(mDevice);
			staticMesh->Draw(mDevice, JOB_COUNT - 1);
		}

		//for (const StaticMeshJob& job : mJobManager->GetStaticMeshJobs())
		{
			//const MaterialInfo& materialInfo = job.Material->GetInfo();
			//
			//bool isUseLightProbe = job.StaticMeshObject->GetMeshObjectInfo().bUseLightProbe;
			//if (materialInfo.RenderModeType == MaterialInfo::ERenderMode::Opaque && isUseLightProbe)
			//{
			//	int tetIndex = 0;
			//	DirectX::SimpleMath::Vector4 weights;
			//	// position 가져오기
			//	DirectX::SimpleMath::Matrix matrix = job.StaticMeshObject->GetTransform();
			//	mLightProbeManager->GetTetIndex(tetIndex, { matrix._41, matrix._42, matrix._43 }, weights);
			//
			//	mLightProbeManager->GetCoefficientTetrahedronWeight(weights, tetIndex, r, g, b);
			//
			//	ConstantBufferHelper::UpdateLightProbeCB(mDevice, mLightProbeCB, r, g, b);
			//
			//	job.StaticMesh->Bind(mDevice);
			//	job.Material->Bind(mDevice);
			//
			//	ConstantBufferHelper::UpdateModelTransformCB(mDevice, mModelTransformCB, job.StaticMeshObject->GetTransform());
			//
			//	job.StaticMesh->Draw(mDevice, job.SubsetIndex);
			//}
		}
	}
}
