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
		Finalize();

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


		auto shadowDSV = mResourceManager->Create<D3D11DepthStencilView>(ED3D11DepthStencilViewType::ShaderInputDepthStencil, ShadowPass::SHADOW_SIZE, ShadowPass::SHADOW_SIZE);
		mShadowSRV = std::make_shared<D3D11ShaderResourceView>(mDevice, shadowDSV);

		mStaticMeshVS = std::make_shared<D3D11VertexShader>(mDevice, L"./resource/internal/shader/ModelVS.hlsl");
		mSkinnedMeshVS = std::make_shared<D3D11VertexShader>(mDevice, L"./resource/internal/shader/ModelVS.hlsl", macroSkinning);
		mGeometryPS = std::make_shared<D3D11PixelShader>(mDevice, L"./resource/internal/shader/ModelPSDeferred.hlsl", macroGeometry);

		mStaticMeshLayout = std::make_shared<D3D11InputLayout>(mDevice, mStaticMeshVS->GetBlob().Get());
		mSkinnedMeshLayout = std::make_shared<D3D11InputLayout>(mDevice, mSkinnedMeshVS->GetBlob().Get());

		mAnisotropicWrapSamplerState = resourceManager->Create<D3D11SamplerState>(ED3D11SamplerState::AnisotropicWrap);
		mShadowSampler = resourceManager->Create<D3D11SamplerState>(ED3D11SamplerState::Shadow);

		mModelTransformCB = std::make_shared<D3D11ConstantBuffer<ModelTransfrom>>(mDevice, ED3D11ConstantBuffer::Transform);
		mSceneTransformCB = std::make_shared<D3D11ConstantBuffer<SceneTrnasform>>(mDevice, ED3D11ConstantBuffer::Transform);
		mBoneTransformCB = std::make_shared<D3D11ConstantBuffer<BoneTransform>>(mDevice, ED3D11ConstantBuffer::Transform);
		mModelTexutreCB = std::make_shared< D3D11ConstantBuffer<ModelTexutre>>(mDevice, ED3D11ConstantBuffer::Transform);

	}
	void DeferredGeometryPass::Finalize()
	{
		mDevice = nullptr;
		mJobManager = nullptr;
		mCameraManager = nullptr;
		mResourceManager = nullptr;

		mDSV = nullptr;
		mShadowSRV = nullptr;

		mAlbedoRTV = nullptr;
		mMetalnessRTV = nullptr;
		mRoughnessRTV = nullptr;
		mNormalRTV = nullptr;
		mEmissiveRTV = nullptr;
		mPositionRTV;
		mShadowRatioRTV = nullptr;

		mStaticMeshLayout = nullptr;
		mSkinnedMeshLayout = nullptr;

		mStaticMeshVS = nullptr;
		mSkinnedMeshVS = nullptr;

		mGeometryPS = nullptr;

		mAnisotropicWrapSamplerState = nullptr;
		mShadowSampler = nullptr;

		mModelTransformCB = nullptr;
		mSceneTransformCB = nullptr;
		mBoneTransformCB = nullptr;
		mModelTexutreCB = nullptr;

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
		mShadowRatioRTV->Clear(mDevice, { 0,0,0,0 });

		std::vector<std::shared_ptr<D3D11RenderTargetView>> renderTargetViews;
		renderTargetViews.reserve(6u);

		renderTargetViews.push_back(mAlbedoRTV);
		renderTargetViews.push_back(mMetalnessRTV);
		renderTargetViews.push_back(mRoughnessRTV);
		renderTargetViews.push_back(mNormalRTV);
		renderTargetViews.push_back(mEmissiveRTV);
		renderTargetViews.push_back(mPositionRTV);
		renderTargetViews.push_back(mShadowRatioRTV);

		D3D11RenderTargetView::Bind(mDevice, renderTargetViews, mDSV);

		mDevice->GetDeviceContext()->RSSetViewports(1, &mViewport);

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

		mDevice->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		mStaticMeshLayout->Bind(mDevice);
		mStaticMeshVS->Bind(mDevice);
		mGeometryPS->Bind(mDevice);
		mModelTransformCB->Bind(mDevice, ED3D11ShaderType::VertexShader);
		mSceneTransformCB->Bind(mDevice, ED3D11ShaderType::VertexShader, 1);
		mModelTexutreCB->Bind(mDevice, ED3D11ShaderType::Pixelshader);
		mAnisotropicWrapSamplerState->Bind(mDevice, 0, ED3D11ShaderType::Pixelshader);
		mShadowSampler->Bind(mDevice, 1, ED3D11ShaderType::Pixelshader);
		mShadowSRV->Bind(mDevice, 5, ED3D11ShaderType::Pixelshader);

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

	void DeferredShadingPass::Initialize(std::shared_ptr<D3D11Device> device,
		std::shared_ptr<D3D11ResourceManager> resourceManager,
		std::shared_ptr< D3D11LightManager> lightManager,
		std::shared_ptr<D3D11CameraManager> cameraManager,
		unsigned short width,
		unsigned short height)
	{
		Finalize();

		mDevice = device;
		mResourceManager = resourceManager;
		mLightManager = lightManager;
		mCameraManager = cameraManager;

		OnResize(width, height);

		D3D_SHADER_MACRO macroShading[] =
		{
			{"SHADING", ""},
			{ NULL, NULL}
		};

		mBackBufferRTV = mResourceManager->Create<D3D11RenderTargetView>(ED3D11RenderTargetViewType::Offscreen, width, height);
		mNullDSV = mResourceManager->Create<D3D11DepthStencilView>(ED3D11DepthStencilViewType::None, width, height);
		mDSV = mResourceManager->Create<D3D11DepthStencilView>(ED3D11DepthStencilViewType::Default, width, height);

		mAnisotropicWrapSamplerState = resourceManager->Create<D3D11SamplerState>(ED3D11SamplerState::AnisotropicWrap);
		mLinearClampSamplerState = resourceManager->Create<D3D11SamplerState>(ED3D11SamplerState::Default);
		mPointClampSamplerState = resourceManager->Create<D3D11SamplerState>(ED3D11SamplerState::PointClamp);

		mFullScreenVS = std::make_shared<D3D11VertexShader>(device, L"./resource/internal/shader/FullScreenVS.hlsl");
		mFullScreenLayout = std::make_shared<D3D11InputLayout>(device, mFullScreenVS->GetBlob().Get());
		mShadingPS = std::make_shared<D3D11PixelShader>(mDevice, L"./resource/internal/shader/ModelPSDeferred.hlsl", macroShading);

		std::vector<DirectX::SimpleMath::Vector2> positions =
		{
			{ -1, 1 },
			{ 1, 1 },
			{ -1, -1 },
			{ 1, -1 }
		};

		std::vector<unsigned int> indices =
		{
			0,1,2,
			1,3,2
		};

		mFullScreenVB = std::make_shared<D3D11VertexBuffer>(device, positions);
		mFullScreenIB = std::make_shared<D3D11IndexBuffer>(device, indices);
	}
	void DeferredShadingPass::Finalize()
	{
		mDevice = nullptr;
		mResourceManager = nullptr;
		mLightManager = nullptr;
		mCameraManager = nullptr;

		mNullDSV = nullptr;
		mDSV = nullptr;

		mAlbedoSRV = nullptr;
		mMetalnessSRV = nullptr;
		mRoughnessSRV = nullptr;
		mNormalSRV = nullptr;
		mEmissiveSRV = nullptr;
		mPositionSRV = nullptr;
		mShadowRatioSRV = nullptr;

		mBackBufferRTV = nullptr;

		mAnisotropicWrapSamplerState = nullptr;
		mLinearClampSamplerState = nullptr;
		mPointClampSamplerState = nullptr;

		mFullScreenLayout = nullptr;
		mFullScreenVS = nullptr;
		mFullScreenVB = nullptr;
		mFullScreenIB = nullptr;
		mShadingPS = nullptr;
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
		mShadowRatioSRV->Bind(mDevice, 9, ED3D11ShaderType::Pixelshader);

		mLightManager->UpdateConstantBuffer(mDevice, mCameraManager->GetPosition(ECameraType::Player), false);
		mLightManager->GetLightConstnatBuffer()->Bind(mDevice, ED3D11ShaderType::Pixelshader, 1);

		mAnisotropicWrapSamplerState->Bind(mDevice, 0, ED3D11ShaderType::Pixelshader);
		mLinearClampSamplerState->Bind(mDevice, 1, ED3D11ShaderType::Pixelshader);
		mPointClampSamplerState->Bind(mDevice, 2, ED3D11ShaderType::Pixelshader);

		mDevice->GetDeviceContext()->DrawIndexed(6, 0, 0);

		mBackBufferRTV->Bind(mDevice, mDSV);
	}
}