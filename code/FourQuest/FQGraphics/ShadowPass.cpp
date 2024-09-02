#include "ShadowPass.h"
#include "ManagementCommon.h"
#include "D3D11Common.h"
#include "RenderJob.h"
#include "Mesh.h"
#include "Material.h"
#include "NodeHierarchy.h"
#include "RenderObject.h"

namespace fq::graphics
{
	void ShadowPass::Initialize(std::shared_ptr<D3D11Device> device,
		std::shared_ptr<D3D11JobManager> jobManager,
		std::shared_ptr<D3D11CameraManager> cameraManager,
		std::shared_ptr<D3D11ResourceManager> resourceManager,
		std::shared_ptr< D3D11LightManager> lightManager)
	{
		Finalize();

		mDevice = device;
		mJobManager = jobManager;
		mCameraManager = cameraManager;
		mResourceManager = resourceManager;
		mLightManager = lightManager;

		mCascadeShadowDSV = mResourceManager->Create<D3D11DepthStencilView>(ED3D11DepthStencilViewType::CascadeShadow3, SHADOW_SIZE, SHADOW_SIZE);
		mPointLightShadowDSV = mResourceManager->Create<D3D11DepthStencilView>(ED3D11DepthStencilViewType::PointLightShadow, Point_LIGHT_SHADOW_SIZE, Point_LIGHT_SHADOW_SIZE);

		mViewport.Width = (float)SHADOW_SIZE;
		mViewport.Height = (float)SHADOW_SIZE;
		mViewport.MinDepth = 0.f;
		mViewport.MaxDepth = 1.f;
		mViewport.TopLeftX = 0.f;
		mViewport.TopLeftY = 0.f;

		auto staticMeshVS = std::make_shared<D3D11VertexShader>(mDevice, L"ModelShadowVS.cso");
		auto skinnedMeshVS = std::make_shared<D3D11VertexShader>(mDevice, L"ModelShadowVS_SKINNING.cso");
		auto shadowGS = std::make_shared<D3D11GeometryShader>(mDevice, L"ModelShadowGS.cso");
		auto shadowRS = std::make_shared<D3D11RasterizerState>(mDevice, ED3D11RasterizerState::Shadow);
		auto pipelieState = std::make_shared<PipelineState>(shadowRS, nullptr, nullptr);
		mStaticMeshShaderProgram = std::make_unique<ShaderProgram>(mDevice, staticMeshVS, shadowGS, nullptr, pipelieState);
		mSkinnedMeshShaderProgram = std::make_unique<ShaderProgram>(mDevice, skinnedMeshVS, shadowGS, nullptr, pipelieState);

		mModelTransformCB = std::make_shared<D3D11ConstantBuffer<ModelTransform>>(mDevice, ED3D11ConstantBuffer::Transform);
		mSceneTransformCB = std::make_shared<D3D11ConstantBuffer<SceneTrnasform>>(mDevice, ED3D11ConstantBuffer::Transform);
		mBoneTransformCB = std::make_shared<D3D11ConstantBuffer<BoneTransform>>(mDevice, ED3D11ConstantBuffer::Transform);
		mLightConditionCB = std::make_shared<D3D11ConstantBuffer<cbLightCondition>>(mDevice, ED3D11ConstantBuffer::Transform);
	}

	void ShadowPass::Finalize()
	{
		mDevice = nullptr;
		mJobManager = nullptr;
		mCameraManager = nullptr;
		mResourceManager = nullptr;
		mLightManager = nullptr;

		mCascadeShadowDSV = nullptr;
		mPointLightShadowDSV = nullptr;

		mStaticMeshShaderProgram = nullptr;
		mSkinnedMeshShaderProgram = nullptr;

		mModelTransformCB = nullptr;
		mSceneTransformCB = nullptr;
		mBoneTransformCB = nullptr;
		mLightConditionCB = nullptr;
	}

