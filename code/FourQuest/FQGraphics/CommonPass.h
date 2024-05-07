#pragma once

#include <string>

#include "ConstantBufferStructure.h"
#include "RenderJob.h"

namespace fq::graphics
{
	class D3D11Device;

	class D3D11InputLayout;
	class D3D11VertexShader;
	class D3D11PixelShader;

	class D3D11RenderTargetView;
	class D3D11ShaderResourceView;
	class D3D11DepthStencilView;

	template <typename T>
	class D3D11ConstantBuffer;
	class D3D11VertexBuffer;
	class D3D11IndexBuffer;

	class D3D11ResourceManager;
	class D3D11CameraManager;
	class D3D11LightManager;
	class D3D11JobManager;

	class D3D11RasterizerState;
	class D3D11SamplerState;
	class D3D11DepthStencilState;
	class D3D11Texture;

	class RenderPass
	{
	public:
		virtual void Finalize() = 0;
		virtual void OnResize(unsigned short width, unsigned short height) = 0;
		virtual void Render() = 0;
	};

	class SkyBoxPass : public RenderPass
	{
	public:
		SkyBoxPass();

		void Initialize(std::shared_ptr<D3D11Device> device,
			std::shared_ptr<D3D11CameraManager> cameraManager,
			std::shared_ptr<D3D11ResourceManager> resourceManager);
		void Finalize() override;
		void OnResize(unsigned short width, unsigned short height) override {}
		void Render() override;

		void SetSkyBox(const std::wstring& path);

	private:
		bool isSetSkyBox;

		std::shared_ptr<D3D11Device> mDevice;
		std::shared_ptr<D3D11CameraManager> mCameraManager;
		std::shared_ptr<D3D11ResourceManager> mResourceManager;

		std::shared_ptr<D3D11VertexShader> mSkyBoxVS;
		std::shared_ptr<D3D11PixelShader> mSkyboxPS;
		std::shared_ptr<D3D11InputLayout> mSkyBoxLayout;

		std::shared_ptr<D3D11VertexBuffer> mSkyBoxVB;
		std::shared_ptr<D3D11IndexBuffer> mSkyboxIB;

		std::shared_ptr<D3D11RenderTargetView> mDrawRTV;
		std::shared_ptr<D3D11DepthStencilView> mDrawDSV;
		std::shared_ptr<D3D11Texture> mSkyBoxTexture;

		std::shared_ptr<D3D11RasterizerState> mCullFrontRS;
		// Todo: 딱 맞는 샘플러 스테이트가 있는지 고민해보기
		std::shared_ptr<D3D11SamplerState> mDefaultSS;
		std::shared_ptr<D3D11DepthStencilState> mLessEqualDepthStencilDS;

		std::shared_ptr<D3D11ConstantBuffer<ViewRotationProjectionMatrix>> mViewProjectionMatrix;
	};

	class ShadowPass : public RenderPass
	{
	public:
		void Initialize(std::shared_ptr<D3D11Device> device,
			std::shared_ptr<D3D11JobManager> jobManager,
			std::shared_ptr<D3D11CameraManager> cameraManager,
			std::shared_ptr<D3D11ResourceManager> resourceManager);
		void Finalize() override;
		void OnResize(unsigned short width, unsigned short height) override {}
		void Render() override;

	public:
		enum { SHADOW_SIZE = 2048 };

	private:
		std::shared_ptr<D3D11Device> mDevice;
		std::shared_ptr<D3D11JobManager> mJobManager;
		std::shared_ptr<D3D11CameraManager> mCameraManager;
		std::shared_ptr<D3D11ResourceManager> mResourceManager;

		D3D11_VIEWPORT mViewport;

		std::shared_ptr<D3D11DepthStencilView> mShadowDSV;

		std::shared_ptr<D3D11VertexShader> mStaticMeshVS;
		std::shared_ptr<D3D11VertexShader> mSkinnedMeshVS;
		std::shared_ptr<D3D11InputLayout> mStaticMeshLayout;
		std::shared_ptr<D3D11InputLayout> mSkinnedMeshLayout;

		std::shared_ptr<D3D11RasterizerState> mShadowRS;
		std::shared_ptr<D3D11RasterizerState> mDefaultRS;

		std::shared_ptr<D3D11ConstantBuffer<ModelTransfrom>> mModelTransformCB;
		std::shared_ptr<D3D11ConstantBuffer<SceneTrnasform>> mSceneTransformCB;
		std::shared_ptr<D3D11ConstantBuffer<BoneTransform>> mBoneTransformCB;
	};

	class FullScreenPass : public RenderPass
	{
	public:
		void Initialize(std::shared_ptr<D3D11Device> device,
			std::shared_ptr<D3D11ResourceManager> resourceManager,
			unsigned short width,
			unsigned short height);
		void Finalize() override;
		void OnResize(unsigned short width, unsigned short height) override;
		void Render() override;

	private:
		std::shared_ptr<D3D11Device> mDevice;
		std::shared_ptr<D3D11ResourceManager> mResourceManager;

		D3D11_VIEWPORT mViewport;

		// global sink
		std::shared_ptr<D3D11RenderTargetView> mSwapChainRTV;
		std::shared_ptr<D3D11RenderTargetView> mBackBufferRTV;
		std::shared_ptr<D3D11ShaderResourceView> mBackBufferSRV;
		std::shared_ptr<D3D11DepthStencilView> mNoneDSV;
		std::shared_ptr<D3D11DepthStencilView> mDSV;

		std::shared_ptr<D3D11InputLayout> mFullScreenLayout;
		std::shared_ptr<D3D11VertexShader> mFullScreenVS;
		std::shared_ptr<D3D11PixelShader> mFullScreenPS;
		std::shared_ptr<D3D11VertexBuffer> mFullScreenVB;
		std::shared_ptr<D3D11IndexBuffer> mFullScreenIB;
		std::shared_ptr<D3D11SamplerState> mPointClampSamplerState;
	};
}