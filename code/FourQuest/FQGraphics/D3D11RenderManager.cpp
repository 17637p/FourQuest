#include "D3D11RenderManager.h"
#include "D3D11Buffer.h"
#include "D3D11Shader.h"
#include "D3D11State.h"
#include "RenderJob.h"
#include "Mesh.h"
#include "Material.h"

namespace fq::graphics
{
	D3D11RenderManager::D3D11RenderManager(const std::shared_ptr<D3D11Device>& device)
	{
		mStaticMeshVS = std::make_shared<D3D11VertexShader>(device, L"../FQGraphics/ModelVS.hlsl");
		mStaticMeshPS = std::make_shared<D3D11PixelShader>(device, L"../FQGraphics/ModelPS.hlsl");
		mStaticMeshLayout = std::make_shared<D3D11InputLayout>(device, mStaticMeshVS->GetBlob().Get());
		mModelTransformCB = std::make_shared<D3D11ConstantBuffer<ModelTransfrom>>(device, ED3D11ConstantBuffer::Transform);
		mSceneTransformCB = std::make_shared<D3D11ConstantBuffer<SceneTrnasform>>(device, ED3D11ConstantBuffer::Transform);
		mSamplerState = std::make_shared<D3D11SamplerState>(device, ED3D11SamplerState::Default);
	}

	void D3D11RenderManager::Render(const std::shared_ptr<D3D11Device>& device, const std::vector<StaticMeshJob>& staticMeshJobs)
	{
		mStaticMeshLayout->Bind(device);
		mStaticMeshVS->Bind(device);
		mStaticMeshPS->Bind(device);
		mModelTransformCB->Bind(device, ED3D11ShaderType::VertexShader);
		mSceneTransformCB->Bind(device, ED3D11ShaderType::VertexShader, 1);
		mSamplerState->Bind(device, 0, ED3D11ShaderType::Pixelshader);

		SceneTrnasform sceneTransform;

		DirectX::SimpleMath::Matrix view = DirectX::XMMatrixLookAtLH({ 0, 0, -500 }, { 0, 0,0 }, { 0, 1, 0 });
		DirectX::SimpleMath::Matrix proj = DirectX::XMMatrixPerspectiveFovLH(0.25f * 3.14f, 1.f, 1.f, 1000.f); // width / height
		sceneTransform.ViewProjMat = (view * proj).Transpose();
		sceneTransform.ShadowViewProjTexMat;
		mSceneTransformCB->Update(device, sceneTransform);

		device->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		for (const StaticMeshJob& job : staticMeshJobs)
		{
			job.StaticMesh->Bind(device);
			job.Material->Bind(device);

			ModelTransfrom modelTransform;
			modelTransform.WorldMat = *job.TransformPtr;
			modelTransform.WorldInvTransposeMat = (*job.TransformPtr).Transpose();
			mModelTransformCB->Update(device, modelTransform);

			job.StaticMesh->Draw(device, job.SubsetIndex);
		}
	}
}