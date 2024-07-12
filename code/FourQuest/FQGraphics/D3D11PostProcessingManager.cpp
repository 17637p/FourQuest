#include <d3d11.h>
#include "D3D11PostProcessingManager.h"
#include "ManagementCommon.h"
#include "D3D11Common.h"
#include "D3D11Buffer.h"

namespace fq::graphics
{
	void D3D11PostProcessingManager::Initialize(std::shared_ptr<D3D11Device> device, std::shared_ptr<D3D11ResourceManager> resourceManager, unsigned short width, unsigned short height)
	{
		mFullScreenVB = std::make_shared<D3D11VertexBuffer>(D3D11VertexBuffer::CreateFullScreenVertexBuffer(device));
		mFullScreenIB = std::make_shared<D3D11IndexBuffer>(D3D11IndexBuffer::CreateFullScreenIndexBuffer(device));
		mNoneDSV = resourceManager->Get<D3D11DepthStencilView>(ED3D11DepthStencilViewType::None);

		mPointClampSS = resourceManager->Create<D3D11SamplerState>(ED3D11SamplerState::PointClamp);
		mLinearWrapSS = resourceManager->Create<D3D11SamplerState>(ED3D11SamplerState::LinearClamp);

		// vertexShader
		auto fullScreenVS = std::make_shared<D3D11VertexShader>(device, L"FullScreenVS.cso");

		// pixelShader
		auto fullScreenPS = std::make_shared<D3D11PixelShader>(device, L"FullScreenPS.cso");
		auto postProcessingPS = std::make_shared<D3D11PixelShader>(device, L"PostProcessingPS.cso");
		auto bloomPS = std::make_shared<D3D11PixelShader>(device, L"BloomPS.cso");

		// computeShader
		mBloomExtractBrightAreasCS = std::make_shared<D3D11ComputeShader>(device, L"BloomExtractBrightAreasCS.cso");
		mBloomDownSampleCS = std::make_shared<D3D11ComputeShader>(device, L"BloomDownSampleCS.cso");
		mBloomAccumulateCS = std::make_shared<D3D11ComputeShader>(device, L"BloomAccumulateCS.cso");
		mBlurHorzCS = std::make_shared<D3D11ComputeShader>(device, L"BlurHorzCS.cso");
		mBlurVertCS = std::make_shared<D3D11ComputeShader>(device, L"BlurVertCS.cso");

		// shaderProgram
		auto pipelieState = std::make_shared<PipelineState>(nullptr, nullptr, nullptr);
		mFullScreenProgram = std::make_unique<ShaderProgram>(device, fullScreenVS, nullptr, fullScreenPS, pipelieState);
		mPostProcessingProgram = std::make_unique<ShaderProgram>(device, fullScreenVS, nullptr, postProcessingPS, pipelieState);
		mBloomProgram = std::make_unique<ShaderProgram>(device, fullScreenVS, nullptr, bloomPS, pipelieState);

		// constantBuffer
		mBloomParamsCB = std::make_shared<D3D11ConstantBuffer<BloomParams>>(device, ED3D11ConstantBuffer::Transform);
		mPostProcessingCB = std::make_shared<D3D11ConstantBuffer<PostProcessingBuffer>>(device, ED3D11ConstantBuffer::Transform);
		OnResize(device, resourceManager, width, height);
	}

