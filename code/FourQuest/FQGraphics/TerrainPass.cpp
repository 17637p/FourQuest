#include "TerrainPass.h"
#include "ManagementCommon.h"
#include "D3D11Common.h"
#include "ShadowPass.h"
#include "RenderJob.h"
#include "Material.h"
#include "Mesh.h"
#include "Define.h"

namespace fq::graphics
{
	TerrainPass::TerrainPass()
		:mDevice(nullptr),
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
		mDevice = device;
		mJobManager = jobManager;
		mCameraManager = cameraManager;
		mResourceManager = resourceManager;
		mLightManager = lightManager;

		mTerrainVS = std::make_shared<D3D11VertexShader>(mDevice, L"./resource/internal/shader/TerrainVS.hlsl");
		mTerrainPS = std::make_shared<D3D11PixelShader>(mDevice, L"./resource/internal/shader/TerrainPS.hlsl");
		mTerrainDS = std::make_shared<D3D11DomainShader>(mDevice, L"./resource/internal/shader/TerrainDS.hlsl");
		mTerrainHS = std::make_shared<D3D11HullShader>(mDevice, L"./resource/internal/shader/TerrainHS.hlsl");
		mTerrainLayout = std::make_shared<D3D11InputLayout>(mDevice, mTerrainVS->GetBlob().Get());

		// 스카이 박스랑 동일하게 가져가면 될듯?
		mDrawRTV = mResourceManager->Get<D3D11RenderTargetView>(ED3D11RenderTargetViewType::Offscreen);
		mDrawDSV = mResourceManager->Get<D3D11DepthStencilView>(ED3D11DepthStencilViewType::Default);
		auto shadowDSV = mResourceManager->Get<D3D11DepthStencilView>(ED3D11DepthStencilViewType::CascadeShadow3);
		mShadowSRV = std::make_shared<D3D11ShaderResourceView>(mDevice, shadowDSV);

		mDefaultRS = std::make_shared<D3D11RasterizerState>(mDevice, ED3D11RasterizerState::Default);
		mAnisotropicWrapSS = std::make_shared<D3D11SamplerState>(mDevice, ED3D11SamplerState::AnisotropicWrap);
		mShadowSS = resourceManager->Get<D3D11SamplerState>(ED3D11SamplerState::Shadow);
		mPointClampSS = resourceManager->Create<D3D11SamplerState>(ED3D11SamplerState::PointClamp);
		mDefaultDS = std::make_shared<D3D11DepthStencilState>(mDevice, ED3D11DepthStencilState::Default);

		mModelTransformCB = std::make_shared<D3D11ConstantBuffer<ModelTransform>>(mDevice, ED3D11ConstantBuffer::Transform);
		mSceneTransformCB = std::make_shared<D3D11ConstantBuffer<SceneTrnasform>>(mDevice, ED3D11ConstantBuffer::Transform);
		mTerrainTextureCB = std::make_shared<D3D11ConstantBuffer<TerrainTexture>>(mDevice, ED3D11ConstantBuffer::TerrainTexture);
		mTerrainHullCB = std::make_shared<D3D11ConstantBuffer<TerrainHull>>(mDevice, ED3D11ConstantBuffer::TerrainTexture);
		mDirectioanlShadowInfoCB = resourceManager->Get<D3D11ConstantBuffer<DirectionalShadowInfo>>(ED3D11ConstantBuffer::DirectionalShadowInfo);
	}

	void TerrainPass::Finalize()
	{
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
		{
			mDrawRTV->Bind(mDevice, mDrawDSV);

			mDevice->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);

			mDefaultRS->Bind(mDevice);
			mAnisotropicWrapSS->Bind(mDevice, 0, ED3D11ShaderType::Pixelshader);
			mShadowSS->Bind(mDevice, 1, ED3D11ShaderType::Pixelshader);
			mPointClampSS->Bind(mDevice, 0, ED3D11ShaderType::VertexShader);
			mPointClampSS->Bind(mDevice, 0, ED3D11ShaderType::DomainShader);
			mDefaultDS->Bind(mDevice);

			TerrainHull terrainHull;
			terrainHull.MinDist = 200;
			terrainHull.MaxDist = 5000;
			terrainHull.MinTess = 0;
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
			mTerrainTextureCB->Bind(mDevice, ED3D11ShaderType::Pixelshader);
			mLightManager->GetLightConstnatBuffer()->Bind(mDevice, ED3D11ShaderType::Pixelshader, 1);
			mDirectioanlShadowInfoCB->Bind(mDevice, ED3D11ShaderType::Pixelshader, 2);
			mShadowSRV->Bind(mDevice, 18, ED3D11ShaderType::Pixelshader);

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
				ConstantBufferHelper::UpdateModelTransformCB(mDevice, mModelTransformCB, job.TransformPtr);

				if (job.TerrainMaterial != nullptr)
				{
					job.TerrainMaterial->Bind(mDevice);
					ConstantBufferHelper::UpdateTerrainTextureCB(mDevice, mTerrainTextureCB, job.TerrainMaterial, job.tempObject);
				}

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