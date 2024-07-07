#include "DeferredGeometryPass.h"
#include "ManagementCommon.h"
#include "D3D11Common.h"
#include "RenderJob.h"
#include "Mesh.h"
#include "Material.h"
#include "NodeHierarchy.h"
#include "RenderObject.h"
#include "Define.h"

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
		mMetalnessRTV = mResourceManager->Get<D3D11RenderTargetView>(ED3D11RenderTargetViewType::Metalness);
		mRoughnessRTV = mResourceManager->Get<D3D11RenderTargetView>(ED3D11RenderTargetViewType::Roughness);
		mNormalRTV = mResourceManager->Get<D3D11RenderTargetView>(ED3D11RenderTargetViewType::Normal);
		mEmissiveRTV = mResourceManager->Get<D3D11RenderTargetView>(ED3D11RenderTargetViewType::Emissive);
		mPositionRTV = mResourceManager->Get<D3D11RenderTargetView>(ED3D11RenderTargetViewType::PositionWClipZ);
		mSourceNormalRTV = mResourceManager->Get<D3D11RenderTargetView>(ED3D11RenderTargetViewType::SourceNormal);
		mSourceTangentRTV = mResourceManager->Get<D3D11RenderTargetView>(ED3D11RenderTargetViewType::SourceTangent);
		mDSV = mResourceManager->Get<D3D11DepthStencilView>(ED3D11DepthStencilViewType::Default);

		auto staticMeshVS = std::make_shared<D3D11VertexShader>(mDevice, L"ModelVS.cso");
		auto staticMeshInstancingVS = std::make_shared<D3D11VertexShader>(mDevice, L"ModelVS_INSTANCING.cso");
		auto skinnedMeshVS = std::make_shared<D3D11VertexShader>(mDevice, L"ModelVS_SKINNING.cso");
		auto geometryPS = std::make_shared<D3D11PixelShader>(mDevice, L"ModelPSDeferred_GEOMETRY.cso");
		mLessEqualStencilReplaceState = mResourceManager->Create<D3D11DepthStencilState>(ED3D11DepthStencilState::LessEqualStencilWriteReplace);
		auto skinningPipelieState = std::make_shared<PipelineState>(nullptr, nullptr, nullptr);
		auto pipelieState = std::make_shared<PipelineState>(nullptr, nullptr, nullptr);
		mStaticMeshShaderProgram = std::make_unique<ShaderProgram>(mDevice, staticMeshVS, nullptr, geometryPS, pipelieState);
		mStaticMeshInstancingShaderProgram = std::make_unique<ShaderProgram>(mDevice, staticMeshInstancingVS, nullptr, geometryPS, pipelieState);
		mSkinnedMeshShaderProgram = std::make_unique<ShaderProgram>(mDevice, skinnedMeshVS, nullptr, geometryPS, pipelieState);

		mAnisotropicWrapSamplerState = resourceManager->Create<D3D11SamplerState>(ED3D11SamplerState::AnisotropicWrap);

		mModelTransformCB = std::make_shared<D3D11ConstantBuffer<ModelTransform>>(mDevice, ED3D11ConstantBuffer::Transform);
		mSceneTransformCB = std::make_shared<D3D11ConstantBuffer<SceneTrnasform>>(mDevice, ED3D11ConstantBuffer::Transform);
		mBoneTransformCB = std::make_shared<D3D11ConstantBuffer<BoneTransform>>(mDevice, ED3D11ConstantBuffer::Transform);
		mMaterialCB = std::make_shared< D3D11ConstantBuffer<CBMaterial>>(mDevice, ED3D11ConstantBuffer::Transform);

		mInstancingVertexBuffer = std::make_shared<D3D11VertexBuffer>(mDevice, MAX_INSTANCEING_SIZE * sizeof(DirectX::SimpleMath::Matrix), sizeof(DirectX::SimpleMath::Matrix), 0);

		D3D11_INPUT_ELEMENT_DESC inputlayouts[] =
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TANGENT",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{ "WORLD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			{ "WORLD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			{ "WORLD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			{ "WORLD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1 }
		};

		HR(device->GetDevice()->CreateInputLayout(
			inputlayouts,
			ARRAYSIZE(inputlayouts),
			staticMeshInstancingVS->GetBlob()->GetBufferPointer(),
			staticMeshInstancingVS->GetBlob()->GetBufferSize(),
			mInstanceIL.GetAddressOf()
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
		mMetalnessRTV = nullptr;
		mRoughnessRTV = nullptr;
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
			mMetalnessRTV->Clear(mDevice);
			mRoughnessRTV->Clear(mDevice);
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
			renderTargetViews.push_back(mMetalnessRTV);
			renderTargetViews.push_back(mRoughnessRTV);
			renderTargetViews.push_back(mNormalRTV);
			renderTargetViews.push_back(mEmissiveRTV);
			renderTargetViews.push_back(mPositionRTV);
			renderTargetViews.push_back(mSourceNormalRTV);
			renderTargetViews.push_back(mSourceTangentRTV);
			D3D11RenderTargetView::Bind(mDevice, renderTargetViews, mDSV);

			mDevice->GetDeviceContext()->RSSetViewports(1, &mViewport);

			mDevice->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			mModelTransformCB->Bind(mDevice, ED3D11ShaderType::VertexShader);
			mSceneTransformCB->Bind(mDevice, ED3D11ShaderType::VertexShader, 1);
			mMaterialCB->Bind(mDevice, ED3D11ShaderType::VertexShader, 3);

			mMaterialCB->Bind(mDevice, ED3D11ShaderType::PixelShader);
			mAnisotropicWrapSamplerState->Bind(mDevice, 0, ED3D11ShaderType::PixelShader);
		}

		// Draw
		{
			// staticMesh
			{
				mStaticMeshShaderProgram->Bind(mDevice);

				for (const StaticMeshJob& job : mJobManager->GetStaticMeshJobs())
				{
					std::shared_ptr<StaticMesh> staticMesh = std::static_pointer_cast<StaticMesh>(job.StaticMeshObject->GetStaticMesh());
					staticMesh->Bind(mDevice);

					const auto& materialInterfaces = job.StaticMeshObject->GetMaterials();

					for (size_t subsetIndex = 0; subsetIndex < materialInterfaces.size(); ++subsetIndex)
					{
						std::shared_ptr<Material> material = std::static_pointer_cast<Material>(materialInterfaces[subsetIndex]);
						const MaterialInfo& materialInfo = material->GetInfo();

						if (materialInfo.RenderModeType == MaterialInfo::ERenderMode::Opaque)
						{
							material->Bind(mDevice);

							ConstantBufferHelper::UpdateModelTransformCB(mDevice, mModelTransformCB, job.Transform);
							ConstantBufferHelper::UpdateModelTextureCB(mDevice, mMaterialCB, material);

							staticMesh->Draw(mDevice, subsetIndex);
						}
					}
				}
			}

			// instanceing staticMesh
			{
				mStaticMeshInstancingShaderProgram->Bind(mDevice);
				mDevice->GetDeviceContext()->IASetInputLayout(mInstanceIL.Get());
				mInstancingVertexBuffer->Bind(mDevice, 1);

				for (const IStaticMeshObject* iStaticMeshObject : mJobManager->GetInstanceStaticMeshObjects())
				{
					const StaticMeshObject* staticMeshObject = static_cast<const StaticMeshObject*>(iStaticMeshObject);
					const std::vector<DirectX::SimpleMath::Matrix>& transforms = staticMeshObject->GetInstanceData();
					const size_t INSTNACING_COUNT = transforms.size();

					if (staticMeshObject->GetInstanceInfo().bIsAppliedDecal)
					{
						mLessEqualStencilReplaceState->Bind(mDevice, 0);
					}
					else
					{
						mLessEqualStencilReplaceState->Bind(mDevice, 1);
					}

					for (size_t i = 0; i < INSTNACING_COUNT; i += MAX_INSTANCEING_SIZE)
					{
						size_t currentInstancingCount = std::min<size_t>(INSTNACING_COUNT - i, MAX_INSTANCEING_SIZE);
						mInstancingVertexBuffer->Update(mDevice, (void*)& transforms[i], currentInstancingCount, sizeof(DirectX::SimpleMath::Matrix));

						auto staticMesh = std::static_pointer_cast<StaticMesh>(staticMeshObject->GetStaticMesh());
						staticMesh->Bind(mDevice);

						auto& materialInterfaces = staticMeshObject->GetMaterials();

						for (size_t subsetIndex = 0; subsetIndex < materialInterfaces.size(); ++subsetIndex)
						{
							auto material = std::static_pointer_cast<Material>(materialInterfaces[subsetIndex]);
							const MaterialInfo& materialInfo = material->GetInfo();

							if (materialInfo.RenderModeType == MaterialInfo::ERenderMode::Opaque)
							{
								ConstantBufferHelper::UpdateModelTextureCB(mDevice, mMaterialCB, material);
								material->Bind(mDevice);

								staticMesh->DrawInstancing(mDevice, currentInstancingCount, subsetIndex);
							}
						}
					}
				}
			}

			mSkinnedMeshShaderProgram->Bind(mDevice);
			mBoneTransformCB->Bind(mDevice, ED3D11ShaderType::VertexShader, 2);
			std::vector<DirectX::SimpleMath::Matrix> identityTransform(BoneTransform::MAX_BOND_COUNT);

			for (const SkinnedMeshJob& job : mJobManager->GetSkinnedMeshJobs())
			{
				const MaterialInfo& materialInfo = job.Material->GetInfo();

				if (materialInfo.RenderModeType == MaterialInfo::ERenderMode::Opaque)
				{
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
}