	void D3D11PostProcessingManager::OnResize(std::shared_ptr<D3D11Device> device, std::shared_ptr<D3D11ResourceManager> resourceManager, unsigned short width, unsigned short height)
	{
		mPostProcessingRTV[0] = std::make_shared<D3D11RenderTargetView>(device, ED3D11RenderTargetViewType::Offscreen, width, height);
		mPostProcessingRTV[1] = std::make_shared<D3D11RenderTargetView>(device, ED3D11RenderTargetViewType::Offscreen, width, height);
		mPostProcessingSRV[0] = std::make_shared<D3D11ShaderResourceView>(device, mPostProcessingRTV[0]);
		mPostProcessingSRV[1] = std::make_shared<D3D11ShaderResourceView>(device, mPostProcessingRTV[1]);
		mSwapChainRTV = resourceManager->Get<D3D11RenderTargetView>(ED3D11RenderTargetViewType::Default);
		mOffscreenRTV = resourceManager->Get<D3D11RenderTargetView>(ED3D11RenderTargetViewType::Offscreen);
		mOffscreenSRV = std::make_shared<D3D11ShaderResourceView>(device, mOffscreenRTV);

		mExtractBrightUAV[0] = std::make_shared<D3D11UnorderedAccessView>(device, width / 2, height / 2);
		mExtractBrightUAV[1] = std::make_shared<D3D11UnorderedAccessView>(device, width / 2, height / 2);
		mExtractBrightSRV[0] = std::make_shared<D3D11ShaderResourceView>(device, mExtractBrightUAV[0], true);
		mExtractBrightSRV[1] = std::make_shared<D3D11ShaderResourceView>(device, mExtractBrightUAV[1], true);

		for (int i = 0; i < DOWN_SCALE_BUFFER_COUNT; ++i)
		{
			float downBufferWidth = width / mDownScaleBufferDenominators[i];
			float downBufferHeight = height / mDownScaleBufferDenominators[i];

			if (downBufferWidth <= 0 || downBufferHeight <= 0)
			{
				break;
			}

			mDownScaleUAVs[i][0] = std::make_shared<D3D11UnorderedAccessView>(device, downBufferWidth, downBufferHeight);
			mDownScaleUAVs[i][1] = std::make_shared<D3D11UnorderedAccessView>(device, downBufferWidth, downBufferHeight);
			mDownScaleSRVs[i][0] = std::make_shared<D3D11ShaderResourceView>(device, mDownScaleUAVs[i][0], true);
			mDownScaleSRVs[i][1] = std::make_shared<D3D11ShaderResourceView>(device, mDownScaleUAVs[i][1], true);
		}
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

		//mOffscreenRTV->UnBind(device);
		//mPostProcessingSRV[mSRVIndex]->UnBind(device, 0, ED3D11ShaderType::PixelShader);

		mSwapChainRTV->Bind(device, mNoneDSV);
	}

