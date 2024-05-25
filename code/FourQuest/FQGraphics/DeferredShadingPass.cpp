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

		D3D_SHADER_MACRO macroShading[] =
		{
			{"SHADING", ""},
			{ NULL, NULL}
		};

		auto fullScreenVS = std::make_shared<D3D11VertexShader>(device, L"./resource/internal/shader/FullScreenVS.hlsl");
		auto shadingPS = std::make_shared<D3D11PixelShader>(mDevice, L"./resource/internal/shader/ModelPSDeferred.hlsl", macroShading);
		auto pipelieState = std::make_shared<PipelineState>(nullptr, nullptr, nullptr);
		mShaderProgram = std::make_unique<ShaderProgram>(mDevice, fullScreenVS, nullptr, shadingPS, pipelieState);

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

		mAlbedoSRV = nullptr;
		mMetalnessSRV = nullptr;
		mRoughnessSRV = nullptr;
		mNormalSRV = nullptr;
		mEmissiveSRV = nullptr;
		mPositionSRV = nullptr;

		mBackBufferRTV = nullptr;

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
						mCameraManager->GetNearPlain(ECameraType::Player),
						mCameraManager->GetFarPlain(ECameraType::Player));

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
			mLightManager->UpdateConstantBuffer(mDevice, mCameraManager->GetPosition(ECameraType::Player), false);
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

			mAlbedoSRV->Bind(mDevice, 0, ED3D11ShaderType::Pixelshader);
			mMetalnessSRV->Bind(mDevice, 1, ED3D11ShaderType::Pixelshader);
			mRoughnessSRV->Bind(mDevice, 2, ED3D11ShaderType::Pixelshader);
			mNormalSRV->Bind(mDevice, 3, ED3D11ShaderType::Pixelshader);
			mEmissiveSRV->Bind(mDevice, 4, ED3D11ShaderType::Pixelshader);
			mPositionSRV->Bind(mDevice, 5, ED3D11ShaderType::Pixelshader);
			mShadowSRV->Bind(mDevice, 9, ED3D11ShaderType::Pixelshader);

			mDirectioanlShadowInfoCB->Bind(mDevice, ED3D11ShaderType::Pixelshader, 0);
			mLightManager->GetLightConstnatBuffer()->Bind(mDevice, ED3D11ShaderType::Pixelshader, 1);

			mAnisotropicWrapSamplerState->Bind(mDevice, 0, ED3D11ShaderType::Pixelshader);
			mLinearClampSamplerState->Bind(mDevice, 1, ED3D11ShaderType::Pixelshader);
			mPointClampSamplerState->Bind(mDevice, 2, ED3D11ShaderType::Pixelshader);
			mShadowSampler->Bind(mDevice, 3, ED3D11ShaderType::Pixelshader);
		}

		// draw
		{
			mDevice->GetDeviceContext()->DrawIndexed(6, 0, 0);
		}
	}
}