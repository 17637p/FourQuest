#include <d3d11.h>
#include "D3D11PostProcessingManager.h"
#include "ManagementCommon.h"
#include "D3D11Common.h"
#include "D3D11Buffer.h"
#include "D3D11Util.h"

namespace fq::graphics
{
	void D3D11PostProcessingManager::Initialize(std::shared_ptr<D3D11Device> device, std::shared_ptr<D3D11ResourceManager> resourceManager,
		std::shared_ptr<D3D11CameraManager> cameraManager,
		unsigned short width, unsigned short height)
	{
		mDevice = device;
		mResourceManager = resourceManager;
		mCameraManager = cameraManager;

		mFullScreenVB = std::make_shared<D3D11VertexBuffer>(D3D11VertexBuffer::CreateFullScreenVertexBuffer(device));
		mFullScreenIB = std::make_shared<D3D11IndexBuffer>(D3D11IndexBuffer::CreateFullScreenIndexBuffer(device));
		mNoneDSV = resourceManager->Get<D3D11DepthStencilView>(ED3D11DepthStencilViewType::None);

		auto defaultDSV = resourceManager->Get<D3D11DepthStencilView>(ED3D11DepthStencilViewType::Default);
		mDSVSRV = std::make_shared<D3D11ShaderResourceView>(device, defaultDSV, DXGI_FORMAT_R24_UNORM_X8_TYPELESS);

		mPointClampSS = resourceManager->Create<D3D11SamplerState>(ED3D11SamplerState::PointClamp);
		mLinearClampSS = resourceManager->Create<D3D11SamplerState>(ED3D11SamplerState::LinearClamp);

		// vertexShader
		auto fullScreenVS = std::make_shared<D3D11VertexShader>(device, L"FullScreenVS.cso");
		auto SSRVS = std::make_shared<D3D11VertexShader>(device, L"TestSSRVS.cso");

		// pixelShader
		auto fullScreenPS = std::make_shared<D3D11PixelShader>(device, L"FullScreenPS.cso");
		auto postProcessingPS = std::make_shared<D3D11PixelShader>(device, L"PostProcessingPS.cso");
		auto SSRPS = std::make_shared<D3D11PixelShader>(device, L"TestSSRPS.cso");

		// computeShader
		mBloomExtractBrightAreasCS = std::make_shared<D3D11ComputeShader>(device, L"BloomExtractBrightAreasCS.cso");
		mBloomDownSampleCS = std::make_shared<D3D11ComputeShader>(device, L"BloomDownSampleCS.cso");
		mBloomAccumulateCS = std::make_shared<D3D11ComputeShader>(device, L"BloomAccumulateCS.cso");
		mBlurHorzCS = std::make_shared<D3D11ComputeShader>(device, L"BlurHorzCS.cso");
		mBlurVertCS = std::make_shared<D3D11ComputeShader>(device, L"BlurVertCS.cso");
		mSSRCS = std::make_shared<D3D11ComputeShader>(device, L"SSRCS.cso");

		// shaderProgram
		auto pipelieState = std::make_shared<PipelineState>(nullptr, nullptr, nullptr);
		mFullScreenProgram = std::make_unique<ShaderProgram>(device, fullScreenVS, nullptr, fullScreenPS, pipelieState);
		mPostProcessingProgram = std::make_unique<ShaderProgram>(device, fullScreenVS, nullptr, postProcessingPS, pipelieState);
		mSSRProgram = std::make_unique<ShaderProgram>(device, SSRVS, nullptr, SSRPS, pipelieState);

		// constantBuffer
		mBloomParamsCB = std::make_shared<D3D11ConstantBuffer<BloomParams>>(device, ED3D11ConstantBuffer::Transform);
		mPostProcessingCB = std::make_shared<D3D11ConstantBuffer<PostProcessingBuffer>>(device, ED3D11ConstantBuffer::Transform);
		mSSRCB = std::make_shared<D3D11ConstantBuffer<SSRInfo>>(device, ED3D11ConstantBuffer::Transform);
		mFogCB = std::make_shared<D3D11ConstantBuffer<Fog>>(device, ED3D11ConstantBuffer::Transform);
		OnResize(device, resourceManager, width, height);
	}

