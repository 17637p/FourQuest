#pragma once

#include "CommonHeader.h"
#include "ConstantBufferStructure.h"
#include "RenderJob.h"
#include "DeferredPipeline.h"
#include "ForwardPipeline.h"
#include "EPipelineType.h"

namespace fq::graphics
{
	template <typename T>
	class D3D11ConstantBuffer;
	class D3D11InputLayout;
	class D3D11VertexShader;
	class D3D11PixelShader;
	class D3D11Device;
	class D3D11SamplerState;
	class D3D11RenderTargetView;
	class D3D11ShaderResourceView;
	class D3D11DepthStencilView;
	class D3D11VertexBuffer;
	class D3D11IndexBuffer;
	class ForwardPipeline;
	class DeferredPipeline;
	class D3D11DebugDrawManager;
	class D3D11CameraManager;
	class D3D11LightManager;
	class D3D11JobManager;
	class D3D11DebugDrawManager;
	class D3D11ParticleManager;
	class D3D11LightProbeManager;

	class D3D11RenderManager
	{
	public:
		D3D11RenderManager();
		~D3D11RenderManager();

		void Initialize(std::shared_ptr<D3D11Device> device,
			std::shared_ptr<D3D11JobManager> jobManager,
			std::shared_ptr<D3D11CameraManager> cameraManager,
			std::shared_ptr< D3D11LightManager> lightManager,
			std::shared_ptr<D3D11ResourceManager> resourceManager,
			std::shared_ptr<D3D11DebugDrawManager> debugDrawManager,
			std::shared_ptr<D3D11ParticleManager> particleManager,
			std::shared_ptr<D3D11ObjectManager> objectManager,
			std::shared_ptr<D3D11LightProbeManager> lightProbeManager,
			unsigned short width,
			unsigned short height,
			EPipelineType pipelineType);
		void Finalize();

		void OnResize(unsigned short width, unsigned short height, unsigned short oriWidth, unsigned short oriHeight);

		void BeginRender();
		void Render();
		void RenderFullScreen();
		void EndRender();

		ID3D11ShaderResourceView* GetBackBufferSRV() const;

	private:
		EPipelineType mPipelineType;
		std::unique_ptr<ForwardPipeline> mForwardPipeline;
		std::unique_ptr<DeferredPipeline> mDeferredPipeline;
	};
}

