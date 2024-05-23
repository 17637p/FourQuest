#include "SingleColorPass.h"
#include "D3D11Common.h"
#include "Mesh.h"
#include "Material.h"

#include "ManagementCommon.h"

#include <IFQRenderObject.h>

fq::graphics::SingleColorPass::SingleColorPass()
{

}

void fq::graphics::SingleColorPass::Initialize(std::shared_ptr<D3D11Device> device, 
	std::shared_ptr<D3D11JobManager> jobManager,
	std::shared_ptr<D3D11CameraManager> cameraManager, 
	std::shared_ptr<D3D11ResourceManager> resourceManager, 
	unsigned short width, 
	unsigned short height)
{
	mDevice = device;
	mJobManager = jobManager;
	mCameraManager = cameraManager;
	mResourceManager = resourceManager;

	auto singleColorVS = std::make_shared<D3D11VertexShader>(device, L"./resource/internal/shader/SingleColorVS.hlsl");
	auto singleColorPS = std::make_shared<D3D11PixelShader>(device, L"./resource/internal/shader/SingleColorPS.hlsl");
	auto pipelieState = std::make_shared<PipelineState>(nullptr, nullptr, nullptr);
	mSingleColorPassShaderProgram = std::make_unique<ShaderProgram>(mDevice, singleColorVS, nullptr, singleColorPS, pipelieState);

	mModelTransformCB = std::make_shared<D3D11ConstantBuffer<ModelTransform>>(mDevice, ED3D11ConstantBuffer::Transform);
	mSceneTransformCB = std::make_shared<D3D11ConstantBuffer<SceneTrnasform>>(mDevice, ED3D11ConstantBuffer::Transform);
	mBoneTransformCB = std::make_shared<D3D11ConstantBuffer<BoneTransform>>(mDevice, ED3D11ConstantBuffer::Transform);
	mOutlineColorCB = std::make_shared<D3D11ConstantBuffer<OutLineColor>>(mDevice, ED3D11ConstantBuffer::Transform);

	mSingleColorRTV = mResourceManager->Create<fq::graphics::D3D11RenderTargetView>(ED3D11RenderTargetViewType::SingleColor, width, height);
	mDSV = mResourceManager->Get<fq::graphics::D3D11DepthStencilView>(ED3D11DepthStencilViewType::Default);
}

void fq::graphics::SingleColorPass::Finalize()
{

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
		mSingleColorRTV->Clear(mDevice, { 1,1,1,1 });
		mDSV->Clear(mDevice);
	}

	// Bind
	{
		mDevice->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		mSingleColorRTV->Bind(mDevice, mDSV);

		mModelTransformCB->Bind(mDevice, ED3D11ShaderType::VertexShader);
		mSceneTransformCB->Bind(mDevice, ED3D11ShaderType::VertexShader, 1);
		mBoneTransformCB->Bind(mDevice, ED3D11ShaderType::VertexShader, 2);
		mOutlineColorCB->Bind(mDevice, ED3D11ShaderType::VertexShader, 3);

		mSingleColorPassShaderProgram->Bind(mDevice);

		// Color 값 있는 친구들 (-1, -1, -1 이 아닌 object 들만 출력)
		for (const SkinnedMeshJob& job : mJobManager->GetSkinnedMeshJobs())
		{
			OutLineColor outlineColor;
			outlineColor.color = job.tempObject->GetOutlineColor();

			mOutlineColorCB->Update(mDevice, outlineColor);

			job.SkinnedMesh->Bind(mDevice);
			job.Material->Bind(mDevice);

			ConstantBufferHelper::UpdateModelTransformCB(mDevice, mModelTransformCB, *job.TransformPtr);
			ConstantBufferHelper::UpdateBoneTransformCB(mDevice, mBoneTransformCB, *job.BoneMatricesPtr);

			job.SkinnedMesh->Draw(mDevice, job.SubsetIndex);
		}
	}
}
