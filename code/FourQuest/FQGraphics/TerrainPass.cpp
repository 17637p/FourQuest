#include "TerrainPass.h"
#include "ManagementCommon.h"
#include "D3D11Common.h"
#include "ShadowPass.h"
#include "RenderJob.h"
#include "Material.h"
#include "Mesh.h"
#include "RenderObject.h"
#include "Define.h"

namespace fq::graphics
{
	TerrainPass::TerrainPass(EPipelineType pipelineType)
		: mPipelineType(pipelineType),
		mDevice(nullptr),
		mJobManager(nullptr),
		mCameraManager(nullptr),
		mResourceManager(nullptr),
		mLightManager(nullptr),
		mTerrainVS(nullptr),
		mTerrainPS(nullptr),
		mTerrainLayout(nullptr),
		mDrawRTV(nullptr),
		mDrawDSV(nullptr),
		mDefaultRS(nullptr),
		mAnisotropicWrapSS(nullptr),
		mDefaultDS(nullptr),
		mModelTransformCB(nullptr),
		mSceneTransformCB(nullptr),
		mTerrainTextureCB(nullptr)
	{
	}

	void TerrainPass::Initialize(std::shared_ptr<D3D11Device> device,
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

		mTerrainVS = std::make_shared<D3D11VertexShader>(mDevice, L"TerrainVS.cso");
		mTerrainDS = std::make_shared<D3D11DomainShader>(mDevice, L"TerrainDS.cso");
		mTerrainHS = std::make_shared<D3D11HullShader>(mDevice, L"TerrainHS.cso");
		mTerrainLayout = std::make_shared<D3D11InputLayout>(mDevice, mTerrainVS->GetBlob().Get());

		// 스카이 박스랑 동일하게 가져가면 될듯?
		switch (mPipelineType)
		{
		case fq::graphics::EPipelineType::Forward:
		{
			mTerrainPS = std::make_shared<D3D11PixelShader>(mDevice, L"TerrainPS.cso");

			mDrawRTV = mResourceManager->Get<D3D11RenderTargetView>(ED3D11RenderTargetViewType::Offscreen);
			auto shadowDSV = mResourceManager->Get<D3D11DepthStencilView>(ED3D11DepthStencilViewType::CascadeShadow3);
			mShadowSRV = std::make_shared<D3D11ShaderResourceView>(mDevice, shadowDSV);
		}
		break;
		case fq::graphics::EPipelineType::Deferred:
		{
			mTerrainPS = std::make_shared<D3D11PixelShader>(mDevice, L"TerrainPS_DEFERRED.cso");

			mAlbedoRTV = mResourceManager->Get<D3D11RenderTargetView>(ED3D11RenderTargetViewType::Albedo);
			mMetalnessRoughnessRTV = mResourceManager->Get<D3D11RenderTargetView>(ED3D11RenderTargetViewType::MetalnessRoughness);
			mNormalRTV = mResourceManager->Get<D3D11RenderTargetView>(ED3D11RenderTargetViewType::Normal);
			mEmissiveRTV = mResourceManager->Get<D3D11RenderTargetView>(ED3D11RenderTargetViewType::Emissive);
			mPositionRTV = mResourceManager->Get<D3D11RenderTargetView>(ED3D11RenderTargetViewType::PositionWClipZ);
			mSourceNormalRTV = mResourceManager->Get<D3D11RenderTargetView>(ED3D11RenderTargetViewType::SourceNormal);
			mSourceTangentRTV = mResourceManager->Get<D3D11RenderTargetView>(ED3D11RenderTargetViewType::SourceTangent);
			mPreCalculatedLightRTV = mResourceManager->Get<D3D11RenderTargetView>(ED3D11RenderTargetViewType::PreCalculatedLight);
		}
		break;
		case fq::graphics::EPipelineType::None:
			assert(false);
			break;
		default:
			assert(false);
			break;
		}

		mDrawDSV = mResourceManager->Get<D3D11DepthStencilView>(ED3D11DepthStencilViewType::Default);

		mDefaultRS = std::make_shared<D3D11RasterizerState>(mDevice, ED3D11RasterizerState::Default);
		mAnisotropicWrapSS = std::make_shared<D3D11SamplerState>(mDevice, ED3D11SamplerState::AnisotropicWrap);
		mShadowSS = resourceManager->Get<D3D11SamplerState>(ED3D11SamplerState::Shadow);
		mPointClampSS = resourceManager->Create<D3D11SamplerState>(ED3D11SamplerState::PointClamp);
		mDefaultDS = std::make_shared<D3D11DepthStencilState>(mDevice, ED3D11DepthStencilState::Default);

		mModelTransformCB = std::make_shared<D3D11ConstantBuffer<ModelTransform>>(mDevice, ED3D11ConstantBuffer::Transform);
		mSceneTransformCB = std::make_shared<D3D11ConstantBuffer<SceneTrnasform>>(mDevice, ED3D11ConstantBuffer::Transform);
		mTerrainTextureCB = std::make_shared<D3D11ConstantBuffer<TerrainTexture>>(mDevice, ED3D11ConstantBuffer::TerrainTexture);
		mTerrainHullCB = std::make_shared<D3D11ConstantBuffer<TerrainHull>>(mDevice, ED3D11ConstantBuffer::TerrainTexture);
		mLightMapInfomationCB = std::make_shared<D3D11ConstantBuffer<LightMapInfomation>>(mDevice, ED3D11ConstantBuffer::Transform);
	}

