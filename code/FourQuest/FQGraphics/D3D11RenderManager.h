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
	class D3D11CameraManager;
	class D3D11LightManager;

	class D3D11RenderManager
	{
	public:
		D3D11RenderManager();
		~D3D11RenderManager() = default;

		void Initialize(const std::shared_ptr<D3D11Device>& device,
			std::shared_ptr<class D3D11ResourceManager>& resourceManager,
			unsigned short width,
			unsigned short height,
			EPipelineType pipelineType);
		void Finalize();

		void OnResize(const std::shared_ptr<D3D11Device>& device,
			std::shared_ptr<class D3D11ResourceManager>& resourceManager,
			unsigned short width,
			unsigned short height);

		void BeginRender(const std::shared_ptr<D3D11Device>& device,
			const std::shared_ptr<D3D11CameraManager>& cameraManager,
			const std::shared_ptr< D3D11LightManager>& lightManager);
		void EndRender(const std::shared_ptr<D3D11Device>& device);

		void Render(const std::shared_ptr<D3D11Device>& device, const std::vector<StaticMeshJob>& staticMeshJobs);
		void Render(const std::shared_ptr<D3D11Device>& device, const std::vector<SkinnedMeshJob>& skinnedMeshJobs);
		void Shading(const std::shared_ptr<D3D11Device>& device);
		void RenderBackBuffer(const std::shared_ptr<D3D11Device>& device);

		ID3D11ShaderResourceView* GetBackBufferSRV() const;

	private:
		void createFullScreenBuffer(const std::shared_ptr<D3D11Device>& device);

	private:
		EPipelineType mPipelineType;
		std::unique_ptr<ForwardPipeline> mForwardPipeline;
		std::unique_ptr<DeferredPipeline> mDeferredPipeline;

		std::shared_ptr<D3D11RenderTargetView> mSwapChainRTV;
		std::shared_ptr<D3D11DepthStencilView> mNullDSV;

		std::shared_ptr<D3D11InputLayout> mFullScreenLayout;
		std::shared_ptr<D3D11VertexShader> mFullScreenVS;
		std::shared_ptr<D3D11PixelShader> mFullScreenPS;
		std::shared_ptr<D3D11VertexBuffer> mFullScreenVB;
		std::shared_ptr<D3D11IndexBuffer> mFullScreenIB;
		std::shared_ptr<D3D11SamplerState> mPointClampSamplerState;
	};
}

