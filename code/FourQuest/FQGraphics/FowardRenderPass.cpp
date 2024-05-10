#include "FowardRenderPass.h"
#include "D3D11Common.h"
#include "ManagementCommon.h"
#include "ShadowPass.h"
#include "RenderJob.h"
#include "Material.h"
#include "Mesh.h"
#include "Define.h"
#include "D3D11Util.h"

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

		auto staticMeshVS = resourceManager->Create<D3D11VertexShader>(ED3D11VertexShader::StaticModel);
		auto skinnedMeshVS = resourceManager->Create <D3D11VertexShader>(ED3D11VertexShader::SkinnedModel);
		auto noneGS = resourceManager->Create<D3D11GeometryShader>(ED3D11GeometryShader::None);
		auto meshPS = resourceManager->Create<D3D11PixelShader >(ED3D11PixelShader::Model);

		std::shared_ptr<PipelineState> pipelineState = std::make_shared<PipelineState>(
			resourceManager->Create<D3D11RasterizerState>(ED3D11RasterizerState::Default),
			resourceManager->Create<D3D11DepthStencilState>(ED3D11DepthStencilState::Default),
			resourceManager->Create<D3D11BlendState>(ED3D11BlendState::Default));

		mStaticMeshProgram = std::make_shared<D3D11ShaderProgram>(device, resourceManager, staticMeshVS, noneGS, meshPS, pipelineState);
		mSkinnedMeshProgram = std::make_shared<D3D11ShaderProgram>(device, resourceManager, skinnedMeshVS, noneGS, meshPS, pipelineState);

		OnResize(width, height);

		mBackBufferRTV = mResourceManager->Get<fq::graphics::D3D11RenderTargetView>(ED3D11RenderTargetViewType::Offscreen);
		mDSV = mResourceManager->Get<fq::graphics::D3D11DepthStencilView>(ED3D11DepthStencilViewType::Default);
		auto shadowDSV = mResourceManager->Get<D3D11DepthStencilView>(ED3D11DepthStencilViewType::CascadeShadow3);
		mShadowSRV = std::make_shared<D3D11ShaderResourceView>(mDevice, shadowDSV);

		// 포인트라이트 SRV 생성
		auto pointLightShadow = mResourceManager->Get<D3D11DepthStencilView>(ED3D11DepthStencilViewType::PointLightShadow);
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

		mStaticMeshProgram = nullptr;
		mSkinnedMeshProgram = nullptr;
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
			cbShadowTransformCascaseEnd directionalShadowData;
			directionalShadowData.ShadowCount = mLightManager->GetDirectionalShadows().size();

			if (directionalShadowData.ShadowCount > 0)
			{
				const std::vector<std::shared_ptr<Light<DirectionalLight>>>& directioanlShadows = mLightManager->GetDirectionalShadows();

				for (size_t i = 0; i < directionalShadowData.ShadowCount; ++i)
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

					directionalShadowData.ShadowViewProj[shaodwIndex] = (shadowTransforms[0] * texTransform).Transpose();
					directionalShadowData.CascadeEnds[i].x = Vector4::Transform({ 0, 0, cascadeEnds[1], 1.f }, cameraProj).z;
					directionalShadowData.ShadowViewProj[shaodwIndex + 1] = (shadowTransforms[1] * texTransform).Transpose();
					directionalShadowData.CascadeEnds[i].y = Vector4::Transform({ 0, 0, cascadeEnds[2], 1.f }, cameraProj).z;
					directionalShadowData.ShadowViewProj[shaodwIndex + 2] = (shadowTransforms[2] * texTransform).Transpose();
					directionalShadowData.CascadeEnds[i].z = Vector4::Transform({ 0, 0, cascadeEnds[3], 1.f }, cameraProj).z;
				}
			}
			mStaticMeshProgram->UpdateConstantBuffer(mDevice, directionalShadowData);

			cbSceneTransform sceneTransform;
			sceneTransform.ViewProjMat = mCameraManager->GetViewMatrix(ECameraType::Player) * mCameraManager->GetProjectionMatrix(ECameraType::Player);
			sceneTransform.ViewProjMat = sceneTransform.ViewProjMat.Transpose();
			mLightManager->UpdateConstantBuffer(mDevice, mCameraManager->GetPosition(ECameraType::Player), false);
			mStaticMeshProgram->UpdateConstantBuffer(mDevice, sceneTransform);

			cbLight lightData = mLightManager->GetLightData();
			lightData.eyePosition = mCameraManager->GetPosition(ECameraType::Player);
			lightData.isUseIBL = false;
			mStaticMeshProgram->UpdateConstantBuffer(mDevice, lightData);
		}

		// Init
		{
			ID3D11ShaderResourceView* SRVs[10] = { NULL };
			mDevice->GetDeviceContext()->PSSetShaderResources(0, 10, SRVs);

			mBackBufferRTV->Clear(mDevice, { 1,1,1,1 });
			mDSV->Clear(mDevice);
		}

		// Bind
		{
			mDevice->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			mDevice->GetDeviceContext()->RSSetViewports(1, &mViewport);

			mBackBufferRTV->Bind(mDevice, mDSV);

			mShadowSRV->Bind(mDevice, 9, ED3D11ShaderType::Pixelshader);
		}

		// Draw
		{
			mStaticMeshProgram->Bind(mDevice);

			auto modelTrnasformCB = mStaticMeshProgram->GetConstantBufferNT<cbModelTransform>();
			auto modelTextureCB = mStaticMeshProgram->GetConstantBufferNT<cbModelTexture>();

			for (const StaticMeshJob& job : mJobManager->GetStaticMeshJobs())
			{
				if (job.ObjectRenderType == EObjectRenderType::Opaque)
				{
					job.StaticMesh->Bind(mDevice);
					job.Material->Bind(mDevice);

					cbModelTransform modelTransform;
					modelTransform.WorldMat = (*job.TransformPtr).Transpose();
					modelTransform.WorldInvTransposeMat = D3D11Util::InverseTranspose((*job.TransformPtr)).Transpose();
					modelTrnasformCB->Update(mDevice, &modelTransform, sizeof(modelTransform));

					cbModelTexture modelTexture;
					modelTexture.bUseAlbedoMap = job.Material->GetHasBaseColor();
					modelTexture.bUseMetalnessMap = job.Material->GetHasMetalness();
					modelTexture.bUseNormalMap = job.Material->GetHasNormal();
					modelTexture.bUseRoughnessMap = job.Material->GetHasRoughness();
					modelTexture.bUseEmissiveMap = job.Material->GetHasEmissive();
					modelTexture.bUseOpacityMap = job.Material->GetHasOpacity();
					modelTextureCB->Update(mDevice, &modelTexture, sizeof(modelTexture));

					job.StaticMesh->Draw(mDevice, job.SubsetIndex);
				}
			}

			mSkinnedMeshProgram->Bind(mDevice);
			auto boneTransformCB = mSkinnedMeshProgram->GetConstantBufferNT<cbBoneTransform>();

			for (const SkinnedMeshJob& job : mJobManager->GetSkinnedMeshJobs())
			{
				if (job.ObjectRenderType == EObjectRenderType::Opaque)
				{
					job.SkinnedMesh->Bind(mDevice);
					job.Material->Bind(mDevice);

					cbModelTransform modelTransform;
					modelTransform.WorldMat = (*job.TransformPtr).Transpose();
					modelTransform.WorldInvTransposeMat = D3D11Util::InverseTranspose((*job.TransformPtr)).Transpose();
					modelTrnasformCB->Update(mDevice, &modelTransform, sizeof(modelTransform));

					cbModelTexture modelTexture;
					modelTexture.bUseAlbedoMap = job.Material->GetHasBaseColor();
					modelTexture.bUseMetalnessMap = job.Material->GetHasMetalness();
					modelTexture.bUseNormalMap = job.Material->GetHasNormal();
					modelTexture.bUseRoughnessMap = job.Material->GetHasRoughness();
					modelTexture.bUseEmissiveMap = job.Material->GetHasEmissive();
					modelTexture.bUseOpacityMap = job.Material->GetHasOpacity();
					modelTextureCB->Update(mDevice, &modelTexture, sizeof(modelTexture));

					size_t matrixCount = std::min<size_t>((*job.BoneMatricesPtr).size(), (size_t)cbBoneTransform::MAX_BOND_COUNT);
					cbBoneTransform boneTransform;
					memcpy(boneTransform.FinalTransforms, (*job.BoneMatricesPtr).data(), sizeof(DirectX::SimpleMath::Matrix) * matrixCount);
					boneTransformCB->Update(mDevice, &boneTransform, sizeof(boneTransform));

					job.SkinnedMesh->Draw(mDevice, job.SubsetIndex);
				}
			}
		}
	}
}