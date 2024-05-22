#include "ParticlePass.h"
#include "ManagementCommon.h"
#include "D3D11Common.h"

namespace fq::graphics
{
	float RandF()
	{
		return (float)(rand()) / (float)RAND_MAX;
	}

	float RandF(float a, float b)
	{
		return a + RandF() * (b - a);
	}

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> CreateRandomTexture1DSRV(ID3D11Device* device)
	{
		using namespace DirectX::SimpleMath;

		// 랜덤 백터 생성
		const size_t ELEMENT_COUNT = 1024u;

		std::vector<Vector4> randomValues(ELEMENT_COUNT);

		for (size_t i = 0; i < ELEMENT_COUNT; ++i)
		{
			randomValues[i].x = RandF(-1000000.0f, 1000000.0f);
			randomValues[i].y = RandF(-1000000.0f, 1000000.0f);
			randomValues[i].z = RandF(-1000000.0f, 1000000.0f);
			randomValues[i].w = RandF(-1000000.0f, 1000000.0f);
		}

		// 텍스처 생성
		D3D11_TEXTURE1D_DESC texDesc;
		texDesc.Width = ELEMENT_COUNT;
		texDesc.MipLevels = 1;
		texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		texDesc.Usage = D3D11_USAGE_IMMUTABLE;
		texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		texDesc.CPUAccessFlags = 0;
		texDesc.MiscFlags = 0;
		texDesc.ArraySize = 1;

		D3D11_SUBRESOURCE_DATA initData;
		initData.pSysMem = &randomValues[0];
		initData.SysMemPitch = ELEMENT_COUNT * sizeof(Vector4);
		initData.SysMemSlicePitch = 0;

		Microsoft::WRL::ComPtr<ID3D11Texture1D> randomTex;
		HR(device->CreateTexture1D(&texDesc, &initData, randomTex.GetAddressOf()));

		// 쉐이더 리소스 뷰 생성
		D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
		viewDesc.Format = texDesc.Format;
		viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1D; // 1D 텍스처
		viewDesc.Texture1D.MipLevels = texDesc.MipLevels;
		viewDesc.Texture1D.MostDetailedMip = 0;

		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> randomTexSRV;
		assert(randomTex != nullptr);
		HR(device->CreateShaderResourceView(randomTex.Get(), &viewDesc, randomTexSRV.GetAddressOf()));

		return randomTexSRV;
	}

	void ParticlePass::Initialize(std::shared_ptr<D3D11Device> device,
		std::shared_ptr<D3D11ParticleManager> paricleManager,
		std::shared_ptr<D3D11CameraManager> cameraManager,
		std::shared_ptr<D3D11ResourceManager> resourceManager,
		std::shared_ptr< D3D11LightManager> lightManager,
		unsigned short width,
		unsigned short height)
	{
		Finalize();

		mDevice = device;
		mParticleManager = paricleManager;
		mCameraManager = cameraManager;
		mLightManager = lightManager;
		mResourceManager = resourceManager;
		OnResize(width, height);
		// rendering shader

		D3D_SHADER_MACRO macroGenerate[] =
		{
			{"GENERATE", ""},
			{ NULL, NULL}
		};
		D3D_SHADER_MACRO macroUpdate[] =
		{
			{"UPDATE", ""},
			{ NULL, NULL}
		};

		auto generateCS = std::make_shared<D3D11ComputeShader>(device, L"./resource/internal/shader/ParticleCS.hlsl", macroGenerate);
		auto updateCS = std::make_shared<D3D11ComputeShader>(device, L"./resource/internal/shader/ParticleCS.hlsl", macroUpdate);
		auto renderVS = std::make_shared<D3D11VertexShader>(device, L"./resource/internal/shader/ParticleVS.hlsl");
		auto renderPS = std::make_shared<D3D11PixelShader>(device, L"./resource/internal/shader/ParticlePS.hlsl");
		auto renderGS = std::make_shared<D3D11GeometryShader>(device, L"./resource/internal/shader/ParticleGS.hlsl");
		auto additiveBlend = resourceManager->Create<D3D11BlendState>(ED3D11BlendState::Additive);
		auto depthOff = resourceManager->Create<D3D11DepthStencilState>(ED3D11DepthStencilState::DepthOff);
		auto cullOff = resourceManager->Create<D3D11RasterizerState>(ED3D11RasterizerState::CullOff);
		auto nonePipelieState = std::make_shared<PipelineState>(nullptr, nullptr, nullptr);
		auto renderPipelieState = std::make_shared<PipelineState>(cullOff, nullptr, additiveBlend);
		mParticleGenerateProgram = std::make_unique<ShaderProgram>(mDevice, nonePipelieState, nullptr, nullptr, nullptr, generateCS);
		mParticleUpdateProgram = std::make_unique<ShaderProgram>(mDevice, nonePipelieState, nullptr, nullptr, nullptr, updateCS);
		mParticleRenderProgram = std::make_unique<ShaderProgram>(mDevice, renderPipelieState, renderVS, renderPS, renderGS, nullptr);
		mLinearSamepler = resourceManager->Create<D3D11SamplerState>(ED3D11SamplerState::LinearWrap);

		mParticleMainCB = std::make_shared<D3D11ConstantBuffer<ParticleMain>>(mDevice, ED3D11ConstantBuffer::Transform);
		mParticleShapeCB = std::make_shared<D3D11ConstantBuffer<ParticleShape>>(mDevice, ED3D11ConstantBuffer::Transform);
		mSceneInfomationCB = std::make_shared<D3D11ConstantBuffer<SceneInfomation>>(mDevice, ED3D11ConstantBuffer::Transform);
		mRandomSRV = CreateRandomTexture1DSRV(mDevice->GetDevice().Get());

		std::vector<unsigned int> vertices(ParticleSystemInfo::MAX_PARTICLE_COUNT, 0);
		mParticleVB = std::make_shared<D3D11VertexBuffer>(device, vertices);

		// 전역 자원 획득
		mBackBufferRTV = mResourceManager->Get<fq::graphics::D3D11RenderTargetView>(ED3D11RenderTargetViewType::Offscreen);
		mDSV = mResourceManager->Get<fq::graphics::D3D11DepthStencilView>(ED3D11DepthStencilViewType::Default);
	}
	void ParticlePass::Finalize()
	{
		mDevice = nullptr;
		mResourceManager = nullptr;
		mCameraManager = nullptr;
		mLightManager = nullptr;
		mParticleManager = nullptr;

		mBackBufferRTV = nullptr;
		mDSV = nullptr;

		mParticleGenerateProgram = nullptr;
		mParticleUpdateProgram = nullptr;
		mParticleRenderProgram = nullptr;
	}

