#include "D3D11RenderManager.h"
#include "D3D11CameraManager.h"
#include "D3D11ResourceManager.h"

#include "D3D11View.h"
#include "D3D11Buffer.h"
#include "D3D11Shader.h"
#include "D3D11State.h"
#include "D3D11Util.h"

#include "RenderJob.h"
#include "Mesh.h"
#include "Material.h"

#include "Define.h"

namespace fq::graphics
{
	void D3D11RenderManager::Initialize(const std::shared_ptr<D3D11Device>& device,
		std::shared_ptr<class D3D11ResourceManager>& resourceManager,
		unsigned short width,
		unsigned short height)
	{

		mBackBufferRTV = resourceManager->Create<fq::graphics::D3D11RenderTargetView>(ED3D11RenderTargetViewType::Offscreen, width, height);
		mBackBufferSRV = std::make_shared<fq::graphics::D3D11ShaderResourceView>(device, mBackBufferRTV);

		mSwapChainRTV = resourceManager->Create<fq::graphics::D3D11RenderTargetView>(ED3D11RenderTargetViewType::Default, width, height);
		mDSV = resourceManager->Create<fq::graphics::D3D11DepthStencilView>(ED3D11DepthStencilViewType::Default, width, height);
		mNullDSV = resourceManager->Create<fq::graphics::D3D11DepthStencilView>(ED3D11DepthStencilViewType::None, width, height);

		D3D_SHADER_MACRO macroSkinning[] =
		{
			{"SKINNING", ""},
			{ NULL, NULL}
		};

		mStaticMeshVS = std::make_shared<D3D11VertexShader>(device, L"../FQGraphics/ModelVS.hlsl");
		mSkinnedMeshVS = std::make_shared<D3D11VertexShader>(device, L"../FQGraphics/ModelVS.hlsl", macroSkinning);
		mFullScreenVS = std::make_shared<D3D11VertexShader>(device, L"../FQGraphics/FullScreenVS.hlsl");

		mMeshPS = std::make_shared<D3D11PixelShader>(device, L"../FQGraphics/ModelPS.hlsl");
		mFullScreenPS = std::make_shared<D3D11PixelShader>(device, L"../FQGraphics/FullScreenPS.hlsl");

		mStaticMeshLayout = std::make_shared<D3D11InputLayout>(device, mStaticMeshVS->GetBlob().Get());
		mSkinnedMeshLayout = std::make_shared<D3D11InputLayout>(device, mSkinnedMeshVS->GetBlob().Get());
		mFullScreenLayout = std::make_shared<D3D11InputLayout>(device, mFullScreenVS->GetBlob().Get());

		mAnisotropicWrapSamplerState = std::make_shared<D3D11SamplerState>(device, ED3D11SamplerState::AnisotropicWrap);
		mLinearClampSamplerState = std::make_shared<D3D11SamplerState>(device, ED3D11SamplerState::Default);
		mPointClampSamplerState = std::make_shared< D3D11SamplerState>(device, ED3D11SamplerState::PointClamp);

		mModelTransformCB = std::make_shared<D3D11ConstantBuffer<ModelTransfrom>>(device, ED3D11ConstantBuffer::Transform);
		mSceneTransformCB = std::make_shared<D3D11ConstantBuffer<SceneTrnasform>>(device, ED3D11ConstantBuffer::Transform);
		mBoneTransformCB = std::make_shared<D3D11ConstantBuffer<BoneTransform>>(device, ED3D11ConstantBuffer::Transform);
		mModelTexutreCB = std::make_shared< D3D11ConstantBuffer<ModelTexutre>>(device, ED3D11ConstantBuffer::Transform);
		mSceneLightCB = std::make_shared<D3D11ConstantBuffer<SceneLight>>(device, ED3D11ConstantBuffer::Transform);

		createFullScreenBuffer(device);

		mViewport.Width = (float)width;
		mViewport.Height = (float)height;
		mViewport.MinDepth = 0.f;
		mViewport.MaxDepth = 1.f;
		mViewport.TopLeftX = 0.f;
		mViewport.TopLeftY = 0.f;
	}

	void D3D11RenderManager::Finalize()
	{

	}

