#include "TransparentRenderPass.h"
#include "ManagementCommon.h"
#include "D3D11Common.h"
#include "ShadowPass.h"
#include "Mesh.h"
#include "Material.h"
#include "BoneHierarchy.h"

namespace fq::graphics
{
	void TransparentRenderPass::Initialize(std::shared_ptr<D3D11Device> device,
		std::shared_ptr<D3D11JobManager> jobManager,
		std::shared_ptr<D3D11CameraManager> cameraManager,
		std::shared_ptr< D3D11LightManager> lightManager,
		std::shared_ptr<D3D11ResourceManager> resourceManager,
		unsigned short width,
		unsigned short height)
	{
		mDevice = device;
		mJobManager = jobManager;
		mCameraManager = cameraManager;
		mLightManager = lightManager;
		mResourceManager = resourceManager;

		mColoraccumulationRTV = mResourceManager->Create<D3D11RenderTargetView>(ED3D11RenderTargetViewType::ColorAcuumulation, width, height);
		mPixelRevealageThresholdRTV = mResourceManager->Create<D3D11RenderTargetView>(ED3D11RenderTargetViewType::PixeldRevealageThreshold, width, height);
		
		mDefaultDSV = mResourceManager->Get<D3D11DepthStencilView>(ED3D11DepthStencilViewType::Default);
		auto shadowDSV = mResourceManager->Get<D3D11DepthStencilView>(ED3D11DepthStencilViewType::CascadeShadow3);
		mShadowSRV = std::make_shared<D3D11ShaderResourceView>(mDevice, shadowDSV);

		D3D_SHADER_MACRO macroSkinning[] =
		{
			{"SKINNING", ""},
			{ NULL, NULL}
		};
		D3D_SHADER_MACRO macroRender[] =
		{
			{"RENDER", ""},
			{ NULL, NULL}
		};

		mStaticMeshVS = std::make_shared<D3D11VertexShader>(mDevice, L"./resource/internal/shader/ModelVS.hlsl");
		mSkinnedMeshVS = std::make_shared<D3D11VertexShader>(mDevice, L"./resource/internal/shader/ModelVS.hlsl", macroSkinning);
		mTransparentRenderPS = std::make_shared<D3D11PixelShader>(mDevice, L"./resource/internal/shader/ModelTransparentPS.hlsl", macroRender);
		mStaticMeshLayout = std::make_shared<D3D11InputLayout>(mDevice, mStaticMeshVS->GetBlob().Get());
		mSkinnedMeshLayout = std::make_shared<D3D11InputLayout>(mDevice, mSkinnedMeshVS->GetBlob().Get());

		mAnisotropicWrapSamplerState = resourceManager->Create<D3D11SamplerState>(ED3D11SamplerState::AnisotropicWrap);
		mShadowSampler = resourceManager->Create<D3D11SamplerState>(ED3D11SamplerState::Shadow);
		mDisableDepthWriteState = resourceManager->Create<D3D11DepthStencilState>(ED3D11DepthStencilState::DisableDepthWirte);
		mOITRenderState = resourceManager->Create<D3D11BlendState>(ED3D11BlendState::OITRender);

		mModelTransformCB = std::make_shared<D3D11ConstantBuffer<cbModelTransform>>(mDevice, ED3D11ConstantBuffer::Transform);
		mSceneTransformCB = std::make_shared<D3D11ConstantBuffer<cbSceneTransform>>(mDevice, ED3D11ConstantBuffer::Transform);
		mBoneTransformCB = std::make_shared<D3D11ConstantBuffer<cbBoneTransform>>(mDevice, ED3D11ConstantBuffer::Transform);
		mModelTexutreCB = std::make_shared< D3D11ConstantBuffer<cbModelTexture>>(mDevice, ED3D11ConstantBuffer::Transform);
		mDirectioanlShadowInfoCB = std::make_shared< D3D11ConstantBuffer<cbShadowTransformCascaseEnd>>(mDevice, ED3D11ConstantBuffer::Transform);
		mAlphaDataCB = std::make_shared< D3D11ConstantBuffer<cbAlpha>>(mDevice, ED3D11ConstantBuffer::Transform);

		mViewport.Width = (float)width;
		mViewport.Height = (float)height;
		mViewport.MinDepth = 0.f;
		mViewport.MaxDepth = 1.f;
		mViewport.TopLeftX = 0.f;
		mViewport.TopLeftY = 0.f;
	}
	void TransparentRenderPass::Finalize()
	{
		mDevice = nullptr;
		mJobManager = nullptr;
		mCameraManager = nullptr;
		mLightManager = nullptr;
		mResourceManager = nullptr;

		mColoraccumulationRTV = nullptr;
		mPixelRevealageThresholdRTV = nullptr;
		mDefaultDSV = nullptr;
		mShadowSRV = nullptr;

		mStaticMeshLayout = nullptr;
		mSkinnedMeshLayout = nullptr;
		mStaticMeshVS = nullptr;
		mSkinnedMeshVS = nullptr;
		mTransparentRenderPS = nullptr;

		mAnisotropicWrapSamplerState = nullptr;
		mShadowSampler = nullptr;
		mOITRenderState = nullptr;
		mDisableDepthWriteState = nullptr;

		mModelTransformCB = nullptr;
		mSceneTransformCB = nullptr;
		mBoneTransformCB = nullptr;
		mModelTexutreCB = nullptr;
	}
	void TransparentRenderPass::OnResize(unsigned short width, unsigned short height)
	{
		mViewport.Width = (float)width;
		mViewport.Height = (float)height;
		mViewport.MinDepth = 0.f;
		mViewport.MaxDepth = 1.f;
		mViewport.TopLeftX = 0.f;
		mViewport.TopLeftY = 0.f;

		mColoraccumulationRTV->OnResize(mDevice, ED3D11RenderTargetViewType::ColorAcuumulation, width, height);
		mPixelRevealageThresholdRTV->OnResize(mDevice, ED3D11RenderTargetViewType::PixeldRevealageThreshold, width, height);
	}
	void TransparentRenderPass::Render()
	{
		using namespace DirectX::SimpleMath;

		// update
		{
			size_t currentDirectionaShadowCount = mLightManager->GetDirectionalShadows().size();
			cbShadowTransformCascaseEnd directionalShadowData;
			directionalShadowData.ShadowCount = currentDirectionaShadowCount;

			if (currentDirectionaShadowCount > 0)
			{
				const std::vector<std::shared_ptr<Light<DirectionalLight>>>& directioanlShadows = mLightManager->GetDirectionalShadows();
				cbShadowTransform directionalShadowTransformData;
				directionalShadowTransformData.ShadowCount = currentDirectionaShadowCount;


				for (size_t i = 0; i < currentDirectionaShadowCount; ++i)
				{
					std::vector<float> cascadeEnds = ShadowPass::CalculateCascadeEnds({ 0.33, 0.66 },
						mCameraManager->GetNearPlain(ECameraType::Player),
						mCameraManager->GetFarPlain(ECameraType::Player));

					std::vector<DirectX::SimpleMath::Matrix> shadowTransforms = ShadowPass::CalculateCascadeShadowTransform(cascadeEnds,
						mCameraManager->GetViewMatrix(ECameraType::Player),
						mCameraManager->GetProjectionMatrix(ECameraType::Player),
						directioanlShadows[i]->GetData().direction);
					assert(shadowTransforms.size() == 3);

					DirectX::SimpleMath::Matrix texTransform =
					{
						 0.5f, 0.0f, 0.0f, 0.0f,
						 0.0f, -0.5f, 0.0f, 0.0f,
						 0.0f, 0.0f, 1.0f, 0.0f,
						 0.5f, 0.5f, 0.0f, 1.0f
					};
					auto cameraProj = mCameraManager->GetProjectionMatrix(ECameraType::Player);
					size_t shaodwIndex = i * cbShadowTransform::MAX_SHADOW_COUNT;

					directionalShadowTransformData.ShadowViewProj[shaodwIndex] = (shadowTransforms[0] * texTransform).Transpose();
					directionalShadowData.CascadeEnds[i].x = Vector4::Transform({ 0, 0, cascadeEnds[1], 1.f }, cameraProj).z;
					directionalShadowTransformData.ShadowViewProj[shaodwIndex + 1] = (shadowTransforms[1] * texTransform).Transpose();
					directionalShadowData.CascadeEnds[i].y = Vector4::Transform({ 0, 0, cascadeEnds[2], 1.f }, cameraProj).z;
					directionalShadowTransformData.ShadowViewProj[shaodwIndex + 2] = (shadowTransforms[2] * texTransform).Transpose();
					directionalShadowData.CascadeEnds[i].z = Vector4::Transform({ 0, 0, cascadeEnds[3], 1.f }, cameraProj).z;
				}
			}

			mDirectioanlShadowInfoCB->Update(mDevice, directionalShadowData);

			cbSceneTransform sceneTransform;
			sceneTransform.ViewProjMat = mCameraManager->GetViewMatrix(ECameraType::Player) * mCameraManager->GetProjectionMatrix(ECameraType::Player);
			sceneTransform.ViewProjMat = sceneTransform.ViewProjMat.Transpose();
			mSceneTransformCB->Update(mDevice, sceneTransform);

			mLightManager->UpdateConstantBuffer(mDevice, mCameraManager->GetPosition(ECameraType::Player), false);
		}

		// init
		{
			mColoraccumulationRTV->Clear(mDevice, { 0,0,0,0 });
			mPixelRevealageThresholdRTV->Clear(mDevice, { 1, 1,1,1 });
		}

		// bind
		{
			ID3D11ShaderResourceView* SRVs[10] = { NULL };
			mDevice->GetDeviceContext()->PSSetShaderResources(0, 10, SRVs);

			mDevice->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			mDevice->GetDeviceContext()->RSSetViewports(1, &mViewport);

			std::vector<std::shared_ptr<D3D11RenderTargetView>> RTVs =
			{
				mColoraccumulationRTV,
				mPixelRevealageThresholdRTV
			};
			D3D11RenderTargetView::Bind(mDevice, RTVs, mDefaultDSV);

			mModelTransformCB->Bind(mDevice, ED3D11ShaderType::VertexShader);
			mSceneTransformCB->Bind(mDevice, ED3D11ShaderType::VertexShader, 1);

			mTransparentRenderPS->Bind(mDevice);
			mAnisotropicWrapSamplerState->Bind(mDevice, 0, ED3D11ShaderType::Pixelshader);
			mShadowSampler->Bind(mDevice, 1, ED3D11ShaderType::Pixelshader);
			mOITRenderState->Bind(mDevice);
			mDisableDepthWriteState->Bind(mDevice);
			mShadowSRV->Bind(mDevice, 9, ED3D11ShaderType::Pixelshader);
			mModelTexutreCB->Bind(mDevice, ED3D11ShaderType::Pixelshader);
			mLightManager->GetLightConstnatBuffer()->Bind(mDevice, ED3D11ShaderType::Pixelshader, 1);
			mAlphaDataCB->Bind(mDevice, ED3D11ShaderType::Pixelshader, 2);
			mDirectioanlShadowInfoCB->Bind(mDevice, ED3D11ShaderType::Pixelshader, 3);
		}

		// Draw
		{
			mStaticMeshLayout->Bind(mDevice);
			mStaticMeshVS->Bind(mDevice);

			for (const StaticMeshJob& job : mJobManager->GetStaticMeshJobs())
			{
				if (job.ObjectRenderType == EObjectRenderType::Transparent)
				{
					job.StaticMesh->Bind(mDevice);
					job.Material->Bind(mDevice);

					ConstantBufferHelper::UpdateModelTransformCB(mDevice, mModelTransformCB, *job.TransformPtr);
					ConstantBufferHelper::UpdateModelTextureCB(mDevice, mModelTexutreCB, job.Material);

					cbAlpha alphaData;
					alphaData.bUseAlphaConstant = true;
					alphaData.Alpha = job.Alpha;

					mAlphaDataCB->Update(mDevice, alphaData);

					job.StaticMesh->Draw(mDevice, job.SubsetIndex);
				}
			}

			mSkinnedMeshLayout->Bind(mDevice);
			mSkinnedMeshVS->Bind(mDevice);
			mBoneTransformCB->Bind(mDevice, ED3D11ShaderType::VertexShader, 2);

			for (const SkinnedMeshJob& job : mJobManager->GetSkinnedMeshJobs())
			{
				if (job.ObjectRenderType == EObjectRenderType::Transparent)
				{
					job.SkinnedMesh->Bind(mDevice);
					job.Material->Bind(mDevice);

					ConstantBufferHelper::UpdateModelTransformCB(mDevice, mModelTransformCB, *job.TransformPtr);
					ConstantBufferHelper::UpdateModelTextureCB(mDevice, mModelTexutreCB, job.Material);
					ConstantBufferHelper::UpdateBoneTransformCB(mDevice, mBoneTransformCB, *job.BoneMatricesPtr);

					cbAlpha alphaData;
					alphaData.bUseAlphaConstant = true;
					alphaData.Alpha = job.Alpha;

					mAlphaDataCB->Update(mDevice, alphaData);

					job.SkinnedMesh->Draw(mDevice, job.SubsetIndex);
				}
			}
		}

		// unbind
		{
			mDevice->GetDeviceContext()->OMSetBlendState(NULL, NULL, 0xFFFFFFFF);
			mDevice->GetDeviceContext()->OMSetDepthStencilState(NULL, 0);
		}
	}

}