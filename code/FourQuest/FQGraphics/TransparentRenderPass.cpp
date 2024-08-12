#include "TransparentRenderPass.h"
#include "ManagementCommon.h"
#include "D3D11Common.h"
#include "ShadowPass.h"
#include "Mesh.h"
#include "Material.h"
#include "NodeHierarchy.h"
#include "RenderObject.h"

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
		Finalize();

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

		auto staticMeshVS = std::make_shared<D3D11VertexShader>(mDevice, L"ModelVS.cso");
		auto staticMeshVertexColorVS = std::make_shared<D3D11VertexShader>(mDevice, L"ModelVS_VERTEX_COLOR.cso");
		auto skinnedMeshVS = std::make_shared<D3D11VertexShader>(mDevice, L"ModelVS_SKINNING.cso");

		auto transparentRenderPS = std::make_shared<D3D11PixelShader>(mDevice, L"ModelTransparentPS_RENDER.cso");
		auto transparentRenderVertexColorPS = std::make_shared<D3D11PixelShader>(mDevice, L"ModelTransparentPS_RENDER_VERTEX_COLOR.cso");
		auto disableDepthWriteState = resourceManager->Create<D3D11DepthStencilState>(ED3D11DepthStencilState::DisableDepthWirte);
		auto OITRenderState = resourceManager->Create<D3D11BlendState>(ED3D11BlendState::OITRender);
		auto pipelieState = std::make_shared<PipelineState>(nullptr, disableDepthWriteState, OITRenderState);
		mStaticMeshShaderProgram = std::make_unique<ShaderProgram>(mDevice, staticMeshVS, nullptr, transparentRenderPS, pipelieState);
		mStaticMeshVertexColorShaderProgram = std::make_unique<ShaderProgram>(mDevice, staticMeshVertexColorVS, nullptr, transparentRenderVertexColorPS, pipelieState);
		mSkinnedMeshShaderProgram = std::make_unique<ShaderProgram>(mDevice, skinnedMeshVS, nullptr, transparentRenderPS, pipelieState);

		mLessEqualStencilReplaceState = mResourceManager->Create<D3D11DepthStencilState>(ED3D11DepthStencilState::LessEqualStencilWriteReplace);
		mAnisotropicWrapSamplerState = resourceManager->Create<D3D11SamplerState>(ED3D11SamplerState::AnisotropicWrap);
		mAnisotropicClampSamplerState = resourceManager->Create<D3D11SamplerState>(ED3D11SamplerState::AnisotropicClamp);
		mShadowSampler = resourceManager->Create<D3D11SamplerState>(ED3D11SamplerState::Shadow);
		mDefualtSampler = resourceManager->Create<D3D11SamplerState>(ED3D11SamplerState::Default);
		mLinearWrap = resourceManager->Create<D3D11SamplerState>(ED3D11SamplerState::LinearWrap);
		mDefaultRasterizer = resourceManager->Create<D3D11RasterizerState>(ED3D11RasterizerState::Default);
		mCullOffRasterizer = resourceManager->Create<D3D11RasterizerState>(ED3D11RasterizerState::CullOff);

		mModelTransformCB = std::make_shared<D3D11ConstantBuffer<ModelTransform>>(mDevice, ED3D11ConstantBuffer::Transform);
		mSceneTransformCB = std::make_shared<D3D11ConstantBuffer<SceneTrnasform>>(mDevice, ED3D11ConstantBuffer::Transform);
		mBoneTransformCB = std::make_shared<D3D11ConstantBuffer<BoneTransform>>(mDevice, ED3D11ConstantBuffer::Transform);
		mMaterialCB = std::make_shared< D3D11ConstantBuffer<CBMaterial>>(mDevice, ED3D11ConstantBuffer::Transform);
		mMaterialInstanceCB = std::make_shared<D3D11ConstantBuffer<CBMaterialInstance>>(mDevice, ED3D11ConstantBuffer::Transform);

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

		mStaticMeshShaderProgram = nullptr;
		mSkinnedMeshShaderProgram = nullptr;

		mLessEqualStencilReplaceState = nullptr;
		mAnisotropicWrapSamplerState = nullptr;
		mAnisotropicClampSamplerState = nullptr;
		mShadowSampler = nullptr;
		mDefualtSampler = nullptr;
		mLinearWrap = nullptr;
		mCullOffRasterizer = nullptr;

		mModelTransformCB = nullptr;
		mSceneTransformCB = nullptr;
		mBoneTransformCB = nullptr;
		mMaterialCB = nullptr;
		mMaterialInstanceCB = nullptr;
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

		// Particle
		// update
		{
			SceneTrnasform sceneTransform;
			sceneTransform.ViewProjMat = mCameraManager->GetViewMatrix(ECameraType::Player) * mCameraManager->GetProjectionMatrix(ECameraType::Player);
			sceneTransform.ViewProjMat = sceneTransform.ViewProjMat.Transpose();
			mSceneTransformCB->Update(mDevice, sceneTransform);
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
			mMaterialCB->Bind(mDevice, ED3D11ShaderType::VertexShader, 3);
			mAnisotropicWrapSamplerState->Bind(mDevice, 0, ED3D11ShaderType::PixelShader);
			mShadowSampler->Bind(mDevice, 1, ED3D11ShaderType::PixelShader);
			mDefualtSampler->Bind(mDevice, 2, ED3D11ShaderType::PixelShader);
			mLinearWrap->Bind(mDevice,3, ED3D11ShaderType::PixelShader);
			mShadowSRV->Bind(mDevice, 9, ED3D11ShaderType::PixelShader);
			mMaterialCB->Bind(mDevice, ED3D11ShaderType::PixelShader);
			mLightManager->GetLightConstnatBuffer()->Bind(mDevice, ED3D11ShaderType::PixelShader, 1);
			mMaterialInstanceCB->Bind(mDevice, ED3D11ShaderType::PixelShader, 2);
			mLightManager->GetShadowConstnatBuffer()->Bind(mDevice, ED3D11ShaderType::PixelShader, 3);
		}

		auto bindingState = [this](const MaterialInfo& materialInfo)
			{
				switch (materialInfo.RasterizeType)
				{
				case ERasterizeMode::TwoSide:
					mCullOffRasterizer->Bind(mDevice);
					break;
				case ERasterizeMode::BackFaceClip:
					mDefaultRasterizer->Bind(mDevice);
					break;
				default:
					assert(false);
				}

				switch (materialInfo.SampleType)
				{
				case ESampleMode::Clamp:
					mAnisotropicClampSamplerState->Bind(mDevice, 0, ED3D11ShaderType::PixelShader);
					break;
				case ESampleMode::Wrap:
					mAnisotropicWrapSamplerState->Bind(mDevice, 0, ED3D11ShaderType::PixelShader);
					break;
				default:
					assert(false);
				}
			};

		// Draw
		{
			for (const StaticMeshJob& job : mJobManager->GetStaticMeshJobs())
			{
				const MaterialInfo& materialInfo = job.Material->GetInfo();

				if (materialInfo.RenderModeType == MaterialInfo::ERenderMode::Transparent)
				{
					job.StaticMesh->Bind(mDevice);
					job.Material->Bind(mDevice);

					bindingState(materialInfo);

					if (job.StaticMesh->GetStaticMeshType() == EStaticMeshType::VertexColor)
					{
						mStaticMeshVertexColorShaderProgram->Bind(mDevice);
					}
					else if (job.StaticMesh->GetStaticMeshType() == EStaticMeshType::Default)
					{
						mStaticMeshShaderProgram->Bind(mDevice);
					}

					if (job.StaticMeshObject->GetMeshObjectInfo().bIsAppliedDecal)
					{
						mLessEqualStencilReplaceState->Bind(mDevice, 0);
					}
					else
					{
						mLessEqualStencilReplaceState->Bind(mDevice, 1);
					}

					if (job.NodeHierarchyInstnace != nullptr)
					{
						ConstantBufferHelper::UpdateModelTransformCB(mDevice, mModelTransformCB, job.NodeHierarchyInstnace->GetRootTransform(job.StaticMeshObject->GetReferenceBoneIndex()) * job.StaticMeshObject->GetTransform());
					}
					else
					{
						ConstantBufferHelper::UpdateModelTransformCB(mDevice, mModelTransformCB, job.StaticMeshObject->GetTransform());
					}

					const auto& uvAnimInstnace = job.StaticMeshObject->GetUVAnimationInstanceOrNull();

					if (uvAnimInstnace != nullptr)
					{
						const auto& nodeName = job.StaticMesh->GetMeshData().NodeName;
						const auto& texTransform = uvAnimInstnace->GetTexTransform(nodeName);
						ConstantBufferHelper::UpdateModelTextureCB(mDevice, mMaterialCB, job.Material, texTransform);
					}
					else
					{
						ConstantBufferHelper::UpdateModelTextureCB(mDevice, mMaterialCB, job.Material);
					}

					ConstantBufferHelper::UpdateMaterialInstance(mDevice, mMaterialInstanceCB, job.StaticMeshObject->GetMaterialInstanceInfo());

					job.StaticMesh->Draw(mDevice, job.SubsetIndex);
				}
			}

			mBoneTransformCB->Bind(mDevice, ED3D11ShaderType::VertexShader, 2);
			mSkinnedMeshShaderProgram->Bind(mDevice);
			std::vector<DirectX::SimpleMath::Matrix> identityTransforms(BoneTransform::MAX_BOND_COUNT);

			for (const SkinnedMeshJob& job : mJobManager->GetSkinnedMeshJobs())
			{
				const MaterialInfo& materialInfo = job.Material->GetInfo();

				if (materialInfo.RenderModeType == MaterialInfo::ERenderMode::Transparent)
				{
					job.SkinnedMesh->Bind(mDevice);
					job.Material->Bind(mDevice);

					bindingState(materialInfo);

					ConstantBufferHelper::UpdateModelTransformCB(mDevice, mModelTransformCB, job.SkinnedMeshObject->GetTransform());
					ConstantBufferHelper::UpdateModelTextureCB(mDevice, mMaterialCB, job.Material);

					if (job.SkinnedMeshObject->GetMeshObjectInfo().bIsAppliedDecal)
					{
						mLessEqualStencilReplaceState->Bind(mDevice, 0);
					}
					else
					{
						mLessEqualStencilReplaceState->Bind(mDevice, 1);
					}

					if (job.NodeHierarchyInstnace != nullptr)
					{
						ConstantBufferHelper::UpdateBoneTransformCB(mDevice, mBoneTransformCB, job.NodeHierarchyInstnace->GetTransposedFinalTransforms());
					}
					else
					{
						ConstantBufferHelper::UpdateBoneTransformCB(mDevice, mBoneTransformCB, identityTransforms);
					}

					ConstantBufferHelper::UpdateMaterialInstance(mDevice, mMaterialInstanceCB, job.SkinnedMeshObject->GetMaterialInstanceInfo());

					job.SkinnedMesh->Draw(mDevice, job.SubsetIndex);
				}
			}
		}

	}
}