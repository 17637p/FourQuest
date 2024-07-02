#include "TrailRenderPass.h"
#include "ManagementCommon.h"
#include "TrailObject.h"
#include "Material.h"

namespace fq::graphics
{
	void TrailRenderPass::Initialize(std::shared_ptr<D3D11Device> device,
		std::shared_ptr<D3D11ObjectManager> objectManager,
		std::shared_ptr<D3D11CameraManager> cameraManager,
		std::shared_ptr<D3D11ResourceManager> resourceManager,
		std::shared_ptr< D3D11LightManager> lightManager,
		unsigned short width,
		unsigned short height)
	{
		Finalize();

		mDevice = device;
		mObjectManager = objectManager;
		mCameraManager = cameraManager;
		mResourceManager = resourceManager;
		mLightManager = lightManager;

		mBackBufferRTV = mResourceManager->Get<D3D11RenderTargetView>(ED3D11RenderTargetViewType::Offscreen);
		mDSV = mResourceManager->Get<D3D11DepthStencilView>(ED3D11DepthStencilViewType::Default);

		OnResize(width, height);

		auto vertexShader = std::make_shared<D3D11VertexShader>(mDevice, L"TrailVS.cso");
		auto pixelShader = std::make_shared<D3D11PixelShader>(mDevice, L"TrailPS.cso");
		mDefaultRasterizer = mResourceManager->Create<D3D11RasterizerState>(ED3D11RasterizerState::Default);
		mCullOffRasterizer = mResourceManager->Create<D3D11RasterizerState>(ED3D11RasterizerState::CullOff);
		auto pipelieState = std::make_shared<PipelineState>(nullptr, nullptr, nullptr);
		mAdditiveState = resourceManager->Create<D3D11BlendState>(ED3D11BlendState::Additive);
		mSubtractiveState = resourceManager->Create<D3D11BlendState>(ED3D11BlendState::Subtractive);
		mModulateState = resourceManager->Create<D3D11BlendState>(ED3D11BlendState::Modulate);
		mAlphaBlendState = resourceManager->Create<D3D11BlendState>(ED3D11BlendState::AlphaBlend);
		mShaderProgram = std::make_unique<ShaderProgram>(mDevice, vertexShader, nullptr, pixelShader, pipelieState);
		mLinearWrapSS = mResourceManager->Create<D3D11SamplerState>(ED3D11SamplerState::LinearWrap);

		mDynamicVB = std::make_shared<D3D11VertexBuffer>(mDevice, sizeof(TrailObject::Vertex) * TrailInfo::MAX_VERTEX_SIZE, sizeof(TrailObject::Vertex), 0);

		mPerFrameCB = std::make_shared<D3D11ConstantBuffer<PerFrame>>(mDevice, ED3D11ConstantBuffer::Transform);
		mMaterialCB = std::make_shared<D3D11ConstantBuffer<CBParticleMaterial>>(mDevice, ED3D11ConstantBuffer::Transform);
	}

	void TrailRenderPass::Finalize()
	{
		mDevice = nullptr;
		mResourceManager = nullptr;
		mCameraManager = nullptr;
		mLightManager = nullptr;
		mObjectManager = nullptr;

		mBackBufferRTV = nullptr;
		mDSV = nullptr;

		mDefaultRasterizer = nullptr;
		mCullOffRasterizer = nullptr;
		mAdditiveState = nullptr;
		mSubtractiveState = nullptr;
		mModulateState = nullptr;
		mAlphaBlendState = nullptr;

		mShaderProgram = nullptr;
		mLinearWrapSS = nullptr;

		mDynamicVB = nullptr;

		mPerFrameCB = nullptr;
		mMaterialCB = nullptr;
	}

	void TrailRenderPass::OnResize(unsigned short width, unsigned short height)
	{
		mViewport.Width = (float)width;
		mViewport.Height = (float)height;
		mViewport.MinDepth = 0.f;
		mViewport.MaxDepth = 1.f;
		mViewport.TopLeftX = 0.f;
		mViewport.TopLeftY = 0.f;
	}

