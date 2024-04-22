#include "D3D11RenderManager.h"
#include "D3D11ResourceManager.h"

#include "D3D11View.h"
#include "D3D11Buffer.h"
#include "D3D11Shader.h"
#include "D3D11State.h"
#include "D3D11Util.h"

#include "RenderJob.h"
#include "Mesh.h"
#include "Material.h"

#include "Define.h"

namespace fq::graphics
{
	void D3D11RenderManager::Initialize(const std::shared_ptr<D3D11Device>& device,
		std::shared_ptr<class D3D11ResourceManager>& resourceManager,
		unsigned short width,
		unsigned short height)
	{
		mRTVRenderer = resourceManager->Create<fq::graphics::D3D11RenderTargetView>(ED3D11RenderTargetViewType::Default, width, height);
		mDSV = resourceManager->Create<fq::graphics::D3D11DepthStencilView>(ED3D11DepthStencilViewType::Default, width, height);

		mStaticMeshVS = std::make_shared<D3D11VertexShader>(device, L"../FQGraphics/ModelVS.hlsl");
		mStaticMeshPS = std::make_shared<D3D11PixelShader>(device, L"../FQGraphics/ModelPS.hlsl");
		mStaticMeshLayout = std::make_shared<D3D11InputLayout>(device, mStaticMeshVS->GetBlob().Get());
		mSamplerState = std::make_shared<D3D11SamplerState>(device, ED3D11SamplerState::Default);

		mModelTransformCB = std::make_shared<D3D11ConstantBuffer<ModelTransfrom>>(device, ED3D11ConstantBuffer::Transform);
		mSceneTransformCB = std::make_shared<D3D11ConstantBuffer<SceneTrnasform>>(device, ED3D11ConstantBuffer::Transform);
		mModelTexutreCB = std::make_shared< D3D11ConstantBuffer<ModelTexutre>>(device, ED3D11ConstantBuffer::Transform);
		mSceneLightCB = std::make_shared<D3D11ConstantBuffer<SceneLight>>(device, ED3D11ConstantBuffer::Transform);

		mViewport.Width = (float)width;
		mViewport.Height = (float)height;
		mViewport.MinDepth = 0.f;
		mViewport.MaxDepth = 1.f;
		mViewport.TopLeftX = 0.f;
		mViewport.TopLeftY = 0.f;
	}

	void D3D11RenderManager::Finalize()
	{

	}

	void D3D11RenderManager::BeginRender(const std::shared_ptr<D3D11Device>& device)
	{
		mDSV->ClearDepth(device);
		mRTVRenderer->Bind(device, mDSV);

		device->GetDeviceContext()->RSSetViewports(1, &mViewport);
	}

	void D3D11RenderManager::EndRender(const std::shared_ptr<D3D11Device>& device)
	{
		Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain = device->GetSwapChain();
		HR(swapChain->Present(0, 0));
	}

	void D3D11RenderManager::OnResize(unsigned short width, unsigned short height)
	{
		mViewport.Width = (float)width;
		mViewport.Height = (float)height;
		mViewport.MinDepth = 0.f;
		mViewport.MaxDepth = 1.f;
		mViewport.TopLeftX = 0.f;
		mViewport.TopLeftY = 0.f;
	}

	void D3D11RenderManager::Render(const std::shared_ptr<D3D11Device>& device, const std::vector<StaticMeshJob>& staticMeshJobs)
	{
		device->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		mStaticMeshLayout->Bind(device);
		mStaticMeshVS->Bind(device);
		mStaticMeshPS->Bind(device);
		mModelTransformCB->Bind(device, ED3D11ShaderType::VertexShader);
		mSceneTransformCB->Bind(device, ED3D11ShaderType::VertexShader, 1);
		mModelTexutreCB->Bind(device, ED3D11ShaderType::Pixelshader);
		mSceneLightCB->Bind(device, ED3D11ShaderType::Pixelshader, 1);
		mSamplerState->Bind(device, 0, ED3D11ShaderType::Pixelshader);

		// 임시 데이터, 카메라 반영시켜줘야 함
		SceneTrnasform sceneTransform;
		DirectX::SimpleMath::Matrix view = DirectX::XMMatrixLookAtLH({ 0, 0, -300 }, { 0, 0,0 }, { 0, 1, 0 });
		DirectX::SimpleMath::Matrix proj = DirectX::XMMatrixPerspectiveFovLH(0.25f * 3.14f, 1.f, 1.f, 1000.f); // width / height
		sceneTransform.ViewProjMat = (view * proj).Transpose();
		sceneTransform.ShadowViewProjTexMat;
		mSceneTransformCB->Update(device, sceneTransform);

		// 임시 데이터, 조명 반영시켜줘야 함
		SceneLight scenelight;
		scenelight.Lights[0].Direction = { -1.0f, 0.0f, 0.f };
		scenelight.Lights[0].Intensity = { 1.f, 1.f, 1.f };
		scenelight.Lights[1].Direction = { 1.0f, 0.f, 0.f };
		scenelight.Lights[1].Direction.Normalize();
		scenelight.Lights[1].Intensity = { 0.f, 0.f, 0.f };
		scenelight.Lights[2].Direction = { 0.0f, -1.f ,0.f };
		scenelight.Lights[2].Direction.Normalize();
		scenelight.Lights[2].Intensity = { 0.f, 0.f, 0.f };
		scenelight.EyePosition = { 0, 0, -300 };
		scenelight.bUseIBL = false;
		mSceneLightCB->Update(device, scenelight);

		for (const StaticMeshJob& job : staticMeshJobs)
		{
			job.StaticMesh->Bind(device);
			job.Material->Bind(device);
			updateModelTransformCB(device, *(job.TransformPtr));
			updateModelTextureCB(device, job.Material);
			job.StaticMesh->Draw(device, job.SubsetIndex);
		}
	}

	void D3D11RenderManager::Render(const std::shared_ptr<D3D11Device>& device, const std::vector<SkinnedMeshJob>& skinnedMeshJobs)
	{

	}

	void D3D11RenderManager::updateModelTransformCB(const std::shared_ptr<D3D11Device>& device, const DirectX::SimpleMath::Matrix& transform)
	{
		ModelTransfrom modelTransform;
		modelTransform.WorldMat = transform.Transpose();
		modelTransform.WorldInvTransposeMat = D3D11Util::InverseTranspose(transform).Transpose();
		mModelTransformCB->Update(device, modelTransform);
	}

	void D3D11RenderManager::updateModelTextureCB(const std::shared_ptr<D3D11Device>& device, const std::shared_ptr<Material>& material)
	{
		ModelTexutre modelTexture;
		modelTexture.bUseAlbedoMap = material->GetHasBaseColor();
		modelTexture.bUseMetalnessMap = material->GetHasMetalness();
		modelTexture.bUseRoughnessMap = material->GetHasRoughness();
		modelTexture.bUseEmissiveMap = material->GetHasEmissive();
		modelTexture.bUseOpacityMap = material->GetHasOpacity();

		mModelTexutreCB->Update(device, modelTexture);
	}
}