	void D3D11PostProcessingManager::Excute(std::shared_ptr<D3D11Device> device)
	{
		if (mPostProcessingInfo.bUseBloom)
		{
			excuteBloom(device);
		}

		PostProcessingBuffer postProcessingBuffer;
		postProcessingBuffer.BloomColorTint = mPostProcessingInfo.BloomColorTint;
		postProcessingBuffer.ShadowColor = mPostProcessingInfo.ShadowColor;
		postProcessingBuffer.HighlightColor = mPostProcessingInfo.HighlightColor;
		postProcessingBuffer.VignettColor = mPostProcessingInfo.VignettColor;

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

		mPostProcessingProgram->Bind(device);

		mPostProcessingRTV[mRTVIndex]->Bind(device, mNoneDSV);

		mPostProcessingSRV[mSRVIndex]->Bind(device, 0, ED3D11ShaderType::PixelShader);
		mExtractBrightSRV[mDownScaleSRVIndex]->Bind(device, 1, ED3D11ShaderType::PixelShader);
		mPointClampSS->Bind(device, 0, ED3D11ShaderType::PixelShader);

		mFullScreenVB->Bind(device);
		mFullScreenIB->Bind(device);
		mPostProcessingCB->Update(device, postProcessingBuffer);
		mPostProcessingCB->Bind(device, ED3D11ShaderType::PixelShader);

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

		unsigned short width = device->GetWidth() / 2;
		unsigned short height = device->GetHeight() / 2;
		mBloomParams.Threshold = mPostProcessingInfo.BloomThreshold;
		mBloomParamsCB->Update(device, mBloomParams);

		// 밝은 부분 추출
		mBloomExtractBrightAreasCS->Bind(device);
		mPostProcessingSRV[mSRVIndex]->Bind(device, 0, ED3D11ShaderType::ComputeShader);
		mExtractBrightUAV[mDownScaleUAVIndex]->Bind(device, 0);
		mPointClampSS->Bind(device, 0, ED3D11ShaderType::PixelShader);
		mBloomParamsCB->Bind(device, ED3D11ShaderType::ComputeShader);
		int dispatchX = align(width, 16) / 16;
		int dispatchY = align(height, 16) / 16;
		device->GetDeviceContext()->Dispatch(dispatchX, dispatchY, 1);
		mExtractBrightUAV[mDownScaleUAVIndex]->UnBind(device, 0);
		mPostProcessingSRV[mSRVIndex]->UnBind(device, 0, ED3D11ShaderType::ComputeShader);
		std::swap(mDownScaleSRVIndex, mDownScaleUAVIndex);

		{
			width = device->GetWidth() / mDownScaleBufferDenominators[0];
			height = device->GetHeight() / mDownScaleBufferDenominators[0];

			// 다운 샘플링
			mBloomDownSampleCS->Bind(device);
			mExtractBrightSRV[mDownScaleSRVIndex]->Bind(device, 0, ED3D11ShaderType::ComputeShader);
			mDownScaleUAVs[0][mDownScaleUAVIndex]->Bind(device, 0);
			mLinearWrapSS->Bind(device, 0, ED3D11ShaderType::ComputeShader);
			mBloomParamsCB->Bind(device, ED3D11ShaderType::ComputeShader);
			dispatchX = align(width, 16) / 16;
			dispatchY = align(height, 16) / 16;
			device->GetDeviceContext()->Dispatch(dispatchX, dispatchY, 1);
			mExtractBrightSRV[mDownScaleSRVIndex]->UnBind(device, 0, ED3D11ShaderType::ComputeShader);
			mDownScaleUAVs[0][mDownScaleUAVIndex]->UnBind(device, 0);
			std::swap(mDownScaleSRVIndex, mDownScaleUAVIndex);

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

			width = device->GetWidth() / mDownScaleBufferDenominators[i + 1];
			height = device->GetHeight() / mDownScaleBufferDenominators[i + 1];

			// 다운 샘플링
			mBloomDownSampleCS->Bind(device);
			mDownScaleSRVs[i][mDownScaleSRVIndex]->Bind(device, 0, ED3D11ShaderType::ComputeShader);
			mDownScaleUAVs[i + 1][mDownScaleUAVIndex]->Bind(device, 0);
			mLinearWrapSS->Bind(device, 0, ED3D11ShaderType::ComputeShader);
			mBloomParamsCB->Bind(device, ED3D11ShaderType::ComputeShader);
			dispatchX = align(width, 16) / 16;
			dispatchY = align(height, 16) / 16;
			device->GetDeviceContext()->Dispatch(dispatchX, dispatchY, 1);
			mDownScaleSRVs[i][mDownScaleSRVIndex]->UnBind(device, 0, ED3D11ShaderType::ComputeShader);
			mDownScaleUAVs[i + 1][mDownScaleUAVIndex]->UnBind(device, 0);
			std::swap(mDownScaleSRVIndex, mDownScaleUAVIndex);

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

		for (int i = DOWN_SCALE_BUFFER_COUNT - 1; i >= 1; --i)
		{
			width = device->GetWidth() / mDownScaleBufferDenominators[i - 1];
			height = device->GetHeight() / mDownScaleBufferDenominators[i - 1];

			if (width <= 0 || height <= 0)
			{
				continue;
			}

			// 업샘플링
			mBloomAccumulateCS->Bind(device);
			mDownScaleSRVs[i][mDownScaleSRVIndex]->Bind(device, 0, ED3D11ShaderType::ComputeShader);
			mDownScaleSRVs[i - 1][mDownScaleUAVIndex]->Bind(device, 1, ED3D11ShaderType::ComputeShader);
			mDownScaleUAVs[i - 1][mDownScaleSRVIndex]->Bind(device, 0);
			mLinearWrapSS->Bind(device, 0, ED3D11ShaderType::ComputeShader);
			mBloomParamsCB->Bind(device, ED3D11ShaderType::ComputeShader);
			dispatchX = align(width, 16) / 16;
			dispatchY = align(height, 16) / 16;
			device->GetDeviceContext()->Dispatch(dispatchX, dispatchY, 1);
			mDownScaleSRVs[i][mDownScaleSRVIndex]->UnBind(device, 0, ED3D11ShaderType::ComputeShader);
			mDownScaleSRVs[i - 1][mDownScaleUAVIndex]->UnBind(device, 1, ED3D11ShaderType::ComputeShader);
			mDownScaleUAVs[i - 1][mDownScaleSRVIndex]->UnBind(device, 0);
			std::swap(mDownScaleSRVIndex, mDownScaleUAVIndex);

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

		width = device->GetWidth() / 2;
		height = device->GetHeight() / 2;
		// 업샘플링
		mBloomAccumulateCS->Bind(device);
		mDownScaleSRVs[0][mDownScaleSRVIndex]->Bind(device, 0, ED3D11ShaderType::ComputeShader);
		mExtractBrightSRV[mDownScaleUAVIndex]->Bind(device, 1, ED3D11ShaderType::ComputeShader);
		mExtractBrightUAV[mDownScaleSRVIndex]->Bind(device, 0);
		mLinearWrapSS->Bind(device, 0, ED3D11ShaderType::ComputeShader);
		mBloomParamsCB->Bind(device, ED3D11ShaderType::ComputeShader);
		dispatchX = align(width, 16) / 16;
		dispatchY = align(height, 16) / 16;
		device->GetDeviceContext()->Dispatch(dispatchX, dispatchY, 1);
		mDownScaleSRVs[0][mDownScaleSRVIndex]->UnBind(device, 0, ED3D11ShaderType::ComputeShader);
		mExtractBrightSRV[mDownScaleUAVIndex]->UnBind(device, 1, ED3D11ShaderType::ComputeShader);
		mExtractBrightUAV[mDownScaleSRVIndex]->UnBind(device, 0);
		std::swap(mDownScaleSRVIndex, mDownScaleUAVIndex);

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
	}
}