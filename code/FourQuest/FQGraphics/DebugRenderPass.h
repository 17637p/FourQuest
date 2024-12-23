#pragma once
#pragma once

#include <memory>
#include <d3d11.h>

#include "Pass.h"

namespace fq::graphics
{
	class DebugRenderPass : public Pass
	{
	public:
		void Initialize(std::shared_ptr<D3D11Device> device,
			std::shared_ptr<D3D11JobManager> jobManager,
			std::shared_ptr<D3D11ObjectManager> objectManager,
			std::shared_ptr<D3D11DebugDrawManager> dbugDrawManager,
			std::shared_ptr<D3D11CameraManager> cameraManager,
			std::shared_ptr<D3D11ResourceManager> resourceManager,
			std::shared_ptr<D3D11ParticleManager> mParticleManager,
			unsigned short width,
			unsigned short height);
		void Finalize() override;
		void OnResize(unsigned short width, unsigned short height) override;
		void Render() override;

	private:
		std::shared_ptr<D3D11Device> mDevice;
		std::shared_ptr<D3D11JobManager> mJobManager;
		std::shared_ptr<D3D11ObjectManager> mObjectManager;
		std::shared_ptr<D3D11DebugDrawManager> mDebugDrawManager;
		std::shared_ptr<D3D11CameraManager> mCameraManager;
		std::shared_ptr<D3D11ResourceManager> mResourceManager;
		std::shared_ptr<D3D11ParticleManager> mParticleManager;

		D3D11_VIEWPORT mViewport;

		std::shared_ptr<D3D11RenderTargetView> mBackBufferRTV;
		std::shared_ptr<D3D11DepthStencilView> mDefaultDSV;
	};
}