#include "FowardRenderPass.h"
#include "D3D11Common.h"
#include "ManagementCommon.h"
#include "ShadowPass.h"
#include "RenderJob.h"
#include "Material.h"
#include "Mesh.h"
#include "RenderObject.h"
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

		auto staticMeshVS = std::make_shared<D3D11VertexShader>(mDevice, L"ModelVS.cso");
		auto skinnedMeshVS = std::make_shared<D3D11VertexShader>(mDevice, L"ModelVS_SKINNING.cso");
		auto meshPS = std::make_shared<D3D11PixelShader>(mDevice, L"ModelPS.cso");
		auto pipelieState = std::make_shared<PipelineState>(nullptr, nullptr, nullptr);
		mStaticMeshShaderProgram = std::make_unique<ShaderProgram>(mDevice, staticMeshVS, nullptr, meshPS, pipelieState);
		mSkinnedMeshShaderProgram = std::make_unique<ShaderProgram>(mDevice, skinnedMeshVS, nullptr, meshPS, pipelieState);

		mAnisotropicWrapSamplerState = resourceManager->Create<D3D11SamplerState>(ED3D11SamplerState::AnisotropicWrap);
		mLinearClampSamplerState = resourceManager->Create<D3D11SamplerState>(ED3D11SamplerState::Default);
		mShadowSampler = resourceManager->Create<D3D11SamplerState>(ED3D11SamplerState::Shadow);

		mModelTransformCB = std::make_shared<D3D11ConstantBuffer<ModelTransform>>(mDevice, ED3D11ConstantBuffer::Transform);
		mSceneTransformCB = std::make_shared<D3D11ConstantBuffer<SceneTrnasform>>(mDevice, ED3D11ConstantBuffer::Transform);
		mBoneTransformCB = std::make_shared<D3D11ConstantBuffer<BoneTransform>>(mDevice, ED3D11ConstantBuffer::Transform);
		mMaterialCB = std::make_shared< D3D11ConstantBuffer<CBMaterial>>(mDevice, ED3D11ConstantBuffer::Transform);
		mDirectioanlShadowInfoCB = resourceManager->Create< D3D11ConstantBuffer<DirectionalShadowInfo>>(ED3D11ConstantBuffer::DirectionalShadowInfo);

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

		mStaticMeshShaderProgram = nullptr;
		mSkinnedMeshShaderProgram = nullptr;

		mDefaultRS = nullptr;
		mAnisotropicWrapSamplerState = nullptr;
		mLinearClampSamplerState = nullptr;
		mPointClampSamplerState = nullptr;
		mShadowSampler = nullptr;

		mModelTransformCB = nullptr;
		mSceneTransformCB = nullptr;
		mBoneTransformCB = nullptr;
		mMaterialCB = nullptr;
		mDirectioanlShadowInfoCB = nullptr;
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
			size_t currentDirectionaShadowCount = mLightManager->GetDirectionalShadows().size();
			DirectionalShadowInfo directionalShadowData;
			directionalShadowData.ShadowCount = currentDirectionaShadowCount;

			if (currentDirectionaShadowCount > 0)
			{
				const std::vector<std::shared_ptr<Light<DirectionalLight>>>& directioanlShadows = mLightManager->GetDirectionalShadows();

				for (size_t i = 0; i < currentDirectionaShadowCount; ++i)
				{
					std::vector<float> cascadeEnds = ShadowPass::CalculateCascadeEnds({ 0.33, 0.66 },
						mCameraManager->GetNearPlane(ECameraType::Player),
						mCameraManager->GetFarPlane(ECameraType::Player));

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

					directionalShadowData.ShadowViewProj[shaodwIndex] = (shadowTransforms[0] * texTransform).Transpose();
					directionalShadowData.CascadeEnds[i].x = Vector4::Transform({ 0, 0, cascadeEnds[1], 1.f }, cameraProj).z;
					directionalShadowData.ShadowViewProj[shaodwIndex + 1] = (shadowTransforms[1] * texTransform).Transpose();
					directionalShadowData.CascadeEnds[i].y = Vector4::Transform({ 0, 0, cascadeEnds[2], 1.f }, cameraProj).z;
					directionalShadowData.ShadowViewProj[shaodwIndex + 2] = (shadowTransforms[2] * texTransform).Transpose();
					directionalShadowData.CascadeEnds[i].z = Vector4::Transform({ 0, 0, cascadeEnds[3], 1.f }, cameraProj).z;
				}
			}

			mDirectioanlShadowInfoCB->Update(mDevice, directionalShadowData);

			SceneTrnasform sceneTransform;
			sceneTransform.ViewProjMat = mCameraManager->GetViewMatrix(ECameraType::Player) * mCameraManager->GetProjectionMatrix(ECameraType::Player);
			sceneTransform.ViewProjMat = sceneTransform.ViewProjMat.Transpose();
			mSceneTransformCB->Update(mDevice, sceneTransform);

			mLightManager->UpdateConstantBuffer(mDevice, mCameraManager->GetPosition(ECameraType::Player), true);
		}

		// Init
		{
			mBackBufferRTV->Clear(mDevice, { 0,0,0,0 });
			mDSV->Clear(mDevice);
		}

		// Bind
		{
			ID3D11ShaderResourceView* SRVs[5] = { NULL };
			mDevice->GetDeviceContext()->PSSetShaderResources(0, 5, SRVs);

			mDevice->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			mDevice->GetDeviceContext()->RSSetViewports(1, &mViewport);

			mBackBufferRTV->Bind(mDevice, mDSV);

			mModelTransformCB->Bind(mDevice, ED3D11ShaderType::VertexShader);
			mSceneTransformCB->Bind(mDevice, ED3D11ShaderType::VertexShader, 1);
			mMaterialCB->Bind(mDevice, ED3D11ShaderType::VertexShader, 3);
			mMaterialCB->Bind(mDevice, ED3D11ShaderType::PixelShader);
			mLightManager->GetLightConstnatBuffer()->Bind(mDevice, ED3D11ShaderType::PixelShader, 1);
			mDirectioanlShadowInfoCB->Bind(mDevice, ED3D11ShaderType::PixelShader, 2);
			mAnisotropicWrapSamplerState->Bind(mDevice, 0, ED3D11ShaderType::PixelShader);
			mLinearClampSamplerState->Bind(mDevice, 1, ED3D11ShaderType::PixelShader);
			mShadowSampler->Bind(mDevice, 2, ED3D11ShaderType::PixelShader);

			const auto& iblTexture = mLightManager->GetIBLTexture();
			if (iblTexture.DiffuseIrradiance != nullptr)
			{
				mDevice->GetDeviceContext()->PSSetShaderResources(6, 1, iblTexture.DiffuseIrradiance->GetSRV().GetAddressOf());
			}
			if (iblTexture.SpecularIBL != nullptr)
			{
				mDevice->GetDeviceContext()->PSSetShaderResources(7, 1, iblTexture.SpecularIBL->GetSRV().GetAddressOf());
			}
			if (iblTexture.SpecularBRDF != nullptr)
			{
				mDevice->GetDeviceContext()->PSSetShaderResources(8, 1, iblTexture.SpecularBRDF->GetSRV().GetAddressOf());
			}

			mShadowSRV->Bind(mDevice, 9, ED3D11ShaderType::PixelShader);
		}

		// Draw
		{
			mStaticMeshShaderProgram->Bind(mDevice);

			for (const StaticMeshJob& job : mJobManager->GetStaticMeshJobs())
			{
				const MaterialInfo& materialInfo = job.Material->GetInfo();

				if (materialInfo.RenderModeType == MaterialInfo::ERenderMode::Opaque)
				{
					job.StaticMesh->Bind(mDevice);
					job.Material->Bind(mDevice);

					ConstantBufferHelper::UpdateModelTransformCB(mDevice, mModelTransformCB, job.StaticMeshObject->GetTransform());
					ConstantBufferHelper::UpdateModelTextureCB(mDevice, mMaterialCB, job.Material);

					job.StaticMesh->Draw(mDevice, job.SubsetIndex);
				}
			}


			mSkinnedMeshShaderProgram->Bind(mDevice);
			mBoneTransformCB->Bind(mDevice, ED3D11ShaderType::VertexShader, 2);
			std::vector<DirectX::SimpleMath::Matrix> identityTransforms(BoneTransform::MAX_BOND_COUNT);

			for (const SkinnedMeshJob& job : mJobManager->GetSkinnedMeshJobs())
			{
				const MaterialInfo& materialInfo = job.Material->GetInfo();

				if (materialInfo.RenderModeType == MaterialInfo::ERenderMode::Opaque)
				{
					job.SkinnedMesh->Bind(mDevice);
					job.Material->Bind(mDevice);

					ConstantBufferHelper::UpdateModelTransformCB(mDevice, mModelTransformCB, job.SkinnedMeshObject->GetTransform());
					ConstantBufferHelper::UpdateModelTextureCB(mDevice, mMaterialCB, job.Material);

					if (job.NodeHierarchyInstnace != nullptr)
					{
						ConstantBufferHelper::UpdateBoneTransformCB(mDevice, mBoneTransformCB, job.NodeHierarchyInstnace->GetTransposedFinalTransforms());
					}
					else
					{
						ConstantBufferHelper::UpdateBoneTransformCB(mDevice, mBoneTransformCB, identityTransforms);
					}

					job.SkinnedMesh->Draw(mDevice, job.SubsetIndex);
				}
			}
		}
	}
}