#include "CommonPass.h"

#include "ManagementCommon.h"
#include "D3D11Common.h"
#include "RenderJob.h"
#include "Material.h"
#include "Mesh.h"
#include "Define.h"
#include "../FQCommon/IFQRenderObject.h"

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
		mShadowTransformCB = std::make_shared<D3D11ConstantBuffer<ShadowTransform>>(mDevice, ED3D11ConstantBuffer::Transform);

		mCascadeShadowDSV = mResourceManager->Create<D3D11DepthStencilView>(ED3D11DepthStencilViewType::CascadeShadow, SHADOW_SIZE, SHADOW_SIZE);
		mPointLightShadowDSV = mResourceManager->Create<D3D11DepthStencilView>(ED3D11DepthStencilViewType::PointLightShadow, Point_LIGHT_SHADOW_SIZE, Point_LIGHT_SHADOW_SIZE);
	}

	void ShadowPass::Finalize()
	{
		mDevice = nullptr;
		mJobManager = nullptr;
		mCameraManager = nullptr;
		mResourceManager = nullptr;

		mCascadeShadowDSV = nullptr;

		mStaticMeshVS = nullptr;
		mSkinnedMeshVS = nullptr;
		mStaticMeshLayout = nullptr;
		mSkinnedMeshLayout = nullptr;

		mShadowRS = nullptr;
		mDefaultRS = nullptr;

		mModelTransformCB = nullptr;
		mSceneTransformCB = nullptr;
		mBoneTransformCB = nullptr;
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


	void FullScreenPass::Initialize(std::shared_ptr<D3D11Device> device,
		std::shared_ptr<D3D11ResourceManager> resourceManager,
		unsigned short width,
		unsigned short height)
	{
		Finalize();

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

		mSwapChainRTV = mResourceManager->Create<D3D11RenderTargetView>(ED3D11RenderTargetViewType::Default, width, height);
		mBackBufferRTV = mResourceManager->Create<D3D11RenderTargetView>(ED3D11RenderTargetViewType::Offscreen, width, height);
		mBackBufferSRV = std::make_shared<D3D11ShaderResourceView>(mDevice, mBackBufferRTV);
		mNoneDSV = mResourceManager->Create<D3D11DepthStencilView>(ED3D11DepthStencilViewType::None, width, height);
		mDSV = mResourceManager->Create<D3D11DepthStencilView>(ED3D11DepthStencilViewType::Default, width, height);
	}
	void FullScreenPass::Finalize()
	{
		mDevice = nullptr;
		mResourceManager = nullptr;

		mSwapChainRTV = nullptr;
		mBackBufferSRV = nullptr;
		mNoneDSV = nullptr;
		mDSV = nullptr;

		mFullScreenLayout = nullptr;
		mFullScreenVS = nullptr;
		mFullScreenPS = nullptr;
		mFullScreenVB = nullptr;
		mFullScreenIB = nullptr;
		mPointClampSamplerState = nullptr;
	}
	void FullScreenPass::OnResize(unsigned short width, unsigned short height)
	{
		mViewport.Width = (float)width;
		mViewport.Height = (float)height;
		mViewport.MinDepth = 0.f;
		mViewport.MaxDepth = 1.f;
		mViewport.TopLeftX = 0.f;
		mViewport.TopLeftY = 0.f;

		mBackBufferSRV->Init(mDevice, mBackBufferRTV);
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
		mDefaultDSV = mResourceManager->Create<D3D11DepthStencilView>(ED3D11DepthStencilViewType::Default, width, height);
		auto shadowDSV = mResourceManager->Create<D3D11DepthStencilView>(ED3D11DepthStencilViewType::CascadeShadow, ShadowPass::SHADOW_SIZE, ShadowPass::SHADOW_SIZE);
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

		mStaticMeshVS = std::make_shared<D3D11VertexShader>(mDevice, L"./resource/internal/shader/ModelVS.hlsl");
		mSkinnedMeshVS = std::make_shared<D3D11VertexShader>(mDevice, L"./resource/internal/shader/ModelVS.hlsl", macroSkinning);
		mTransparentRenderPS = std::make_shared<D3D11PixelShader>(mDevice, L"./resource/internal/shader/ModelTransparentPS.hlsl", macroRender);
		mStaticMeshLayout = std::make_shared<D3D11InputLayout>(mDevice, mStaticMeshVS->GetBlob().Get());
		mSkinnedMeshLayout = std::make_shared<D3D11InputLayout>(mDevice, mSkinnedMeshVS->GetBlob().Get());

		mAnisotropicWrapSamplerState = resourceManager->Create<D3D11SamplerState>(ED3D11SamplerState::AnisotropicWrap);
		mShadowSampler = resourceManager->Create<D3D11SamplerState>(ED3D11SamplerState::Shadow);
		mDisableDepthWriteState = resourceManager->Create<D3D11DepthStencilState>(ED3D11DepthStencilState::DisableDepthWirte);
		mOITRenderState = resourceManager->Create<D3D11BlendState>(ED3D11BlendState::OITRender);

		mModelTransformCB = std::make_shared<D3D11ConstantBuffer<ModelTransform>>(mDevice, ED3D11ConstantBuffer::Transform);
		mSceneTransformCB = std::make_shared<D3D11ConstantBuffer<SceneTrnasform>>(mDevice, ED3D11ConstantBuffer::Transform);
		mBoneTransformCB = std::make_shared<D3D11ConstantBuffer<BoneTransform>>(mDevice, ED3D11ConstantBuffer::Transform);
		mModelTexutreCB = std::make_shared< D3D11ConstantBuffer<ModelTexutre>>(mDevice, ED3D11ConstantBuffer::Transform);
		mShadowViewProjTexCB = std::make_shared< D3D11ConstantBuffer<ShadowTransform>>(mDevice, ED3D11ConstantBuffer::Transform);
		mCascadeEndCB = std::make_shared< D3D11ConstantBuffer<CascadeEnd>>(mDevice, ED3D11ConstantBuffer::Transform);
		mAlphaDataCB = std::make_shared< D3D11ConstantBuffer<AlphaData>>(mDevice, ED3D11ConstantBuffer::Transform);

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

		mStaticMeshLayout = nullptr;
		mSkinnedMeshLayout = nullptr;
		mStaticMeshVS = nullptr;
		mSkinnedMeshVS = nullptr;
		mTransparentRenderPS = nullptr;

		mAnisotropicWrapSamplerState = nullptr;

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
			if (mLightManager->GetDirectionalLights().size() != 0)
			{
				std::vector<float> cascadeEnds = ShadowPass::CalculateCascadeEnds({ 0.33, 0.66 },
					mCameraManager->GetNearPlain(ECameraType::Player),
					mCameraManager->GetFarPlain(ECameraType::Player));
				std::vector<DirectX::SimpleMath::Matrix> shadowTransforms = ShadowPass::CalculateCascadeShadowTransform(cascadeEnds,
					mCameraManager->GetViewMatrix(ECameraType::Player),
					mCameraManager->GetProjectionMatrix(ECameraType::Player),
					mLightManager->GetDirectionalLights().front()->GetData().direction);
				assert(shadowTransforms.size() == 3);

				ShadowTransform shadowTransformData;
				DirectX::SimpleMath::Matrix texTransform =
				{
					 0.5f, 0.0f, 0.0f, 0.0f,
					 0.0f, -0.5f, 0.0f, 0.0f,
					 0.0f, 0.0f, 1.0f, 0.0f,
					 0.5f, 0.5f, 0.0f, 1.0f
				};
				shadowTransformData.ShadowViewProj[0] = (shadowTransforms[0] * texTransform).Transpose();
				shadowTransformData.ShadowViewProj[1] = (shadowTransforms[1] * texTransform).Transpose();
				shadowTransformData.ShadowViewProj[2] = (shadowTransforms[2] * texTransform).Transpose();
				mShadowViewProjTexCB->Update(mDevice, shadowTransformData);

				CascadeEnd cascadeEndData;
				auto cameraProj = mCameraManager->GetProjectionMatrix(ECameraType::Player);
				cascadeEndData.CascadeEnds.x = Vector4::Transform({ 0, 0, cascadeEnds[1], 1.f }, cameraProj).z;
				cascadeEndData.CascadeEnds.y = Vector4::Transform({ 0, 0, cascadeEnds[2], 1.f }, cameraProj).z;
				cascadeEndData.CascadeEnds.z = Vector4::Transform({ 0, 0, cascadeEnds[3], 1.f }, cameraProj).z;
				mCascadeEndCB->Update(mDevice, cascadeEndData);
			}
			else
			{
				ShadowTransform shadowTransformData;
				shadowTransformData.ShadowViewProj[0] = Matrix::Identity;
				shadowTransformData.ShadowViewProj[1] = Matrix::Identity;
				shadowTransformData.ShadowViewProj[2] = Matrix::Identity;
				mShadowViewProjTexCB->Update(mDevice, shadowTransformData);

				CascadeEnd cascadeEndData;
				cascadeEndData.CascadeEnds.x = 0;
				cascadeEndData.CascadeEnds.y = 0;
				cascadeEndData.CascadeEnds.z = 0;
				mCascadeEndCB->Update(mDevice, cascadeEndData);
			}

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

			mTransparentRenderPS->Bind(mDevice);
			mAnisotropicWrapSamplerState->Bind(mDevice, 0, ED3D11ShaderType::Pixelshader);
			mShadowSampler->Bind(mDevice, 1, ED3D11ShaderType::Pixelshader);
			mOITRenderState->Bind(mDevice);
			mDisableDepthWriteState->Bind(mDevice);
			mShadowSRV->Bind(mDevice, 9, ED3D11ShaderType::Pixelshader);
			mModelTexutreCB->Bind(mDevice, ED3D11ShaderType::Pixelshader);
			mLightManager->GetLightConstnatBuffer()->Bind(mDevice, ED3D11ShaderType::Pixelshader, 1);
			mAlphaDataCB->Bind(mDevice, ED3D11ShaderType::Pixelshader, 2);
			mShadowViewProjTexCB->Bind(mDevice, ED3D11ShaderType::Pixelshader, 3);
			mCascadeEndCB->Bind(mDevice, ED3D11ShaderType::Pixelshader, 4);
		}

		// Draw
		{
			mStaticMeshLayout->Bind(mDevice);
			mStaticMeshVS->Bind(mDevice);

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

			mSkinnedMeshLayout->Bind(mDevice);
			mSkinnedMeshVS->Bind(mDevice);
			mBoneTransformCB->Bind(mDevice, ED3D11ShaderType::VertexShader, 2);

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

		// unbind
		{
			mDevice->GetDeviceContext()->OMSetBlendState(NULL, NULL, 0xFFFFFFFF);
			mDevice->GetDeviceContext()->OMSetDepthStencilState(NULL, 0);
		}
	}

	void TransparentCompositePass::Initialize(std::shared_ptr<D3D11Device> device,
		std::shared_ptr<D3D11ResourceManager> resourceManager,
		unsigned short width,
		unsigned short height)
	{
		mDevice = device;
		mResourceManager = resourceManager;

		mBackBufferRTV = mResourceManager->Create<D3D11RenderTargetView>(ED3D11RenderTargetViewType::Offscreen, width, height);
		mNullDSV = mResourceManager->Create<D3D11DepthStencilView>(ED3D11DepthStencilViewType::None, width, height);
		mDefaultDSV = mResourceManager->Create<D3D11DepthStencilView>(ED3D11DepthStencilViewType::Default, width, height);
		auto coloraccumulationRTV = mResourceManager->Create<D3D11RenderTargetView>(ED3D11RenderTargetViewType::ColorAcuumulation, width, height);
		auto pixelRevealageThresholdRTV = mResourceManager->Create<D3D11RenderTargetView>(ED3D11RenderTargetViewType::PixeldRevealageThreshold, width, height);
		mColoraccumulationSRV = std::make_shared<D3D11ShaderResourceView>(mDevice, coloraccumulationRTV);
		mPixelRevealageThresholdSRV = std::make_shared<D3D11ShaderResourceView>(mDevice, pixelRevealageThresholdRTV);

		mOITCompositeState = resourceManager->Create<D3D11BlendState>(ED3D11BlendState::OITComposite);

		D3D_SHADER_MACRO macroComposite[] =
		{
			{"COMPOSITE", ""},
			{ NULL, NULL}
		};

		mFullScreenVS = std::make_shared<D3D11VertexShader>(device, L"./resource/internal/shader/FullScreenVS.hlsl");
		mFullScreenLayout = std::make_shared<D3D11InputLayout>(device, mFullScreenVS->GetBlob().Get());
		mTransparentCompositePS = std::make_shared<D3D11PixelShader>(mDevice, L"./resource/internal/shader/ModelTransparentPS.hlsl", macroComposite);
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
	}
	void TransparentCompositePass::Finalize()
	{
		mDevice = nullptr;
		mResourceManager = nullptr;

		mBackBufferRTV = nullptr;
		mDefaultDSV = nullptr;
		mColoraccumulationSRV = nullptr;
		mPixelRevealageThresholdSRV = nullptr;

		mFullScreenLayout = nullptr;
		mFullScreenVS = nullptr;
		mTransparentCompositePS = nullptr;
		mFullScreenVB = nullptr;
		mFullScreenIB = nullptr;
		mPointClampSamplerState = nullptr;
	}
	void TransparentCompositePass::OnResize(unsigned short width, unsigned short height)
	{
		mViewport.Width = (float)width;
		mViewport.Height = (float)height;
		mViewport.MinDepth = 0.f;
		mViewport.MaxDepth = 1.f;
		mViewport.TopLeftX = 0.f;
		mViewport.TopLeftY = 0.f;

		auto coloraccumulationRTV = mResourceManager->Create<D3D11RenderTargetView>(ED3D11RenderTargetViewType::ColorAcuumulation, width, height);
		auto pixelRevealageThresholdRTV = mResourceManager->Create<D3D11RenderTargetView>(ED3D11RenderTargetViewType::PixeldRevealageThreshold, width, height);
		mColoraccumulationSRV = std::make_shared<D3D11ShaderResourceView>(mDevice, coloraccumulationRTV);
		mPixelRevealageThresholdSRV = std::make_shared<D3D11ShaderResourceView>(mDevice, pixelRevealageThresholdRTV);

	}
	void TransparentCompositePass::Render()
	{
		ID3D11ShaderResourceView* SRVs[10] = { NULL };
		mDevice->GetDeviceContext()->PSSetShaderResources(0, 10, SRVs);

		mDevice->GetDeviceContext()->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		mDevice->GetDeviceContext()->RSSetViewports(1, &mViewport);

		mBackBufferRTV->Bind(mDevice, mNullDSV);
		mColoraccumulationSRV->Bind(mDevice, 0, ED3D11ShaderType::Pixelshader);
		mPixelRevealageThresholdSRV->Bind(mDevice, 1, ED3D11ShaderType::Pixelshader);
		mPointClampSamplerState->Bind(mDevice, 0, ED3D11ShaderType::Pixelshader);

		mFullScreenLayout->Bind(mDevice);
		mFullScreenVS->Bind(mDevice);
		mTransparentCompositePS->Bind(mDevice);
		mFullScreenVB->Bind(mDevice);
		mFullScreenIB->Bind(mDevice);
		mOITCompositeState->Bind(mDevice);

		mDevice->GetDeviceContext()->DrawIndexed(6, 0, 0);

		// unbind
		{
			mDevice->GetDeviceContext()->OMSetBlendState(NULL, NULL, 0xFFFFFFFF);
		}
	}

	// ----------------------------------------------------------------------
	//							SkyBoxPass
	// ----------------------------------------------------------------------
	SkyBoxPass::SkyBoxPass()
		:isSetSkyBox(false)
	{
	}

	void SkyBoxPass::Initialize(std::shared_ptr<D3D11Device> device, std::shared_ptr<D3D11CameraManager> cameraManager, std::shared_ptr<D3D11ResourceManager> resourceManager)
	{
		mDevice = device;
		mCameraManager = cameraManager;
		mResourceManager = resourceManager;

		mSkyBoxVS = std::make_shared<D3D11VertexShader>(device, L"./resource/internal/shader/SkyBoxVS.hlsl");
		mSkyboxPS = std::make_shared<D3D11PixelShader>(device, L"./resource/internal/shader/SkyBoxPS.hlsl");
		mSkyBoxLayout = std::make_shared<D3D11InputLayout>(device, mSkyBoxVS->GetBlob().Get());

		mDefaultSS = resourceManager->Create<D3D11SamplerState>(ED3D11SamplerState::Default);

		std::vector<DirectX::SimpleMath::Vector3> positions =
		{
			{ DirectX::SimpleMath::Vector3(-1.0f, -1.0f, -1.0f)}, // 0
			{ DirectX::SimpleMath::Vector3(-1.0f, +1.0f, -1.0f)},  // 1
			{ DirectX::SimpleMath::Vector3(+1.0f, +1.0f, -1.0f)},  // 2
			{ DirectX::SimpleMath::Vector3(+1.0f, -1.0f, -1.0f)},  // 3							   					   
			{ DirectX::SimpleMath::Vector3(-1.0f, -1.0f, +1.0f)}, // 4
			{ DirectX::SimpleMath::Vector3(-1.0f, +1.0f, +1.0f)}, // 5
			{ DirectX::SimpleMath::Vector3(+1.0f, +1.0f, +1.0f)}, // 6
			{ DirectX::SimpleMath::Vector3(+1.0f, -1.0f, +1.0f)} // 7
		};

		std::vector<unsigned int> indices =
		{
			0, 1, 2,
			0, 2, 3,

			4, 6, 5,
			4, 7, 6,

			4, 5, 1,
			4, 1, 0,

			3, 2, 6,
			3, 6, 7,

			1, 5, 6,
			1, 6, 2,

			4, 0, 3,
			4, 3, 7
		};

		mSkyBoxVB = std::make_shared<D3D11VertexBuffer>(device, positions);
		mSkyboxIB = std::make_shared<D3D11IndexBuffer>(device, indices);

		mDrawRTV = mResourceManager->Get<D3D11RenderTargetView>(ED3D11RenderTargetViewType::Offscreen);
		mDrawDSV = mResourceManager->Get<D3D11DepthStencilView>(ED3D11DepthStencilViewType::Default);

		mCullFrontRS = mResourceManager->Create<D3D11RasterizerState>(ED3D11RasterizerState::CullFront);
		mDefaultSS = mResourceManager->Get<D3D11SamplerState>(ED3D11SamplerState::Default);
		mLessEqualDepthStencilDS = mResourceManager->Create<D3D11DepthStencilState>(ED3D11DepthStencilState::LessEqual);

		mViewProjectionMatrix = mResourceManager->Create<D3D11ConstantBuffer<ViewRotationProjectionMatrix>>(ED3D11ConstantBuffer::ViewRotationProj);
	}

	void SkyBoxPass::Finalize()
	{

	}

	void SkyBoxPass::Render()
	{
		if (!isSetSkyBox)
		{
			return;
		}

		mDevice->GetDeviceContext()->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		mDrawRTV->Bind(mDevice, mDrawDSV);

		mSkyBoxLayout->Bind(mDevice);
		mSkyBoxVS->Bind(mDevice);
		mSkyboxPS->Bind(mDevice);

		mSkyBoxVB->Bind(mDevice);
		mSkyboxIB->Bind(mDevice);

		mDefaultSS->Bind(mDevice, 0, ED3D11ShaderType::Pixelshader);
		mCullFrontRS->Bind(mDevice);
		mLessEqualDepthStencilDS->Bind(mDevice);

		mSkyBoxTexture->Bind(mDevice, 0, ED3D11ShaderType::Pixelshader);

		ViewRotationProjectionMatrix viewProjectionMatrix;
		DirectX::SimpleMath::Matrix view = mCameraManager->GetViewMatrix(ECameraType::Player);
		view._41 = 0;
		view._42 = 0;
		view._43 = 0;
		viewProjectionMatrix.ViewProjMat = (view * mCameraManager->GetProjectionMatrix(ECameraType::Player)).Transpose();

		mViewProjectionMatrix->Update(mDevice, viewProjectionMatrix);
		mViewProjectionMatrix->Bind(mDevice, ED3D11ShaderType::VertexShader, 0);

		mDevice->GetDeviceContext()->DrawIndexed(36, 0, 0);
	}

	void SkyBoxPass::SetSkyBox(const std::wstring& path)
	{
		if (path == L"")
		{
			isSetSkyBox = false;
			return;
		}

		isSetSkyBox = true;
		mSkyBoxTexture = mResourceManager->Create<D3D11Texture>(path);
	}

	void DebugRenderPass::Initialize(std::shared_ptr<D3D11Device> device,
		std::shared_ptr<D3D11JobManager> jobManager,
		std::shared_ptr<D3D11DebugDrawManager> debugDrawManager,
		std::shared_ptr<D3D11CameraManager> cameraManager,
		std::shared_ptr<D3D11ResourceManager> resourceManager,
		unsigned short width,
		unsigned short height)
	{
		mDevice = device;
		mJobManager = jobManager;
		mDebugDrawManager = debugDrawManager;
		mCameraManager = cameraManager;
		mResourceManager = resourceManager;

		mViewport.Width = (float)width;
		mViewport.Height = (float)height;
		mViewport.MinDepth = 0.f;
		mViewport.MaxDepth = 1.f;
		mViewport.TopLeftX = 0.f;
		mViewport.TopLeftY = 0.f;

		mBackBufferRTV = mResourceManager->Create<D3D11RenderTargetView>(ED3D11RenderTargetViewType::Offscreen, width, height);
		mDefaultDSV = mResourceManager->Create<D3D11DepthStencilView>(ED3D11DepthStencilViewType::Default, width, height);
	}
	void DebugRenderPass::Finalize()
	{
		mDevice = nullptr;
		mJobManager = nullptr;
		mDebugDrawManager = nullptr;
		mCameraManager = nullptr;
		mResourceManager = nullptr;

		mBackBufferRTV = nullptr;
		mDefaultDSV = nullptr;
	}
	void DebugRenderPass::OnResize(unsigned short width, unsigned short height)
	{
		mViewport.Width = (float)width;
		mViewport.Height = (float)height;
		mViewport.MinDepth = 0.f;
		mViewport.MaxDepth = 1.f;
		mViewport.TopLeftX = 0.f;
		mViewport.TopLeftY = 0.f;
	}
	void DebugRenderPass::Render()
	{
		mBackBufferRTV->Bind(mDevice, mDefaultDSV);

		mDebugDrawManager->Excute(mDevice, mCameraManager);
	}
}