#pragma once

#include "CommonHeader.h"
#include "ConstantBufferStructure.h"
#include "RenderJob.h"

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

	class DeferredPipeline
	{
	public:
		void Initialize(const std::shared_ptr<D3D11Device>& device,
			std::shared_ptr<class D3D11ResourceManager>& resourceManager,
			unsigned short width,
			unsigned short height);
		void Finalize();

		void OnResize(const std::shared_ptr<D3D11Device>& device,
			std::shared_ptr<class D3D11ResourceManager>& resourceManager,
			unsigned short width,
			unsigned short height);

		void BeginRender(const std::shared_ptr<D3D11Device>& device, const std::shared_ptr<class D3D11CameraManager>& cameraManager);
		void EndRender(const std::shared_ptr<D3D11Device>& device);

		void Render(const std::shared_ptr<D3D11Device>& device, const std::vector<StaticMeshJob>& staticMeshJobs);
		void Render(const std::shared_ptr<D3D11Device>& device, const std::vector<SkinnedMeshJob>& skinnedMeshJobs);
		void Shading(const std::shared_ptr<D3D11Device>& device);

		std::shared_ptr<D3D11ShaderResourceView>& GetBackBufferSRV();

		void SetFullScreenBindable(std::shared_ptr<D3D11InputLayout> fullScreenLayout,
			std::shared_ptr<D3D11VertexShader> fullScreenVS,
			std::shared_ptr<D3D11VertexBuffer> fullScreenVB,
			std::shared_ptr<D3D11IndexBuffer> fullScreenIB)
		{
			mFullScreenLayout = fullScreenLayout;
			mFullScreenVS = fullScreenVS;
			mFullScreenVB = fullScreenVB;
			mFullScreenIB = fullScreenIB;
		}

	private:
		D3D11_VIEWPORT mViewport;

		std::shared_ptr<D3D11RenderTargetView> mBackBufferRTV;
		std::shared_ptr<D3D11ShaderResourceView> mBackBufferSRV;
		std::shared_ptr<D3D11DepthStencilView> mDSV;

		// G-Buffer
		std::shared_ptr<D3D11RenderTargetView> mAlbedoRTV;
		std::shared_ptr<D3D11RenderTargetView> mMetalnessRTV;
		std::shared_ptr<D3D11RenderTargetView> mRoughnessRTV;
		std::shared_ptr<D3D11RenderTargetView> mNormalRTV;
		std::shared_ptr<D3D11RenderTargetView> mEmissiveRTV;
		std::shared_ptr<D3D11RenderTargetView> mPositionRTV;

		std::shared_ptr<D3D11ShaderResourceView> mAlbedoSRV;
		std::shared_ptr<D3D11ShaderResourceView> mMetalnessSRV;
		std::shared_ptr<D3D11ShaderResourceView> mRoughnessSRV;
		std::shared_ptr<D3D11ShaderResourceView> mNormalSRV;
		std::shared_ptr<D3D11ShaderResourceView> mEmissiveSRV;
		std::shared_ptr<D3D11ShaderResourceView> mPositionSRV;

		std::shared_ptr<D3D11InputLayout> mStaticMeshLayout;
		std::shared_ptr<D3D11InputLayout> mSkinnedMeshLayout;

		std::shared_ptr<D3D11VertexShader> mStaticMeshVS;
		std::shared_ptr<D3D11VertexShader> mSkinnedMeshVS;

		std::shared_ptr<D3D11PixelShader> mGeometryPS;
		std::shared_ptr<D3D11PixelShader> mShadingPS;

		std::shared_ptr<D3D11SamplerState> mAnisotropicWrapSamplerState;
		std::shared_ptr<D3D11SamplerState> mLinearClampSamplerState;
		std::shared_ptr<D3D11SamplerState> mPointClampSamplerState;

		std::shared_ptr<D3D11ConstantBuffer<ModelTransfrom>> mModelTransformCB;
		std::shared_ptr<D3D11ConstantBuffer<SceneTrnasform>> mSceneTransformCB;
		std::shared_ptr<D3D11ConstantBuffer<BoneTransform>> mBoneTransformCB;
		std::shared_ptr<D3D11ConstantBuffer<ModelTexutre>> mModelTexutreCB;
		std::shared_ptr<D3D11ConstantBuffer<SceneLight>> mSceneLightCB;

		std::shared_ptr<D3D11InputLayout> mFullScreenLayout;
		std::shared_ptr<D3D11VertexShader> mFullScreenVS;
		std::shared_ptr<D3D11VertexBuffer> mFullScreenVB;
		std::shared_ptr<D3D11IndexBuffer> mFullScreenIB;
	};
}

