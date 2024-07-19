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

		mDirectioanlShadowInfoCB = resourceManager->Create< D3D11ConstantBuffer<DirectionalShadowInfo>>(ED3D11ConstantBuffer::DirectionalShadowInfo);
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
		mMetalnessRoughnessSRV = nullptr;
		mPreCalculatedSRV = nullptr;
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

		mDirectioanlShadowInfoCB = nullptr;
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
		auto MetalnessRoughnessRTV = mResourceManager->Get<D3D11RenderTargetView>(ED3D11RenderTargetViewType::MetalnessRoughness);
		auto NormalRTV = mResourceManager->Get<D3D11RenderTargetView>(ED3D11RenderTargetViewType::Normal);
		auto EmissiveRTV = mResourceManager->Get<D3D11RenderTargetView>(ED3D11RenderTargetViewType::Emissive);
		auto PositionRTV = mResourceManager->Get<D3D11RenderTargetView>(ED3D11RenderTargetViewType::PositionWClipZ);
		auto preCalculatedLightRTV = mResourceManager->Get<D3D11RenderTargetView>(ED3D11RenderTargetViewType::PreCalculatedLight);

		mAlbedoSRV = std::make_shared<D3D11ShaderResourceView>(mDevice, AlbedoRTV);
		mMetalnessRoughnessSRV = std::make_shared<D3D11ShaderResourceView>(mDevice, MetalnessRoughnessRTV);
		mNormalSRV = std::make_shared<D3D11ShaderResourceView>(mDevice, NormalRTV);
		mEmissiveSRV = std::make_shared<D3D11ShaderResourceView>(mDevice, EmissiveRTV);
		mPositionSRV = std::make_shared<D3D11ShaderResourceView>(mDevice, PositionRTV);
		mPreCalculatedSRV = std::make_shared<D3D11ShaderResourceView>(mDevice, preCalculatedLightRTV);
	}
	void DeferredShadingPass::Render()
	{
		using namespace DirectX::SimpleMath;
		// update
		{
			size_t currentDirectionaShadowCount = mLightManager->GetDirectionalShadows().size();
			DirectionalShadowInfo directionalShadowData;
			directionalShadowData.ShadowCount = currentDirectionaShadowCount;

			if (currentDirectionaShadowCount > 0)
			{
				const std::vector<std::shared_ptr<Light<DirectionalLight>>>& directioanlShadows = mLightManager->GetDirectionalShadows();

				for (size_t i = 0; i < currentDirectionaShadowCount; ++i)
				{
					std::vector<float> cascadeEnds = ShadowPass::CalculateCascadeEnds({ 0.33, 0.66 },
						mCameraManager->GetNearPlane(ECameraType::Player),
						mCameraManager->GetFarPlane(ECameraType::Player));

					std::vector<DirectX::SimpleMath::Matrix> shadowTransforms = ShadowPass::CalculateCascadeShadowTransform(cascadeEnds,
						mCameraManager->GetViewMatrix(ECameraType::Player),
						mCameraManager->GetProjectionMatrix(ECameraType::Player),
						directioanlShadows[i]->GetData().direction);
					assert(shadowTransforms.size() == 3);

					DirectX::SimpleMath::Matrix texTransform =
					{
						 0.5f, 0.0f, 0.0f, 0.0f,
						 0.0f, -0.5f, 0.0f, 0.0f,
						 0.0f, 0.0f, 1.0f, 0.0f,
						 0.5f, 0.5f, 0.0f, 1.0f
					};
					auto cameraProj = mCameraManager->GetProjectionMatrix(ECameraType::Player);
					size_t shaodwIndex = i * DirectionalShadowTransform::MAX_SHADOW_COUNT;

					directionalShadowData.ShadowViewProj[shaodwIndex] = (shadowTransforms[0] * texTransform).Transpose();
					directionalShadowData.CascadeEnds[i].x = Vector4::Transform({ 0, 0, cascadeEnds[1], 1.f }, cameraProj).z;
					directionalShadowData.ShadowViewProj[shaodwIndex + 1] = (shadowTransforms[1] * texTransform).Transpose();
					directionalShadowData.CascadeEnds[i].y = Vector4::Transform({ 0, 0, cascadeEnds[2], 1.f }, cameraProj).z;
					directionalShadowData.ShadowViewProj[shaodwIndex + 2] = (shadowTransforms[2] * texTransform).Transpose();
					directionalShadowData.CascadeEnds[i].z = Vector4::Transform({ 0, 0, cascadeEnds[3], 1.f }, cameraProj).z;
				}
			}

			mDirectioanlShadowInfoCB->Update(mDevice, directionalShadowData);
			mLightManager->UpdateConstantBuffer(mDevice, mCameraManager->GetPosition(ECameraType::Player), true);
		}

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
			mMetalnessRoughnessSRV->Bind(mDevice, 1, ED3D11ShaderType::PixelShader);
			mNormalSRV->Bind(mDevice, 2, ED3D11ShaderType::PixelShader);
			mEmissiveSRV->Bind(mDevice, 3, ED3D11ShaderType::PixelShader);
			mPositionSRV->Bind(mDevice, 4, ED3D11ShaderType::PixelShader);
			mPreCalculatedSRV->Bind(mDevice, 5, ED3D11ShaderType::PixelShader);
			
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

			mDirectioanlShadowInfoCB->Bind(mDevice, ED3D11ShaderType::PixelShader, 0);
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