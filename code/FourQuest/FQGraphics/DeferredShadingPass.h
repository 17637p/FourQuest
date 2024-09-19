#pragma once

#include <memory>
#include <d3d11.h>
#include "Pass.h"
#include "ConstantBufferStructure.h"

namespace fq::graphics
{
	class DeferredShadingPass : public Pass
	{
	public:
		void Initialize(std::shared_ptr<D3D11Device> device,
			std::shared_ptr<D3D11ResourceManager> resourceManager,
			std::shared_ptr<D3D11LightManager> lightManager,
			std::shared_ptr<D3D11CameraManager> cameraManager,
			unsigned short width,
			unsigned short height);
		void Finalize() override;
		void OnResize(unsigned short width, unsigned short height) override;
		void Render() override;

	private:
		std::shared_ptr<D3D11Device> mDevice;
		std::shared_ptr<D3D11ResourceManager> mResourceManager;
		std::shared_ptr<D3D11LightManager> mLightManager;
		std::shared_ptr<D3D11CameraManager> mCameraManager;

		D3D11_VIEWPORT mViewport;

		std::shared_ptr<D3D11ShaderResourceView> mShadowSRV;
		std::shared_ptr<D3D11DepthStencilView> mNullDSV;
		std::shared_ptr<D3D11RenderTargetView> mBackBufferRTV;

		std::shared_ptr<D3D11ShaderResourceView> mAlbedoSRV;
		std::shared_ptr<D3D11ShaderResourceView> mMetalnessRoughnessSRV;
		std::shared_ptr<D3D11ShaderResourceView> mNormalSRV;
		std::shared_ptr<D3D11ShaderResourceView> mEmissiveSRV;
		std::shared_ptr<D3D11ShaderResourceView> mPositionSRV;
		std::shared_ptr<D3D11ShaderResourceView> mPreCalculatedSRV;

		std::unique_ptr<class ShaderProgram> mShaderProgram;

		std::shared_ptr<D3D11SamplerState> mAnisotropicWrapSamplerState;
		std::shared_ptr<D3D11SamplerState> mLinearClampSamplerState;
		std::shared_ptr<D3D11SamplerState> mPointClampSamplerState;
		std::shared_ptr<D3D11SamplerState> mShadowSampler;

		std::shared_ptr<D3D11VertexBuffer> mFullScreenVB;
		std::shared_ptr<D3D11IndexBuffer> mFullScreenIB;
	};
}