	void D3D11PostProcessingManager::OnResize(std::shared_ptr<D3D11Device> device, std::shared_ptr<D3D11ResourceManager> resourceManager, unsigned short width, unsigned short height)
	{
		mPostProcessingRTV[0] = std::make_shared<D3D11RenderTargetView>(device, ED3D11RenderTargetViewType::Offscreen, width, height);
		mPostProcessingRTV[1] = std::make_shared<D3D11RenderTargetView>(device, ED3D11RenderTargetViewType::Offscreen, width, height);
		mSSRUAV = std::make_shared<D3D11UnorderedAccessView>(device, width, height);
		mSSRSRV = std::make_shared<D3D11ShaderResourceView>(device, mSSRUAV, true);
		mPostProcessingSRV[0] = std::make_shared<D3D11ShaderResourceView>(device, mPostProcessingRTV[0]);
		mPostProcessingSRV[1] = std::make_shared<D3D11ShaderResourceView>(device, mPostProcessingRTV[1]);
		mSwapChainRTV = resourceManager->Get<D3D11RenderTargetView>(ED3D11RenderTargetViewType::Default);
		mOffscreenRTV = resourceManager->Get<D3D11RenderTargetView>(ED3D11RenderTargetViewType::Offscreen);
		mOffscreenSRV = std::make_shared<D3D11ShaderResourceView>(device, mOffscreenRTV);

		unsigned short extractBrightWidth = std::max<unsigned short>(1, width / 2);
		unsigned short extractBrightHeight = std::max<unsigned short>(1, height / 2);

		mExtractBrightUAV[0] = std::make_shared<D3D11UnorderedAccessView>(device, extractBrightWidth, extractBrightHeight);
		mExtractBrightUAV[1] = std::make_shared<D3D11UnorderedAccessView>(device, extractBrightWidth, extractBrightHeight);
		mExtractBrightSRV[0] = std::make_shared<D3D11ShaderResourceView>(device, mExtractBrightUAV[0], true);
		mExtractBrightSRV[1] = std::make_shared<D3D11ShaderResourceView>(device, mExtractBrightUAV[1], true);

		for (int i = 0; i < DOWN_SCALE_BUFFER_COUNT; ++i)
		{
			unsigned short downBufferWidth = std::max<unsigned short>(1, width / mDownScaleBufferDenominators[i]);
			unsigned short downBufferHeight = std::max<unsigned short>(1, height / mDownScaleBufferDenominators[i]);

			mDownScaleUAVs[i][0] = std::make_shared<D3D11UnorderedAccessView>(device, downBufferWidth, downBufferHeight);
			mDownScaleUAVs[i][1] = std::make_shared<D3D11UnorderedAccessView>(device, downBufferWidth, downBufferHeight);
			mDownScaleSRVs[i][0] = std::make_shared<D3D11ShaderResourceView>(device, mDownScaleUAVs[i][0], true);
			mDownScaleSRVs[i][1] = std::make_shared<D3D11ShaderResourceView>(device, mDownScaleUAVs[i][1], true);
		}

		auto defaultDSV = resourceManager->Get<D3D11DepthStencilView>(ED3D11DepthStencilViewType::Default);
		mDSVSRV = std::make_shared<D3D11ShaderResourceView>(device, defaultDSV, DXGI_FORMAT_R24_UNORM_X8_TYPELESS);

		auto normalRTV = resourceManager->Get<D3D11RenderTargetView>(ED3D11RenderTargetViewType::Normal);
		mNormalSSRSRV = std::make_shared<D3D11ShaderResourceView>(device, normalRTV);
	}

	// 이런 함수는 그냥 버퍼 받게끔 풀어버리는 게 더 나을 거 같긴 하네
	void D3D11PostProcessingManager::CopyOffscreenBuffer(std::shared_ptr<D3D11Device> device)
	{
		mFullScreenProgram->Bind(device);

		mPostProcessingRTV[mRTVIndex]->Bind(device, mNoneDSV);
		mOffscreenSRV->Bind(device, 0, ED3D11ShaderType::PixelShader);
		mPointClampSS->Bind(device, 0, ED3D11ShaderType::PixelShader);

		mFullScreenVB->Bind(device);
		mFullScreenIB->Bind(device);

		device->GetDeviceContext()->DrawIndexed(6, 0, 0);

		mPostProcessingRTV[mRTVIndex]->UnBind(device);
		swapPostProcessingBuffer();
	}

