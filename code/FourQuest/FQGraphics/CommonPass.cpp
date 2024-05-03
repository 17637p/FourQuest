#include "CommonPass.h"

#include "D3D11JobManager.h"
#include "D3D11ResourceManager.h"
#include "D3D11CameraManager.h"
#include "D3D11LightManager.h"
#include "D3D11Common.h"
#include "RenderJob.h"
#include "Material.h"
#include "Mesh.h"
#include "Define.h"

namespace fq::graphics
{
	void ShadowPass::Initialize(std::shared_ptr<D3D11Device> device,
		std::shared_ptr<D3D11JobManager> jobManager,
		std::shared_ptr<D3D11CameraManager> cameraManager,
		std::shared_ptr<D3D11ResourceManager> resourceManager,
		std::shared_ptr< D3D11LightManager> lightManager)
	{
		mDevice = device;
		mJobManager = jobManager;
		mCameraManager = cameraManager;
		mResourceManager = resourceManager;
		mLightManager = lightManager;

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

		mModelTransformCB = std::make_shared<D3D11ConstantBuffer<ModelTransfrom>>(mDevice, ED3D11ConstantBuffer::Transform);
		mSceneTransformCB = std::make_shared<D3D11ConstantBuffer<SceneTrnasform>>(mDevice, ED3D11ConstantBuffer::Transform);
		mBoneTransformCB = std::make_shared<D3D11ConstantBuffer<BoneTransform>>(mDevice, ED3D11ConstantBuffer::Transform);
		mShadowTransformCB = std::make_shared<D3D11ConstantBuffer<ShadowTransform>>(mDevice, ED3D11ConstantBuffer::Transform);

		mCascadeShadowDSV = mResourceManager->Create<D3D11DepthStencilView>(ED3D11DepthStencilViewType::CascadeShadow, SHADOW_SIZE, SHADOW_SIZE);
		mPointLightShadowDSV = mResourceManager->Create<D3D11DepthStencilView>(ED3D11DepthStencilViewType::PointLightShadow, Point_LIGHT_SHADOW_SIZE, Point_LIGHT_SHADOW_SIZE);
	}

	void ShadowPass::Finalize()
	{

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

		if (mLightManager->GetDirectionalLights().size() == 0)
		{
			mCascadeShadowDSV->Clear(mDevice);
			return;
		}

		std::vector<float> cascadeEnds = CalculateCascadeEnds({ 0.33, 0.66 },
			mCameraManager->GetNearPlain(ECameraType::Player),
			mCameraManager->GetFarPlain(ECameraType::Player));
		std::vector<DirectX::SimpleMath::Matrix> shadowTransforms = CalculateCascadeShadowTransform(cascadeEnds,
			mCameraManager->GetViewMatrix(ECameraType::Player),
			mCameraManager->GetProjectionMatrix(ECameraType::Player),
			mLightManager->GetDirectionalLights().front()->GetData().direction);
		assert(shadowTransforms.size() == 3);

		ShadowTransform shadowTransform;
		shadowTransform.ShadowViewProj[0] = shadowTransforms[0].Transpose();
		shadowTransform.ShadowViewProj[1] = shadowTransforms[1].Transpose();
		shadowTransform.ShadowViewProj[2] = shadowTransforms[2].Transpose();
		mShadowTransformCB->Update(mDevice, shadowTransform);
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
		mShadowTransformCB->Bind(mDevice, ED3D11ShaderType::GeometryShader);
		mShadowRS->Bind(mDevice);

		for (const StaticMeshJob& job : mJobManager->GetStaticMeshJobs())
		{
			job.StaticMesh->Bind(mDevice);
			job.Material->Bind(mDevice);

			ConstantBufferHelper::UpdateModelTransformCB(mDevice, mModelTransformCB, *job.TransformPtr);

			job.StaticMesh->Draw(mDevice, job.SubsetIndex);
		}

		mSkinnedMeshLayout->Bind(mDevice);
		mSkinnedMeshVS->Bind(mDevice);
		mBoneTransformCB->Bind(mDevice, ED3D11ShaderType::VertexShader, 1);

		for (const SkinnedMeshJob& job : mJobManager->GetSkinnedMeshJobs())
		{
			job.SkinnedMesh->Bind(mDevice);
			job.Material->Bind(mDevice);

			ConstantBufferHelper::UpdateModelTransformCB(mDevice, mModelTransformCB, *job.TransformPtr);
			ConstantBufferHelper::UpdateBoneTransformCB(mDevice, mBoneTransformCB, *job.BoneMatricesPtr);

			job.SkinnedMesh->Draw(mDevice, job.SubsetIndex);
		}

		mDefaultRS->Bind(mDevice);
		mDevice->GetDeviceContext()->GSSetShader(NULL, NULL, NULL);
	}