	void TerrainPass::Finalize()
	{
		mDevice = nullptr;
		mJobManager = nullptr;
		mCameraManager = nullptr;
		mResourceManager = nullptr;
		mLightManager = nullptr;

		mTerrainVS = nullptr;
		mTerrainPS = nullptr;
		mTerrainDS = nullptr;
		mTerrainHS = nullptr;
		mTerrainLayout = nullptr;

		mDrawRTV = nullptr;
		mDrawDSV = nullptr;
		mShadowSRV = nullptr;

		mDefaultRS = nullptr;
		mAnisotropicWrapSS = nullptr;
		mPointClampSS = nullptr;
		mShadowSS = nullptr;
		mDefaultDS = nullptr;

		mModelTransformCB = nullptr;
		mSceneTransformCB = nullptr;
		mTerrainHullCB = nullptr;
		mTerrainTextureCB = nullptr;
		mLightMapInfomationCB = nullptr;

		mAlbedoRTV = nullptr;
		mMetalnessRoughnessRTV = nullptr;
		mNormalRTV = nullptr;
		mEmissiveRTV = nullptr;
		mPositionRTV = nullptr;
		mSourceNormalRTV = nullptr;
		mSourceTangentRTV = nullptr;
		mPreCalculatedLightRTV = nullptr;
	}

	void TerrainPass::Render()
	{
		// update
		{
			SceneTrnasform sceneTransform;
			sceneTransform.ViewProjMat = mCameraManager->GetViewMatrix(ECameraType::Player) * mCameraManager->GetProjectionMatrix(ECameraType::Player);
			sceneTransform.ViewProjMat = sceneTransform.ViewProjMat.Transpose();
			mSceneTransformCB->Update(mDevice, sceneTransform);
		}

		// Bind
		const std::shared_ptr<D3D11TextureArray>& lightMapTexture = mLightManager->GetLightMapTextureArray();
		const std::shared_ptr<D3D11TextureArray>& lightmapDirectionTexture = mLightManager->GetLightMapDirectionTextureArray();

		{
			mDevice->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);

			if (lightMapTexture != nullptr)
			{
				lightMapTexture->Bind(mDevice, 19, ED3D11ShaderType::PixelShader);
			}
			if (lightmapDirectionTexture != nullptr)
			{
				lightmapDirectionTexture->Bind(mDevice, 20, ED3D11ShaderType::PixelShader);
			}

			mDefaultRS->Bind(mDevice);
			mAnisotropicWrapSS->Bind(mDevice, 0, ED3D11ShaderType::PixelShader);
			mShadowSS->Bind(mDevice, 1, ED3D11ShaderType::PixelShader);
			mPointClampSS->Bind(mDevice, 0, ED3D11ShaderType::VertexShader);
			mPointClampSS->Bind(mDevice, 0, ED3D11ShaderType::DomainShader);
			mDefaultDS->Bind(mDevice);

			switch (mPipelineType)
			{
			case fq::graphics::EPipelineType::Forward:
				mDrawRTV->Bind(mDevice, mDrawDSV);
				mShadowSRV->Bind(mDevice, 18, ED3D11ShaderType::PixelShader);
				break;
			case fq::graphics::EPipelineType::Deferred:
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
				D3D11RenderTargetView::Bind(mDevice, renderTargetViews, mDrawDSV);
			}
			break;
			case fq::graphics::EPipelineType::None:
				assert(false);
				break;
			default:
				assert(false);
				break;
			}