	void ParticlePass::OnResize(unsigned short width, unsigned short height)
	{
		mViewport.Width = (float)width;
		mViewport.Height = (float)height;
		mViewport.MinDepth = 0.f;
		mViewport.MaxDepth = 1.f;
		mViewport.TopLeftX = 0.f;
		mViewport.TopLeftY = 0.f;
	}

	void ParticlePass::Render()
	{
		// 초기화
		{
			ID3D11ShaderResourceView* SRVs[5] = { NULL };
			mDevice->GetDeviceContext()->PSSetShaderResources(0, 5, SRVs);
		}

		std::map<size_t, std::shared_ptr<ParticleSystem>> emitters = mParticleManager->GetParticleSystems();

		static float totalTime = 0.f;
		totalTime += 1.f / 60.f;

		// 공통 바인딩
		{
			mLinearSamepler->Bind(mDevice, 0, ED3D11ShaderType::ComputeShader);
			mDevice->GetDeviceContext()->CSSetShaderResources(0, 1, mRandomSRV.GetAddressOf());
			mParticleMainCB->Bind(mDevice, ED3D11ShaderType::ComputeShader, 0);
			mParticleShapeCB->Bind(mDevice, ED3D11ShaderType::ComputeShader, 1);
			mParticleMainCB->Bind(mDevice, ED3D11ShaderType::ComputeShader, 2);
		}

		// 생성 및 갱신
		{
			for (const auto& [key, emitter] : emitters)
			{
				// 업데이트
				updateParticleCB(emitter);

				// 생성
				if (emitter->Emission() && emitter->GetInfo().MaxParticleCount > emitter->GetParticleCount())
				{
					mParticleGenerateProgram->Bind(mDevice);
					ID3D11UnorderedAccessView* pNullUAVs[2] = { NULL, };
					mDevice->GetDeviceContext()->CSSetUnorderedAccessViews(0, 2, pNullUAVs, NULL);

					emitter->GetAppendUAV()->Bind(mDevice, 0);
					size_t emissionCount = 1;
					mDevice->GetDeviceContext()->Dispatch(emissionCount, 1, 1);
					emitter->SetParticleCount(emitter->GetParticleCount() + emissionCount);

					{
						auto buffer = emitter->GetAppendBuffer()->GetBuffer();
						auto debugBuffer = emitter->GetAppendBuffer()->GetDebugBuffer();

						mDevice->GetDeviceContext()->CopyResource(debugBuffer.Get(), buffer.Get());

						D3D11_MAPPED_SUBRESOURCE mappedData;
						mDevice->GetDeviceContext()->Map(debugBuffer.Get(), 0, D3D11_MAP_READ, 0, &mappedData);
						Particle* particle = reinterpret_cast<Particle*>(mappedData.pData);
						mDevice->GetDeviceContext()->Unmap(debugBuffer.Get(), 0);
					}
				}

				emitter->Swap();
				emitter->GetAppendUAV()->Clear(mDevice);

				// 갱신
				if (emitter->GetParticleCount() > 0)
				{
					mParticleUpdateProgram->Bind(mDevice);
					ID3D11UnorderedAccessView* pNullUAVs[2] = { NULL, };
					mDevice->GetDeviceContext()->CSSetUnorderedAccessViews(0, 2, pNullUAVs, NULL);

					emitter->GetAppendUAV()->Bind(mDevice, 0);
					emitter->GetConsumeUAV()->Bind(mDevice, 1);

					size_t dispatchX = (emitter->GetParticleCount() - 1) / 512 + 1;
					mDevice->GetDeviceContext()->Dispatch(dispatchX, 1, 1);
				}

				emitter->Swap();
			}
		}

		// 언바인딩
		{
			ID3D11ShaderResourceView* SRVs[5] = { NULL, };
			mDevice->GetDeviceContext()->CSSetShaderResources(0, 5, SRVs);
		}

		// 랜더링
		{
			mParticleUpdateProgram->Bind(mDevice);
			ID3D11Buffer* buffers[10] = { NULL };
			mDevice->GetDeviceContext()->CSSetConstantBuffers(0, 10, buffers);
			ID3D11UnorderedAccessView* pNullUAVs[2] = { NULL, };
			mDevice->GetDeviceContext()->CSSetUnorderedAccessViews(0, 2, pNullUAVs, NULL);
			mDevice->GetDeviceContext()->IASetIndexBuffer(NULL, DXGI_FORMAT_R32_UINT, 0);
			mDevice->GetDeviceContext()->IASetVertexBuffers(0, 0, NULL, NULL, NULL);

			mParticleVB->Bind(mDevice);

			mBackBufferRTV->Bind(mDevice, mDSV);
			mParticleRenderProgram->Bind(mDevice);
			mSceneInfomationCB->Bind(mDevice, ED3D11ShaderType::GeometryShader, 0);
			SceneInfomation sceneInfo;
			sceneInfo.ViewProjMat = (mCameraManager->GetViewMatrix(ECameraType::Player) * mCameraManager->GetProjectionMatrix(ECameraType::Player)).Transpose();
			sceneInfo.EyePosition = (DirectX::SimpleMath::Vector4)mCameraManager->GetPosition(ECameraType::Player);
			mSceneInfomationCB->Update(mDevice, sceneInfo);

			mDevice->GetDeviceContext()->RSSetViewports(1, &mViewport);
			mDevice->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
			// 널에 해당하는 인풋어셈블러를 바인딩시켜줘야 되는 거 같은데;

			for (const auto& [key, emitter] : emitters)
			{
				emitter->Swap();
				emitter->GetConsumeSRV()->Bind(mDevice, 0, ED3D11ShaderType::VertexShader);

				mDevice->GetDeviceContext()->Draw(emitter->GetParticleCount(), 0);
			}

			ID3D11ShaderResourceView* SRVs[5] = { NULL };
			mDevice->GetDeviceContext()->VSSetShaderResources(0, 5, SRVs);
		}
	}