	void FullScreenPass::Initialize(std::shared_ptr<D3D11Device> device,
		std::shared_ptr<D3D11ResourceManager> resourceManager,
		unsigned short width,
		unsigned short height)
	{
		mDevice = device;
		mResourceManager = resourceManager;

		mFullScreenVS = std::make_shared<D3D11VertexShader>(device, L"./resource/internal/shader/FullScreenVS.hlsl");
		mFullScreenLayout = std::make_shared<D3D11InputLayout>(device, mFullScreenVS->GetBlob().Get());
		mFullScreenPS = std::make_shared<D3D11PixelShader>(device, L"./resource/internal/shader/FullScreenPS.hlsl");

		mPointClampSamplerState = resourceManager->Create<D3D11SamplerState>(ED3D11SamplerState::PointClamp);

		std::vector<DirectX::SimpleMath::Vector2> positions =
		{
			{ -1, 1 },
			{ 1, 1 },
			{ -1, -1 },
			{ 1, -1 }
		};

		std::vector<unsigned int> indices =
		{
			0,1,2,
			1,3,2
		};

		mFullScreenVB = std::make_shared<D3D11VertexBuffer>(device, positions);
		mFullScreenIB = std::make_shared<D3D11IndexBuffer>(device, indices);

		mViewport.Width = (float)width;
		mViewport.Height = (float)height;
		mViewport.MinDepth = 0.f;
		mViewport.MaxDepth = 1.f;
		mViewport.TopLeftX = 0.f;
		mViewport.TopLeftY = 0.f;

		// 리소스 매니저로부터 얻어오는 게 맞나?
		mSwapChainRTV = mResourceManager->Create<D3D11RenderTargetView>(ED3D11RenderTargetViewType::Default, width, height);
		auto backBufferRTV = mResourceManager->Create<D3D11RenderTargetView>(ED3D11RenderTargetViewType::Offscreen, width, height);
		mBackBufferSRV = std::make_shared<D3D11ShaderResourceView>(mDevice, backBufferRTV);
		mNoneDSV = mResourceManager->Create<D3D11DepthStencilView>(ED3D11DepthStencilViewType::None, width, height);
		mDSV = mResourceManager->Create<D3D11DepthStencilView>(ED3D11DepthStencilViewType::Default, width, height);
	}
	void FullScreenPass::Finalize()
	{

	}
	void FullScreenPass::OnResize(unsigned short width, unsigned short height)
	{
		mViewport.Width = (float)width;
		mViewport.Height = (float)height;
		mViewport.MinDepth = 0.f;
		mViewport.MaxDepth = 1.f;
		mViewport.TopLeftX = 0.f;
		mViewport.TopLeftY = 0.f;

		auto backBufferRTV = mResourceManager->Create<D3D11RenderTargetView>(ED3D11RenderTargetViewType::Offscreen, width, height);
		mBackBufferSRV = std::make_shared<D3D11ShaderResourceView>(mDevice, backBufferRTV);
	}
	void FullScreenPass::Render()
	{
		ID3D11ShaderResourceView* SRVs[10] = { NULL };
		mDevice->GetDeviceContext()->PSSetShaderResources(0, 10, SRVs);

		mDevice->GetDeviceContext()->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		mDevice->GetDeviceContext()->RSSetViewports(1, &mViewport);

		mSwapChainRTV->Bind(mDevice, mNoneDSV);
		mBackBufferSRV->Bind(mDevice, 0, ED3D11ShaderType::Pixelshader);

		mFullScreenLayout->Bind(mDevice);
		mFullScreenVS->Bind(mDevice);
		mFullScreenPS->Bind(mDevice);
		mFullScreenVB->Bind(mDevice);
		mFullScreenIB->Bind(mDevice);
		mPointClampSamplerState->Bind(mDevice, 1, ED3D11ShaderType::Pixelshader);

		mDevice->GetDeviceContext()->DrawIndexed(6, 0, 0);
		mSwapChainRTV->Bind(mDevice, mDSV);
	}
}