#include "FowardPass.h"

#include "D3D11JobManager.h"
#include "D3D11ResourceManager.h"
#include "D3D11CameraManager.h"
#include "D3D11LightManager.h"
#include "D3D11Common.h"
#include "RenderJob.h"
#include "Material.h"
#include "Mesh.h"
#include "Define.h"

namespace fq::graphics
{
	void ForwardRenderPass::Initialize(std::shared_ptr<D3D11Device> device,
		std::shared_ptr<D3D11JobManager> jobManager,
		std::shared_ptr<D3D11CameraManager> cameraManager,
		std::shared_ptr< D3D11LightManager> lightManager,
		std::shared_ptr<D3D11ResourceManager> resourceManager,
		unsigned short width,
		unsigned short height)
	{
		Finalize();

		mDevice = device;
		mJobManager = jobManager;
		mCameraManager = cameraManager;
		mLightManager = lightManager;
		mResourceManager = resourceManager;

		D3D_SHADER_MACRO macroSkinning[] =
		{
			{"SKINNING", ""},
			{ NULL, NULL}
		};

		mStaticMeshVS = std::make_shared<D3D11VertexShader>(mDevice, L"./resource/internal/shader/ModelVS.hlsl");
		mSkinnedMeshVS = std::make_shared<D3D11VertexShader>(mDevice, L"./resource/internal/shader/ModelVS.hlsl", macroSkinning);
		mMeshPS = std::make_shared<D3D11PixelShader>(mDevice, L"./resource/internal/shader/ModelPS.hlsl");

		mStaticMeshLayout = std::make_shared<D3D11InputLayout>(mDevice, mStaticMeshVS->GetBlob().Get());
		mSkinnedMeshLayout = std::make_shared<D3D11InputLayout>(mDevice, mSkinnedMeshVS->GetBlob().Get());

		mAnisotropicWrapSamplerState = resourceManager->Create<D3D11SamplerState>(ED3D11SamplerState::AnisotropicWrap);
		mLinearClampSamplerState = resourceManager->Create<D3D11SamplerState>(ED3D11SamplerState::Default);
		mShadowSampler = resourceManager->Create<D3D11SamplerState>(ED3D11SamplerState::Shadow);
		mDefaultRS = resourceManager->Create<D3D11RasterizerState>(ED3D11RasterizerState::Default);

		mModelTransformCB = std::make_shared<D3D11ConstantBuffer<ModelTransfrom>>(mDevice, ED3D11ConstantBuffer::Transform);
		mSceneTransformCB = std::make_shared<D3D11ConstantBuffer<SceneTrnasform>>(mDevice, ED3D11ConstantBuffer::Transform);
		mBoneTransformCB = std::make_shared<D3D11ConstantBuffer<BoneTransform>>(mDevice, ED3D11ConstantBuffer::Transform);
		mModelTexutreCB = std::make_shared< D3D11ConstantBuffer<ModelTexutre>>(mDevice, ED3D11ConstantBuffer::Transform);

		mViewport.Width = (float)width;
		mViewport.Height = (float)height;
		mViewport.MinDepth = 0.f;
		mViewport.MaxDepth = 1.f;
		mViewport.TopLeftX = 0.f;
		mViewport.TopLeftY = 0.f;

		mBackBufferRTV = mResourceManager->Create<fq::graphics::D3D11RenderTargetView>(ED3D11RenderTargetViewType::Offscreen, width, height);
		mDSV = mResourceManager->Create<fq::graphics::D3D11DepthStencilView>(ED3D11DepthStencilViewType::Default, width, height);
		auto shadowDSV = mResourceManager->Create<D3D11DepthStencilView>(ED3D11DepthStencilViewType::ShaderInputDepthStencil, ShadowPass::SHADOW_SIZE, ShadowPass::SHADOW_SIZE);
		mShadowSRV = std::make_shared<D3D11ShaderResourceView>(mDevice, shadowDSV);
	}
	void ForwardRenderPass::Finalize()
	{
		mDevice = nullptr;
		mJobManager = nullptr;
		mCameraManager = nullptr;
		mLightManager = nullptr;
		mResourceManager = nullptr;

		mBackBufferRTV = nullptr;
		mDSV = nullptr;
		mShadowSRV = nullptr;

		mStaticMeshLayout = nullptr;
		mSkinnedMeshLayout = nullptr;
		mStaticMeshVS = nullptr;
		mSkinnedMeshVS = nullptr;
		mMeshPS = nullptr;

		mAnisotropicWrapSamplerState = nullptr;
		mLinearClampSamplerState = nullptr;
		mPointClampSamplerState = nullptr;
		mShadowSampler = nullptr;

		mModelTransformCB = nullptr;
		mSceneTransformCB = nullptr;
		mBoneTransformCB = nullptr;

		mModelTexutreCB = nullptr;
	}