			TerrainHull terrainHull;
			terrainHull.MinDist = 200;
			terrainHull.MaxDist = 5000;
			terrainHull.MinTess = 1;
			terrainHull.MaxTess = 10;
			terrainHull.gEyePosW = mCameraManager->GetPosition(ECameraType::Player);
			// 절두체 면 계산 
			DirectX::SimpleMath::Matrix viewProjMat =
				mCameraManager->GetViewMatrix(ECameraType::Player) *
				mCameraManager->GetProjectionMatrix(ECameraType::Player);
			// 왼쪽 평면
			terrainHull.WorldFrustumPlanes[0].x = viewProjMat._14 + viewProjMat._11;
			terrainHull.WorldFrustumPlanes[0].y = viewProjMat._24 + viewProjMat._21;
			terrainHull.WorldFrustumPlanes[0].z = viewProjMat._34 + viewProjMat._31;
			terrainHull.WorldFrustumPlanes[0].w = viewProjMat._44 + viewProjMat._41;
			// 오른쪽 평면
			terrainHull.WorldFrustumPlanes[1].x = viewProjMat._14 - viewProjMat._11;
			terrainHull.WorldFrustumPlanes[1].y = viewProjMat._24 - viewProjMat._21;
			terrainHull.WorldFrustumPlanes[1].z = viewProjMat._34 - viewProjMat._31;
			terrainHull.WorldFrustumPlanes[1].w = viewProjMat._44 - viewProjMat._41;
			// 아래쪽 평면
			terrainHull.WorldFrustumPlanes[2].x = viewProjMat._14 + viewProjMat._12;
			terrainHull.WorldFrustumPlanes[2].y = viewProjMat._24 + viewProjMat._22;
			terrainHull.WorldFrustumPlanes[2].z = viewProjMat._34 + viewProjMat._32;
			terrainHull.WorldFrustumPlanes[2].w = viewProjMat._44 + viewProjMat._42;
			// 윗쪽 평면
			terrainHull.WorldFrustumPlanes[3].x = viewProjMat._14 - viewProjMat._12;
			terrainHull.WorldFrustumPlanes[3].y = viewProjMat._24 - viewProjMat._22;
			terrainHull.WorldFrustumPlanes[3].z = viewProjMat._34 - viewProjMat._32;
			terrainHull.WorldFrustumPlanes[3].w = viewProjMat._44 - viewProjMat._42;
			// 가까운 평면
			terrainHull.WorldFrustumPlanes[4].x = viewProjMat._13;
			terrainHull.WorldFrustumPlanes[4].y = viewProjMat._23;
			terrainHull.WorldFrustumPlanes[4].z = viewProjMat._33;
			terrainHull.WorldFrustumPlanes[4].w = viewProjMat._43;
			// 먼 평면
			terrainHull.WorldFrustumPlanes[5].x = viewProjMat._14 - viewProjMat._13;
			terrainHull.WorldFrustumPlanes[5].y = viewProjMat._24 - viewProjMat._23;
			terrainHull.WorldFrustumPlanes[5].z = viewProjMat._34 - viewProjMat._33;
			terrainHull.WorldFrustumPlanes[5].w = viewProjMat._44 - viewProjMat._43;

			for (UINT i = 0; i < 6; i++)
			{
				DirectX::SimpleMath::Vector4 v = DirectX::XMPlaneNormalize(DirectX::XMLoadFloat4(&terrainHull.WorldFrustumPlanes[i]));
				DirectX::XMStoreFloat4(&terrainHull.WorldFrustumPlanes[i], v);
			}

			mTerrainHullCB->Update(mDevice, terrainHull);

			mModelTransformCB->Bind(mDevice, ED3D11ShaderType::VertexShader);
			mModelTransformCB->Bind(mDevice, ED3D11ShaderType::DomainShader);
			mSceneTransformCB->Bind(mDevice, ED3D11ShaderType::DomainShader, 1);
			mTerrainHullCB->Bind(mDevice, ED3D11ShaderType::HullShader, 0);
			mTerrainTextureCB->Bind(mDevice, ED3D11ShaderType::PixelShader);
			mLightManager->GetLightConstnatBuffer()->Bind(mDevice, ED3D11ShaderType::PixelShader, 1);
			mLightManager->GetShadowConstnatBuffer()->Bind(mDevice, ED3D11ShaderType::PixelShader, 2);
			mLightMapInfomationCB->Bind(mDevice, ED3D11ShaderType::PixelShader, 3);

			mTerrainVS->Bind(mDevice);
			mTerrainPS->Bind(mDevice);
			mTerrainDS->Bind(mDevice);
			mTerrainHS->Bind(mDevice);
			mTerrainLayout->Bind(mDevice);
		}

		// Draw
		{
			for (const TerrainMeshJob& job : mJobManager->GetTerrainMeshJobs())
			{
				job.TerrainMesh->Bind(mDevice);
				ConstantBufferHelper::UpdateModelTransformCB(mDevice, mModelTransformCB, job.TerrainMeshObject->GetTransform());

				if (job.TerrainMaterial != nullptr)
				{
					job.TerrainMaterial->Bind(mDevice);
					ConstantBufferHelper::UpdateTerrainTextureCB(mDevice, mTerrainTextureCB, job.TerrainMaterial, job.TerrainMeshObject);
				}

				LightMapInfomation lightmapInfo;
				lightmapInfo.bUseLightmap = lightMapTexture != nullptr && job.TerrainMeshObject->GetIsStatic();
				lightmapInfo.bUseDirection = lightmapDirectionTexture != nullptr;
				lightmapInfo.UVIndex = job.TerrainMeshObject->GetLightmapIndex();
				lightmapInfo.UVScaleOffset = job.TerrainMeshObject->GetLightmapUVScaleOffset();
				mLightMapInfomationCB->Update(mDevice, lightmapInfo);

				job.TerrainMesh->Draw(mDevice, job.SubsetIndex);
			}
		}

		// unbind
		{
			ID3D11ShaderResourceView* srv = nullptr;
			mDevice->GetDeviceContext()->PSSetShaderResources(18, 1, &srv);

			ID3D11HullShader* hullShader = nullptr;
			mDevice->GetDeviceContext()->HSSetShader(hullShader, NULL, NULL);

			ID3D11DomainShader* domainShader = nullptr;
			mDevice->GetDeviceContext()->DSSetShader(domainShader, NULL, NULL);
		}
	}
}