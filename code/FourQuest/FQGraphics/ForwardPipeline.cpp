#include "ForwardPipeline.h"

#include "D3D11ResourceManager.h"
#include "D3D11CameraManager.h"
#include "D3D11Common.h"
#include "RenderJob.h"
#include "Material.h"
#include "Mesh.h"
#include "Define.h"

namespace fq::graphics
{
	void ForwardPipeline::Initialize(const std::shared_ptr<D3D11Device>& device,
		std::shared_ptr<class D3D11ResourceManager>& resourceManager,
		unsigned short width,
		unsigned short height)
	{
		mBackBufferRTV = resourceManager->Create<fq::graphics::D3D11RenderTargetView>(ED3D11RenderTargetViewType::Offscreen, width, height);
		mBackBufferSRV = std::make_shared<fq::graphics::D3D11ShaderResourceView>(device, mBackBufferRTV);
		mDSV = resourceManager->Create<fq::graphics::D3D11DepthStencilView>(ED3D11DepthStencilViewType::Default, width, height);

		D3D_SHADER_MACRO macroSkinning[] =
		{
			{"SKINNING", ""},
			{ NULL, NULL}
		};

		mStaticMeshVS = std::make_shared<D3D11VertexShader>(device, L"./resource/internal/shader/ModelVS.hlsl");
		mSkinnedMeshVS = std::make_shared<D3D11VertexShader>(device, L"./resource/internal/shader/ModelVS.hlsl", macroSkinning);
		mMeshPS = std::make_shared<D3D11PixelShader>(device, L"./resource/internal/shader/ModelPS.hlsl");

		mStaticMeshLayout = std::make_shared<D3D11InputLayout>(device, mStaticMeshVS->GetBlob().Get());
		mSkinnedMeshLayout = std::make_shared<D3D11InputLayout>(device, mSkinnedMeshVS->GetBlob().Get());

		mAnisotropicWrapSamplerState = resourceManager->Create<D3D11SamplerState>(ED3D11SamplerState::AnisotropicWrap);
		mLinearClampSamplerState = resourceManager->Create<D3D11SamplerState>(ED3D11SamplerState::Default);

		mModelTransformCB = std::make_shared<D3D11ConstantBuffer<ModelTransfrom>>(device, ED3D11ConstantBuffer::Transform);
		mSceneTransformCB = std::make_shared<D3D11ConstantBuffer<SceneTrnasform>>(device, ED3D11ConstantBuffer::Transform);
		mBoneTransformCB = std::make_shared<D3D11ConstantBuffer<BoneTransform>>(device, ED3D11ConstantBuffer::Transform);
		mModelTexutreCB = std::make_shared< D3D11ConstantBuffer<ModelTexutre>>(device, ED3D11ConstantBuffer::Transform);
		mSceneLightCB = std::make_shared<D3D11ConstantBuffer<SceneLight>>(device, ED3D11ConstantBuffer::Transform);

		mViewport.Width = (float)width;
		mViewport.Height = (float)height;
		mViewport.MinDepth = 0.f;
		mViewport.MaxDepth = 1.f;
		mViewport.TopLeftX = 0.f;
		mViewport.TopLeftY = 0.f;
	}
	void ForwardPipeline::Finalize()
	{
		mBackBufferRTV = nullptr;
		mBackBufferSRV = nullptr;
		mDSV = nullptr;

		mStaticMeshLayout = nullptr;
		mSkinnedMeshLayout = nullptr;
		mStaticMeshVS = nullptr;
		mSkinnedMeshVS = nullptr;
		mMeshPS = nullptr;

		mAnisotropicWrapSamplerState = nullptr;
		mLinearClampSamplerState = nullptr;
		mPointClampSamplerState = nullptr;

		mModelTransformCB = nullptr;
		mSceneTransformCB = nullptr;
		mBoneTransformCB = nullptr;
		mModelTexutreCB = nullptr;
		mSceneLightCB = nullptr;
	}

	void ForwardPipeline::OnResize(const std::shared_ptr<D3D11Device>& device,
		std::shared_ptr<class D3D11ResourceManager>& resourceManager,
		unsigned short width,
		unsigned short height)
	{
		mBackBufferRTV->Release();
		mDSV->Release();

		mBackBufferRTV->OnResize(device, ED3D11RenderTargetViewType::Offscreen, width, height);
		mDSV->OnResize(device, ED3D11DepthStencilViewType::Default, width, height);

		mBackBufferSRV = std::make_shared<fq::graphics::D3D11ShaderResourceView>(device, mBackBufferRTV);

		mViewport.Width = (float)width;
		mViewport.Height = (float)height;
		mViewport.MinDepth = 0.f;
		mViewport.MaxDepth = 1.f;
		mViewport.TopLeftX = 0.f;
		mViewport.TopLeftY = 0.f;
	}

