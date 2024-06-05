#pragma once

#include <memory>
#include <string>

#include "Pass.h"
#include "ConstantBufferStructure.h"

namespace fq::graphics
{
	class SSAOPass : public Pass
	{
	public:
		void Initialize(std::shared_ptr<D3D11Device> device,
			std::shared_ptr<D3D11JobManager> jobManager,
			std::shared_ptr<D3D11CameraManager> cameraManager,
			std::shared_ptr<D3D11ResourceManager> resourceManager,
			unsigned short width,
			unsigned short height);
		void Finalize() override;
		void OnResize(unsigned short width, unsigned short height) override;
		void Render() override;

	private:
		void BuildFrustumFarCorners(float fovy, float farZ);

	private:
		unsigned short mWidth;
		unsigned short mHeight;

		std::shared_ptr<D3D11Device> mDevice;
		std::shared_ptr<D3D11JobManager> mJobManager;
		std::shared_ptr<D3D11CameraManager> mCameraManager;
		std::shared_ptr<D3D11ResourceManager> mResourceManager;

		std::shared_ptr<D3D11ShaderResourceView> mSSAODepthSRV;
		std::shared_ptr<D3D11RenderTargetView> mSSAORTV;
		std::shared_ptr<D3D11DepthStencilView> mNoneDSV;

		std::unique_ptr<class ShaderProgram> mSSAOPassShaderProgram;

		std::shared_ptr<D3D11VertexBuffer> mFullScreenVB;
		std::shared_ptr<D3D11IndexBuffer> mFullScreenIB;

		FrustumCorners mFrustumCorner;
		std::shared_ptr<D3D11ConstantBuffer<FrustumCorners>> mFrustumCornersCB;
	};
}