	void D3D11PostProcessingManager::RenderFullScreen(std::shared_ptr<D3D11Device> device)
	{
		mFullScreenProgram->Bind(device);

		mOffscreenRTV->Bind(device, mNoneDSV);
		mPostProcessingSRV[mSRVIndex]->Bind(device, 0, ED3D11ShaderType::PixelShader);
		mPointClampSS->Bind(device, 0, ED3D11ShaderType::PixelShader);

		mFullScreenVB->Bind(device);
		mFullScreenIB->Bind(device);

		device->GetDeviceContext()->DrawIndexed(6, 0, 0);

		mOffscreenRTV->UnBind(device);
		mPostProcessingSRV[mSRVIndex]->UnBind(device, 0, ED3D11ShaderType::PixelShader);

		//mSwapChainRTV->Bind(device, mNoneDSV);
	}

	void D3D11PostProcessingManager::SetPostProcessingInfo(const PostProcessingInfo& postProcessingInfo)
	{
		if (mPostProcessingInfo.BlendTextureName != postProcessingInfo.BlendTextureName)
		{
			mBlendTextureOrNull = mResourceManager->Create<D3D11Texture>(postProcessingInfo.BlendTextureName);
		}

		mPostProcessingInfo = postProcessingInfo;
	}

	void D3D11PostProcessingManager::Excute(std::shared_ptr<D3D11Device> device)
	{
		if (mPostProcessingInfo.bUseBloom)
		{
			excuteBloom(device);
		}

		//mPostProcessingInfo.bUseSSR = true;
		if (mPostProcessingInfo.bUseSSR)
		{
			/// SSR CS
			//Temp SSR
			// 상수 버퍼 Setting
			SSRInfo ssrInfo;
			ssrInfo.viewMat = mCameraManager->GetViewMatrix(ECameraType::Player).Transpose();
			ssrInfo.projMat = mCameraManager->GetProjectionMatrix(ECameraType::Player).Transpose();
			ssrInfo.invProjMat = mCameraManager->GetProjectionMatrix(ECameraType::Player).Invert().Transpose();
			ssrInfo.viewSizeX = 1920;
			ssrInfo.viewSizeY = 1080;
			ssrInfo.maxIteration = mPostProcessingInfo.max_iteration;
			ssrInfo.maxThickness = mPostProcessingInfo.max_thickness;
			mSSRCB->Update(device, ssrInfo);

			mPointClampSS->Bind(device, 0, ED3D11ShaderType::ComputeShader);
			mSSRCB->Bind(device, ED3D11ShaderType::ComputeShader, 0);
			mSSRCS->Bind(device);
			mNormalSSRSRV->Bind(device, 0, ED3D11ShaderType::ComputeShader);
			mDSVSRV->Bind(device, 1, ED3D11ShaderType::ComputeShader);
			mPostProcessingSRV[mSRVIndex]->Bind(device, 2, ED3D11ShaderType::ComputeShader);
			mSSRUAV->Bind(device, 0);
			device->GetDeviceContext()->Dispatch(128, 128, 1);

			mPostProcessingSRV[mSRVIndex]->UnBind(device, 2, ED3D11ShaderType::ComputeShader);
			mDSVSRV->UnBind(device, 1, ED3D11ShaderType::ComputeShader);
			mNormalSSRSRV->UnBind(device, 0, ED3D11ShaderType::ComputeShader);
			mSSRUAV->UnBind(device, 0);
			///
		}

		PostProcessingBuffer postProcessingBuffer;
		postProcessingBuffer.BloomColorTint = mPostProcessingInfo.BloomColorTint;
		postProcessingBuffer.ShadowColor = mPostProcessingInfo.ShadowColor;
		postProcessingBuffer.HighlightColor = mPostProcessingInfo.HighlightColor;
		postProcessingBuffer.VignettColor = mPostProcessingInfo.VignettColor;
		postProcessingBuffer.BlendColor = mPostProcessingInfo.BlendColor;
		postProcessingBuffer.BlendTextureColor = mPostProcessingInfo.BlendTextureColor;

		postProcessingBuffer.Exposure = mPostProcessingInfo.Exposure;
		postProcessingBuffer.Contrast = mPostProcessingInfo.Contrast;
		postProcessingBuffer.Saturation = mPostProcessingInfo.Saturation;
		postProcessingBuffer.Gamma = mPostProcessingInfo.Gamma;

		postProcessingBuffer.BloomIntensity = mPostProcessingInfo.BloomIntensity;
		postProcessingBuffer.BloomThreshold = mPostProcessingInfo.BloomThreshold;
		postProcessingBuffer.Balance = mPostProcessingInfo.Balance;
		postProcessingBuffer.VignettRadius = mPostProcessingInfo.VignettRadius;

		postProcessingBuffer.VignettSmoothness = mPostProcessingInfo.VignettSmoothness;
		postProcessingBuffer.bUseColorAdjustment = mPostProcessingInfo.bUseColorAdjustment;
		postProcessingBuffer.bUseBloom = mPostProcessingInfo.bUseBloom;
		postProcessingBuffer.bUseSplitToning = mPostProcessingInfo.bUseSplitToning;

		postProcessingBuffer.bUseVignett = mPostProcessingInfo.bUseVignett;
		postProcessingBuffer.bUseToneMapping = mPostProcessingInfo.bUseToneMapping;
		postProcessingBuffer.bUseFog = mPostProcessingInfo.bUseFog;

		postProcessingBuffer.bUseGrayScale = mPostProcessingInfo.bUseGrayScale;
		postProcessingBuffer.bUseBlendColor = mPostProcessingInfo.bUseBlendColor;
		postProcessingBuffer.bUseBlendTexture = mPostProcessingInfo.bUseBlendTexture;

		// Fog
		Fog fog;
		fog.nearPlane = mCameraManager->GetNearPlane(ECameraType::Player);
		fog.farPlane = mCameraManager->GetFarPlane(ECameraType::Player);
		fog.visibleArea = mPostProcessingInfo.fogVisibleArea;
		fog.color = mPostProcessingInfo.fogColor;
		mFogCB->Update(device, fog);
		mFogCB->Bind(device, ED3D11ShaderType::PixelShader, 1);
		//

		mPostProcessingProgram->Bind(device);

		mPostProcessingRTV[mRTVIndex]->Bind(device, mNoneDSV);

		if (mPostProcessingInfo.bUseSSR)
		{
			mSSRSRV->Bind(device, 0, ED3D11ShaderType::PixelShader);
		}
		else
		{
			mPostProcessingSRV[mSRVIndex]->Bind(device, 0, ED3D11ShaderType::PixelShader);
		}
		mExtractBrightSRV[mDownScaleUAVIndex]->Bind(device, 1, ED3D11ShaderType::PixelShader);
		mDSVSRV->Bind(device, 3, ED3D11ShaderType::PixelShader);
		
		if (mBlendTextureOrNull != nullptr)
		{
			mBlendTextureOrNull->Bind(device, 4, ED3D11ShaderType::PixelShader);
		}

		mPointClampSS->Bind(device, 0, ED3D11ShaderType::PixelShader);
		mLinearClampSS->Bind(device, 1, ED3D11ShaderType::PixelShader);

		mFullScreenVB->Bind(device);
		mFullScreenIB->Bind(device);
		mPostProcessingCB->Update(device, postProcessingBuffer);
		mPostProcessingCB->Bind(device, ED3D11ShaderType::PixelShader);

		/// SSR VS, PS
		//mSSRProgram->Bind(device);
		//mNormalSSRSRV->Bind(device, 0, ED3D11ShaderType::PixelShader);
		//mDSVSRV->Bind(device, 1, ED3D11ShaderType::PixelShader);
		//mPointClampSS->Bind(device, 0, ED3D11ShaderType::PixelShader);
		//mFullScreenVB->Bind(device);
		//mFullScreenIB->Bind(device);
		//mSSRCB->Bind(device, ED3D11ShaderType::PixelShader, 0);
		//
		////mSwapChainRTV->Bind(device, mNoneDSV);
		////mColorSSRSRV->Bind(device, 2, ED3D11ShaderType::PixelShader);
		//
		//mPostProcessingRTV[mRTVIndex]->Bind(device, mNoneDSV);
		//mPostProcessingSRV[mSRVIndex]->Bind(device, 2, ED3D11ShaderType::PixelShader);
		/// 

		device->GetDeviceContext()->DrawIndexed(6, 0, 0);

		mPostProcessingRTV[mRTVIndex]->UnBind(device);
		mPostProcessingSRV[mSRVIndex]->UnBind(device, 0, ED3D11ShaderType::PixelShader);
		swapPostProcessingBuffer();
	}

