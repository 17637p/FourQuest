#include "DeferredPass.h"

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
	void DeferredGeometryPass::Initialize(std::shared_ptr<D3D11Device> device,
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

		OnResize(width, height);

		mDSV = mResourceManager->Create<D3D11DepthStencilView>(ED3D11DepthStencilViewType::Default, width, height);

		D3D_SHADER_MACRO macroSkinning[] =
		{
			{"SKINNING", ""},
			{ NULL, NULL}
		};

		D3D_SHADER_MACRO macroGeometry[] =
		{
			{"GEOMETRY", ""},
			{ NULL, NULL}
		};

		mStaticMeshVS = std::make_shared<D3D11VertexShader>(mDevice, L"./resource/internal/shader/ModelVS.hlsl");
		mSkinnedMeshVS = std::make_shared<D3D11VertexShader>(mDevice, L"./resource/internal/shader/ModelVS.hlsl", macroSkinning);
		mGeometryPS = std::make_shared<D3D11PixelShader>(mDevice, L"./resource/internal/shader/ModelPSDeferred.hlsl", macroGeometry);

		mStaticMeshLayout = std::make_shared<D3D11InputLayout>(mDevice, mStaticMeshVS->GetBlob().Get());
		mSkinnedMeshLayout = std::make_shared<D3D11InputLayout>(mDevice, mSkinnedMeshVS->GetBlob().Get());

		mAnisotropicWrapSamplerState = resourceManager->Create<D3D11SamplerState>(ED3D11SamplerState::AnisotropicWrap);

		mModelTransformCB = std::make_shared<D3D11ConstantBuffer<ModelTransfrom>>(mDevice, ED3D11ConstantBuffer::Transform);
		mSceneTransformCB = std::make_shared<D3D11ConstantBuffer<SceneTrnasform>>(mDevice, ED3D11ConstantBuffer::Transform);
		mBoneTransformCB = std::make_shared<D3D11ConstantBuffer<BoneTransform>>(mDevice, ED3D11ConstantBuffer::Transform);
		mModelTexutreCB = std::make_shared< D3D11ConstantBuffer<ModelTexutre>>(mDevice, ED3D11ConstantBuffer::Transform);

	}
	void DeferredGeometryPass::Finalize()
	{

	}
	void DeferredGeometryPass::OnResize(unsigned short width, unsigned short height)
	{
		mViewport.Width = (float)width;
		mViewport.Height = (float)height;
		mViewport.MinDepth = 0.f;
		mViewport.MaxDepth = 1.f;
		mViewport.TopLeftX = 0.f;
		mViewport.TopLeftY = 0.f;
	}
	void DeferredGeometryPass::Render()
	{
		ID3D11ShaderResourceView* NullSRVs[10] = { NULL, };
		mDevice->GetDeviceContext()->PSSetShaderResources(0, ARRAYSIZE(NullSRVs), NullSRVs);

		mDSV->Clear(mDevice);
		mAlbedoRTV->Clear(mDevice, { 1.f, 1.f, 1.f, 1.f });
		mMetalnessRTV->Clear(mDevice);
		mRoughnessRTV->Clear(mDevice);
		mNormalRTV->Clear(mDevice, { 1000, 0, 0, 0 });
		mEmissiveRTV->Clear(mDevice);
		mPositionRTV->Clear(mDevice);

		std::vector<std::shared_ptr<D3D11RenderTargetView>> renderTargetViews;
		renderTargetViews.reserve(6u);

		renderTargetViews.push_back(mAlbedoRTV);
		renderTargetViews.push_back(mMetalnessRTV);
		renderTargetViews.push_back(mRoughnessRTV);
		renderTargetViews.push_back(mNormalRTV);
		renderTargetViews.push_back(mEmissiveRTV);
		renderTargetViews.push_back(mPositionRTV);

		D3D11RenderTargetView::Bind(mDevice, renderTargetViews, mDSV);

		mDevice->GetDeviceContext()->RSSetViewports(1, &mViewport);

		SceneTrnasform sceneTransform;
		sceneTransform.ViewProjMat = mCameraManager->GetViewMatrix(ECameraType::Player) * DirectX::SimpleMath::Matrix::CreateTranslation(100, 0, 0) * mCameraManager->GetProjectionMatrix(ECameraType::Player);
		sceneTransform.ViewProjMat = sceneTransform.ViewProjMat.Transpose();
		DirectX::SimpleMath::Matrix texTransform =
		{
			 0.5f, 0.0f, 0.0f, 0.0f,
			 0.0f, -0.5f, 0.0f, 0.0f,
			 0.0f, 0.0f, 1.0f, 0.0f,
			 0.5f, 0.5f, 0.0f, 1.0f
		};
		sceneTransform.ShadowViewProjTexMat = mCameraManager->GetViewMatrix(ECameraType::Player) * mCameraManager->GetProjectionMatrix(ECameraType::Player) * texTransform;
		sceneTransform.ShadowViewProjTexMat = sceneTransform.ShadowViewProjTexMat.Transpose();
		mSceneTransformCB->Update(mDevice, sceneTransform);

		mDevice->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		mStaticMeshLayout->Bind(mDevice);
		mStaticMeshVS->Bind(mDevice);
		mGeometryPS->Bind(mDevice);
		mModelTransformCB->Bind(mDevice, ED3D11ShaderType::VertexShader);
		mSceneTransformCB->Bind(mDevice, ED3D11ShaderType::VertexShader, 1);
		mModelTexutreCB->Bind(mDevice, ED3D11ShaderType::Pixelshader);
		mAnisotropicWrapSamplerState->Bind(mDevice, 0, ED3D11ShaderType::Pixelshader);

		for (const StaticMeshJob& job : mJobManager->GetStaticMeshJobs())
		{
			job.StaticMesh->Bind(mDevice);
			job.Material->Bind(mDevice);

			ConstantBufferHelper::UpdateModelTransformCB(mDevice, mModelTransformCB, *job.TransformPtr);
			ConstantBufferHelper::UpdateModelTextureCB(mDevice, mModelTexutreCB, job.Material);

			job.StaticMesh->Draw(mDevice, job.SubsetIndex);
		}

		mDevice->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		mSkinnedMeshLayout->Bind(mDevice);

		mSkinnedMeshVS->Bind(mDevice);
		mModelTransformCB->Bind(mDevice, ED3D11ShaderType::VertexShader);
		mSceneTransformCB->Bind(mDevice, ED3D11ShaderType::VertexShader, 1);
		mBoneTransformCB->Bind(mDevice, ED3D11ShaderType::VertexShader, 2);

		mGeometryPS->Bind(mDevice);
		mModelTexutreCB->Bind(mDevice, ED3D11ShaderType::Pixelshader);
		mAnisotropicWrapSamplerState->Bind(mDevice, 0, ED3D11ShaderType::Pixelshader);

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

	void DeferredShadingPass::Initialize(std::shared_ptr<D3D11Device> mDevice,
		std::shared_ptr<D3D11ResourceManager> resourceManager,
		std::shared_ptr< D3D11LightManager> lightManager,
		std::shared_ptr<D3D11CameraManager> cameraManager,
		unsigned short width,
		unsigned short height)
	{
		mDevice = mDevice;
		mResourceManager = resourceManager;
		mLightManager = lightManager;
		mCameraManager = cameraManager;

		OnResize(width, height);

		D3D_SHADER_MACRO macroShading[] =
		{
			{"SHADING", ""},
			{ NULL, NULL}
		};

		mShadingPS = std::make_shared<D3D11PixelShader>(mDevice, L"./resource/internal/shader/ModelPSDeferred.hlsl", macroShading);

		mAnisotropicWrapSamplerState = resourceManager->Create<D3D11SamplerState>(ED3D11SamplerState::AnisotropicWrap);
		mLinearClampSamplerState = resourceManager->Create<D3D11SamplerState>(ED3D11SamplerState::Default);
		mPointClampSamplerState = resourceManager->Create<D3D11SamplerState>(ED3D11SamplerState::PointClamp);
		mPointClampSamplerState = resourceManager->Create<D3D11SamplerState>(ED3D11SamplerState::PointClamp);
	}
	void DeferredShadingPass::Finalize()
	{

	}
	void DeferredShadingPass::OnResize(unsigned short width, unsigned short height)
	{
		mViewport.Width = (float)width;
		mViewport.Height = (float)height;
		mViewport.MinDepth = 0.f;
		mViewport.MaxDepth = 1.f;
		mViewport.TopLeftX = 0.f;
		mViewport.TopLeftY = 0.f;
	}
	void DeferredShadingPass::Render()
	{
		ID3D11ShaderResourceView* NullSRVs[10] = { NULL, };
		mDevice->GetDeviceContext()->PSSetShaderResources(0, ARRAYSIZE(NullSRVs), NullSRVs);

		mBackBufferRTV->Bind(mDevice, mNullDSV);

		mDevice->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		mFullScreenLayout->Bind(mDevice);
		mFullScreenVB->Bind(mDevice);
		mFullScreenIB->Bind(mDevice);

		mFullScreenVS->Bind(mDevice);

		mShadingPS->Bind(mDevice);
		mAlbedoSRV->Bind(mDevice, 0, ED3D11ShaderType::Pixelshader);
		mMetalnessSRV->Bind(mDevice, 1, ED3D11ShaderType::Pixelshader);
		mRoughnessSRV->Bind(mDevice, 2, ED3D11ShaderType::Pixelshader);
		mNormalSRV->Bind(mDevice, 3, ED3D11ShaderType::Pixelshader);
		mEmissiveSRV->Bind(mDevice, 4, ED3D11ShaderType::Pixelshader);
		mPositionSRV->Bind(mDevice, 5, ED3D11ShaderType::Pixelshader);

		mLightManager->UpdateConstantBuffer(mDevice, mCameraManager->GetPosition(ECameraType::Player), false);
		mLightManager->GetLightConstnatBuffer()->Bind(mDevice, ED3D11ShaderType::Pixelshader, 1);

		mAnisotropicWrapSamplerState->Bind(mDevice, 0, ED3D11ShaderType::Pixelshader);
		mLinearClampSamplerState->Bind(mDevice, 1, ED3D11ShaderType::Pixelshader);
		mPointClampSamplerState->Bind(mDevice, 2, ED3D11ShaderType::Pixelshader);

		mDevice->GetDeviceContext()->DrawIndexed(6, 0, 0);
	}
}