	void ShadowPass::Render()
	{
		// pointLight
		{
			// 포인트 라이트의 위치를 가져온 후 6방향에 해당하는 카메라를 만들어준다.
			// DSV 바인딩 후 지오메트리 쉐이더로 한 방에 랜더링하기
		}

		// todo : buffer check?
		// if (mPrevDirectionalShadowCount < currentDirectionaShadowCount)
		// {
		// 		// buffer check
		// 		if (currentDirectionaShadowCount == 1)
		// 		{
		//			mCascadeShadowDSV = mResourceManager->Create<D3D11DepthStencilView>(ED3D11DepthStencilViewType::CascadeShadow1, SHADOW_SIZE, SHADOW_SIZE);
		// 		}
		// 		else if (currentDirectionaShadowCount == 2)
		// 		{
		//			mCascadeShadowDSV = mResourceManager->Create<D3D11DepthStencilView>(ED3D11DepthStencilViewType::CascadeShadow2, SHADOW_SIZE, SHADOW_SIZE);
		// 		}
		// 		else if (currentDirectionaShadowCount == 3)
		// 		{
		//			mCascadeShadowDSV = mResourceManager->Create<D3D11DepthStencilView>(ED3D11DepthStencilViewType::CascadeShadow3, SHADOW_SIZE, SHADOW_SIZE);
		// 		}
		// 		else
		// 		{
		//			assert(false);
		// 		}
		// }
		// mPrevDirectionalShadowCount = currentDirectionaShadowCount;

		size_t currentDirectionaShadowCount = mLightManager->GetDirectionalShadows().size();

		if (currentDirectionaShadowCount == 0)
		{
			return;
		}

		// 쉐이더 프로그램을 하나 만들어서 처리 대상을 줄이면 좋을 거 같긴 하네
		ID3D11ShaderResourceView* SRVs[10] = { NULL };
		mDevice->GetDeviceContext()->PSSetShaderResources(0, 10, SRVs);

		mDevice->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		mDevice->GetDeviceContext()->RSSetViewports(1, &mViewport);

		mCascadeShadowDSV->Clear(mDevice);
		ID3D11RenderTargetView* renderTargets[1] = { NULL, };
		mDevice->GetDeviceContext()->OMSetRenderTargets(1, renderTargets, mCascadeShadowDSV->GetDSV().Get());

		mDevice->GetDeviceContext()->PSSetShader(NULL, NULL, NULL);

		mModelTransformCB->Bind(mDevice, ED3D11ShaderType::VertexShader);
		mLightManager->GetShadowConstnatBuffer()->Bind(mDevice, ED3D11ShaderType::GeometryShader);
		mLightConditionCB->Bind(mDevice, ED3D11ShaderType::GeometryShader, 1);

		mStaticMeshShaderProgram->Bind(mDevice);

		std::shared_ptr<StaticMesh> staticMeshCache = nullptr;

		for (const StaticMeshJob& job : mJobManager->GetStaticMeshJobs())
		{
			if (job.StaticMeshObject->GetMeshObjectInfo().bUseShadow)
			{
				if (staticMeshCache != job.StaticMesh)
				{
					staticMeshCache = job.StaticMesh;
					job.StaticMesh->Bind(mDevice);
				}

				ConstantBufferHelper::UpdateModelTransformCB(mDevice, mModelTransformCB, job.StaticMeshObject->GetTransform());

				cbLightCondition lightCondition;
				lightCondition.bIsStatic = job.StaticMeshObject->GetMeshObjectInfo().ObjectType == MeshObjectInfo::EObjectType::Static ? true : false;
				mLightConditionCB->Update(mDevice, lightCondition);

				job.StaticMesh->Draw(mDevice, job.SubsetIndex);
			}
		}

		cbLightCondition lightCondition;
		lightCondition.bIsStatic = false;
		mLightConditionCB->Update(mDevice, lightCondition);

		mBoneTransformCB->Bind(mDevice, ED3D11ShaderType::VertexShader, 1);
		mSkinnedMeshShaderProgram->Bind(mDevice);
		std::vector<DirectX::SimpleMath::Matrix> identityTransform(BoneTransform::MAX_BOND_COUNT);
		ConstantBufferHelper::UpdateBoneTransformCB(mDevice, mBoneTransformCB, identityTransform);

		std::shared_ptr<SkinnedMesh> skinnedMeshCache = nullptr;
		std::shared_ptr<NodeHierarchyInstance> nodeHierarchyInstanceCache = nullptr;

		for (const SkinnedMeshJob& job : mJobManager->GetSkinnedMeshJobs())
		{
			if (job.SkinnedMeshObject->GetMeshObjectInfo().bUseShadow)
			{
				if (skinnedMeshCache != job.SkinnedMesh)
				{
					skinnedMeshCache = job.SkinnedMesh;
					job.SkinnedMesh->Bind(mDevice);
				}

				ConstantBufferHelper::UpdateModelTransformCB(mDevice, mModelTransformCB, job.SkinnedMeshObject->GetTransform());

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

				job.SkinnedMesh->Draw(mDevice, job.SubsetIndex);
			}
		}
	}
}