	void D3D11RenderManager::BeginRender(const std::shared_ptr<D3D11Device>& device, const std::shared_ptr<class D3D11CameraManager>& cameraManager)
	{
		mDSV->Clear(device);
		mBackBufferRTV->Clear(device, { 1.f, 1.f, 1.f, 1.f });

		ID3D11ShaderResourceView* NullSRVs[10] = { NULL, };
		device->GetDeviceContext()->PSSetShaderResources(0, ARRAYSIZE(NullSRVs), NullSRVs);

		mBackBufferRTV->Bind(device, mDSV);
		device->GetDeviceContext()->RSSetViewports(1, &mViewport);

		// 임시 데이터, 카메라 반영시켜줘야 함
		SceneTrnasform sceneTransform;
		DirectX::SimpleMath::Matrix view = cameraManager->GetViewMatrix(ECameraType::Player);
		DirectX::SimpleMath::Matrix proj = cameraManager->GetProjectionMatrix(ECameraType::Player);
		sceneTransform.ViewProjMat = (view * proj).Transpose();
		sceneTransform.ShadowViewProjTexMat;
		mSceneTransformCB->Update(device, sceneTransform);

		// 임시 데이터, 조명 반영시켜줘야 함
		SceneLight scenelight;
		scenelight.Lights[0].Direction = { 0.0f, 0.0f, 1.f };
		scenelight.Lights[0].Intensity = { 1.f, 1.f, 1.f };
		scenelight.Lights[1].Direction = { 1.0f, 0.f, 0.f };
		scenelight.Lights[1].Direction.Normalize();
		scenelight.Lights[1].Intensity = { 1.f, 1.f, 1.f };
		scenelight.Lights[2].Direction = { 0.0f, -1.f ,0.f };
		scenelight.Lights[2].Direction.Normalize();
		scenelight.Lights[2].Intensity = { 1.f, 1.f, 1.f };
		scenelight.EyePosition = (DirectX::SimpleMath::Vector4)cameraManager->GetPosition(ECameraType::Player);
		scenelight.bUseIBL = false;
		mSceneLightCB->Update(device, scenelight);
	}

	void D3D11RenderManager::EndRender(const std::shared_ptr<D3D11Device>& device)
	{
		Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain = device->GetSwapChain();
		HR(swapChain->Present(0, 0));
	}

	void D3D11RenderManager::OnResize(const std::shared_ptr<D3D11Device>& device,
		std::shared_ptr<class D3D11ResourceManager>& resourceManager,
		unsigned short width,
		unsigned short height)
	{
		mBackBufferRTV->Release();
		mSwapChainRTV->Release();
		mDSV->Release();
		mNullDSV->Release();

		device->OnResize(width, height);

		mBackBufferRTV->OnResize(device, ED3D11RenderTargetViewType::Offscreen, width, height);
		mSwapChainRTV->OnResize(device, ED3D11RenderTargetViewType::Default, width, height);

		mBackBufferSRV = std::make_shared<fq::graphics::D3D11ShaderResourceView>(device, mBackBufferRTV);

		mDSV->OnResize(device, ED3D11DepthStencilViewType::Default, width, height);
		mNullDSV->OnResize(device, ED3D11DepthStencilViewType::None, width, height);

		mViewport.Width = (float)width;
		mViewport.Height = (float)height;
		mViewport.MinDepth = 0.f;
		mViewport.MaxDepth = 1.f;
		mViewport.TopLeftX = 0.f;
		mViewport.TopLeftY = 0.f;
	}

	void D3D11RenderManager::Render(const std::shared_ptr<D3D11Device>& device, const std::vector<StaticMeshJob>& staticMeshJobs)
	{
		device->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		mStaticMeshLayout->Bind(device);
		mStaticMeshVS->Bind(device);
		mMeshPS->Bind(device);
		mModelTransformCB->Bind(device, ED3D11ShaderType::VertexShader);
		mSceneTransformCB->Bind(device, ED3D11ShaderType::VertexShader, 1);
		mModelTexutreCB->Bind(device, ED3D11ShaderType::Pixelshader);
		mSceneLightCB->Bind(device, ED3D11ShaderType::Pixelshader, 1);
		mAnisotropicWrapSamplerState->Bind(device, 0, ED3D11ShaderType::Pixelshader);
		mLinearClampSamplerState->Bind(device, 1, ED3D11ShaderType::Pixelshader);


		for (const StaticMeshJob& job : staticMeshJobs)
		{
			job.StaticMesh->Bind(device);
			job.Material->Bind(device);

			updateModelTransformCB(device, *(job.TransformPtr));
			updateModelTextureCB(device, job.Material);

			job.StaticMesh->Draw(device, job.SubsetIndex);
		}
	}

