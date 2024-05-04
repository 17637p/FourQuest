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
		std::shared_ptr<D3D11ResourceManager> resourceManager)
	{
		mDevice = device;
		mJobManager = jobManager;
		mCameraManager = cameraManager;
		mResourceManager = resourceManager;

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
		mStaticMeshLayout = std::make_shared<D3D11InputLayout>(mDevice, mStaticMeshVS->GetBlob().Get());
		mSkinnedMeshLayout = std::make_shared<D3D11InputLayout>(mDevice, mSkinnedMeshVS->GetBlob().Get());

		mShadowRS = std::make_shared<D3D11RasterizerState>(mDevice, ED3D11RasterizerState::Shadow);
		mDefaultRS = std::make_shared<D3D11RasterizerState>(mDevice, ED3D11RasterizerState::Default);

		mModelTransformCB = std::make_shared<D3D11ConstantBuffer<ModelTransfrom>>(mDevice, ED3D11ConstantBuffer::Transform);
		mSceneTransformCB = std::make_shared<D3D11ConstantBuffer<SceneTrnasform>>(mDevice, ED3D11ConstantBuffer::Transform);
		mBoneTransformCB = std::make_shared<D3D11ConstantBuffer<BoneTransform>>(mDevice, ED3D11ConstantBuffer::Transform);

		mShadowDSV = mResourceManager->Create<D3D11DepthStencilView>(ED3D11DepthStencilViewType::ShaderInputDepthStencil, SHADOW_SIZE, SHADOW_SIZE);
	}

	void ShadowPass::Finalize()
	{

	}

	void ShadowPass::Render()
	{
		// 쉐이더 프로그램을 하나 만들어서 처리 대상을 줄이면 좋을 거 같긴 하네
		ID3D11ShaderResourceView* SRVs[10] = { NULL };
		mDevice->GetDeviceContext()->PSSetShaderResources(0, 10, SRVs);

		mDevice->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		mDevice->GetDeviceContext()->RSSetViewports(1, &mViewport);

		mShadowDSV->Clear(mDevice);
		ID3D11RenderTargetView* renderTargets[1] = { NULL, };
		mDevice->GetDeviceContext()->OMSetRenderTargets(1, renderTargets, mShadowDSV->GetDSV().Get());

		mStaticMeshLayout->Bind(mDevice);
		mStaticMeshVS->Bind(mDevice);

		mDevice->GetDeviceContext()->PSSetShader(NULL, NULL, NULL);
		mModelTransformCB->Bind(mDevice, ED3D11ShaderType::VertexShader);
		mSceneTransformCB->Bind(mDevice, ED3D11ShaderType::VertexShader, 1);
		mShadowRS->Bind(mDevice);

		// 쉐도우 캠 반영, 혹은 라이트마다 처리하는 것도 괜찮을듯
		SceneTrnasform sceneTransform;
		sceneTransform.ViewProjMat = mCameraManager->GetViewMatrix(ECameraType::Player) * DirectX::SimpleMath::Matrix::CreateTranslation(100, 0, 0) * mCameraManager->GetProjectionMatrix(ECameraType::Player);
		sceneTransform.ViewProjMat = sceneTransform.ViewProjMat.Transpose();
		mSceneTransformCB->Update(mDevice, sceneTransform);

		for (const StaticMeshJob& job : mJobManager->GetStaticMeshJobs())
		{
			job.StaticMesh->Bind(mDevice);
			job.Material->Bind(mDevice);

			ConstantBufferHelper::UpdateModelTransformCB(mDevice, mModelTransformCB, *job.TransformPtr);

			job.StaticMesh->Draw(mDevice, job.SubsetIndex);
		}

		mSkinnedMeshLayout->Bind(mDevice);
		mSkinnedMeshVS->Bind(mDevice);
		mBoneTransformCB->Bind(mDevice, ED3D11ShaderType::VertexShader, 2);

		for (const SkinnedMeshJob& job : mJobManager->GetSkinnedMeshJobs())
		{
			job.SkinnedMesh->Bind(mDevice);
			job.Material->Bind(mDevice);

			ConstantBufferHelper::UpdateModelTransformCB(mDevice, mModelTransformCB, *job.TransformPtr);
			ConstantBufferHelper::UpdateBoneTransformCB(mDevice, mBoneTransformCB, *job.BoneMatricesPtr);

			job.SkinnedMesh->Draw(mDevice, job.SubsetIndex);
		}

		mDefaultRS->Bind(mDevice);
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

		mBackBufferRTV = mResourceManager->Get<D3D11RenderTargetView>(ED3D11RenderTargetViewType::Offscreen);
		mDefaultDSV = mResourceManager->Get<D3D11DepthStencilView>(ED3D11DepthStencilViewType::Default);

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
		mBackBufferRTV->Clear(mDevice, { 1,1,1,1 });
		mDefaultDSV->Clear(mDevice);

		if (!isSetSkyBox)
		{
			return;
		}

		mDevice->GetDeviceContext()->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		mBackBufferRTV->Bind(mDevice, mDefaultDSV);

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
}