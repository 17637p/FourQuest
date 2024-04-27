#include "DeferredPipeline.h"

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
	void DeferredPipeline::Initialize(const std::shared_ptr<D3D11Device>& device,
		std::shared_ptr<class D3D11ResourceManager>& resourceManager,
		const std::shared_ptr<D3D11LightManager>& lightManager,
		unsigned short width,
		unsigned short height)
	{
		mBackBufferRTV = resourceManager->Create<fq::graphics::D3D11RenderTargetView>(ED3D11RenderTargetViewType::Offscreen, width, height);
		mBackBufferSRV = std::make_shared<fq::graphics::D3D11ShaderResourceView>(device, mBackBufferRTV);
		mNullDSV = resourceManager->Create<fq::graphics::D3D11DepthStencilView>(ED3D11DepthStencilViewType::None, width, height);
		mDSV = resourceManager->Create<fq::graphics::D3D11DepthStencilView>(ED3D11DepthStencilViewType::Default, width, height);

		mAlbedoRTV = std::make_shared<fq::graphics::D3D11RenderTargetView>(device, ED3D11RenderTargetViewType::Offscreen, width, height);
		mMetalnessRTV = std::make_shared<fq::graphics::D3D11RenderTargetView>(device, ED3D11RenderTargetViewType::OffscreenGrayscale, width, height);
		mRoughnessRTV = std::make_shared<fq::graphics::D3D11RenderTargetView>(device, ED3D11RenderTargetViewType::OffscreenGrayscale, width, height);
		mNormalRTV = std::make_shared<fq::graphics::D3D11RenderTargetView>(device, ED3D11RenderTargetViewType::OffscreenHDR, width, height);
		mEmissiveRTV = std::make_shared<fq::graphics::D3D11RenderTargetView>(device, ED3D11RenderTargetViewType::Offscreen, width, height);
		mPositionRTV = std::make_shared<fq::graphics::D3D11RenderTargetView>(device, ED3D11RenderTargetViewType::OffscreenHDR, width, height);

		mAlbedoSRV = std::make_shared<fq::graphics::D3D11ShaderResourceView>(device, mAlbedoRTV);
		mMetalnessSRV = std::make_shared<fq::graphics::D3D11ShaderResourceView>(device, mMetalnessRTV);
		mRoughnessSRV = std::make_shared<fq::graphics::D3D11ShaderResourceView>(device, mRoughnessRTV);
		mNormalSRV = std::make_shared<fq::graphics::D3D11ShaderResourceView>(device, mNormalRTV);
		mEmissiveSRV = std::make_shared<fq::graphics::D3D11ShaderResourceView>(device, mEmissiveRTV);
		mPositionSRV = std::make_shared<fq::graphics::D3D11ShaderResourceView>(device, mPositionRTV);

		D3D_SHADER_MACRO macroSkinning[] =
		{
			{"SKINNING", ""},
			{ NULL, NULL}
		};

		D3D_SHADER_MACRO macroGeometry[] =
		{
			{"GEOMETRY", ""},
			{ NULL, NULL}
		};

		D3D_SHADER_MACRO macroShading[] =
		{
			{"SHADING", ""},
			{ NULL, NULL}
		};

		mStaticMeshVS = std::make_shared<D3D11VertexShader>(device, L"./resource/internal/shader/ModelVS.hlsl");
		mSkinnedMeshVS = std::make_shared<D3D11VertexShader>(device, L"./resource/internal/shader/ModelVS.hlsl", macroSkinning);
		mGeometryPS = std::make_shared<D3D11PixelShader>(device, L"./resource/internal/shader/ModelPSDeferred.hlsl", macroGeometry);
		mShadingPS = std::make_shared<D3D11PixelShader>(device, L"./resource/internal/shader/ModelPSDeferred.hlsl", macroShading);

		mStaticMeshLayout = std::make_shared<D3D11InputLayout>(device, mStaticMeshVS->GetBlob().Get());
		mSkinnedMeshLayout = std::make_shared<D3D11InputLayout>(device, mSkinnedMeshVS->GetBlob().Get());

		mAnisotropicWrapSamplerState = resourceManager->Create<D3D11SamplerState>(ED3D11SamplerState::AnisotropicWrap);
		mLinearClampSamplerState = resourceManager->Create<D3D11SamplerState>(ED3D11SamplerState::Default);
		mPointClampSamplerState = resourceManager->Create<D3D11SamplerState>(ED3D11SamplerState::PointClamp);

		mModelTransformCB = std::make_shared<D3D11ConstantBuffer<ModelTransfrom>>(device, ED3D11ConstantBuffer::Transform);
		mSceneTransformCB = std::make_shared<D3D11ConstantBuffer<SceneTrnasform>>(device, ED3D11ConstantBuffer::Transform);
		mBoneTransformCB = std::make_shared<D3D11ConstantBuffer<BoneTransform>>(device, ED3D11ConstantBuffer::Transform);
		mModelTexutreCB = std::make_shared< D3D11ConstantBuffer<ModelTexutre>>(device, ED3D11ConstantBuffer::Transform);
		mLightConstantBuffer = lightManager->GetLightConstnatBuffer();

		mViewport.Width = (float)width;
		mViewport.Height = (float)height;
		mViewport.MinDepth = 0.f;
		mViewport.MaxDepth = 1.f;
		mViewport.TopLeftX = 0.f;
		mViewport.TopLeftY = 0.f;
	}
	void DeferredPipeline::Finalize()
	{
		mBackBufferRTV = nullptr;
		mBackBufferSRV = nullptr;
		mDSV = nullptr;

		mAlbedoRTV = nullptr;
		mMetalnessRTV = nullptr;
		mRoughnessRTV = nullptr;
		mNormalRTV = nullptr;
		mEmissiveRTV = nullptr;
		mPositionRTV = nullptr;

		mAlbedoSRV = nullptr;
		mMetalnessSRV = nullptr;
		mRoughnessSRV = nullptr;
		mNormalSRV = nullptr;
		mEmissiveSRV = nullptr;
		mPositionSRV = nullptr;

		mStaticMeshLayout = nullptr;
		mSkinnedMeshLayout = nullptr;

		mStaticMeshVS = nullptr;
		mSkinnedMeshVS = nullptr;

		mGeometryPS = nullptr;
		mShadingPS = nullptr;

		mAnisotropicWrapSamplerState = nullptr;
		mLinearClampSamplerState = nullptr;
		mPointClampSamplerState = nullptr;

		mModelTransformCB = nullptr;
		mSceneTransformCB = nullptr;
		mBoneTransformCB = nullptr;
		mModelTexutreCB = nullptr;
		mLightConstantBuffer = nullptr;

		mFullScreenLayout = nullptr;
		mFullScreenVS = nullptr;
		mFullScreenVB = nullptr;
		mFullScreenIB = nullptr;
	}

	void DeferredPipeline::OnResize(const std::shared_ptr<D3D11Device>& device,
		std::shared_ptr<class D3D11ResourceManager>& resourceManager,
		unsigned short width,
		unsigned short height)
	{
		mBackBufferRTV->Release();
		mDSV->Release();
		mAlbedoRTV->Release();
		mMetalnessRTV->Release();
		mRoughnessRTV->Release();
		mNormalRTV->Release();
		mEmissiveRTV->Release();
		mPositionRTV->Release();

		mBackBufferRTV->OnResize(device, ED3D11RenderTargetViewType::Offscreen, width, height);
		mDSV->OnResize(device, ED3D11DepthStencilViewType::Default, width, height);
		mAlbedoRTV->OnResize(device, ED3D11RenderTargetViewType::Offscreen, width, height);
		mMetalnessRTV->OnResize(device, ED3D11RenderTargetViewType::OffscreenGrayscale, width, height);
		mRoughnessRTV->OnResize(device, ED3D11RenderTargetViewType::OffscreenGrayscale, width, height);
		mNormalRTV->OnResize(device, ED3D11RenderTargetViewType::OffscreenHDR, width, height);
		mEmissiveRTV->OnResize(device, ED3D11RenderTargetViewType::Offscreen, width, height);
		mPositionRTV->OnResize(device, ED3D11RenderTargetViewType::OffscreenHDR, width, height);

		mBackBufferSRV->Init(device, mBackBufferRTV);
		mAlbedoSRV->Init(device, mAlbedoRTV);
		mMetalnessSRV->Init(device, mMetalnessRTV);
		mRoughnessSRV->Init(device, mRoughnessRTV);
		mNormalSRV->Init(device, mNormalRTV);
		mEmissiveSRV->Init(device, mEmissiveRTV);
		mPositionSRV->Init(device, mPositionRTV);

		mViewport.Width = (float)width;
		mViewport.Height = (float)height;
		mViewport.MinDepth = 0.f;
		mViewport.MaxDepth = 1.f;
		mViewport.TopLeftX = 0.f;
		mViewport.TopLeftY = 0.f;
	}

	void DeferredPipeline::BeginRender(const std::shared_ptr<D3D11Device>& device,
		const std::shared_ptr<D3D11CameraManager>& cameraManager,
		const std::shared_ptr<D3D11LightManager>& lightManager)
	{
		ID3D11ShaderResourceView* NullSRVs[10] = { NULL, };
		device->GetDeviceContext()->PSSetShaderResources(0, ARRAYSIZE(NullSRVs), NullSRVs);

		mDSV->Clear(device);
		mBackBufferRTV->Clear(device, { 1.f, 1.f, 1.f, 1.f });
		mAlbedoRTV->Clear(device, { 1.f, 1.f, 1.f, 1.f });
		mMetalnessRTV->Clear(device);
		mRoughnessRTV->Clear(device);
		mNormalRTV->Clear(device, { 1000, 0, 0, 0 });
		mEmissiveRTV->Clear(device);
		mPositionRTV->Clear(device);

		mBackBufferRTV->Bind(device, mDSV);

		std::vector<std::shared_ptr<D3D11RenderTargetView>> renderTargetViews;
		renderTargetViews.reserve(6u);

		renderTargetViews.push_back(mAlbedoRTV);
		renderTargetViews.push_back(mMetalnessRTV);
		renderTargetViews.push_back(mRoughnessRTV);
		renderTargetViews.push_back(mNormalRTV);
		renderTargetViews.push_back(mEmissiveRTV);
		renderTargetViews.push_back(mPositionRTV);

		D3D11RenderTargetView::Bind(device, renderTargetViews, mDSV);

		device->GetDeviceContext()->RSSetViewports(1, &mViewport);

		SceneTrnasform sceneTransform;
		DirectX::SimpleMath::Matrix view = cameraManager->GetViewMatrix(ECameraType::Player);
		DirectX::SimpleMath::Matrix proj = cameraManager->GetProjectionMatrix(ECameraType::Player);
		sceneTransform.ViewProjMat = (view * proj).Transpose();
		sceneTransform.ShadowViewProjTexMat;
		mSceneTransformCB->Update(device, sceneTransform);

		lightManager->UpdateConstantBuffer(device, cameraManager->GetPosition(ECameraType::Player), false);
	}
	void DeferredPipeline::EndRender(const std::shared_ptr<D3D11Device>& device)
	{
		Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain = device->GetSwapChain();
		HR(swapChain->Present(0, 0));
	}

	void DeferredPipeline::Render(const std::shared_ptr<D3D11Device>& device, const std::vector<StaticMeshJob>& staticMeshJobs)
	{
		device->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		mStaticMeshLayout->Bind(device);
		mStaticMeshVS->Bind(device);
		mGeometryPS->Bind(device);
		mModelTransformCB->Bind(device, ED3D11ShaderType::VertexShader);
		mSceneTransformCB->Bind(device, ED3D11ShaderType::VertexShader, 1);
		mModelTexutreCB->Bind(device, ED3D11ShaderType::Pixelshader);
		mAnisotropicWrapSamplerState->Bind(device, 0, ED3D11ShaderType::Pixelshader);

		for (const StaticMeshJob& job : staticMeshJobs)
		{
			job.StaticMesh->Bind(device);
			job.Material->Bind(device);

			ConstantBufferHelper::UpdateModelTransformCB(device, mModelTransformCB, *job.TransformPtr);
			ConstantBufferHelper::UpdateModelTextureCB(device, mModelTexutreCB, job.Material);

			job.StaticMesh->Draw(device, job.SubsetIndex);
		}
	}
	void DeferredPipeline::Render(const std::shared_ptr<D3D11Device>& device, const std::vector<SkinnedMeshJob>& skinnedMeshJobs)
	{
		device->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		mSkinnedMeshLayout->Bind(device);

		mSkinnedMeshVS->Bind(device);
		mModelTransformCB->Bind(device, ED3D11ShaderType::VertexShader);
		mSceneTransformCB->Bind(device, ED3D11ShaderType::VertexShader, 1);
		mBoneTransformCB->Bind(device, ED3D11ShaderType::VertexShader, 2);

		mGeometryPS->Bind(device);
		mModelTexutreCB->Bind(device, ED3D11ShaderType::Pixelshader);
		mAnisotropicWrapSamplerState->Bind(device, 0, ED3D11ShaderType::Pixelshader);

		for (const SkinnedMeshJob& job : skinnedMeshJobs)
		{
			job.SkinnedMesh->Bind(device);
			job.Material->Bind(device);

			ConstantBufferHelper::UpdateModelTransformCB(device, mModelTransformCB, *job.TransformPtr);
			ConstantBufferHelper::UpdateModelTextureCB(device, mModelTexutreCB, job.Material);
			ConstantBufferHelper::UpdateBoneTransformCB(device, mBoneTransformCB, *job.BoneMatricesPtr);

			job.SkinnedMesh->Draw(device, job.SubsetIndex);
		}
	}
	void DeferredPipeline::Shading(const std::shared_ptr<D3D11Device>& device)
	{
		ID3D11ShaderResourceView* NullSRVs[10] = { NULL, };
		device->GetDeviceContext()->PSSetShaderResources(0, ARRAYSIZE(NullSRVs), NullSRVs);

		mBackBufferRTV->Bind(device, mNullDSV);

		device->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		mFullScreenLayout->Bind(device);
		mFullScreenVB->Bind(device);
		mFullScreenIB->Bind(device);

		mFullScreenVS->Bind(device);

		mShadingPS->Bind(device);
		mAlbedoSRV->Bind(device, 0, ED3D11ShaderType::Pixelshader);
		mMetalnessSRV->Bind(device, 1, ED3D11ShaderType::Pixelshader);
		mRoughnessSRV->Bind(device, 2, ED3D11ShaderType::Pixelshader);
		mNormalSRV->Bind(device, 3, ED3D11ShaderType::Pixelshader);
		mEmissiveSRV->Bind(device, 4, ED3D11ShaderType::Pixelshader);
		mPositionSRV->Bind(device, 5, ED3D11ShaderType::Pixelshader);
		mLightConstantBuffer->Bind(device, ED3D11ShaderType::Pixelshader, 1);
		mAnisotropicWrapSamplerState->Bind(device, 0, ED3D11ShaderType::Pixelshader);
		mLinearClampSamplerState->Bind(device, 1, ED3D11ShaderType::Pixelshader);
		mPointClampSamplerState->Bind(device, 2, ED3D11ShaderType::Pixelshader);

		device->GetDeviceContext()->DrawIndexed(6, 0, 0);

		mBackBufferRTV->Bind(device, mDSV);
	}

	std::shared_ptr<D3D11ShaderResourceView>& DeferredPipeline::GetBackBufferSRV()
	{
		return mBackBufferSRV;
	}
}