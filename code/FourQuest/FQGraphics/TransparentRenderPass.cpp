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

		auto staticMeshVS = std::make_shared<D3D11VertexShader>(mDevice, L"./resource/internal/shader/ModelVS.hlsl");
		auto skinnedMeshVS = std::make_shared<D3D11VertexShader>(mDevice, L"./resource/internal/shader/ModelVS.hlsl", macroSkinning);
		auto transparentRenderPS = std::make_shared<D3D11PixelShader>(mDevice, L"./resource/internal/shader/ModelTransparentPS.hlsl", macroRender);
		auto disableDepthWriteState = resourceManager->Create<D3D11DepthStencilState>(ED3D11DepthStencilState::DisableDepthWirte);
		auto OITRenderState = resourceManager->Create<D3D11BlendState>(ED3D11BlendState::OITRender);
		auto pipelieState = std::make_shared<PipelineState>(nullptr, disableDepthWriteState, OITRenderState);
		mStaticMeshShaderProgram = std::make_unique<ShaderProgram>(mDevice, staticMeshVS, nullptr, transparentRenderPS, pipelieState);
		mSkinnedMeshShaderProgram = std::make_unique<ShaderProgram>(mDevice, skinnedMeshVS, nullptr, transparentRenderPS, pipelieState);

		mAnisotropicWrapSamplerState = resourceManager->Create<D3D11SamplerState>(ED3D11SamplerState::AnisotropicWrap);
		mShadowSampler = resourceManager->Create<D3D11SamplerState>(ED3D11SamplerState::Shadow);
		mModelTransformCB = std::make_shared<D3D11ConstantBuffer<ModelTransform>>(mDevice, ED3D11ConstantBuffer::Transform);
		mSceneTransformCB = std::make_shared<D3D11ConstantBuffer<SceneTrnasform>>(mDevice, ED3D11ConstantBuffer::Transform);
		mBoneTransformCB = std::make_shared<D3D11ConstantBuffer<BoneTransform>>(mDevice, ED3D11ConstantBuffer::Transform);
		mModelTexutreCB = std::make_shared< D3D11ConstantBuffer<ModelTexutre>>(mDevice, ED3D11ConstantBuffer::Transform);
		mDirectioanlShadowInfoCB = std::make_shared< D3D11ConstantBuffer<DirectionalShadowInfo>>(mDevice, ED3D11ConstantBuffer::Transform);
		mAlphaDataCB = std::make_shared<D3D11ConstantBuffer<AlphaData>>(mDevice, ED3D11ConstantBuffer::Transform);

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

		mStaticMeshShaderProgram = nullptr;
		mSkinnedMeshShaderProgram = nullptr;

		mColoraccumulationRTV = nullptr;
		mPixelRevealageThresholdRTV = nullptr;
		mDefaultDSV = nullptr;
		mShadowSRV = nullptr;


		mAnisotropicWrapSamplerState = nullptr;
		mShadowSampler = nullptr;

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
			DirectionalShadowInfo directionalShadowData;
			directionalShadowData.ShadowCount = currentDirectionaShadowCount;

			if (currentDirectionaShadowCount > 0)
			{
				const std::vector<std::shared_ptr<Light<DirectionalLight>>>& directioanlShadows = mLightManager->GetDirectionalShadows();
				DirectionalShadowTransform directionalShadowTransformData;
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
					size_t shaodwIndex = i * DirectionalShadowTransform::MAX_SHADOW_COUNT;

					directionalShadowTransformData.ShadowViewProj[shaodwIndex] = (shadowTransforms[0] * texTransform).Transpose();
					directionalShadowData.CascadeEnds[i].x = Vector4::Transform({ 0, 0, cascadeEnds[1], 1.f }, cameraProj).z;
					directionalShadowTransformData.ShadowViewProj[shaodwIndex + 1] = (shadowTransforms[1] * texTransform).Transpose();
					directionalShadowData.CascadeEnds[i].y = Vector4::Transform({ 0, 0, cascadeEnds[2], 1.f }, cameraProj).z;
					directionalShadowTransformData.ShadowViewProj[shaodwIndex + 2] = (shadowTransforms[2] * texTransform).Transpose();
					directionalShadowData.CascadeEnds[i].z = Vector4::Transform({ 0, 0, cascadeEnds[3], 1.f }, cameraProj).z;
				}
			}

			mDirectioanlShadowInfoCB->Update(mDevice, directionalShadowData);

			SceneTrnasform sceneTransform;
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

			mAnisotropicWrapSamplerState->Bind(mDevice, 0, ED3D11ShaderType::Pixelshader);
			mShadowSampler->Bind(mDevice, 1, ED3D11ShaderType::Pixelshader);
			mShadowSRV->Bind(mDevice, 9, ED3D11ShaderType::Pixelshader);
			mModelTexutreCB->Bind(mDevice, ED3D11ShaderType::Pixelshader);
			mLightManager->GetLightConstnatBuffer()->Bind(mDevice, ED3D11ShaderType::Pixelshader, 1);
			mAlphaDataCB->Bind(mDevice, ED3D11ShaderType::Pixelshader, 2);
			mDirectioanlShadowInfoCB->Bind(mDevice, ED3D11ShaderType::Pixelshader, 3);
		}

		// Draw
		{
			mStaticMeshShaderProgram->Bind(mDevice);

			for (const StaticMeshJob& job : mJobManager->GetStaticMeshJobs())
			{
				if (job.ObjectRenderType == EObjectRenderType::Transparent)
				{
					job.StaticMesh->Bind(mDevice);
					job.Material->Bind(mDevice);

					ConstantBufferHelper::UpdateModelTransformCB(mDevice, mModelTransformCB, *job.TransformPtr);
					ConstantBufferHelper::UpdateModelTextureCB(mDevice, mModelTexutreCB, job.Material);

					AlphaData alphaData;
					alphaData.bUseAlphaConstant = true;
					alphaData.Alpha = job.Alpha;

					mAlphaDataCB->Update(mDevice, alphaData);

					job.StaticMesh->Draw(mDevice, job.SubsetIndex);
				}
			}

			mBoneTransformCB->Bind(mDevice, ED3D11ShaderType::VertexShader, 2);
			mSkinnedMeshShaderProgram->Bind(mDevice);

			for (const SkinnedMeshJob& job : mJobManager->GetSkinnedMeshJobs())
			{
				if (job.ObjectRenderType == EObjectRenderType::Transparent)
				{
					job.SkinnedMesh->Bind(mDevice);
					job.Material->Bind(mDevice);

					ConstantBufferHelper::UpdateModelTransformCB(mDevice, mModelTransformCB, *job.TransformPtr);
					ConstantBufferHelper::UpdateModelTextureCB(mDevice, mModelTexutreCB, job.Material);
					ConstantBufferHelper::UpdateBoneTransformCB(mDevice, mBoneTransformCB, *job.BoneMatricesPtr);

					AlphaData alphaData;
					alphaData.bUseAlphaConstant = true;
					alphaData.Alpha = job.Alpha;

					mAlphaDataCB->Update(mDevice, alphaData);

					job.SkinnedMesh->Draw(mDevice, job.SubsetIndex);
				}
			}
		}
	}
}