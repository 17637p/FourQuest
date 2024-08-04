#include "DeferredShadingPass.h"
#include "ManagementCommon.h"
#include "D3D11Common.h"
#include "ShadowPass.h"

namespace fq::graphics
{
	void DeferredShadingPass::Initialize(std::shared_ptr<D3D11Device> device,
		std::shared_ptr<D3D11ResourceManager> resourceManager,
		std::shared_ptr< D3D11LightManager> lightManager,
		std::shared_ptr<D3D11CameraManager> cameraManager,
		unsigned short width,
		unsigned short height)
	{
		Finalize();

		mDevice = device;
		mResourceManager = resourceManager;
		mLightManager = lightManager;
		mCameraManager = cameraManager;

		OnResize(width, height);


		auto shadowDSV = mResourceManager->Get<D3D11DepthStencilView>(ED3D11DepthStencilViewType::CascadeShadow3);
		mShadowSRV = std::make_shared<D3D11ShaderResourceView>(mDevice, shadowDSV);

		mBackBufferRTV = mResourceManager->Get<D3D11RenderTargetView>(ED3D11RenderTargetViewType::Offscreen);
		mNullDSV = mResourceManager->Get<D3D11DepthStencilView>(ED3D11DepthStencilViewType::None);

		mAnisotropicWrapSamplerState = resourceManager->Create<D3D11SamplerState>(ED3D11SamplerState::AnisotropicWrap);
		mLinearClampSamplerState = resourceManager->Create<D3D11SamplerState>(ED3D11SamplerState::Default);
		mPointClampSamplerState = resourceManager->Create<D3D11SamplerState>(ED3D11SamplerState::AnisotropicClamp);
		mShadowSampler = resourceManager->Create<D3D11SamplerState>(ED3D11SamplerState::Shadow);

		auto fullScreenVS = std::make_shared<D3D11VertexShader>(device, L"FullScreenVS.cso");
		auto shadingPS = std::make_shared<D3D11PixelShader>(mDevice, L"ModelPSDeferred_SHADING.cso");
		auto pipelieState = std::make_shared<PipelineState>(nullptr, nullptr, nullptr);
		mShaderProgram = std::make_unique<ShaderProgram>(mDevice, fullScreenVS, nullptr, shadingPS, pipelieState);

		mFullScreenVB = std::make_shared<D3D11VertexBuffer>(D3D11VertexBuffer::CreateFullScreenVertexBuffer(device));
		mFullScreenIB = std::make_shared<D3D11IndexBuffer>(D3D11IndexBuffer::CreateFullScreenIndexBuffer(device));
	}
	void DeferredShadingPass::Finalize()
	{
		mDevice = nullptr;
		mResourceManager = nullptr;
		mLightManager = nullptr;
		mCameraManager = nullptr;

		mShadowSRV = nullptr;
		mNullDSV = nullptr;
		mBackBufferRTV = nullptr;

		mAlbedoSRV = nullptr;
		mMetalnessSRV = nullptr;
		mRoughnessSRV = nullptr;
		mNormalSRV = nullptr;
		mEmissiveSRV = nullptr;
		mPositionSRV = nullptr;

		mShaderProgram = nullptr;

		mAnisotropicWrapSamplerState = nullptr;
		mLinearClampSamplerState = nullptr;
		mPointClampSamplerState = nullptr;
		mShadowSampler = nullptr;

		mFullScreenVB = nullptr;
		mFullScreenIB = nullptr;
	}
	void DeferredShadingPass::OnResize(unsigned short width, unsigned short height)
	{
		mViewport.Width = (float)width;
		mViewport.Height = (float)height;
		mViewport.MinDepth = 0.f;
		mViewport.MaxDepth = 1.f;
		mViewport.TopLeftX = 0.f;
		mViewport.TopLeftY = 0.f;

		auto AlbedoRTV = mResourceManager->Get<D3D11RenderTargetView>(ED3D11RenderTargetViewType::Albedo);
		auto MetalnessRTV = mResourceManager->Get<D3D11RenderTargetView>(ED3D11RenderTargetViewType::Metalness);
		auto RoughnessRTV = mResourceManager->Get<D3D11RenderTargetView>(ED3D11RenderTargetViewType::Roughness);
		auto NormalRTV = mResourceManager->Get<D3D11RenderTargetView>(ED3D11RenderTargetViewType::Normal);
		auto EmissiveRTV = mResourceManager->Get<D3D11RenderTargetView>(ED3D11RenderTargetViewType::Emissive);
		auto PositionRTV = mResourceManager->Get<D3D11RenderTargetView>(ED3D11RenderTargetViewType::PositionWClipZ);

		mAlbedoSRV = std::make_shared<D3D11ShaderResourceView>(mDevice, AlbedoRTV);
		mMetalnessSRV = std::make_shared<D3D11ShaderResourceView>(mDevice, MetalnessRTV);
		mRoughnessSRV = std::make_shared<D3D11ShaderResourceView>(mDevice, RoughnessRTV);
		mNormalSRV = std::make_shared<D3D11ShaderResourceView>(mDevice, NormalRTV);
		mEmissiveSRV = std::make_shared<D3D11ShaderResourceView>(mDevice, EmissiveRTV);
		mPositionSRV = std::make_shared<D3D11ShaderResourceView>(mDevice, PositionRTV);
	}
	void DeferredShadingPass::Render()
	{
		using namespace DirectX::SimpleMath;

		// init
		{
			ID3D11ShaderResourceView* NullSRVs[10] = { NULL, };
			mDevice->GetDeviceContext()->PSSetShaderResources(0, ARRAYSIZE(NullSRVs), NullSRVs);
		}

		// bind
		{
			mBackBufferRTV->Bind(mDevice, mNullDSV);

			mDevice->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			mShaderProgram->Bind(mDevice);

			mFullScreenVB->Bind(mDevice);
			mFullScreenIB->Bind(mDevice);

			mAlbedoSRV->Bind(mDevice, 0, ED3D11ShaderType::PixelShader);
			mMetalnessSRV->Bind(mDevice, 1, ED3D11ShaderType::PixelShader);
			mRoughnessSRV->Bind(mDevice, 2, ED3D11ShaderType::PixelShader);
			mNormalSRV->Bind(mDevice, 3, ED3D11ShaderType::PixelShader);
			mEmissiveSRV->Bind(mDevice, 4, ED3D11ShaderType::PixelShader);
			mPositionSRV->Bind(mDevice, 5, ED3D11ShaderType::PixelShader);
			
			const auto& iblTexture = mLightManager->GetIBLTexture();
			if (iblTexture.DiffuseIrradiance != nullptr)
			{
				mDevice->GetDeviceContext()->PSSetShaderResources(6, 1, iblTexture.DiffuseIrradiance->GetSRV().GetAddressOf());
			}
			if (iblTexture.SpecularIBL != nullptr)
			{
				mDevice->GetDeviceContext()->PSSetShaderResources(7, 1, iblTexture.SpecularIBL->GetSRV().GetAddressOf());
			}
			if (iblTexture.SpecularBRDF != nullptr)
			{
				mDevice->GetDeviceContext()->PSSetShaderResources(8, 1, iblTexture.SpecularBRDF->GetSRV().GetAddressOf());
			}

			mShadowSRV->Bind(mDevice, 9, ED3D11ShaderType::PixelShader);

			mLightManager->GetShadowConstnatBuffer()->Bind(mDevice, ED3D11ShaderType::PixelShader, 0);
			mLightManager->GetLightConstnatBuffer()->Bind(mDevice, ED3D11ShaderType::PixelShader, 1);

			mAnisotropicWrapSamplerState->Bind(mDevice, 0, ED3D11ShaderType::PixelShader);
			mLinearClampSamplerState->Bind(mDevice, 1, ED3D11ShaderType::PixelShader);
			mPointClampSamplerState->Bind(mDevice, 2, ED3D11ShaderType::PixelShader);
			mShadowSampler->Bind(mDevice, 3, ED3D11ShaderType::PixelShader);
		}

		// draw
		{
			mDevice->GetDeviceContext()->DrawIndexed(6, 0, 0);
		}
	}
}