	void ForwardPipeline::BeginRender(const std::shared_ptr<D3D11Device>& device, const std::shared_ptr<class D3D11CameraManager>& cameraManager)
	{
		mDSV->Clear(device);
		mBackBufferRTV->Clear(device, { 1.f, 1.f, 1.f, 1.f });

		ID3D11ShaderResourceView* NullSRVs[10] = { NULL, };
		device->GetDeviceContext()->PSSetShaderResources(0, ARRAYSIZE(NullSRVs), NullSRVs);

		mBackBufferRTV->Bind(device, mDSV);
		device->GetDeviceContext()->RSSetViewports(1, &mViewport);

		SceneTrnasform sceneTransform;
		DirectX::SimpleMath::Matrix view = cameraManager->GetViewMatrix(ECameraType::Player);
		DirectX::SimpleMath::Matrix proj = cameraManager->GetProjectionMatrix(ECameraType::Player);
		sceneTransform.ViewProjMat = (view * proj).Transpose();
		sceneTransform.ShadowViewProjTexMat;
		mSceneTransformCB->Update(device, sceneTransform);

		// 임시 데이터, 조명 반영시켜줘야 함
		SceneLight scenelight;
		scenelight.Lights[0].Direction = { 0.0f, 0.0f, 1.f };
		scenelight.Lights[0].Intensity = { 1.f, 1.f, 1.f };
		scenelight.Lights[1].Direction = { 1.0f, 0.f, 0.f };
		scenelight.Lights[1].Direction.Normalize();
		scenelight.Lights[1].Intensity = { 1.f, 1.f, 1.f };
		scenelight.Lights[2].Direction = { 0.0f, -1.f ,0.f };
		scenelight.Lights[2].Direction.Normalize();
		scenelight.Lights[2].Intensity = { 1.f, 1.f, 1.f };
		scenelight.EyePosition = (DirectX::SimpleMath::Vector4)cameraManager->GetPosition(ECameraType::Player);
		scenelight.bUseIBL = false;
		mSceneLightCB->Update(device, scenelight);
	}
	void ForwardPipeline::EndRender(const std::shared_ptr<D3D11Device>& device)
	{
		Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain = device->GetSwapChain();
		HR(swapChain->Present(0, 0));
	}

	void ForwardPipeline::Render(const std::shared_ptr<D3D11Device>& device, const std::vector<StaticMeshJob>& staticMeshJobs)
	{
		device->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		mStaticMeshLayout->Bind(device);
		mStaticMeshVS->Bind(device);
		mMeshPS->Bind(device);
		mModelTransformCB->Bind(device, ED3D11ShaderType::VertexShader);
		mSceneTransformCB->Bind(device, ED3D11ShaderType::VertexShader, 1);
		mModelTexutreCB->Bind(device, ED3D11ShaderType::Pixelshader);
		mSceneLightCB->Bind(device, ED3D11ShaderType::Pixelshader, 1);
		mAnisotropicWrapSamplerState->Bind(device, 0, ED3D11ShaderType::Pixelshader);
		mLinearClampSamplerState->Bind(device, 1, ED3D11ShaderType::Pixelshader);

		for (const StaticMeshJob& job : staticMeshJobs)
		{
			job.StaticMesh->Bind(device);
			job.Material->Bind(device);

			ConstantBufferHelper::UpdateModelTransformCB(device, mModelTransformCB, *job.TransformPtr);
			ConstantBufferHelper::UpdateModelTextureCB(device, mModelTexutreCB, job.Material);

			job.StaticMesh->Draw(device, job.SubsetIndex);
		}
	}
	void ForwardPipeline::Render(const std::shared_ptr<D3D11Device>& device, const std::vector<SkinnedMeshJob>& skinnedMeshJobs)
	{
		device->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		mSkinnedMeshLayout->Bind(device);
		mSkinnedMeshVS->Bind(device);
		mMeshPS->Bind(device);
		mModelTransformCB->Bind(device, ED3D11ShaderType::VertexShader);
		mSceneTransformCB->Bind(device, ED3D11ShaderType::VertexShader, 1);
		mBoneTransformCB->Bind(device, ED3D11ShaderType::VertexShader, 2);
		mModelTexutreCB->Bind(device, ED3D11ShaderType::Pixelshader);
		mSceneLightCB->Bind(device, ED3D11ShaderType::Pixelshader, 1);
		mAnisotropicWrapSamplerState->Bind(device, 0, ED3D11ShaderType::Pixelshader);
		mLinearClampSamplerState->Bind(device, 1, ED3D11ShaderType::Pixelshader);

		for (const SkinnedMeshJob& job : skinnedMeshJobs)
		{
			job.SkinnedMesh->Bind(device);
			job.Material->Bind(device);

			ConstantBufferHelper::UpdateModelTransformCB(device, mModelTransformCB, *job.TransformPtr);
			ConstantBufferHelper::UpdateModelTextureCB(device, mModelTexutreCB, job.Material);
			ConstantBufferHelper::UpdateBoneTransformCB(device, mBoneTransformCB, *job.BoneMatricesPtr);

			job.SkinnedMesh->Draw(device, job.SubsetIndex);
		}
	}

	std::shared_ptr<D3D11ShaderResourceView>& ForwardPipeline::GetBackBufferSRV()
	{
		return mBackBufferSRV;
	}
}