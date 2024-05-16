#include "ParticlePass.h"
#include "ManagementCommon.h"
#include "D3D11Common.h"

namespace fq::graphics
{
	void ParticlePass::Initialize(std::shared_ptr<D3D11Device> device,
		std::shared_ptr<D3D11ParticleManager> paricleManager,
		std::shared_ptr<D3D11CameraManager> cameraManager,
		std::shared_ptr<D3D11ResourceManager> resourceManager,
		std::shared_ptr< D3D11LightManager> lightManager)
	{
		Finalize();

		mDevice = device;
		mParticleManager = paricleManager;
		mCameraManager = cameraManager;
		mLightManager = lightManager;
		mResourceManager = resourceManager;

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
		auto nonePipelieState = std::make_shared<PipelineState>(nullptr, nullptr, nullptr);
		auto renderPipelieState = std::make_shared<PipelineState>(nullptr, nullptr, additiveBlend);
		mParticleGenerateProgram = std::make_unique<ShaderProgram>(mDevice, nonePipelieState, nullptr, nullptr, nullptr, generateCS);
		mParticleUpdateProgram = std::make_unique<ShaderProgram>(mDevice, nonePipelieState, nullptr, nullptr, nullptr, updateCS);
		mParticleRenderProgram = std::make_unique<ShaderProgram>(mDevice, renderPipelieState, renderVS, renderPS, renderGS, nullptr);

		mEmitterCB = std::make_shared<D3D11ConstantBuffer<Emitter>>(mDevice, ED3D11ConstantBuffer::Transform);

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
			ID3D11ShaderResourceView* SRVs[10] = { NULL };
			mDevice->GetDeviceContext()->PSSetShaderResources(0, 10, SRVs);
		}

		std::map<size_t, std::shared_ptr<ParticleEmitter>> emitters = mParticleManager->GetParticleEmitters();

		// 생성
		{
			mParticleGenerateProgram->Bind(mDevice);
			ID3D11Buffer* buffers[10] = { NULL };
			mDevice->GetDeviceContext()->CSSetConstantBuffers(0, 10, buffers);
			mEmitterCB->Bind(mDevice, ED3D11ShaderType::ComputeShader, 0);
			ID3D11UnorderedAccessView* pNullUAVs[2] = { NULL, };
			mDevice->GetDeviceContext()->CSSetUnorderedAccessViews(0, 2, pNullUAVs, NULL);

			for (const auto& [key, emitter] : emitters)
			{
				emitter->GetAppendUAV()->Bind(mDevice, 0);
				// 생성 개수만큼 dispatch
				// 현재 개수를 트레킹할 수 있는 거아닌가 그러면

				if (emitter->GetInfo().MaxParticleCount > emitter->GetParticleCount())
				{
					Emitter emitterInfo;
					emitterInfo.InitPositionW = DirectX::SimpleMath::Vector4(emitter->GetInfo().InitPosition);
					emitterInfo.InitSizeW = DirectX::SimpleMath::Vector4(emitter->GetInfo().InitSize);
					emitterInfo.InitVelocityW = DirectX::SimpleMath::Vector4(emitter->GetInfo().InitVelocity);
					emitterInfo.InitLifeTime = emitter->GetInfo().InitTimeToLive;
					emitterInfo.ReadomPositionW = DirectX::SimpleMath::Vector4(emitter->GetInfo().RandomRangePosition);
					emitterInfo.ReadomSizeW = DirectX::SimpleMath::Vector4(emitter->GetInfo().RandomSize);
					emitterInfo.ReadomVelocityW = DirectX::SimpleMath::Vector4(emitter->GetInfo().RandomRangeVelocity);
					emitterInfo.ReadomLifeTime = (emitter->GetInfo().RandomRangeTimeToLive);
					emitterInfo.ParticleCount = (emitter->GetParticleCount());
					emitterInfo.DeltaTime = 1.f / 60.f;
					mEmitterCB->Update(mDevice, emitterInfo);

					size_t emissionCount = 1;
					mDevice->GetDeviceContext()->Dispatch(1, 1, 1);
					emitter->SetParticleCount(emitter->GetParticleCount() + emissionCount);

					{	
						auto buffer = emitter->GetAppendBuffer()->GetBuffer();
						auto debugBuffer = emitter->GetAppendBuffer()->GetDebugBuffer();

						mDevice->GetDeviceContext()->CopyResource(debugBuffer.Get(), buffer.Get());

						D3D11_MAPPED_SUBRESOURCE mappedData;
						mDevice->GetDeviceContext()->Map(debugBuffer.Get(), 0, D3D11_MAP_READ, 0, &mappedData);

						Particle* particle = reinterpret_cast<Particle*>(mappedData.pData);

						auto consumeBuffer = emitter->GetConsumeBuffer()->GetBuffer();
						auto consumeDebugBuffer = emitter->GetConsumeBuffer()->GetDebugBuffer();

						mDevice->GetDeviceContext()->CopyResource(consumeDebugBuffer.Get(), consumeBuffer.Get());

						D3D11_MAPPED_SUBRESOURCE consumeMappedData;
						mDevice->GetDeviceContext()->Map(consumeDebugBuffer.Get(), 0, D3D11_MAP_READ, 0, &consumeMappedData);

						Particle* comsumeParticle = reinterpret_cast<Particle*>(consumeMappedData.pData);

						mDevice->GetDeviceContext()->Unmap(debugBuffer.Get(), 0);
						mDevice->GetDeviceContext()->Unmap(consumeDebugBuffer.Get(), 0);
					}
				}

				emitter->Swap();
				emitter->GetAppendUAV()->Clear(mDevice);
			}
		}