	void D3D11RenderManager::Render(const std::shared_ptr<D3D11Device>& device, const std::vector<SkinnedMeshJob>& skinnedMeshJobs)
	{
		device->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		mSkinnedMeshLayout->Bind(device);
		mSkinnedMeshVS->Bind(device);
		mMeshPS->Bind(device);
		mModelTransformCB->Bind(device, ED3D11ShaderType::VertexShader);
		mSceneTransformCB->Bind(device, ED3D11ShaderType::VertexShader, 1);
		mBoneTransformCB->Bind(device, ED3D11ShaderType::VertexShader, 2);
		mModelTexutreCB->Bind(device, ED3D11ShaderType::Pixelshader);
		mSceneLightCB->Bind(device, ED3D11ShaderType::Pixelshader, 1);
		mAnisotropicWrapSamplerState->Bind(device, 0, ED3D11ShaderType::Pixelshader);
		mLinearClampSamplerState->Bind(device, 1, ED3D11ShaderType::Pixelshader);

		for (const SkinnedMeshJob& job : skinnedMeshJobs)
		{
			job.SkinnedMesh->Bind(device);
			job.Material->Bind(device);

			updateModelTransformCB(device, *job.TransformPtr);
			updateModelTextureCB(device, job.Material);
			updateBoneTransformCB(device, *job.BoneMatricesPtr);

			job.SkinnedMesh->Draw(device, job.SubsetIndex);
		}
	}

	void D3D11RenderManager::RenderBackBuffer(const std::shared_ptr<D3D11Device>& device)
	{
		mSwapChainRTV->Bind(device, mNullDSV);
		mBackBufferSRV->Bind(device, 0, ED3D11ShaderType::Pixelshader);
		mFullScreenVB->Bind(device);
		mFullScreenIB->Bind(device);

		mFullScreenLayout->Bind(device);
		mFullScreenVS->Bind(device);
		mFullScreenPS->Bind(device);

		mPointClampSamplerState->Bind(device, 0, ED3D11ShaderType::Pixelshader);

		device->GetDeviceContext()->DrawIndexed(6u, 0, 0);
	}

	void D3D11RenderManager::updateModelTransformCB(const std::shared_ptr<D3D11Device>& device, const DirectX::SimpleMath::Matrix& transform)
	{
		ModelTransfrom modelTransform;
		modelTransform.WorldMat = transform.Transpose();
		modelTransform.WorldInvTransposeMat = D3D11Util::InverseTranspose(transform).Transpose();
		mModelTransformCB->Update(device, modelTransform);
	}

	void D3D11RenderManager::updateModelTextureCB(const std::shared_ptr<D3D11Device>& device, const std::shared_ptr<Material>& material)
	{
		ModelTexutre modelTexture;
		modelTexture.bUseAlbedoMap = material->GetHasBaseColor();
		modelTexture.bUseMetalnessMap = material->GetHasMetalness();
		modelTexture.bUseNormalMap = material->GetHasNormal();
		modelTexture.bUseRoughnessMap = material->GetHasRoughness();
		modelTexture.bUseEmissiveMap = material->GetHasEmissive();
		modelTexture.bUseOpacityMap = material->GetHasOpacity();

		mModelTexutreCB->Update(device, modelTexture);
	}

	void D3D11RenderManager::updateBoneTransformCB(const std::shared_ptr<D3D11Device>& device, const std::vector<DirectX::SimpleMath::Matrix>& finalTransforms)
	{
		size_t matrixCount = std::min<size_t>(finalTransforms.size(), (size_t)BoneTransform::MAX_BOND_COUNT);

		BoneTransform boneTransform;
		memcpy(boneTransform.FinalTransforms, finalTransforms.data(), sizeof(DirectX::SimpleMath::Matrix) * matrixCount);

		mBoneTransformCB->Update(device, boneTransform);
	}

	ID3D11ShaderResourceView* D3D11RenderManager::GetBackBufferSRV() const
	{
		return mBackBufferSRV->GetSRV().Get();
	}

	void D3D11RenderManager::createFullScreenBuffer(const std::shared_ptr<D3D11Device>& device)
	{
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
	}
}