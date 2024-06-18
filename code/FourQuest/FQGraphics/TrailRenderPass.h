#pragma once

#include <d3d11.h>
#include <memory>
#include "Pass.h"
#include "D3D11Common.h"

namespace fq::graphics
{
	class D3D11TrailManager;


	class TrailRenderPass : public Pass
	{
	public:
		struct PerFrame
		{
			DirectX::SimpleMath::Matrix ViewProj;
		};

	public:
		void Initialize(std::shared_ptr<D3D11Device> device,
			std::shared_ptr<D3D11TrailManager> trailManager,
			std::shared_ptr<D3D11CameraManager> cameraManager,
			std::shared_ptr<D3D11ResourceManager> resourceManager,
			std::shared_ptr< D3D11LightManager> lightManager,
			unsigned short width,
			unsigned short height);
		void Finalize() override;
		void OnResize(unsigned short width, unsigned short height) override;
		void Render() override;

	private:
		std::shared_ptr<D3D11Device> mDevice;
		std::shared_ptr<D3D11ResourceManager> mResourceManager;
		std::shared_ptr<D3D11CameraManager> mCameraManager;
		std::shared_ptr<D3D11LightManager> mLightManager;
		std::shared_ptr<D3D11TrailManager> mTrailManager;

		std::shared_ptr<D3D11RenderTargetView> mBackBufferRTV;
		std::shared_ptr<D3D11DepthStencilView> mDSV;

		D3D11_VIEWPORT mViewport;

		std::shared_ptr<ShaderProgram> mShaderProgram;
		std::shared_ptr<D3D11SamplerState> mLinearWrapSS;

		std::shared_ptr<D3D11VertexBuffer> mDynamicVB;

		std::shared_ptr<D3D11ConstantBuffer<PerFrame>> mPerFrameCB;
	};
}