		// 갱신
		{
			mParticleUpdateProgram->Bind(mDevice);
			ID3D11Buffer* buffers[10] = { NULL };
			mDevice->GetDeviceContext()->CSSetConstantBuffers(0, 10, buffers);

			ID3D11UnorderedAccessView* pNullUAVs[2] = { NULL, };
			mDevice->GetDeviceContext()->CSSetUnorderedAccessViews(0, 2, pNullUAVs, NULL);

			mEmitterCB->Bind(mDevice, ED3D11ShaderType::ComputeShader, 0);

			for (const auto& [key, emitter] : emitters)
			{
				emitter->GetAppendUAV()->Bind(mDevice, 0);
				emitter->GetConsumeUAV()->Bind(mDevice, 1);

				Emitter emitterInfo;
				emitterInfo.InitPositionW = DirectX::SimpleMath::Vector4(emitter->GetInfo().InitPosition);
				emitterInfo.InitSizeW = DirectX::SimpleMath::Vector4(emitter->GetInfo().InitSize);
				emitterInfo.InitVelocityW = DirectX::SimpleMath::Vector4(emitter->GetInfo().InitVelocity);
				emitterInfo.InitLifeTime = emitter->GetInfo().InitTimeToLive;
				emitterInfo.ReadomPositionW = DirectX::SimpleMath::Vector4(emitter->GetInfo().RandomRangePosition);
				emitterInfo.ReadomSizeW = DirectX::SimpleMath::Vector4(emitter->GetInfo().RandomSize);
				emitterInfo.ReadomVelocityW = DirectX::SimpleMath::Vector4(emitter->GetInfo().RandomRangeVelocity);
				emitterInfo.ReadomLifeTime = (emitter->GetInfo().RandomRangeTimeToLive);
				emitterInfo.ParticleCount = (emitter->GetParticleCount());
				emitterInfo.DeltaTime = 1.f / 60.f;
				mEmitterCB->Update(mDevice, emitterInfo);

				if (emitter->GetParticleCount() > 0)
				{
					size_t dispatchX = (emitter->GetParticleCount() - 1) / 512 + 1;
					//mDevice->GetDeviceContext()->Dispatch(dispatchX, 0, 0);
					mDevice->GetDeviceContext()->Dispatch(emitter->GetParticleCount(), 1, 1);

					{
						auto buffer = emitter->GetAppendBuffer()->GetBuffer();
						auto debugBuffer = emitter->GetAppendBuffer()->GetDebugBuffer();

						mDevice->GetDeviceContext()->CopyResource(debugBuffer.Get(), buffer.Get());

						D3D11_MAPPED_SUBRESOURCE mappedData;
						mDevice->GetDeviceContext()->Map(debugBuffer.Get(), 0, D3D11_MAP_READ, 0, &mappedData);

						Particle* particle = reinterpret_cast<Particle*>(mappedData.pData);

						auto consumeBuffer = emitter->GetConsumeBuffer()->GetBuffer();
						auto consumeDebugBuffer = emitter->GetConsumeBuffer()->GetDebugBuffer();

						mDevice->GetDeviceContext()->CopyResource(consumeDebugBuffer.Get(), consumeBuffer.Get());

						D3D11_MAPPED_SUBRESOURCE consumeMappedData;
						mDevice->GetDeviceContext()->Map(consumeDebugBuffer.Get(), 0, D3D11_MAP_READ, 0, &consumeMappedData);

						Particle* comsumeParticle = reinterpret_cast<Particle*>(consumeMappedData.pData);
						mDevice->GetDeviceContext()->Unmap(debugBuffer.Get(), 0);
						mDevice->GetDeviceContext()->Unmap(consumeDebugBuffer.Get(), 0);
					}
				}

				emitter->Swap();
			}

			// 랜더링
			{
				mParticleUpdateProgram->Bind(mDevice);
				ID3D11Buffer* buffers[10] = { NULL };
				mDevice->GetDeviceContext()->CSSetConstantBuffers(0, 10, buffers);
				ID3D11UnorderedAccessView* pNullUAVs[2] = { NULL, };
				mDevice->GetDeviceContext()->CSSetUnorderedAccessViews(0, 2, pNullUAVs, NULL);

				mBackBufferRTV->Bind(mDevice, mDSV);
				mParticleRenderProgram->Bind(mDevice);
				mDevice->GetDeviceContext()->RSSetViewports(1, &mViewport);
				mDevice->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
				// 널에 해당하는 인풋어셈블러를 바인딩시켜줘야 되는 거 같은데;

				for (const auto& [key, emitter] : emitters)
				{
					emitter->Swap();
					emitter->GetConsumeSRV()->Bind(mDevice, 0, ED3D11ShaderType::VertexShader);

					mDevice->GetDeviceContext()->Draw(emitter->GetParticleCount(), 0);
				}

				ID3D11ShaderResourceView* SRVs[10] = { NULL };
				mDevice->GetDeviceContext()->VSSetShaderResources(0, 10, SRVs);
			}
		}
	}
}