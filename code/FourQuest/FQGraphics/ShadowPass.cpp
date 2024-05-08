#include "ShadowPass.h"
#include "ManagementCommon.h"
#include "D3D11Common.h"
#include "RenderJob.h"
#include "Mesh.h"
#include "Material.h"
#include "BoneHierarchy.h"

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

		D3D_SHADER_MACRO macroSkinning[] =
		{
			{"SKINNING", ""},
			{ NULL, NULL}
		};

		mStaticMeshVS = std::make_shared<D3D11VertexShader>(mDevice, L"./resource/internal/shader/ModelShadowVS.hlsl");
		mSkinnedMeshVS = std::make_shared<D3D11VertexShader>(mDevice, L"./resource/internal/shader/ModelShadowVS.hlsl", macroSkinning);
		mShadowGS = std::make_shared<D3D11GeometryShader>(mDevice, L"./resource/internal/shader/ModelShadowGS.hlsl");
		mStaticMeshLayout = std::make_shared<D3D11InputLayout>(mDevice, mStaticMeshVS->GetBlob().Get());
		mSkinnedMeshLayout = std::make_shared<D3D11InputLayout>(mDevice, mSkinnedMeshVS->GetBlob().Get());

		mShadowRS = std::make_shared<D3D11RasterizerState>(mDevice, ED3D11RasterizerState::Shadow);
		mDefaultRS = std::make_shared<D3D11RasterizerState>(mDevice, ED3D11RasterizerState::Default);

		mModelTransformCB = std::make_shared<D3D11ConstantBuffer<ModelTransform>>(mDevice, ED3D11ConstantBuffer::Transform);
		mSceneTransformCB = std::make_shared<D3D11ConstantBuffer<SceneTrnasform>>(mDevice, ED3D11ConstantBuffer::Transform);
		mBoneTransformCB = std::make_shared<D3D11ConstantBuffer<BoneTransform>>(mDevice, ED3D11ConstantBuffer::Transform);
		mDirectionalShadowTransformCB = std::make_shared<D3D11ConstantBuffer<DirectionalShadowTransform>>(mDevice, ED3D11ConstantBuffer::Transform);
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

		mStaticMeshVS = nullptr;
		mSkinnedMeshVS = nullptr;
		mShadowGS = nullptr;
		mStaticMeshLayout = nullptr;
		mSkinnedMeshLayout = nullptr;

		mShadowRS = nullptr;
		mDefaultRS = nullptr;

		mModelTransformCB = nullptr;
		mSceneTransformCB = nullptr;
		mBoneTransformCB = nullptr;
		mDirectionalShadowTransformCB = nullptr;
	}

	std::vector<float> ShadowPass::CalculateCascadeEnds(std::vector<float> ratios, float nearZ, float farZ)
	{
		std::vector<float> cascadeEnds;
		cascadeEnds.reserve(ratios.size() + 2);

		cascadeEnds.push_back(nearZ);

		float distanceZ = farZ - nearZ;

		for (float ratio : ratios)
		{
			cascadeEnds.push_back(ratio * distanceZ);
		}

		cascadeEnds.push_back(farZ);

		return cascadeEnds;
	}

	std::vector<DirectX::SimpleMath::Matrix> ShadowPass::CalculateCascadeShadowTransform(std::vector<float> cascadeEnds, DirectX::SimpleMath::Matrix cameraView, DirectX::SimpleMath::Matrix cameraProj, DirectX::SimpleMath::Vector3 lightDirection)
	{
		using namespace DirectX::SimpleMath;

		const size_t CASCADE_COUNT = 3u;
		assert(cascadeEnds.size() >= 4);

		DirectX::BoundingFrustum frustum(cameraProj);
		Matrix viewInverse = cameraView.Invert();

		std::array<std::array<Vector3, 8>, CASCADE_COUNT> sliceFrustums;

		for (size_t i = 0; i < sliceFrustums.size(); ++i)
		{
			std::array<Vector3, 8>& sliceFrustum = sliceFrustums[i];
			float curEnd = cascadeEnds[i];
			float nextEnd = cascadeEnds[i + 1];

			sliceFrustum[0] = Vector3::Transform({ frustum.RightSlope * curEnd, frustum.TopSlope * curEnd, curEnd }, viewInverse);
			sliceFrustum[1] = Vector3::Transform({ frustum.RightSlope * curEnd, frustum.BottomSlope * curEnd, curEnd }, viewInverse);
			sliceFrustum[2] = Vector3::Transform({ frustum.LeftSlope * curEnd, frustum.TopSlope * curEnd, curEnd }, viewInverse);
			sliceFrustum[3] = Vector3::Transform({ frustum.LeftSlope * curEnd, frustum.BottomSlope * curEnd, curEnd }, viewInverse);

			sliceFrustum[4] = Vector3::Transform({ frustum.RightSlope * nextEnd, frustum.TopSlope * nextEnd, nextEnd }, viewInverse);
			sliceFrustum[5] = Vector3::Transform({ frustum.RightSlope * nextEnd, frustum.BottomSlope * nextEnd, nextEnd }, viewInverse);
			sliceFrustum[6] = Vector3::Transform({ frustum.LeftSlope * nextEnd, frustum.TopSlope * nextEnd, nextEnd }, viewInverse);
			sliceFrustum[7] = Vector3::Transform({ frustum.LeftSlope * nextEnd, frustum.BottomSlope * nextEnd, nextEnd }, viewInverse);
		}

		std::vector<DirectX::SimpleMath::Matrix> result;
		result.reserve(CASCADE_COUNT);

		for (size_t i = 0; i < CASCADE_COUNT; ++i)
		{
			const std::array<Vector3, 8>& sliceFrustum = sliceFrustums[i];

			DirectX::SimpleMath::Vector3 centerPos = { 0.f, 0.f, 0.f };
			for (const DirectX::SimpleMath::Vector3& pos : sliceFrustum)
			{
				centerPos += pos;
			}
			centerPos /= 8.f;

			float radius = 0.f;
			for (const DirectX::SimpleMath::Vector3& pos : sliceFrustum)
			{
				float distance = DirectX::SimpleMath::Vector3::Distance(centerPos, pos);
				radius = std::max<float>(radius, distance);
			}

			radius = std::ceil(radius * 16.f) / 16.f;

			DirectX::SimpleMath::Vector3 maxExtents = { radius, radius, radius };
			DirectX::SimpleMath::Vector3 minExtents = -maxExtents;
			DirectX::SimpleMath::Vector3 shadowPos = centerPos + lightDirection * minExtents.z;

			DirectX::SimpleMath::Vector3 cascadeExtents = maxExtents - minExtents;
			DirectX::SimpleMath::Matrix lightView = DirectX::XMMatrixLookAtLH(shadowPos, centerPos, { 0, 1, 0 });
			DirectX::SimpleMath::Matrix lightProj = DirectX::XMMatrixOrthographicOffCenterLH(minExtents.x, maxExtents.x, minExtents.y, maxExtents.y, 0.f, cascadeExtents.z);

			result.push_back(lightView * lightProj);
		}

		return result;
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

		const std::vector<std::shared_ptr<Light<DirectionalLight>>>& directioanlShadows = mLightManager->GetDirectionalShadows();
		DirectionalShadowTransform directionalShadowTransformData;
		directionalShadowTransformData.ShadowCount = currentDirectionaShadowCount;

		for (size_t i = 0; i < currentDirectionaShadowCount; ++i)
		{
			std::vector<float> cascadeEnds = CalculateCascadeEnds({ 0.33, 0.66 },
				mCameraManager->GetNearPlain(ECameraType::Player),
				mCameraManager->GetFarPlain(ECameraType::Player));

			std::vector<DirectX::SimpleMath::Matrix> shadowTransforms = CalculateCascadeShadowTransform(cascadeEnds,
				mCameraManager->GetViewMatrix(ECameraType::Player),
				mCameraManager->GetProjectionMatrix(ECameraType::Player),
				directioanlShadows[i]->GetData().direction);
			assert(shadowTransforms.size() == 3);


			size_t shaodwIndex = i * DirectionalShadowTransform::MAX_SHADOW_COUNT;

			directionalShadowTransformData.ShadowViewProj[shaodwIndex + 0] = shadowTransforms[0].Transpose();
			directionalShadowTransformData.ShadowViewProj[shaodwIndex + 1] = shadowTransforms[1].Transpose();
			directionalShadowTransformData.ShadowViewProj[shaodwIndex + 2] = shadowTransforms[2].Transpose();
		}

		mDirectionalShadowTransformCB->Update(mDevice, directionalShadowTransformData);

		// 쉐이더 프로그램을 하나 만들어서 처리 대상을 줄이면 좋을 거 같긴 하네
		ID3D11ShaderResourceView* SRVs[10] = { NULL };
		mDevice->GetDeviceContext()->PSSetShaderResources(0, 10, SRVs);

		mDevice->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		mDevice->GetDeviceContext()->RSSetViewports(1, &mViewport);

		mCascadeShadowDSV->Clear(mDevice);
		ID3D11RenderTargetView* renderTargets[1] = { NULL, };
		mDevice->GetDeviceContext()->OMSetRenderTargets(1, renderTargets, mCascadeShadowDSV->GetDSV().Get());

		mStaticMeshLayout->Bind(mDevice);
		mStaticMeshVS->Bind(mDevice);
		mShadowGS->Bind(mDevice);
		mDevice->GetDeviceContext()->PSSetShader(NULL, NULL, NULL);

		mModelTransformCB->Bind(mDevice, ED3D11ShaderType::VertexShader);
		mDirectionalShadowTransformCB->Bind(mDevice, ED3D11ShaderType::GeometryShader);
		mShadowRS->Bind(mDevice);

		for (const StaticMeshJob& job : mJobManager->GetStaticMeshJobs())
		{
			if (job.bUseShadow)
			{
				job.StaticMesh->Bind(mDevice);
				job.Material->Bind(mDevice);

				ConstantBufferHelper::UpdateModelTransformCB(mDevice, mModelTransformCB, *job.TransformPtr);

				job.StaticMesh->Draw(mDevice, job.SubsetIndex);
			}
		}

		mSkinnedMeshLayout->Bind(mDevice);
		mSkinnedMeshVS->Bind(mDevice);
		mBoneTransformCB->Bind(mDevice, ED3D11ShaderType::VertexShader, 1);

		for (const SkinnedMeshJob& job : mJobManager->GetSkinnedMeshJobs())
		{
			if (job.bUseShadow)
			{
				job.SkinnedMesh->Bind(mDevice);
				job.Material->Bind(mDevice);

				ConstantBufferHelper::UpdateModelTransformCB(mDevice, mModelTransformCB, *job.TransformPtr);
				ConstantBufferHelper::UpdateBoneTransformCB(mDevice, mBoneTransformCB, *job.BoneMatricesPtr);

				job.SkinnedMesh->Draw(mDevice, job.SubsetIndex);
			}
		}

		mDefaultRS->Bind(mDevice);
		mDevice->GetDeviceContext()->GSSetShader(NULL, NULL, NULL);
	}
}