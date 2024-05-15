#pragma once

#include <d3d11.h>
#include <memory>
#include "Pass.h"

namespace fq::graphics
{
	class ShaderProgram;

	class ParticlePass : public Pass
	{
	public:
		void Initialize(std::shared_ptr<D3D11Device> device,
			std::shared_ptr<D3D11ParticleManager> paricleManager,
			std::shared_ptr<D3D11CameraManager> cameraManager,
			std::shared_ptr<D3D11ResourceManager> resourceManager,
			std::shared_ptr< D3D11LightManager> lightManager);
		void Finalize() override;
		void OnResize(unsigned short width, unsigned short height);
		void Render() override;

	private:
		std::shared_ptr<D3D11Device> mDevice;
		std::shared_ptr<D3D11ResourceManager> mResourceManager;
		std::shared_ptr<D3D11CameraManager> mCameraManager;
		std::shared_ptr< D3D11LightManager> mLightManager;
		std::shared_ptr<D3D11ParticleManager> mParticleManager;

		D3D11_VIEWPORT mViewport;

		std::shared_ptr<D3D11RenderTargetView> mBackBufferRTV;
		std::shared_ptr<D3D11DepthStencilView> mDSV;

		// 기본은 증감 혼합
		std::unique_ptr<ShaderProgram> mParticleGenerateProgram;
		std::unique_ptr<ShaderProgram> mParticleUpdateProgram;
		std::unique_ptr<ShaderProgram> mParticleRenderProgram;
	};
}

