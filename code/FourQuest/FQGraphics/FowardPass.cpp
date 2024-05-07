#include "FowardPass.h"

#include "CommonPass.h"
#include "D3D11Common.h"
#include "ManagementCommon.h"
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

		mModelTransformCB = std::make_shared<D3D11ConstantBuffer<ModelTransfrom>>(mDevice, ED3D11ConstantBuffer::Transform);
		mSceneTransformCB = std::make_shared<D3D11ConstantBuffer<SceneTrnasform>>(mDevice, ED3D11ConstantBuffer::Transform);
		mBoneTransformCB = std::make_shared<D3D11ConstantBuffer<BoneTransform>>(mDevice, ED3D11ConstantBuffer::Transform);
		mModelTexutreCB = std::make_shared< D3D11ConstantBuffer<ModelTexutre>>(mDevice, ED3D11ConstantBuffer::Transform);
		mShadowViewProjTexCB = std::make_shared< D3D11ConstantBuffer<ShadowTransform>>(mDevice, ED3D11ConstantBuffer::Transform);
		mCascadeEndCB = std::make_shared< D3D11ConstantBuffer<CascadeEnd>>(mDevice, ED3D11ConstantBuffer::Transform);

		mViewport.Width = (float)width;
		mViewport.Height = (float)height;
		mViewport.MinDepth = 0.f;
		mViewport.MaxDepth = 1.f;
		mViewport.TopLeftX = 0.f;
		mViewport.TopLeftY = 0.f;

		mBackBufferRTV = mResourceManager->Create<fq::graphics::D3D11RenderTargetView>(ED3D11RenderTargetViewType::Offscreen, width, height);
		mDSV = mResourceManager->Create<fq::graphics::D3D11DepthStencilView>(ED3D11DepthStencilViewType::Default, width, height);
		auto shadowDSV = mResourceManager->Create<D3D11DepthStencilView>(ED3D11DepthStencilViewType::CascadeShadow, ShadowPass::SHADOW_SIZE, ShadowPass::SHADOW_SIZE);
		mShadowSRV = std::make_shared<D3D11ShaderResourceView>(mDevice, shadowDSV);

		// 포인트라이트 SRV 생성
		auto pointLightShadow = mResourceManager->Create<D3D11DepthStencilView>(ED3D11DepthStencilViewType::PointLightShadow, ShadowPass::Point_LIGHT_SHADOW_SIZE, ShadowPass::Point_LIGHT_SHADOW_SIZE);
		mPointLightShadowSRV = std::make_shared<D3D11ShaderResourceView>(mDevice, pointLightShadow);
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
		mPointLightShadowSRV = nullptr;

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
		using namespace DirectX::SimpleMath;

		// Update
		{
			if (mLightManager->GetDirectionalLights().size() != 0)
			{
				std::vector<float> cascadeEnds = ShadowPass::CalculateCascadeEnds({ 0.33, 0.66 },
					mCameraManager->GetNearPlain(ECameraType::Player),
					mCameraManager->GetFarPlain(ECameraType::Player));
				std::vector<DirectX::SimpleMath::Matrix> shadowTransforms = ShadowPass::CalculateCascadeShadowTransform(cascadeEnds,
					mCameraManager->GetViewMatrix(ECameraType::Player),
					mCameraManager->GetProjectionMatrix(ECameraType::Player),
					mLightManager->GetDirectionalLights().front()->GetData().direction);
				assert(shadowTransforms.size() == 3);

				ShadowTransform shadowTransformData;
				DirectX::SimpleMath::Matrix texTransform =
				{
					 0.5f, 0.0f, 0.0f, 0.0f,
					 0.0f, -0.5f, 0.0f, 0.0f,
					 0.0f, 0.0f, 1.0f, 0.0f,
					 0.5f, 0.5f, 0.0f, 1.0f
				};
				shadowTransformData.ShadowViewProj[0] = (shadowTransforms[0] * texTransform).Transpose();
				shadowTransformData.ShadowViewProj[1] = (shadowTransforms[1] * texTransform).Transpose();
				shadowTransformData.ShadowViewProj[2] = (shadowTransforms[2] * texTransform).Transpose();
				mShadowViewProjTexCB->Update(mDevice, shadowTransformData);

				CascadeEnd cascadeEndData;
				auto cameraProj = mCameraManager->GetProjectionMatrix(ECameraType::Player);
				cascadeEndData.CascadeEnds.x = Vector4::Transform({ 0, 0, cascadeEnds[1], 1.f }, cameraProj).z;
				cascadeEndData.CascadeEnds.y = Vector4::Transform({ 0, 0, cascadeEnds[2], 1.f }, cameraProj).z;
				cascadeEndData.CascadeEnds.z = Vector4::Transform({ 0, 0, cascadeEnds[3], 1.f }, cameraProj).z;
				mCascadeEndCB->Update(mDevice, cascadeEndData);
			}
			else
			{
				ShadowTransform shadowTransformData;
				shadowTransformData.ShadowViewProj[0] = Matrix::Identity;
				shadowTransformData.ShadowViewProj[1] = Matrix::Identity;
				shadowTransformData.ShadowViewProj[2] = Matrix::Identity;
				mShadowViewProjTexCB->Update(mDevice, shadowTransformData);

				CascadeEnd cascadeEndData;
				cascadeEndData.CascadeEnds.x = 0;
				cascadeEndData.CascadeEnds.y = 0;
				cascadeEndData.CascadeEnds.z = 0;
				mCascadeEndCB->Update(mDevice, cascadeEndData);
			}

			SceneTrnasform sceneTransform;
			sceneTransform.ViewProjMat = mCameraManager->GetViewMatrix(ECameraType::Player) * mCameraManager->GetProjectionMatrix(ECameraType::Player);
			sceneTransform.ViewProjMat = sceneTransform.ViewProjMat.Transpose();
			mSceneTransformCB->Update(mDevice, sceneTransform);

			mLightManager->UpdateConstantBuffer(mDevice, mCameraManager->GetPosition(ECameraType::Player), false);
		}

		// Init
		{
			mBackBufferRTV->Clear(mDevice, { 1,1,1,1 });
			mDSV->Clear(mDevice);
		}

		// Bind
		{
			ID3D11ShaderResourceView* SRVs[10] = { NULL };
			mDevice->GetDeviceContext()->PSSetShaderResources(0, 10, SRVs);

			mDevice->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			mDevice->GetDeviceContext()->RSSetViewports(1, &mViewport);

			mBackBufferRTV->Bind(mDevice, mDSV);

			mModelTransformCB->Bind(mDevice, ED3D11ShaderType::VertexShader);
			mSceneTransformCB->Bind(mDevice, ED3D11ShaderType::VertexShader, 1);
			mCascadeEndCB->Bind(mDevice, ED3D11ShaderType::VertexShader, 3);

			mMeshPS->Bind(mDevice);
			mModelTexutreCB->Bind(mDevice, ED3D11ShaderType::Pixelshader);
			mLightManager->GetLightConstnatBuffer()->Bind(mDevice, ED3D11ShaderType::Pixelshader, 1);
			mShadowViewProjTexCB->Bind(mDevice, ED3D11ShaderType::Pixelshader, 2);
			mCascadeEndCB->Bind(mDevice, ED3D11ShaderType::Pixelshader, 3);
			mAnisotropicWrapSamplerState->Bind(mDevice, 0, ED3D11ShaderType::Pixelshader);
			mLinearClampSamplerState->Bind(mDevice, 1, ED3D11ShaderType::Pixelshader);
			mShadowSampler->Bind(mDevice, 2, ED3D11ShaderType::Pixelshader);
			mShadowSRV->Bind(mDevice, 9, ED3D11ShaderType::Pixelshader);
		}

		// Draw
		{
			mStaticMeshLayout->Bind(mDevice);
			mStaticMeshVS->Bind(mDevice);

			for (const StaticMeshJob& job : mJobManager->GetStaticMeshJobs())
			{
				if (job.ObjectRenderType == EObjectRenderType::Opaque)
				{
					job.StaticMesh->Bind(mDevice);
					job.Material->Bind(mDevice);

					ConstantBufferHelper::UpdateModelTransformCB(mDevice, mModelTransformCB, *job.TransformPtr);
					ConstantBufferHelper::UpdateModelTextureCB(mDevice, mModelTexutreCB, job.Material);

					job.StaticMesh->Draw(mDevice, job.SubsetIndex);
				}
			}

			mSkinnedMeshLayout->Bind(mDevice);
			mSkinnedMeshVS->Bind(mDevice);
			mBoneTransformCB->Bind(mDevice, ED3D11ShaderType::VertexShader, 2);

			for (const SkinnedMeshJob& job : mJobManager->GetSkinnedMeshJobs())
			{
				if (job.ObjectRenderType == EObjectRenderType::Opaque)
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
	}
}