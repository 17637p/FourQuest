#pragma once

#include <memory>

#include <directxtk/SimpleMath.h>
#include <d3d11.h>

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

	class D3D11RenderManager
	{
	public:
		struct ModelTransfrom
		{
			DirectX::SimpleMath::Matrix WorldMat;
			DirectX::SimpleMath::Matrix WorldInvTransposeMat;
		};

		struct SceneTrnasform
		{
			DirectX::SimpleMath::Matrix ViewProjMat;
			DirectX::SimpleMath::Matrix ShadowViewProjTexMat;
		};

		struct BoneTransform
		{
			enum { MAX_BOND_COUNT = 128 };
			DirectX::SimpleMath::Matrix FinalTransforms[128];
		};

		struct ModelTexutre
		{
			int bUseAlbedoMap;
			int bUseMetalnessMap;
			int bUseRoughnessMap;
			int bUseNormalMap;
			int bUseEmissiveMap;
			int bUseOpacityMap;
			int unused[2];
		};

		struct SceneLight
		{
			struct Light
			{
				DirectX::SimpleMath::Vector4 Direction;
				DirectX::SimpleMath::Vector4 Intensity;
			} Lights[3];

			DirectX::SimpleMath::Vector4 EyePosition;
			int bUseIBL;
			int unused[3];
		};

	public:
		D3D11RenderManager() = default;
		~D3D11RenderManager() = default;

		void Initialize(const std::shared_ptr<D3D11Device>& device,
			std::shared_ptr<class D3D11ResourceManager>& resourceManager,
			unsigned short width,
			unsigned short height);
		void Finalize();

		void BeginRender(const std::shared_ptr<D3D11Device>& device);
		void EndRender(const std::shared_ptr<D3D11Device>& device);

		void OnResize(const std::shared_ptr<D3D11Device>& device,
			std::shared_ptr<class D3D11ResourceManager>& resourceManager,
			unsigned short width,
			unsigned short height);

		void Render(const std::shared_ptr<D3D11Device>& device, const std::vector<StaticMeshJob>& staticMeshJobs);
		void Render(const std::shared_ptr<D3D11Device>& device, const std::vector<SkinnedMeshJob>& skinnedMeshJobs);
		void RenderBackBuffer(const std::shared_ptr<D3D11Device>& device);

		ID3D11ShaderResourceView* GetBackBufferSRV() const;

	private:
		void updateModelTransformCB(const std::shared_ptr<D3D11Device>& device, const DirectX::SimpleMath::Matrix& transform);
		void updateModelTextureCB(const std::shared_ptr<D3D11Device>& device, const std::shared_ptr<Material>& material);
		void updateBoneTransformCB(const std::shared_ptr<D3D11Device>& device, const std::vector<DirectX::SimpleMath::Matrix>& finalTransforms);

		void createFullScreenBuffer(const std::shared_ptr<D3D11Device>& device);

	private:
		D3D11_VIEWPORT mViewport;
		std::shared_ptr<D3D11RenderTargetView> mBackBufferRTV;
		std::shared_ptr<D3D11RenderTargetView> mSwapChainRTV;

		std::shared_ptr<D3D11ShaderResourceView> mBackBufferSRV;

		std::shared_ptr<D3D11DepthStencilView> mDSV;
		std::shared_ptr<D3D11DepthStencilView> mNullDSV;

		std::shared_ptr<D3D11InputLayout> mStaticMeshLayout;
		std::shared_ptr<D3D11InputLayout> mSkinnedMeshLayout;
		std::shared_ptr<D3D11InputLayout> mFullScreenLayout;

		std::shared_ptr<D3D11VertexShader> mStaticMeshVS;
		std::shared_ptr<D3D11VertexShader> mSkinnedMeshVS;
		std::shared_ptr<D3D11VertexShader> mFullScreenVS;

		std::shared_ptr<D3D11PixelShader> mMeshPS;
		std::shared_ptr<D3D11PixelShader> mFullScreenPS;

		std::shared_ptr<D3D11SamplerState> mAnisotropicWrapSamplerState;
		std::shared_ptr<D3D11SamplerState> mLinearClampSamplerState;
		std::shared_ptr<D3D11SamplerState> mPointClampSamplerState;

		std::shared_ptr<D3D11ConstantBuffer<ModelTransfrom>> mModelTransformCB;
		std::shared_ptr<D3D11ConstantBuffer<SceneTrnasform>> mSceneTransformCB;
		std::shared_ptr<D3D11ConstantBuffer<BoneTransform>> mBoneTransformCB;
		std::shared_ptr<D3D11ConstantBuffer<ModelTexutre>> mModelTexutreCB;
		std::shared_ptr<D3D11ConstantBuffer<SceneLight>> mSceneLightCB;

		std::shared_ptr<D3D11VertexBuffer> mFullScreenVB;
		std::shared_ptr<D3D11IndexBuffer> mFullScreenIB;
	};
}

