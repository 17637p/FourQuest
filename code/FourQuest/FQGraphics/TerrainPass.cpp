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
		mTerrainLayout = std::make_shared<D3D11InputLayout>(mDevice, mTerrainVS->GetBlob().Get());

		// 스카이 박스랑 동일하게 가져가면 될듯?
		mDrawRTV = mResourceManager->Get<D3D11RenderTargetView>(ED3D11RenderTargetViewType::Offscreen);
		mDrawDSV = mResourceManager->Get<D3D11DepthStencilView>(ED3D11DepthStencilViewType::Default);
		auto shadowDSV = mResourceManager->Get<D3D11DepthStencilView>(ED3D11DepthStencilViewType::CascadeShadow3);
		mShadowSRV = std::make_shared<D3D11ShaderResourceView>(mDevice, shadowDSV);

		mDefaultRS = std::make_shared<D3D11RasterizerState>(mDevice, ED3D11RasterizerState::Default);
		mAnisotropicWrapSS = std::make_shared<D3D11SamplerState>(mDevice, ED3D11SamplerState::AnisotropicWrap);
		mShadowSS = resourceManager->Get<D3D11SamplerState>(ED3D11SamplerState::Shadow);
		mDefaultDS = std::make_shared<D3D11DepthStencilState>(mDevice, ED3D11DepthStencilState::Default);

		mModelTransformCB = std::make_shared<D3D11ConstantBuffer<ModelTransform>>(mDevice, ED3D11ConstantBuffer::Transform);
		mSceneTransformCB = std::make_shared<D3D11ConstantBuffer<SceneTrnasform>>(mDevice, ED3D11ConstantBuffer::Transform);
		mTerrainTextureCB = std::make_shared<D3D11ConstantBuffer<TerrainTexture>>(mDevice, ED3D11ConstantBuffer::TerrainTexture);
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

			mDevice->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			mDefaultRS->Bind(mDevice);
			mAnisotropicWrapSS->Bind(mDevice, 0, ED3D11ShaderType::PixelShader);
			mShadowSS->Bind(mDevice, 1, ED3D11ShaderType::PixelShader);
			mDefaultDS->Bind(mDevice);

			mModelTransformCB->Bind(mDevice, ED3D11ShaderType::VertexShader);
			mSceneTransformCB->Bind(mDevice, ED3D11ShaderType::VertexShader, 1);
			mTerrainTextureCB->Bind(mDevice, ED3D11ShaderType::PixelShader);
			mLightManager->GetLightConstnatBuffer()->Bind(mDevice, ED3D11ShaderType::PixelShader, 1);
			mDirectioanlShadowInfoCB->Bind(mDevice, ED3D11ShaderType::PixelShader, 2);
			mShadowSRV->Bind(mDevice, 17, ED3D11ShaderType::PixelShader);

			mTerrainVS->Bind(mDevice);
			mTerrainPS->Bind(mDevice);
			mTerrainLayout->Bind(mDevice);
		}

		// Draw
		{
			for (const TerrainMeshJob& job : mJobManager->GetTerrainMeshJobs())
			{
				job.StaticMesh->Bind(mDevice);
				ConstantBufferHelper::UpdateModelTransformCB(mDevice, mModelTransformCB, *job.TransformPtr);

				if (job.TerrainMaterial != nullptr)
				{
					job.TerrainMaterial->Bind(mDevice);
					ConstantBufferHelper::UpdateTerrainTextureCB(mDevice, mTerrainTextureCB, job.TerrainMaterial, job.tempObject);
				}

				job.StaticMesh->Draw(mDevice, job.SubsetIndex);
			}
		}

		// unbind
		{
			ID3D11ShaderResourceView* srv = nullptr;
			mDevice->GetDeviceContext()->PSSetShaderResources(17, 1, &srv);
		}
	}
}