	void TrailRenderPass::Render()
	{
		// update
		{
			PerFrame perFrame;
			perFrame.ViewProj = mCameraManager->GetViewMatrix(ECameraType::Player) * mCameraManager->GetProjectionMatrix(ECameraType::Player);
			perFrame.ViewProj = perFrame.ViewProj.Transpose();

			mPerFrameCB->Update(mDevice, perFrame);
		}

		// bind
		{
			mDevice->GetDeviceContext()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
			mShaderProgram->Bind(mDevice);
			mPerFrameCB->Bind(mDevice, ED3D11ShaderType::VertexShader);
			mMaterialCB->Bind(mDevice, ED3D11ShaderType::VertexShader, 1);
			mMaterialCB->Bind(mDevice, ED3D11ShaderType::PixelShader);
			mDynamicVB->Bind(mDevice);
			mBackBufferRTV->Bind(mDevice, mDSV);
			mLinearWrapSS->Bind(mDevice, 0, ED3D11ShaderType::PixelShader);
		}

		// render
		{
			const std::set<std::shared_ptr<ITrailObject>>& trailObjects = mObjectManager->GetTrailObjects();

			for (std::shared_ptr<ITrailObject> trailObjectInterface : trailObjects)
			{
				std::shared_ptr<TrailObject> trailObject = std::static_pointer_cast<TrailObject>(trailObjectInterface);
				std::shared_ptr<ParticleMaterial> material = std::static_pointer_cast<ParticleMaterial>(trailObject->GetIParticleMaterial());

				material->Bind(mDevice);

				const auto& materialInfo = material->GetInfo();
				CBParticleMaterial particleMaterialCB;
				particleMaterialCB.BaseColor = materialInfo.BaseColor;
				particleMaterialCB.EmissiveColor = materialInfo.EmissiveColor;
				particleMaterialCB.TexTransform = DirectX::SimpleMath::Matrix::CreateScale(materialInfo.Tiling.x, materialInfo.Tiling.y, 0) * DirectX::SimpleMath::Matrix::CreateTranslation(materialInfo.Offset.x, materialInfo.Offset.y, 0);
				particleMaterialCB.RenderMode = (int)materialInfo.RenderModeType;
				particleMaterialCB.ColorMode = (int)materialInfo.ColorModeType;
				particleMaterialCB.bUseAlbedoMap = material->GetHasBaseColor();
				particleMaterialCB.bUseEmissiveMap = material->GetHasEmissive();
				particleMaterialCB.AlphaCutoff = materialInfo.AlphaCutoff;
				particleMaterialCB.bUseMultiplyAlpha = materialInfo.bUseMultiplyAlpha;
				mMaterialCB->Update(mDevice, particleMaterialCB);

				// dynamic vertex update
				trailObject->Simulate(mCameraManager->GetPosition(ECameraType::Player));
				const auto& vertices = trailObject->GetVertices();
				mDynamicVB->Update(mDevice, vertices);

				switch (materialInfo.RenderModeType)
				{
				case ParticleMaterialInfo::ERenderMode::Additive:
					mAdditiveState->Bind(mDevice);
					break;
				case ParticleMaterialInfo::ERenderMode::Subtractive:
					mSubtractiveState->Bind(mDevice);
					break;
				case ParticleMaterialInfo::ERenderMode::Modulate:
					mModulateState->Bind(mDevice);
					break;
				case ParticleMaterialInfo::ERenderMode::AlphaBlend:
					mAlphaBlendState->Bind(mDevice);
					break;
				default:
					assert(false);
					break;
				}

				if (materialInfo.bIsTwoSide)
				{
					mCullOffRasterizer->Bind(mDevice);
				}
				else
				{
					mDefaultRasterizer->Bind(mDevice);
				}

				// draw
				mDevice->GetDeviceContext()->Draw(vertices.size(), 0);
			}
		}
	}
}