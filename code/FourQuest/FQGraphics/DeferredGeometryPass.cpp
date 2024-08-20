#include "DeferredGeometryPass.h"
#include "ManagementCommon.h"
#include "D3D11Common.h"
#include "RenderJob.h"
#include "Mesh.h"
#include "Material.h"
#include "NodeHierarchy.h"
#include "RenderObject.h"
#include "D3D11Util.h"

namespace fq::graphics
{
	void DeferredGeometryPass::Initialize(std::shared_ptr<D3D11Device> device,
		std::shared_ptr<D3D11JobManager> jobManager,
		std::shared_ptr<D3D11CameraManager> cameraManager,
		std::shared_ptr<D3D11ResourceManager> resourceManager,
		std::shared_ptr< D3D11LightManager> lightManager,
		unsigned short width,
		unsigned short height)
	{
		Finalize();

		mDevice = device;
		mJobManager = jobManager;
		mCameraManager = cameraManager;
		mResourceManager = resourceManager;
		mLightManager = lightManager;

		OnResize(width, height);

		// renderTarget
		mAlbedoRTV = mResourceManager->Get<D3D11RenderTargetView>(ED3D11RenderTargetViewType::Albedo);
		mMetalnessRoughnessRTV = mResourceManager->Get<D3D11RenderTargetView>(ED3D11RenderTargetViewType::MetalnessRoughness);
		mNormalRTV = mResourceManager->Get<D3D11RenderTargetView>(ED3D11RenderTargetViewType::Normal);
		mEmissiveRTV = mResourceManager->Get<D3D11RenderTargetView>(ED3D11RenderTargetViewType::Emissive);
		mPositionRTV = mResourceManager->Get<D3D11RenderTargetView>(ED3D11RenderTargetViewType::PositionWClipZ);
		mSourceNormalRTV = mResourceManager->Get<D3D11RenderTargetView>(ED3D11RenderTargetViewType::SourceNormal);
		mSourceTangentRTV = mResourceManager->Get<D3D11RenderTargetView>(ED3D11RenderTargetViewType::SourceTangent);
		mPreCalculatedLightRTV = mResourceManager->Get<D3D11RenderTargetView>(ED3D11RenderTargetViewType::PreCalculatedLight);
		mDSV = mResourceManager->Get<D3D11DepthStencilView>(ED3D11DepthStencilViewType::Default);

		// state
		mLessEqualStencilReplaceState = mResourceManager->Create<D3D11DepthStencilState>(ED3D11DepthStencilState::LessEqualStencilWriteReplace);
		mAnisotropicWrapSamplerState = resourceManager->Create<D3D11SamplerState>(ED3D11SamplerState::AnisotropicWrap);
		mAnisotropicClampSamplerState = resourceManager->Create<D3D11SamplerState>(ED3D11SamplerState::AnisotropicClamp);
		mPointWrapSamplerState = resourceManager->Create<D3D11SamplerState>(ED3D11SamplerState::PointWrap);
		mLinearWrapSamplerState = resourceManager->Create<D3D11SamplerState>(ED3D11SamplerState::LinearWrap);
		mDefaultRasterizer = resourceManager->Create<D3D11RasterizerState>(ED3D11RasterizerState::Default);
		mCullOffRasterizer = resourceManager->Create<D3D11RasterizerState>(ED3D11RasterizerState::CullOff);

		// shader
		auto staticMeshVS = std::make_shared<D3D11VertexShader>(mDevice, L"ModelVS.cso");
		auto staticMeshStaticVS = std::make_shared<D3D11VertexShader>(mDevice, L"ModelVS_STATIC.cso");
		auto staticMeshVertexColorVS = std::make_shared<D3D11VertexShader>(mDevice, L"ModelVS_VERTEX_COLOR.cso");
		auto skinnedMeshVS = std::make_shared<D3D11VertexShader>(mDevice, L"ModelVS_SKINNING.cso");

		auto geometryPS = std::make_shared<D3D11PixelShader>(mDevice, L"ModelPSDeferred_GEOMETRY.cso");
		auto geometryStaticPS = std::make_shared<D3D11PixelShader>(mDevice, L"ModelPSDeferred_GEOMETRY_STATIC.cso");
		auto geometryVertexColorPS = std::make_shared<D3D11PixelShader>(mDevice, L"ModelPSDeferred_GEOMETRY_VERTEX_COLOR.cso");

		auto skinningPipelieState = std::make_shared<PipelineState>(nullptr, nullptr, nullptr);
		auto pipelieState = std::make_shared<PipelineState>(nullptr, nullptr, nullptr);

		mStaticMeshShaderProgram = std::make_unique<ShaderProgram>(mDevice, staticMeshVS, nullptr, geometryPS, pipelieState);
		mLightmapStaticMeshShaderProgram = std::make_unique<ShaderProgram>(mDevice, staticMeshStaticVS, nullptr, geometryStaticPS, pipelieState);
		mVertexColorStaticMeshShaderProgram = std::make_unique<ShaderProgram>(mDevice, staticMeshVertexColorVS, nullptr, geometryVertexColorPS, pipelieState);
		mSkinnedMeshShaderProgram = std::make_unique<ShaderProgram>(mDevice, skinnedMeshVS, nullptr, geometryPS, pipelieState);

		// constant buffer
		mModelTransformCB = std::make_shared<D3D11ConstantBuffer<ModelTransform>>(mDevice, ED3D11ConstantBuffer::Transform);
		mLightMapInformationCB = std::make_shared<D3D11ConstantBuffer<LightMapInfomation>>(mDevice, ED3D11ConstantBuffer::Transform);
		mSceneTransformCB = std::make_shared<D3D11ConstantBuffer<SceneTrnasform>>(mDevice, ED3D11ConstantBuffer::Transform);
		mBoneTransformCB = std::make_shared<D3D11ConstantBuffer<BoneTransform>>(mDevice, ED3D11ConstantBuffer::Transform);
		mMaterialCB = std::make_shared< D3D11ConstantBuffer<CBMaterial>>(mDevice, ED3D11ConstantBuffer::Transform);
		mMaterialInstanceCB = std::make_shared<D3D11ConstantBuffer<CBMaterialInstance>>(mDevice, ED3D11ConstantBuffer::Transform);
	}

