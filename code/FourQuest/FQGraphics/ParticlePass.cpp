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

		auto generateCS = std::make_shared<D3D11VertexShader>(device, L"./resource/internal/shader/FullScreenVS.hlsl");
		auto updateCS = std::make_shared<D3D11VertexShader>(device, L"./resource/internal/shader/FullScreenVS.hlsl");
		auto renderVS = std::make_shared<D3D11VertexShader>(device, L"./resource/internal/shader/FullScreenVS.hlsl");
		auto renderPS = std::make_shared<D3D11PixelShader>(device, L"./resource/internal/shader/FullScreenPS.hlsl");
		auto renderGS = std::make_shared<D3D11PixelShader>(device, L"./resource/internal/shader/FullScreenPS.hlsl");
		auto nonePipelieState = std::make_shared<PipelineState>(nullptr, nullptr, nullptr);
		auto renderPipelieState = std::make_shared<PipelineState>(nullptr, nullptr, nullptr);
		mParticleGenerateProgram = std::make_unique<ShaderProgram>(mDevice, nonePipelieState, nullptr, nullptr, nullptr, generateCS);
		mParticleUpdateProgram = std::make_unique<ShaderProgram>(mDevice, nonePipelieState, nullptr, nullptr, nullptr, updateCS);
		mParticleRenderProgram = std::make_unique<ShaderProgram>(mDevice, renderPipelieState, renderVS, renderPS, renderGS, nullptr);

		// ���� �ڿ� ȹ��
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
		// �ʱ�ȭ
		{
			ID3D11ShaderResourceView* SRVs[10] = { NULL };
			mDevice->GetDeviceContext()->PSSetShaderResources(0, 10, SRVs);
		}

		// ����
		{
			mParticleGenerateProgram->Bind(mDevice);
		}

		// ����
		{
			mParticleUpdateProgram->Bind(mDevice);
		}

		// ������
		{
			mBackBufferRTV->Bind(mDevice, mDSV);
			mParticleRenderProgram->Bind(mDevice);
			mDevice->GetDeviceContext()->RSSetViewports(1, &mViewport);
			mDevice->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

			const std::map<size_t, ParticleEmitter>& emitters = mParticleManager->GetParticleEmitters();

			for (const auto& [id, emitter] : emitters)
			{
				const std::vector<Particle*>& particles = emitter.GetParticles();

				// particles �����ͷ� �ν��Ͻ� ������ ä���

				// ������ŭ ������
				mDevice->GetDeviceContext()->DrawInstanced();
			}
		}
	}
}