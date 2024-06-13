#pragma once

#include <memory>
#include <directxtk\SimpleMath.h>
#include "Pass.h"
#include "D3D11Common.h"

namespace fq::graphics
{
	class D3D11DecalManager;

	namespace decal
	{
		struct PerFrame
		{
			DirectX::SimpleMath::Matrix TexTransform;
			DirectX::SimpleMath::Matrix World;
			DirectX::SimpleMath::Matrix View;
			DirectX::SimpleMath::Matrix Proj;
			DirectX::SimpleMath::Matrix InvWV;

			DirectX::SimpleMath::Vector2 Deproject;
			float NormalThresholdInRadian;
			float AlphaClipThreshold;
			
			int bUseMultiplyAlpha;
			int bUseAlphaClip;
			int bUseAlbedoMap;
			int bUseMetalnessMap;

			int bUseRoughnessMap;
			int bUseNormalMap;
			int bUseEmissiveMap;
			float unused[1];
		};
	}

	class DeferredDecalPass : public Pass
	{
	public:
		void Initialize(std::shared_ptr<D3D11Device> device,
			std::shared_ptr<D3D11ResourceManager> resourceManager,
			std::shared_ptr<D3D11CameraManager> cameraManager,
			std::shared_ptr<D3D11DecalManager> decalManager,
			std::shared_ptr<D3D11DebugDrawManager> debugDrawManager,
			unsigned short width,
			unsigned short height);
		void Finalize() override;
		void OnResize(unsigned short width, unsigned short height) override;
		void Render() override;

	private:
		void InitBlendState();

	private:
		std::shared_ptr<D3D11Device> mDevice;
		std::shared_ptr<D3D11ResourceManager> mResourceManager;
		std::shared_ptr<D3D11CameraManager> mCameraManager;
		std::shared_ptr<D3D11DecalManager> mDecalManager;
		std::shared_ptr<D3D11DebugDrawManager> mDebugDrawManager;

		D3D11_VIEWPORT mViewport;

		std::shared_ptr<D3D11RenderTargetView> mAlbedoRTV;
		std::shared_ptr<D3D11RenderTargetView> mMetalnessRTV;
		std::shared_ptr<D3D11RenderTargetView> mRoughnessRTV;
		std::shared_ptr<D3D11RenderTargetView> mNormalRTV;
		std::shared_ptr<D3D11RenderTargetView> mEmissiveRTV;

		std::shared_ptr<class D3D11DepthStencilView> mDefualtDSV;

		std::shared_ptr<D3D11ShaderResourceView> mPositionWSRV;
		std::shared_ptr<D3D11ShaderResourceView> mNormalSRV;
		std::shared_ptr<D3D11ShaderResourceView> mSourceNormalSRV;
		std::shared_ptr<D3D11ShaderResourceView> mSourceTangentSRV;

		std::shared_ptr<D3D11DepthStencilState> mStencilComparisionEqual;
		std::shared_ptr<ShaderProgram> mDecalProgram;
		std::shared_ptr<D3D11SamplerState> mAnisotropicWrapSamplerState;
		std::shared_ptr<D3D11SamplerState> mLinearClampSamplerState;
		std::shared_ptr<D3D11SamplerState> mPointClampSamplerState;

		std::shared_ptr<D3D11ConstantBuffer<decal::PerFrame>> mPerFrameCB;

		std::shared_ptr<D3D11VertexBuffer> mBoxVB;
		std::shared_ptr<D3D11IndexBuffer> mBoxIB;

		Microsoft::WRL::ComPtr<ID3D11BlendState> mBlendStates[32];
	};
}