	void ParticlePass::updateParticleCB(const std::shared_ptr<ParticleSystem>& particleSystems)
	{
		using namespace DirectX::SimpleMath;

		ParticleSystemInfo& info = particleSystems->GetInfo();

		ParticleMain particleMain;
		particleMain.Transform = Matrix::CreateScale(info.WorldScale) * Matrix::CreateFromYawPitchRoll(info.WorldRotation) * Matrix::CreateTranslation(info.WorldPosition);
		memcpy(&particleMain.StartSize, info.StartSize, sizeof(particleMain.StartSize));
		memcpy(&particleMain.StartColor1, info.StartColor[0], sizeof(particleMain.StartColor1));
		memcpy(&particleMain.StartColor2, info.StartColor[1], sizeof(particleMain.StartColor2));
		memcpy(&particleMain.StartLifeTime, info.StartLifeTime, sizeof(particleMain.StartLifeTime));
		memcpy(&particleMain.StartSpeed, info.StartSpeed, sizeof(particleMain.StartSpeed));
		memcpy(&particleMain.StartRotation, info.StartRotation, sizeof(particleMain.StartRotation));
		memcpy(&particleMain.GravityModifier, info.GravityModifier, sizeof(particleMain.GravityModifier));
		particleMain.StartLifeTimeOption = (int)info.StartLifeTimeOption;
		particleMain.StartSpeedOption = (int)info.StartSpeedOption;
		particleMain.StartSizeOption = (int)info.StartSizeOption;
		particleMain.StartRotationOption = (int)info.StartRotationOption;
		particleMain.StartColorOption = (int)info.StartColorOption;
		particleMain.GravityModifierOption = (int)info.GravityModifierOption;
		particleMain.RandomSeed = info.RandomSeed = rand();
		particleMain.DeltaTime = 0.001f;
		particleMain.SimulationSpeed = info.SimulationSpeed;
		particleMain.ParticleCount = particleSystems->GetParticleCount();
		mParticleMainCB->Update(mDevice, particleMain);

		ParticleShape particleShape;
		particleShape.ShapeType = (int)info.ShapeType;
		particleShape.ModeType = (int)info.ModeType;
		particleShape.Angle = info.Angle;
		particleShape.Radius = info.Radius;
		particleShape.DountRadius = info.DountRadius;
		particleShape.Arc = info.Arc;
		particleShape.Speed = info.Speed;
		particleShape.Spread = info.Spread;
		particleShape.RadiusThickness = info.RadiusThickness;
		mParticleShapeCB->Update(mDevice, particleShape);
	}
}