	void DeferredGeometryPass::Finalize()
	{
		mDevice = nullptr;
		mJobManager = nullptr;
		mCameraManager = nullptr;
		mResourceManager = nullptr;
		mLightManager = nullptr;

		mDSV = nullptr;

		mAlbedoRTV = nullptr;
		mMetalnessRoughnessRTV = nullptr;
		mNormalRTV = nullptr;
		mEmissiveRTV = nullptr;
		mPositionRTV = nullptr;
		mSourceNormalRTV = nullptr;
		mSourceTangentRTV = nullptr;

		mLessEqualStencilReplaceState = nullptr;
		mAnisotropicWrapSamplerState = nullptr;
		mAnisotropicClampSamplerState = nullptr;
		mPointWrapSamplerState = nullptr;
		mLinearWrapSamplerState = nullptr;
		mDefaultRasterizer = nullptr;
		mCullOffRasterizer = nullptr;

		mStaticMeshShaderProgram = nullptr;
		mLightmapStaticMeshShaderProgram = nullptr;
		mVertexColorStaticMeshShaderProgram = nullptr;
		mSkinnedMeshShaderProgram = nullptr;

		mModelTransformCB = nullptr;
		mLightMapInformationCB = nullptr;
		mSceneTransformCB = nullptr;
		mBoneTransformCB = nullptr;
		mMaterialCB = nullptr;
		mMaterialInstanceCB = nullptr;
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
		using namespace DirectX::SimpleMath;

		// update
		{
			SceneTrnasform sceneTransform;
			sceneTransform.ViewMat = mCameraManager->GetViewMatrix(ECameraType::Player).Transpose();
			sceneTransform.ViewProjMat = mCameraManager->GetViewMatrix(ECameraType::Player) * mCameraManager->GetProjectionMatrix(ECameraType::Player);
			sceneTransform.ViewProjMat = sceneTransform.ViewProjMat.Transpose();
			mSceneTransformCB->Update(mDevice, sceneTransform);
		}

		// Init
		{
			ID3D11ShaderResourceView* NullSRVs[10] = { NULL, };
			mDevice->GetDeviceContext()->PSSetShaderResources(0, ARRAYSIZE(NullSRVs), NullSRVs);

			mDSV->Clear(mDevice);
			mAlbedoRTV->Clear(mDevice);
			mMetalnessRoughnessRTV->Clear(mDevice);
			mNormalRTV->Clear(mDevice, { 1000, 0, 0, 0 });
			mEmissiveRTV->Clear(mDevice);
			mPositionRTV->Clear(mDevice);
			mSourceNormalRTV->Clear(mDevice, { 1000,0,0,0 });
			mSourceTangentRTV->Clear(mDevice, { 1000,0,0,0 });
			mPreCalculatedLightRTV->Clear(mDevice, { 0, 0,0, -1000 });
		}

		bool bUseLightmap = false;
		// Bind
		{
			std::vector<std::shared_ptr<D3D11RenderTargetView>> renderTargetViews;
			renderTargetViews.reserve(8u);
			renderTargetViews.push_back(mAlbedoRTV);
			renderTargetViews.push_back(mMetalnessRoughnessRTV);
			renderTargetViews.push_back(mNormalRTV);
			renderTargetViews.push_back(mEmissiveRTV);
			renderTargetViews.push_back(mPositionRTV);
			renderTargetViews.push_back(mSourceNormalRTV);
			renderTargetViews.push_back(mSourceTangentRTV);
			renderTargetViews.push_back(mPreCalculatedLightRTV);
			D3D11RenderTargetView::Bind(mDevice, renderTargetViews, mDSV);

			mDevice->GetDeviceContext()->RSSetViewports(1, &mViewport);

			mDevice->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			mModelTransformCB->Bind(mDevice, ED3D11ShaderType::VertexShader);
			mSceneTransformCB->Bind(mDevice, ED3D11ShaderType::VertexShader, 1);
			mBoneTransformCB->Bind(mDevice, ED3D11ShaderType::VertexShader, 2);
			mMaterialCB->Bind(mDevice, ED3D11ShaderType::VertexShader, 3);
			mLightMapInformationCB->Bind(mDevice, ED3D11ShaderType::VertexShader, 4);

			mMaterialCB->Bind(mDevice, ED3D11ShaderType::PixelShader);
			mLightMapInformationCB->Bind(mDevice, ED3D11ShaderType::PixelShader, 1);
			mMaterialInstanceCB->Bind(mDevice, ED3D11ShaderType::PixelShader, 2);

			mAnisotropicWrapSamplerState->Bind(mDevice, 0, ED3D11ShaderType::PixelShader);
			mPointWrapSamplerState->Bind(mDevice, 1, ED3D11ShaderType::PixelShader);
			mLinearWrapSamplerState->Bind(mDevice, 2, ED3D11ShaderType::PixelShader);
			mLightManager->GetLightConstnatBuffer()->Bind(mDevice, ED3D11ShaderType::PixelShader, 3);

			const std::shared_ptr<D3D11TextureArray>& lightMapTexture = mLightManager->GetLightMapTextureArray();
			const std::shared_ptr<D3D11TextureArray>& lightmapDirectionTexture = mLightManager->GetLightMapDirectionTextureArray();

			if (lightMapTexture != nullptr)
			{
				lightMapTexture->Bind(mDevice, 6, ED3D11ShaderType::PixelShader);
			}
			if (lightmapDirectionTexture != nullptr)
			{
				lightmapDirectionTexture->Bind(mDevice, 7, ED3D11ShaderType::PixelShader);
			}

			bUseLightmap = lightMapTexture != nullptr && lightmapDirectionTexture != nullptr;
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
			std::shared_ptr<StaticMesh> staticMeshCache = nullptr;
			std::shared_ptr<Material> materialCache = nullptr;

			for (const StaticMeshJob& job : mJobManager->GetStaticMeshJobs())
			{
				const MaterialInfo& materialInfo = job.Material->GetInfo();
				const MeshObjectInfo& meshObjectInfo = job.StaticMeshObject->GetMeshObjectInfo();

				if (materialInfo.RenderModeType == MaterialInfo::ERenderMode::Transparent)
				{
					continue;
				}

				if (staticMeshCache != job.StaticMesh)
				{
					staticMeshCache = job.StaticMesh;
					job.StaticMesh->Bind(mDevice);
				}

				if (materialCache != job.Material)
				{
					materialCache = job.Material;
					job.Material->Bind(mDevice);
				}

				switch (meshObjectInfo.ObjectType)
				{
				case MeshObjectInfo::EObjectType::Static:
				{
					mLightmapStaticMeshShaderProgram->Bind(mDevice);
					LightMapInfomation lightmapInfo;
					lightmapInfo.UVScaleOffset = job.StaticMeshObject->GetLightmapUVScaleOffset();
					lightmapInfo.UVIndex = job.StaticMeshObject->GetLightmapIndex();
					lightmapInfo.bUseLightmap = mLightManager->GetLightMapTextureArray() != nullptr;
					lightmapInfo.bUseDirection = mLightManager->GetLightMapDirectionTextureArray() != nullptr;
					mLightMapInformationCB->Update(mDevice, lightmapInfo);

					break;
				}
				case MeshObjectInfo::EObjectType::Dynamic:
				{
					if (job.StaticMesh->GetStaticMeshType() == EStaticMeshType::VertexColor)
					{
						mVertexColorStaticMeshShaderProgram->Bind(mDevice);
					}
					else if (job.StaticMesh->GetStaticMeshType() == EStaticMeshType::Default)
					{
						mStaticMeshShaderProgram->Bind(mDevice);
					}
					else if (job.StaticMesh->GetStaticMeshType() == EStaticMeshType::Static)
					{
						mStaticMeshShaderProgram->Bind(mDevice);
					}

					break;
				}
				default:
					assert(false);
				}

				bindingState(materialInfo);

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

			mSkinnedMeshShaderProgram->Bind(mDevice);
			std::vector<DirectX::SimpleMath::Matrix> identityTransform(BoneTransform::MAX_BOND_COUNT);
			ConstantBufferHelper::UpdateBoneTransformCB(mDevice, mBoneTransformCB, identityTransform);

			std::shared_ptr<NodeHierarchyInstance> nodeHierarchyInstanceCache = nullptr;
			std::shared_ptr<SkinnedMesh> skinnedMeshCache = nullptr;
			materialCache = nullptr;

			for (const SkinnedMeshJob& job : mJobManager->GetSkinnedMeshJobs())
			{
				const MaterialInfo& materialInfo = job.Material->GetInfo();

				if (materialInfo.RenderModeType == MaterialInfo::ERenderMode::Transparent)
				{
					continue;
				}

				if (skinnedMeshCache != job.SkinnedMesh)
				{
					skinnedMeshCache = job.SkinnedMesh;
					job.SkinnedMesh->Bind(mDevice);
				}

				if (materialCache != job.Material)
				{
					materialCache = job.Material;
					job.Material->Bind(mDevice);
				}

				bindingState(materialInfo);

				if (job.SkinnedMeshObject->GetMeshObjectInfo().bIsAppliedDecal)
				{
					mLessEqualStencilReplaceState->Bind(mDevice, 0);
				}
				else
				{
					mLessEqualStencilReplaceState->Bind(mDevice, 1);
				}

				ConstantBufferHelper::UpdateModelTransformCB(mDevice, mModelTransformCB, job.SkinnedMeshObject->GetTransform());
				ConstantBufferHelper::UpdateModelTextureCB(mDevice, mMaterialCB, job.Material);

				if (nodeHierarchyInstanceCache != job.NodeHierarchyInstnace)
				{
					nodeHierarchyInstanceCache = job.NodeHierarchyInstnace;

					if (job.NodeHierarchyInstnace != nullptr)
					{
						ConstantBufferHelper::UpdateBoneTransformCB(mDevice, mBoneTransformCB, job.NodeHierarchyInstnace->GetTransposedFinalTransforms());
					}
					else
					{
						ConstantBufferHelper::UpdateBoneTransformCB(mDevice, mBoneTransformCB, identityTransform);
					}
				}

				ConstantBufferHelper::UpdateMaterialInstance(mDevice, mMaterialInstanceCB, job.SkinnedMeshObject->GetMaterialInstanceInfo());

				job.SkinnedMesh->Draw(mDevice, job.SubsetIndex);
			}
		}
	}
}