	void ForwardRenderPass::OnResize(unsigned short width, unsigned short height)
	{
		mViewport.Width = (float)width;
		mViewport.Height = (float)height;
		mViewport.MinDepth = 0.f;
		mViewport.MaxDepth = 1.f;
		mViewport.TopLeftX = 0.f;
		mViewport.TopLeftY = 0.f;
	}

	void ForwardRenderPass::Render()
	{
		// 쉐이더 프로그램을 하나 만들어서 처리 대상을 줄이면 좋을 거 같긴 하네
		ID3D11ShaderResourceView* SRVs[10] = { NULL };
		mDevice->GetDeviceContext()->PSSetShaderResources(0, 10, SRVs);

		mDevice->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		mDevice->GetDeviceContext()->RSSetViewports(1, &mViewport);

		//mBackBufferRTV->Clear(mDevice, { 1,1,1,1 });
		//mDSV->Clear(mDevice);
		mBackBufferRTV->Bind(mDevice, mDSV);

		mStaticMeshLayout->Bind(mDevice);
		mStaticMeshVS->Bind(mDevice);

		mMeshPS->Bind(mDevice);
		mModelTransformCB->Bind(mDevice, ED3D11ShaderType::VertexShader);
		mSceneTransformCB->Bind(mDevice, ED3D11ShaderType::VertexShader, 1);
		mModelTexutreCB->Bind(mDevice, ED3D11ShaderType::Pixelshader);
		mLightManager->GetLightConstnatBuffer()->Bind(mDevice, ED3D11ShaderType::Pixelshader, 1);
		mAnisotropicWrapSamplerState->Bind(mDevice, 0, ED3D11ShaderType::Pixelshader);
		mLinearClampSamplerState->Bind(mDevice, 1, ED3D11ShaderType::Pixelshader);
		mShadowSampler->Bind(mDevice, 2, ED3D11ShaderType::Pixelshader);
		mDefaultRS->Bind(mDevice);

		mShadowSRV->Bind(mDevice, 9, ED3D11ShaderType::Pixelshader);
		SceneTrnasform sceneTransform;
		sceneTransform.ViewProjMat = mCameraManager->GetViewMatrix(ECameraType::Player) * mCameraManager->GetProjectionMatrix(ECameraType::Player);
		sceneTransform.ViewProjMat = sceneTransform.ViewProjMat.Transpose();
		DirectX::SimpleMath::Matrix texTransform =
		{
			 0.5f, 0.0f, 0.0f, 0.0f,
			 0.0f, -0.5f, 0.0f, 0.0f,
			 0.0f, 0.0f, 1.0f, 0.0f,
			 0.5f, 0.5f, 0.0f, 1.0f
		};
		sceneTransform.ShadowViewProjTexMat = mCameraManager->GetViewMatrix(ECameraType::Player) * DirectX::SimpleMath::Matrix::CreateTranslation(100, 0, 0) * mCameraManager->GetProjectionMatrix(ECameraType::Player) * texTransform;
		sceneTransform.ShadowViewProjTexMat = sceneTransform.ShadowViewProjTexMat.Transpose();
		mSceneTransformCB->Update(mDevice, sceneTransform);

		mLightManager->UpdateConstantBuffer(mDevice, mCameraManager->GetPosition(ECameraType::Player), false);

		for (const StaticMeshJob& job : mJobManager->GetStaticMeshJobs())
		{
			job.StaticMesh->Bind(mDevice);
			job.Material->Bind(mDevice);

			ConstantBufferHelper::UpdateModelTransformCB(mDevice, mModelTransformCB, *job.TransformPtr);
			ConstantBufferHelper::UpdateModelTextureCB(mDevice, mModelTexutreCB, job.Material);

			job.StaticMesh->Draw(mDevice, job.SubsetIndex);
		}

		mSkinnedMeshLayout->Bind(mDevice);
		mSkinnedMeshVS->Bind(mDevice);
		mBoneTransformCB->Bind(mDevice, ED3D11ShaderType::VertexShader, 2);

		for (const SkinnedMeshJob& job : mJobManager->GetSkinnedMeshJobs())
		{
			job.SkinnedMesh->Bind(mDevice);
			job.Material->Bind(mDevice);

			ConstantBufferHelper::UpdateModelTransformCB(mDevice, mModelTransformCB, *job.TransformPtr);
			ConstantBufferHelper::UpdateModelTextureCB(mDevice, mModelTexutreCB, job.Material);
			ConstantBufferHelper::UpdateBoneTransformCB(mDevice, mBoneTransformCB, *job.BoneMatricesPtr);

			job.SkinnedMesh->Draw(mDevice, job.SubsetIndex);
		}
	}
}