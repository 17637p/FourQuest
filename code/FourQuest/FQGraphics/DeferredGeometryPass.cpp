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

		mAlbedoRTV = mResourceManager->Get<D3D11RenderTargetView>(ED3D11RenderTargetViewType::Albedo);
		mMetalnessRoughnessRTV = mResourceManager->Get<D3D11RenderTargetView>(ED3D11RenderTargetViewType::MetalnessRoughness);
		mNormalRTV = mResourceManager->Get<D3D11RenderTargetView>(ED3D11RenderTargetViewType::Normal);
		mEmissiveRTV = mResourceManager->Get<D3D11RenderTargetView>(ED3D11RenderTargetViewType::Emissive);
		mPositionRTV = mResourceManager->Get<D3D11RenderTargetView>(ED3D11RenderTargetViewType::PositionWClipZ);
		mSourceNormalRTV = mResourceManager->Get<D3D11RenderTargetView>(ED3D11RenderTargetViewType::SourceNormal);
		mSourceTangentRTV = mResourceManager->Get<D3D11RenderTargetView>(ED3D11RenderTargetViewType::SourceTangent);
		mPreCalculatedLightRTV = mResourceManager->Get<D3D11RenderTargetView>(ED3D11RenderTargetViewType::PreCalculatedLight);
		mDSV = mResourceManager->Get<D3D11DepthStencilView>(ED3D11DepthStencilViewType::Default);

		auto staticMeshVS = std::make_shared<D3D11VertexShader>(mDevice, L"ModelVS.cso");
		auto staticMeshStaticVS = std::make_shared<D3D11VertexShader>(mDevice, L"ModelVS_STATIC.cso");
		auto skinnedMeshVS = std::make_shared<D3D11VertexShader>(mDevice, L"ModelVS_SKINNING.cso");
		auto geometryPS = std::make_shared<D3D11PixelShader>(mDevice, L"ModelPSDeferred_GEOMETRY.cso");
		auto geometryStaticPS = std::make_shared<D3D11PixelShader>(mDevice, L"ModelPSDeferred_GEOMETRY_STATIC.cso");
		mLessEqualStencilReplaceState = mResourceManager->Create<D3D11DepthStencilState>(ED3D11DepthStencilState::LessEqualStencilWriteReplace);
		auto skinningPipelieState = std::make_shared<PipelineState>(nullptr, nullptr, nullptr);
		auto pipelieState = std::make_shared<PipelineState>(nullptr, nullptr, nullptr);
		mStaticMeshShaderProgram = std::make_unique<ShaderProgram>(mDevice, staticMeshVS, nullptr, geometryPS, pipelieState);
		mStaticMeshStaticShaderProgram = std::make_unique<ShaderProgram>(mDevice, staticMeshStaticVS, nullptr, geometryStaticPS, pipelieState);
		mSkinnedMeshShaderProgram = std::make_unique<ShaderProgram>(mDevice, skinnedMeshVS, nullptr, geometryPS, pipelieState);

		mAnisotropicWrapSamplerState = resourceManager->Create<D3D11SamplerState>(ED3D11SamplerState::AnisotropicWrap);
		mAnisotropicClampSamplerState = resourceManager->Create<D3D11SamplerState>(ED3D11SamplerState::AnisotropicClamp);
		mDefaultRasterizer = resourceManager->Create<D3D11RasterizerState>(ED3D11RasterizerState::Default);
		mCullOffRasterizer = resourceManager->Create<D3D11RasterizerState>(ED3D11RasterizerState::CullOff);

		mModelTransformCB = std::make_shared<D3D11ConstantBuffer<ModelTransform>>(mDevice, ED3D11ConstantBuffer::Transform);
		mStaticModelTransformCB = std::make_shared<D3D11ConstantBuffer<StaticModelTransform>>(mDevice, ED3D11ConstantBuffer::Transform);
		mSceneTransformCB = std::make_shared<D3D11ConstantBuffer<SceneTrnasform>>(mDevice, ED3D11ConstantBuffer::Transform);
		mBoneTransformCB = std::make_shared<D3D11ConstantBuffer<BoneTransform>>(mDevice, ED3D11ConstantBuffer::Transform);
		mMaterialCB = std::make_shared< D3D11ConstantBuffer<CBMaterial>>(mDevice, ED3D11ConstantBuffer::Transform);

		D3D11_INPUT_ELEMENT_DESC inputLayoutDesc[] =
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "UV", 1, DXGI_FORMAT_R32G32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		HR(mDevice->GetDevice()->CreateInputLayout(
			inputLayoutDesc,
			ARRAYSIZE(inputLayoutDesc),
			staticMeshStaticVS->GetBlob()->GetBufferPointer(),
			staticMeshStaticVS->GetBlob()->GetBufferSize(),
			mStaticMeshnputLayouts.GetAddressOf()
		));
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
		mStaticMeshShaderProgram = nullptr;
		mSkinnedMeshShaderProgram = nullptr;

		mAnisotropicWrapSamplerState = nullptr;

		mModelTransformCB = nullptr;
		mSceneTransformCB = nullptr;
		mBoneTransformCB = nullptr;
		mMaterialCB = nullptr;

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
		}

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
			mSceneTransformCB->Bind(mDevice, ED3D11ShaderType::VertexShader, 1);
			mMaterialCB->Bind(mDevice, ED3D11ShaderType::VertexShader, 3);

			mMaterialCB->Bind(mDevice, ED3D11ShaderType::PixelShader);
			mAnisotropicWrapSamplerState->Bind(mDevice, 0, ED3D11ShaderType::PixelShader);

			const std::shared_ptr<D3D11Texture>& lightMapTexture = mLightManager->GetLightMapTexture();
			if (lightMapTexture != nullptr)
			{
				lightMapTexture->Bind(mDevice, 5, ED3D11ShaderType::PixelShader);
			}
		}

		// Draw
		{
			for (const StaticMeshJob& job : mJobManager->GetStaticMeshJobs())
			{
				const MaterialInfo& materialInfo = job.Material->GetInfo();
				const MeshObjectInfo& meshObjectInfo = job.StaticMeshObject->GetMeshObjectInfo();

				if (materialInfo.RenderModeType == MaterialInfo::ERenderMode::Transparent)
				{
					continue;
				}

				job.StaticMesh->Bind(mDevice);

				switch (meshObjectInfo.ObjectType)
				{
				case MeshObjectInfo::EObjectType::Static:
				{
					mStaticMeshStaticShaderProgram->Bind(mDevice);
					mDevice->GetDeviceContext()->IASetInputLayout(mStaticMeshnputLayouts.Get());
					mStaticModelTransformCB->Bind(mDevice, ED3D11ShaderType::VertexShader);

					if (job.StaticMesh->GetUV1Buffer() != nullptr)
					{
						job.StaticMesh->GetUV1Buffer()->Bind(mDevice, 1);
					}

					StaticModelTransform staticModelTransform;
					staticModelTransform.WorldMat = job.StaticMeshObject->GetTransform().Transpose();
					staticModelTransform.WorldInvTransposeMat = D3D11Util::InverseTranspose(job.StaticMeshObject->GetTransform()).Transpose();
					staticModelTransform.UV1ScaleOffset = job.StaticMeshObject->GetUVScaleOffset();

					mStaticModelTransformCB->Update(mDevice, staticModelTransform);

					break;
				}
				case MeshObjectInfo::EObjectType::Dynamic:
				{
					mStaticMeshShaderProgram->Bind(mDevice);
					job.Material->Bind(mDevice);
					mModelTransformCB->Bind(mDevice, ED3D11ShaderType::VertexShader);

					if (job.NodeHierarchyInstnace != nullptr)
					{
						ConstantBufferHelper::UpdateModelTransformCB(mDevice, mModelTransformCB, job.NodeHierarchyInstnace->GetRootTransform(job.StaticMeshObject->GetReferenceBoneIndex()) * job.StaticMeshObject->GetTransform());
					}
					else
					{
						ConstantBufferHelper::UpdateModelTransformCB(mDevice, mModelTransformCB, job.StaticMeshObject->GetTransform());
					}

					break;
				}
				default:
					assert(false);
				}

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

				if (job.StaticMeshObject->GetMeshObjectInfo().bIsAppliedDecal)
				{
					mLessEqualStencilReplaceState->Bind(mDevice, 0);
				}
				else
				{
					mLessEqualStencilReplaceState->Bind(mDevice, 1);
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

				job.StaticMesh->Draw(mDevice, job.SubsetIndex);
			}

			mSkinnedMeshShaderProgram->Bind(mDevice);
			mBoneTransformCB->Bind(mDevice, ED3D11ShaderType::VertexShader, 2);
			std::vector<DirectX::SimpleMath::Matrix> identityTransform(BoneTransform::MAX_BOND_COUNT);

			for (const SkinnedMeshJob& job : mJobManager->GetSkinnedMeshJobs())
			{
				const MaterialInfo& materialInfo = job.Material->GetInfo();

				if (materialInfo.RenderModeType == MaterialInfo::ERenderMode::Transparent)
				{
					continue;
				}

				job.SkinnedMesh->Bind(mDevice);
				job.Material->Bind(mDevice);

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

				if (job.NodeHierarchyInstnace != nullptr)
				{
					ConstantBufferHelper::UpdateBoneTransformCB(mDevice, mBoneTransformCB, job.NodeHierarchyInstnace->GetTransposedFinalTransforms());
				}
				else
				{
					ConstantBufferHelper::UpdateBoneTransformCB(mDevice, mBoneTransformCB, identityTransform);
				}

				job.SkinnedMesh->Draw(mDevice, job.SubsetIndex);
			}
		}
	}
}