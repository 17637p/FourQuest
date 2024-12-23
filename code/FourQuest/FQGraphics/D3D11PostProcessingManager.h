#pragma once

#include <memory>
#include <d3d11.h>
#include <wrl.h>
#include <directxtk\SimpleMath.h>
#include "../FQCommon/FQCommonGraphics.h"

namespace fq::graphics
{
	class D3D11Device;
	class D3D11ResourceManager;
	class D3D11CameraManager;

	template <typename T>
	class D3D11ConstantBuffer;

	class D3D11PostProcessingManager
	{
	public:
		D3D11PostProcessingManager() = default;
		~D3D11PostProcessingManager() = default;

		void Initialize(std::shared_ptr<D3D11Device> device, std::shared_ptr<D3D11ResourceManager> resourceManager, std::shared_ptr<D3D11CameraManager> cameraManager, unsigned short width, unsigned short height);
		void OnResize(std::shared_ptr<D3D11Device> device, std::shared_ptr<D3D11ResourceManager> resourceManager, unsigned short width, unsigned short height);

		void Excute(std::shared_ptr<D3D11Device> device);

		void CopyOffscreenBuffer(std::shared_ptr<D3D11Device> device);
		void RenderFullScreen(std::shared_ptr<D3D11Device> device);

		void SetPostProcessingInfo(const PostProcessingInfo& postProcessingInfo);
		const PostProcessingInfo& GetPostProcessingInfo() const { return mPostProcessingInfo; }

	private:
		void excuteBloom(std::shared_ptr<D3D11Device> device);

	private:
		void swapPostProcessingBuffer()
		{
			std::swap(mRTVIndex, mSRVIndex);
		}

	private:
		struct PostProcessingBuffer
		{
			DirectX::SimpleMath::Color BloomColorTint = { 1, 1, 1, 1 };
			DirectX::SimpleMath::Color ShadowColor = { 0, 0, 0, 0 };
			DirectX::SimpleMath::Color HighlightColor = { 0, 0, 0, 0 };
			DirectX::SimpleMath::Color VignettColor = { 0, 0, 0, 0 };
			DirectX::SimpleMath::Color BlendColor = { 0, 0, 0, 0 };
			DirectX::SimpleMath::Color BlendTextureColor0 = { 1, 1, 1, 1 };
			DirectX::SimpleMath::Color BlendTextureColor1 = { 1, 1, 1, 1 };

			float Exposure = 1.f;
			float Contrast = 1.f;
			float Saturation = 1.f;
			float Gamma = 2.2f;

			float BloomIntensity = 1.f;
			float BloomThreshold = 1.7f;
			float Balance = 0.5f;
			float VignettRadius;

			float VignettSmoothness;
			int bUseColorAdjustment;
			int bUseBloom;
			int bUseSplitToning;

			int bUseVignett;
			int bUseToneMapping;
			int bUseHueVsSatCurve;
			int bUseFog;

			int bUseGrayScale;
			int bUseBlendColor;
			int bUseBlendTexture0;
			int bUseBlendTexture1;
		};

		struct BloomParams
		{
			float Threshold;
			float Scatter;
			int bUseScatter;
			float unused[1];
		} mBloomParams;

		struct Fog
		{
			DirectX::SimpleMath::Vector4 color;
			float nearPlane;
			float farPlane;
			float visibleArea;
			float unused[1]; // 아마도 함수 타입 들어갈듯 일단은 선형 함수!
		};

		struct SSRInfo
		{
			DirectX::SimpleMath::Matrix viewMat;
			DirectX::SimpleMath::Matrix projMat;
			DirectX::SimpleMath::Matrix invProjMat;
			int viewSizeX;
			int viewSizeY;
			float maxIteration;
			float maxThickness;
		};

	private:
		std::shared_ptr<D3D11Device> mDevice;
		std::shared_ptr<D3D11ResourceManager> mResourceManager;
		std::shared_ptr<D3D11CameraManager> mCameraManager;

		std::shared_ptr<class D3D11VertexBuffer> mFullScreenVB;
		std::shared_ptr<class D3D11IndexBuffer> mFullScreenIB;
		std::shared_ptr<class D3D11DepthStencilView> mNoneDSV;

		std::shared_ptr<class D3D11RenderTargetView> mPostProcessingRTV[2];
		std::shared_ptr<class D3D11ShaderResourceView> mPostProcessingSRV[2];
		std::shared_ptr<class D3D11ShaderResourceView> mOffscreenSRV;
		std::shared_ptr<class D3D11ShaderResourceView> mDSVSRV;
		std::shared_ptr<class D3D11RenderTargetView> mOffscreenRTV;
		std::shared_ptr<class D3D11RenderTargetView> mSwapChainRTV;
		unsigned int mRTVIndex = 0;
		unsigned int mSRVIndex = 1;

		std::shared_ptr<class D3D11SamplerState> mPointClampSS;
		std::shared_ptr<class D3D11SamplerState> mLinearClampSS;

		std::unique_ptr<class ShaderProgram> mFullScreenProgram;
		std::unique_ptr<class ShaderProgram> mPostProcessingProgram;
		std::unique_ptr<class ShaderProgram> mSSRProgram;
		PostProcessingInfo mPostProcessingInfo;
		std::shared_ptr<class D3D11Texture> mBlendTextureOrNull[PostProcessingInfo::BLEND_TEXTURE_SIZE];
		std::shared_ptr<D3D11ConstantBuffer<PostProcessingBuffer>> mPostProcessingCB;
		std::shared_ptr<D3D11ConstantBuffer<Fog>> mFogCB;

		// bloom
		// downScaledBuffer4
		enum { DOWN_SCALE_BUFFER_COUNT = 4 };
		std::shared_ptr<class D3D11ComputeShader> mBloomExtractBrightAreasCS;
		std::shared_ptr<class D3D11ComputeShader> mBloomDownSampleCS;
		std::shared_ptr<class D3D11ComputeShader> mBloomAccumulateCS;
		std::shared_ptr<class D3D11ComputeShader> mBlurHorzCS;
		std::shared_ptr<class D3D11ComputeShader> mBlurVertCS;
		std::shared_ptr<class D3D11ComputeShader> mSSRCS;
		std::shared_ptr<D3D11ConstantBuffer<BloomParams>> mBloomParamsCB;
		std::shared_ptr<class D3D11UnorderedAccessView> mExtractBrightUAV[2];
		std::shared_ptr<class D3D11ShaderResourceView> mExtractBrightSRV[2];
		std::shared_ptr<class D3D11UnorderedAccessView> mDownScaleUAVs[DOWN_SCALE_BUFFER_COUNT][2];
		std::shared_ptr<class D3D11ShaderResourceView> mDownScaleSRVs[DOWN_SCALE_BUFFER_COUNT][2];

		// SSR
		std::shared_ptr<class D3D11UnorderedAccessView> mSSRUAV;
		std::shared_ptr<class D3D11ShaderResourceView> mSSRSRV;
		std::shared_ptr<class D3D11ShaderResourceView> mNormalSSRSRV;
		std::shared_ptr<D3D11ConstantBuffer<SSRInfo>> mSSRCB;

		unsigned int mDownScaleBufferDenominators[DOWN_SCALE_BUFFER_COUNT] = { 4, 8, 16, 32 };
		unsigned int mDownScaleSRVIndex = 0;
		unsigned int mDownScaleUAVIndex = 1;
	};
}