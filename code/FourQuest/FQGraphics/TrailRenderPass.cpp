#include "TrailRenderPass.h"
#include "ManagementCommon.h"
#include "TrailObject.h"

namespace fq::graphics
{
	void TrailRenderPass::Initialize(std::shared_ptr<D3D11Device> device,
		std::shared_ptr<D3D11TrailManager> trailManager,
		std::shared_ptr<D3D11CameraManager> cameraManager,
		std::shared_ptr<D3D11ResourceManager> resourceManager,
		std::shared_ptr< D3D11LightManager> lightManager,
		unsigned short width,
		unsigned short height)
	{
		Finalize();

		mDevice = device;
		mTrailManager = trailManager;
		mCameraManager = cameraManager;
		mResourceManager = resourceManager;
		mLightManager = lightManager;

		mBackBufferRTV = mResourceManager->Get<D3D11RenderTargetView>(ED3D11RenderTargetViewType::Offscreen);
		mDSV = mResourceManager->Get<D3D11DepthStencilView>(ED3D11DepthStencilViewType::Default);

		OnResize(width, height);

		auto vertexShader = std::make_shared<D3D11VertexShader>(mDevice, L"TrailVS.cso");
		auto pixelShader = std::make_shared<D3D11PixelShader>(mDevice, L"TrailPS.cso");
		auto twoSideRasterizer = mResourceManager->Create<D3D11RasterizerState>(ED3D11RasterizerState::CullOff);
		auto pipelieState = std::make_shared<PipelineState>(twoSideRasterizer, nullptr, nullptr);
		mShaderProgram = std::make_unique<ShaderProgram>(mDevice, vertexShader, nullptr, pixelShader, pipelieState);
		mLinearWrapSS = mResourceManager->Create<D3D11SamplerState>(ED3D11SamplerState::LinearWrap);

		mDynamicVB = std::make_shared<D3D11VertexBuffer>(mDevice, sizeof(TrailObject::Vertex) * TrailInfo::MAX_VERTEX_SIZE, sizeof(TrailObject::Vertex), 0);

		mPerFrameCB = std::make_shared<D3D11ConstantBuffer<PerFrame>>(mDevice, ED3D11ConstantBuffer::Transform);
	}

	void TrailRenderPass::Finalize()
	{
		mDevice = nullptr;
		mResourceManager = nullptr;
		mCameraManager = nullptr;
		mLightManager = nullptr;
		mTrailManager = nullptr;

		mBackBufferRTV = nullptr;
		mDSV = nullptr;

		mShaderProgram = nullptr;
		mLinearWrapSS = nullptr;

		mDynamicVB = nullptr;

		mPerFrameCB = nullptr;
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
			mDynamicVB->Bind(mDevice);
			mBackBufferRTV->Bind(mDevice, mDSV);
			mLinearWrapSS->Bind(mDevice, 0, ED3D11ShaderType::PixelShader);
		}

		// render
		{
			const std::set<ITrailObject*>& trailObjects = mTrailManager->GetTrailObjects();

			for (ITrailObject* trailObjectInterface : trailObjects)
			{
				TrailObject* trailObject = static_cast<TrailObject*>(trailObjectInterface);

				// get material;
				// material binding;

				// dynamic vertex update
				trailObject->Simulate(mCameraManager->GetPosition(ECameraType::Player));
				const auto& vertices = trailObject->GetVertices();
				mDynamicVB->Update(mDevice, vertices);

				if (trailObject->GetTexture() != nullptr)
				{
					trailObject->GetTexture()->Bind(mDevice, 0, ED3D11ShaderType::PixelShader);
				}

				// draw
				mDevice->GetDeviceContext()->Draw(vertices.size(), 0);
			}
		}
	}
}