	void D3D11PostProcessingManager::excuteBloom(std::shared_ptr<D3D11Device> device)
	{
		auto align = [](int value, int aligment)
			{
				return (value + aligment - 1) / aligment * aligment;
			};

		unsigned short width = std::max<unsigned short>(1, device->GetWidth() / 2);
		unsigned short height = std::max<unsigned short>(1, device->GetHeight() / 2);
		mBloomParams.Threshold = mPostProcessingInfo.BloomThreshold;
		mBloomParams.Scatter = mPostProcessingInfo.BloomScatter;
		mBloomParams.bUseScatter = mPostProcessingInfo.bUseBloomScatter;
		mBloomParamsCB->Update(device, mBloomParams);

		// 밝은 부분 추출
		mBloomExtractBrightAreasCS->Bind(device);
		mPostProcessingSRV[mSRVIndex]->Bind(device, 0, ED3D11ShaderType::ComputeShader);
		mExtractBrightUAV[mDownScaleSRVIndex]->Bind(device, 0);
		mLinearClampSS->Bind(device, 0, ED3D11ShaderType::ComputeShader);
		mBloomParamsCB->Bind(device, ED3D11ShaderType::ComputeShader);
		int dispatchX = align(width, 16) / 16;
		int dispatchY = align(height, 16) / 16;
		device->GetDeviceContext()->Dispatch(dispatchX, dispatchY, 1);
		mExtractBrightUAV[mDownScaleSRVIndex]->UnBind(device, 0);
		mPostProcessingSRV[mSRVIndex]->UnBind(device, 0, ED3D11ShaderType::ComputeShader);

		{
			width = std::max<unsigned short>(1, device->GetWidth() / mDownScaleBufferDenominators[0]);
			height = std::max<unsigned short>(1, device->GetHeight() / mDownScaleBufferDenominators[0]);

			// 다운 샘플링
			mBloomDownSampleCS->Bind(device);
			mExtractBrightSRV[mDownScaleSRVIndex]->Bind(device, 0, ED3D11ShaderType::ComputeShader);
			mDownScaleUAVs[0][mDownScaleSRVIndex]->Bind(device, 0);
			mLinearClampSS->Bind(device, 0, ED3D11ShaderType::ComputeShader);
			mBloomParamsCB->Bind(device, ED3D11ShaderType::ComputeShader);
			dispatchX = align(width, 16) / 16;
			dispatchY = align(height, 16) / 16;
			device->GetDeviceContext()->Dispatch(dispatchX, dispatchY, 1);
			mExtractBrightSRV[mDownScaleSRVIndex]->UnBind(device, 0, ED3D11ShaderType::ComputeShader);
			mDownScaleUAVs[0][mDownScaleSRVIndex]->UnBind(device, 0);

			// 수평 흐리기
			mBlurHorzCS->Bind(device);
			mDownScaleSRVs[0][mDownScaleSRVIndex]->Bind(device, 0, ED3D11ShaderType::ComputeShader);
			mDownScaleUAVs[0][mDownScaleUAVIndex]->Bind(device, 0);
			dispatchX = align(width, 256) / 256;
			dispatchY = height;
			device->GetDeviceContext()->Dispatch(dispatchX, dispatchY, 1);
			mDownScaleSRVs[0][mDownScaleSRVIndex]->UnBind(device, 0, ED3D11ShaderType::ComputeShader);
			mDownScaleUAVs[0][mDownScaleUAVIndex]->UnBind(device, 0);
			std::swap(mDownScaleSRVIndex, mDownScaleUAVIndex);

			// 수직 흐리기
			mBlurVertCS->Bind(device);
			mDownScaleSRVs[0][mDownScaleSRVIndex]->Bind(device, 0, ED3D11ShaderType::ComputeShader);
			mDownScaleUAVs[0][mDownScaleUAVIndex]->Bind(device, 0);
			dispatchX = width;
			dispatchY = align(height, 256) / 256;
			device->GetDeviceContext()->Dispatch(dispatchX, dispatchY, 1);
			mDownScaleSRVs[0][mDownScaleSRVIndex]->UnBind(device, 0, ED3D11ShaderType::ComputeShader);
			mDownScaleUAVs[0][mDownScaleUAVIndex]->UnBind(device, 0);
			std::swap(mDownScaleSRVIndex, mDownScaleUAVIndex);
		}

		for (int i = 0; i < DOWN_SCALE_BUFFER_COUNT - 1; ++i)
		{
			if (width <= 0 || height <= 0)
			{
				break;
			}

			width = std::max<unsigned short>(1, device->GetWidth() / mDownScaleBufferDenominators[i + 1]);
			height = std::max<unsigned short>(1, device->GetHeight() / mDownScaleBufferDenominators[i + 1]);

			// 다운 샘플링
			mBloomDownSampleCS->Bind(device);
			mDownScaleSRVs[i][mDownScaleSRVIndex]->Bind(device, 0, ED3D11ShaderType::ComputeShader);
			mDownScaleUAVs[i + 1][mDownScaleSRVIndex]->Bind(device, 0);
			mLinearClampSS->Bind(device, 0, ED3D11ShaderType::ComputeShader);
			mBloomParamsCB->Bind(device, ED3D11ShaderType::ComputeShader);
			dispatchX = align(width, 16) / 16;
			dispatchY = align(height, 16) / 16;
			device->GetDeviceContext()->Dispatch(dispatchX, dispatchY, 1);
			mDownScaleSRVs[i][mDownScaleSRVIndex]->UnBind(device, 0, ED3D11ShaderType::ComputeShader);
			mDownScaleUAVs[i + 1][mDownScaleSRVIndex]->UnBind(device, 0);

			// 수평 흐리기
			mBlurHorzCS->Bind(device);
			mDownScaleSRVs[i + 1][mDownScaleSRVIndex]->Bind(device, 0, ED3D11ShaderType::ComputeShader);
			mDownScaleUAVs[i + 1][mDownScaleUAVIndex]->Bind(device, 0);
			dispatchX = align(width, 256) / 256;
			dispatchY = height;
			device->GetDeviceContext()->Dispatch(dispatchX, dispatchY, 1);
			mDownScaleSRVs[i + 1][mDownScaleSRVIndex]->UnBind(device, 0, ED3D11ShaderType::ComputeShader);
			mDownScaleUAVs[i + 1][mDownScaleUAVIndex]->UnBind(device, 0);
			std::swap(mDownScaleSRVIndex, mDownScaleUAVIndex);

			// 수직 흐리기
			mBlurVertCS->Bind(device);
			mDownScaleSRVs[i + 1][mDownScaleSRVIndex]->Bind(device, 0, ED3D11ShaderType::ComputeShader);
			mDownScaleUAVs[i + 1][mDownScaleUAVIndex]->Bind(device, 0);
			dispatchX = width;
			dispatchY = align(height, 256) / 256;
			device->GetDeviceContext()->Dispatch(dispatchX, dispatchY, 1);
			mDownScaleSRVs[i + 1][mDownScaleSRVIndex]->UnBind(device, 0, ED3D11ShaderType::ComputeShader);
			mDownScaleUAVs[i + 1][mDownScaleUAVIndex]->UnBind(device, 0);
			std::swap(mDownScaleSRVIndex, mDownScaleUAVIndex);
		}

		{
			width = std::max<unsigned short>(1, device->GetWidth() / mDownScaleBufferDenominators[DOWN_SCALE_BUFFER_COUNT - 2]);
			height = std::max<unsigned short>(1, device->GetHeight() / mDownScaleBufferDenominators[DOWN_SCALE_BUFFER_COUNT - 2]);

			// 업샘플링
			mBloomAccumulateCS->Bind(device);
			mDownScaleSRVs[DOWN_SCALE_BUFFER_COUNT - 1][mDownScaleSRVIndex]->Bind(device, 0, ED3D11ShaderType::ComputeShader);
			mDownScaleSRVs[DOWN_SCALE_BUFFER_COUNT - 2][mDownScaleSRVIndex]->Bind(device, 1, ED3D11ShaderType::ComputeShader);
			mDownScaleUAVs[DOWN_SCALE_BUFFER_COUNT - 2][mDownScaleUAVIndex]->Bind(device, 0);
			mLinearClampSS->Bind(device, 0, ED3D11ShaderType::ComputeShader);
			mBloomParamsCB->Bind(device, ED3D11ShaderType::ComputeShader);
			dispatchX = align(width, 16) / 16;
			dispatchY = align(height, 16) / 16;
			device->GetDeviceContext()->Dispatch(dispatchX, dispatchY, 1);
			mDownScaleSRVs[DOWN_SCALE_BUFFER_COUNT - 1][mDownScaleSRVIndex]->UnBind(device, 0, ED3D11ShaderType::ComputeShader);
			mDownScaleSRVs[DOWN_SCALE_BUFFER_COUNT - 2][mDownScaleSRVIndex]->UnBind(device, 1, ED3D11ShaderType::ComputeShader);
			mDownScaleUAVs[DOWN_SCALE_BUFFER_COUNT - 2][mDownScaleUAVIndex]->UnBind(device, 0);

			// 수평 흐리기
			mBlurHorzCS->Bind(device);
			mDownScaleSRVs[DOWN_SCALE_BUFFER_COUNT - 2][mDownScaleUAVIndex]->Bind(device, 0, ED3D11ShaderType::ComputeShader);
			mDownScaleUAVs[DOWN_SCALE_BUFFER_COUNT - 2][mDownScaleSRVIndex]->Bind(device, 0);
			dispatchX = align(width, 256) / 256;
			dispatchY = height;
			device->GetDeviceContext()->Dispatch(dispatchX, dispatchY, 1);
			mDownScaleSRVs[DOWN_SCALE_BUFFER_COUNT - 2][mDownScaleUAVIndex]->UnBind(device, 0, ED3D11ShaderType::ComputeShader);
			mDownScaleUAVs[DOWN_SCALE_BUFFER_COUNT - 2][mDownScaleSRVIndex]->UnBind(device, 0);
			std::swap(mDownScaleSRVIndex, mDownScaleUAVIndex);

			// 수직 흐리기
			mBlurVertCS->Bind(device);
			mDownScaleSRVs[DOWN_SCALE_BUFFER_COUNT - 2][mDownScaleUAVIndex]->Bind(device, 0, ED3D11ShaderType::ComputeShader);
			mDownScaleUAVs[DOWN_SCALE_BUFFER_COUNT - 2][mDownScaleSRVIndex]->Bind(device, 0);
			dispatchX = width;
			dispatchY = align(height, 256) / 256;
			device->GetDeviceContext()->Dispatch(dispatchX, dispatchY, 1);
			mDownScaleSRVs[DOWN_SCALE_BUFFER_COUNT - 2][mDownScaleUAVIndex]->UnBind(device, 0, ED3D11ShaderType::ComputeShader);
			mDownScaleUAVs[DOWN_SCALE_BUFFER_COUNT - 2][mDownScaleSRVIndex]->UnBind(device, 0);
			std::swap(mDownScaleSRVIndex, mDownScaleUAVIndex);
		}

		for (int i = DOWN_SCALE_BUFFER_COUNT - 2; i >= 1; --i)
		{
			width = std::max<unsigned short>(1, device->GetWidth() / mDownScaleBufferDenominators[i - 1]);
			height = std::max<unsigned short>(1, device->GetHeight() / mDownScaleBufferDenominators[i - 1]);

			if (width <= 0 || height <= 0)
			{
				continue;
			}

			// 업샘플링
			mBloomAccumulateCS->Bind(device);
			mDownScaleSRVs[i][mDownScaleUAVIndex]->Bind(device, 0, ED3D11ShaderType::ComputeShader);
			mDownScaleSRVs[i - 1][mDownScaleSRVIndex]->Bind(device, 1, ED3D11ShaderType::ComputeShader);
			mDownScaleUAVs[i - 1][mDownScaleUAVIndex]->Bind(device, 0);
			mLinearClampSS->Bind(device, 0, ED3D11ShaderType::ComputeShader);
			mBloomParamsCB->Bind(device, ED3D11ShaderType::ComputeShader);
			dispatchX = align(width, 16) / 16;
			dispatchY = align(height, 16) / 16;
			device->GetDeviceContext()->Dispatch(dispatchX, dispatchY, 1);
			mDownScaleSRVs[i][mDownScaleUAVIndex]->UnBind(device, 0, ED3D11ShaderType::ComputeShader);
			mDownScaleSRVs[i - 1][mDownScaleSRVIndex]->UnBind(device, 1, ED3D11ShaderType::ComputeShader);
			mDownScaleUAVs[i - 1][mDownScaleUAVIndex]->UnBind(device, 0);

			// 수평 흐리기
			mBlurHorzCS->Bind(device);
			mDownScaleSRVs[i - 1][mDownScaleUAVIndex]->Bind(device, 0, ED3D11ShaderType::ComputeShader);
			mDownScaleUAVs[i - 1][mDownScaleSRVIndex]->Bind(device, 0);
			dispatchX = align(width, 256) / 256;
			dispatchY = height;
			device->GetDeviceContext()->Dispatch(dispatchX, dispatchY, 1);
			mDownScaleSRVs[i - 1][mDownScaleUAVIndex]->UnBind(device, 0, ED3D11ShaderType::ComputeShader);
			mDownScaleUAVs[i - 1][mDownScaleSRVIndex]->UnBind(device, 0);
			std::swap(mDownScaleSRVIndex, mDownScaleUAVIndex);

			// 수직 흐리기
			mBlurVertCS->Bind(device);
			mDownScaleSRVs[i - 1][mDownScaleUAVIndex]->Bind(device, 0, ED3D11ShaderType::ComputeShader);
			mDownScaleUAVs[i - 1][mDownScaleSRVIndex]->Bind(device, 0);
			dispatchX = width;
			dispatchY = align(height, 256) / 256;
			device->GetDeviceContext()->Dispatch(dispatchX, dispatchY, 1);
			mDownScaleSRVs[i - 1][mDownScaleUAVIndex]->UnBind(device, 0, ED3D11ShaderType::ComputeShader);
			mDownScaleUAVs[i - 1][mDownScaleSRVIndex]->UnBind(device, 0);
			std::swap(mDownScaleSRVIndex, mDownScaleUAVIndex);
		}

		width = std::max<unsigned short>(1, device->GetWidth() / 2);
		height = std::max<unsigned short>(1, device->GetHeight() / 2);

		// 업샘플링
		mBloomAccumulateCS->Bind(device);
		mDownScaleSRVs[0][mDownScaleUAVIndex]->Bind(device, 0, ED3D11ShaderType::ComputeShader);
		mExtractBrightSRV[mDownScaleSRVIndex]->Bind(device, 1, ED3D11ShaderType::ComputeShader);
		mExtractBrightUAV[mDownScaleUAVIndex]->Bind(device, 0);
		mLinearClampSS->Bind(device, 0, ED3D11ShaderType::ComputeShader);
		mBloomParamsCB->Bind(device, ED3D11ShaderType::ComputeShader);
		dispatchX = align(width, 16) / 16;
		dispatchY = align(height, 16) / 16;
		device->GetDeviceContext()->Dispatch(dispatchX, dispatchY, 1);
		mDownScaleSRVs[0][mDownScaleUAVIndex]->UnBind(device, 0, ED3D11ShaderType::ComputeShader);
		mExtractBrightSRV[mDownScaleSRVIndex]->UnBind(device, 1, ED3D11ShaderType::ComputeShader);
		mExtractBrightUAV[mDownScaleUAVIndex]->UnBind(device, 0);

		// 수평 흐리기
		mBlurHorzCS->Bind(device);
		mExtractBrightSRV[mDownScaleUAVIndex]->Bind(device, 0, ED3D11ShaderType::ComputeShader);
		mExtractBrightUAV[mDownScaleSRVIndex]->Bind(device, 0);
		dispatchX = align(width, 256) / 256;
		dispatchY = height;
		device->GetDeviceContext()->Dispatch(dispatchX, dispatchY, 1);
		mExtractBrightSRV[mDownScaleUAVIndex]->UnBind(device, 0, ED3D11ShaderType::ComputeShader);
		mExtractBrightUAV[mDownScaleSRVIndex]->UnBind(device, 0);
		std::swap(mDownScaleSRVIndex, mDownScaleUAVIndex);

		// 수직 흐리기
		mBlurVertCS->Bind(device);
		mExtractBrightSRV[mDownScaleUAVIndex]->Bind(device, 0, ED3D11ShaderType::ComputeShader);
		mExtractBrightUAV[mDownScaleSRVIndex]->Bind(device, 0);
		dispatchX = width;
		dispatchY = align(height, 256) / 256;
		device->GetDeviceContext()->Dispatch(dispatchX, dispatchY, 1);
		mExtractBrightSRV[mDownScaleUAVIndex]->UnBind(device, 0, ED3D11ShaderType::ComputeShader);
		mExtractBrightUAV[mDownScaleSRVIndex]->UnBind(device, 0);
		std::swap(mDownScaleSRVIndex, mDownScaleUAVIndex);
	}
}