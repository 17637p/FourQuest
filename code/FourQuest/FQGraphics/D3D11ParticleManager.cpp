#include "D3D11ParticleManager.h"
#include "D3D11Common.h"
#include "ManagementCommon.h"

namespace fq::graphics
{
	float D3D11ParticleManager::RandF()
	{
		return (float)(rand()) / (float)RAND_MAX;
	}

	float D3D11ParticleManager::RandF(float a, float b)
	{
		return a + RandF() * (b - a);
	}

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> D3D11ParticleManager::CreateRandomTexture1DSRV(ID3D11Device* device)
	{
		using namespace DirectX::SimpleMath;

		// 랜덤 백터 생성
		const size_t ELEMENT_COUNT = 1024u;

		std::vector<Vector4> randomValues(ELEMENT_COUNT);

		for (size_t i = 0; i < ELEMENT_COUNT; ++i)
		{
			randomValues[i].x = RandF(-1.0f, 1.0f);
			randomValues[i].y = RandF(-1.0f, 1.0f);
			randomValues[i].z = RandF(-1.0f, 1.0f);
			randomValues[i].w = RandF(-1.0f, 1.0f);
		}

		// 텍스처 생성
		D3D11_TEXTURE1D_DESC texDesc;
		texDesc.Width = ELEMENT_COUNT;
		texDesc.MipLevels = 1;
		texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		texDesc.Usage = D3D11_USAGE_IMMUTABLE;
		texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		texDesc.CPUAccessFlags = 0;
		texDesc.MiscFlags = 0;
		texDesc.ArraySize = 1;

		D3D11_SUBRESOURCE_DATA initData;
		initData.pSysMem = &randomValues[0];
		initData.SysMemPitch = ELEMENT_COUNT * sizeof(Vector4);
		initData.SysMemSlicePitch = 0;

		Microsoft::WRL::ComPtr<ID3D11Texture1D> randomTex;
		HR(device->CreateTexture1D(&texDesc, &initData, randomTex.GetAddressOf()));

		// 쉐이더 리소스 뷰 생성
		D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
		viewDesc.Format = texDesc.Format;
		viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1D; // 1D 텍스처
		viewDesc.Texture1D.MipLevels = texDesc.MipLevels;
		viewDesc.Texture1D.MostDetailedMip = 0;

		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> randomTexSRV;
		assert(randomTex != nullptr);
		HR(device->CreateShaderResourceView(randomTex.Get(), &viewDesc, randomTexSRV.GetAddressOf()));

		return randomTexSRV;
	}

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> D3D11ParticleManager::CreateRandomTexture2DSRV(ID3D11Device* device)
	{
		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Width = 1024;
		desc.Height = 1024;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.MipLevels = 1;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;

		float* values = new float[desc.Width * desc.Height * 4];
		float* ptr = values;
		for (UINT i = 0; i < desc.Width * desc.Height; i++)
		{
			ptr[0] = RandF(-1.0f, 1.0f);
			ptr[1] = RandF(-1.0f, 1.0f);
			ptr[2] = RandF(-1.0f, 1.0f);
			ptr[3] = RandF(-1.0f, 1.0f);
			ptr += 4;
		}

		D3D11_SUBRESOURCE_DATA data;
		data.pSysMem = values;
		data.SysMemPitch = desc.Width * 16;
		data.SysMemSlicePitch = 0;

		Microsoft::WRL::ComPtr<ID3D11Texture2D> randomTex;
		device->CreateTexture2D(&desc, &data, randomTex.GetAddressOf());

		delete[] values;

		D3D11_SHADER_RESOURCE_VIEW_DESC srv;
		srv.Format = desc.Format;
		srv.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srv.Texture2D.MipLevels = 1;
		srv.Texture2D.MostDetailedMip = 0;

		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> randomTexSRV;
		assert(randomTex != nullptr);
		device->CreateShaderResourceView(randomTex.Get(), &srv, randomTexSRV.GetAddressOf());

		return randomTexSRV;
	}

	int D3D11ParticleManager::Align(int value, int aligment)
	{
		return (value + aligment - 1) / aligment * aligment;
	}

	void D3D11ParticleManager::CalculateNumToEmit(std::shared_ptr<EmitterParams> emitter, std::shared_ptr<EmissionRate> emissionRate, float frameTime)
	{
		if (emissionRate->ParticlesPerSecond < 0.f)
		{
			return;
		}

		emissionRate->Accumlation += emissionRate->ParticlesPerSecond * frameTime;

		if (emissionRate->Accumlation > 1.f)
		{
			double integerPart;

			emissionRate->Accumlation = modf(emissionRate->Accumlation, &integerPart);
			emitter->NumToEmit = (int)integerPart;
		}
	}

	void D3D11ParticleManager::Initialize(const std::shared_ptr<D3D11Device> device, std::shared_ptr<D3D11ResourceManager> resourceManager, std::shared_ptr<D3D11CameraManager> cameraManager)
	{
		mDevice = device;
		mResourceManager = resourceManager;
		mCameraManager = cameraManager;

		mSwapChainRTV = mResourceManager->Get<D3D11RenderTargetView>(ED3D11RenderTargetViewType::Default);
		mDSV = mResourceManager->Get<D3D11DepthStencilView>(ED3D11DepthStencilViewType::Default);

		mLinearWrap = mResourceManager->Create<D3D11SamplerState>(ED3D11SamplerState::LinearWrap);
		mPointClamp = mResourceManager->Create<D3D11SamplerState>(ED3D11SamplerState::PointClamp);

		mFrameTime = 0.f;
		mbIsUdpatedFrameTime = false;

		mNumDeadParticlesOnInit = 0;
		mNumDeadParticlesAfterEmit = 0;
		mNumDeadParticlesAfterSimulation = 0;
		mNumActiveParticlesAfterSimulation = 0;

		mInitDeadListCS = std::make_shared<D3D11ComputeShader>(mDevice, L"./resource/internal/shader/ParticleDeadListInitCS.hlsl");
		mInitParticlesCS = std::make_shared<D3D11ComputeShader>(mDevice, L"./resource/internal/shader/ParticleInitCS.hlsl");
		mEmitCS = std::make_shared<D3D11ComputeShader>(mDevice, L"./resource/internal/shader/ParticleEmitCS.hlsl");
		mSimulateCS = std::make_shared<D3D11ComputeShader>(mDevice, L"./resource/internal/shader/ParticleSimulateCS.hlsl");

		std::shared_ptr<PipelineState> pipelineState = std::make_shared<PipelineState>(nullptr, nullptr, nullptr);
		auto particleRenderVS = std::make_shared<D3D11VertexShader>(mDevice, L"./resource/internal/shader/ParticleVS.hlsl");
		auto particleRednerPS = std::make_shared<D3D11PixelShader>(mDevice, L"./resource/internal/shader/ParticlePS.hlsl");
		auto particleRednerGS = std::make_shared<D3D11GeometryShader>(mDevice, L"./resource/internal/shader/ParticleGS.hlsl");
		mRenderProgram = std::make_shared<ShaderProgram>(mDevice, particleRenderVS, particleRednerGS, particleRednerPS, pipelineState);

		mRandomTextureSRV = CreateRandomTexture2DSRV(mDevice->GetDevice().Get());

		// 글로벌 파티클 풀 생성
		// 캐시 히트를 높이기 위해 A는 랜더링 관련, B는 시뮬레이션 관련 데이터를 다룬다.
		D3D11_BUFFER_DESC desc;
		desc.ByteWidth = sizeof(GPUParticlePartA) * D3D11ParticleManager::MAX_PARTICLE;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		desc.StructureByteStride = sizeof(GPUParticlePartA);
		mDevice->GetDevice()->CreateBuffer(&desc, nullptr, mParticleBufferA.GetAddressOf());

		desc.ByteWidth = sizeof(GPUParticlePartB) * MAX_PARTICLE;
		desc.StructureByteStride = sizeof(GPUParticlePartB);
		mDevice->GetDevice()->CreateBuffer(&desc, nullptr, mParticleBufferB.GetAddressOf());

		D3D11_SHADER_RESOURCE_VIEW_DESC srv;
		srv.Format = DXGI_FORMAT_UNKNOWN;
		srv.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		srv.Buffer.ElementOffset = 0;
		srv.Buffer.ElementWidth = MAX_PARTICLE;
		mDevice->GetDevice()->CreateShaderResourceView(mParticleBufferA.Get(), &srv, mParticleBufferASRV.GetAddressOf());

		D3D11_UNORDERED_ACCESS_VIEW_DESC uav;
		uav.Format = DXGI_FORMAT_UNKNOWN;
		uav.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		uav.Buffer.FirstElement = 0;
		uav.Buffer.NumElements = MAX_PARTICLE;
		uav.Buffer.Flags = 0;
		mDevice->GetDevice()->CreateUnorderedAccessView(mParticleBufferA.Get(), &uav, mParticleBufferAUAV.GetAddressOf());
		mDevice->GetDevice()->CreateUnorderedAccessView(mParticleBufferB.Get(), &uav, mParticleBufferBUAV.GetAddressOf());

		// 파티클의 뷰 스페이스 위치는 시뮬레이션 중 캐시되므로 버퍼를 할당한다.
		desc.ByteWidth = 16 * MAX_PARTICLE;
		desc.StructureByteStride = 16;
		mDevice->GetDevice()->CreateBuffer(&desc, 0, mViewSpaceParticlePositions.GetAddressOf());
		mDevice->GetDevice()->CreateShaderResourceView(mViewSpaceParticlePositions.Get(), &srv, mViewSpaceParticlePositionsSRV.GetAddressOf());
		mDevice->GetDevice()->CreateUnorderedAccessView(mViewSpaceParticlePositions.Get(), &uav, mViewSpaceParticlePositionsUAV.GetAddressOf());

		// 재계산을 방지하기 위해 파티클의 최대 반경을 캐시한다.
		// 줄무늬 파티클에서만 처리되는 연산으로 X, Y 최대 반경을 저장한다.
		desc.ByteWidth = 4 * MAX_PARTICLE;
		desc.StructureByteStride = 4;
		mDevice->GetDevice()->CreateBuffer(&desc, 0, mMaxRadiusBuffer.GetAddressOf());
		mDevice->GetDevice()->CreateShaderResourceView(mMaxRadiusBuffer.Get(), &srv, mMaxRadiusBufferSRV.GetAddressOf());
		mDevice->GetDevice()->CreateUnorderedAccessView(mMaxRadiusBuffer.Get(), &uav, mMaxRadiusBufferUAV.GetAddressOf());

		// 죽은 파티클 인덱스 목록 생성
		desc.ByteWidth = sizeof(UINT) * MAX_PARTICLE;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		desc.StructureByteStride = sizeof(UINT);
		mDevice->GetDevice()->CreateBuffer(&desc, nullptr, mDeadListBuffer.GetAddressOf());
		uav.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_APPEND; // 추가 버퍼로 생성
		mDevice->GetDevice()->CreateUnorderedAccessView(mDeadListBuffer.Get(), &uav, mDeadListUAV.GetAddressOf());

		struct IndexBufferElement
		{
			float		distanceSquard;	// 카메라와의 제곱 거리
			float		index;		// 전역 파티클 인덱스
		};

		// 정렬할 살아있는 파티클의 인덱스 버퍼
		desc.ByteWidth = sizeof(IndexBufferElement) * MAX_PARTICLE;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		desc.StructureByteStride = sizeof(IndexBufferElement);
		mDevice->GetDevice()->CreateBuffer(&desc, nullptr, mAliveIndexBuffer.GetAddressOf());

		srv.Format = DXGI_FORMAT_UNKNOWN;
		srv.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		srv.Buffer.ElementOffset = 0;
		srv.Buffer.ElementWidth = MAX_PARTICLE;
		mDevice->GetDevice()->CreateShaderResourceView(mAliveIndexBuffer.Get(), &srv, mAliveIndexBufferSRV.GetAddressOf());

		uav.Buffer.NumElements = MAX_PARTICLE;
		uav.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_COUNTER;
		uav.Format = DXGI_FORMAT_UNKNOWN;
		mDevice->GetDevice()->CreateUnorderedAccessView(mAliveIndexBuffer.Get(), &uav, mAliveIndexBufferUAV.GetAddressOf());

		// 간접 호출을 위한 상수 버퍼와 UAV
		ZeroMemory(&desc, sizeof(desc));
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
		desc.ByteWidth = 5 * sizeof(UINT);
		desc.MiscFlags = D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS;
		mDevice->GetDevice()->CreateBuffer(&desc, nullptr, mIndirectDrawArgsBuffer.GetAddressOf());

		ZeroMemory(&uav, sizeof(uav));
		uav.Format = DXGI_FORMAT_R32_UINT;
		uav.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		uav.Buffer.FirstElement = 0;
		uav.Buffer.NumElements = 5;
		uav.Buffer.Flags = 0;
		mDevice->GetDevice()->CreateUnorderedAccessView(mIndirectDrawArgsBuffer.Get(), &uav, mIndirectDrawArgsBufferUAV.GetAddressOf());

		// GPU 파티클 카운터를 읽어올 디버깅용 스테이징 버퍼
#if _DEBUG
		ZeroMemory(&desc, sizeof(desc));
		desc.Usage = D3D11_USAGE_STAGING;
		desc.BindFlags = 0;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		desc.ByteWidth = sizeof(UINT);
		mDevice->GetDevice()->CreateBuffer(&desc, nullptr, mDebugCounterBuffer.GetAddressOf());
#endif

		ZeroMemory(&desc, sizeof(desc));
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.ByteWidth = sizeof(EmitterConstantBuffer);
		mDevice->GetDevice()->CreateBuffer(&desc, nullptr, mEmitterCB.GetAddressOf());

		ZeroMemory(&desc, sizeof(desc));
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = 0;
		desc.ByteWidth = 4 * sizeof(UINT);
		mDevice->GetDevice()->CreateBuffer(&desc, nullptr, mDeadListCB.GetAddressOf());
		mDevice->GetDevice()->CreateBuffer(&desc, nullptr, mActiveListCB.GetAddressOf());

		D3D11_BUFFER_DESC cbDesc;
		ZeroMemory(&cbDesc, sizeof(cbDesc));
		cbDesc.Usage = D3D11_USAGE_DYNAMIC;
		cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbDesc.ByteWidth = sizeof(PER_FRAME_CONSTANT_BUFFER);
		mDevice->GetDevice()->CreateBuffer(&cbDesc, nullptr, mPerFrameCB.GetAddressOf());
	}

	void D3D11ParticleManager::Reset()
	{
		mInitDeadListCS->Bind(mDevice);
		UINT initialCount[] = { 0 };
		mDevice->GetDeviceContext()->CSSetUnorderedAccessViews(0, 1, mDeadListUAV.GetAddressOf(), initialCount);
		mDevice->GetDeviceContext()->Dispatch(Align(MAX_PARTICLE, 256) / 256, 1, 1);

#if _DEBUG
		mNumDeadParticlesOnInit = readCounter(mDeadListUAV.Get());
#endif
		ID3D11UnorderedAccessView* uavs[] = { mParticleBufferAUAV.Get(), mParticleBufferBUAV.Get() };
		UINT initialCounts[] = { (UINT)-1, (UINT)-1 };

		mInitParticlesCS->Bind(mDevice);
		mDevice->GetDeviceContext()->CSSetUnorderedAccessViews(0, ARRAYSIZE(uavs), uavs, initialCounts);
		mDevice->GetDeviceContext()->Dispatch(Align(MAX_PARTICLE, 256) / 256, 1, 1);

		mbIsReset = false;
	}

	void D3D11ParticleManager::UpdatePerFrame()
	{
		if (mbIsUdpatedFrameTime)
		{
			mGlobalConstantBuffer.m_ElapsedTime += mFrameTime;
		}
		mGlobalConstantBuffer.m_FrameIndex++;

		DirectX::XMMATRIX mView = mCameraManager->GetViewMatrix(ECameraType::Player);
		DirectX::XMMATRIX mProj = mCameraManager->GetProjectionMatrix(ECameraType::Player);
		DirectX::XMMATRIX mViewProjection = mView * mProj;

		mGlobalConstantBuffer.m_ViewProjection = DirectX::XMMatrixTranspose(mViewProjection);
		mGlobalConstantBuffer.m_View = DirectX::XMMatrixTranspose(mView);
		mGlobalConstantBuffer.m_Projection = DirectX::XMMatrixTranspose(mProj);

		DirectX::XMMATRIX viewProjInv = DirectX::XMMatrixInverse(nullptr, mViewProjection);
		mGlobalConstantBuffer.m_ViewProjInv = DirectX::XMMatrixTranspose(viewProjInv);

		DirectX::XMMATRIX viewInv = DirectX::XMMatrixInverse(nullptr, mView);
		mGlobalConstantBuffer.m_ViewInv = DirectX::XMMatrixTranspose(viewInv);

		DirectX::XMMATRIX projInv = DirectX::XMMatrixInverse(nullptr, mProj);
		mGlobalConstantBuffer.m_ProjectionInv = DirectX::XMMatrixTranspose(projInv);

		mGlobalConstantBuffer.m_SunDirectionVS = DirectX::XMVector4Transform(mGlobalConstantBuffer.m_SunDirection, mView);

		mGlobalConstantBuffer.m_EyePosition = mCameraManager->GetPosition(ECameraType::Player);

		mGlobalConstantBuffer.m_FrameTime = mFrameTime;

		mGlobalConstantBuffer.m_AlphaThreshold = (float)97 / 100.0f;
		mGlobalConstantBuffer.m_CollisionThickness = (float)40 * 0.1f;

		mGlobalConstantBuffer.m_CollisionsEnabled = 0;
		mGlobalConstantBuffer.m_EnableSleepState = 0;
		mGlobalConstantBuffer.m_ShowSleepingParticles = 0;

		// temp
		DirectX::XMVECTOR spawnPosition = DirectX::XMVectorSet(2.0f, 70.0f, 26.0f, 1.0f);
		mGlobalConstantBuffer.m_StartColor[0] = DirectX::XMVectorSet(10.0f, 10.0f, 0.0f, 1.0f);
		mGlobalConstantBuffer.m_EndColor[0] = DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 1.0f);
		mGlobalConstantBuffer.m_EmitterLightingCenter[0] = spawnPosition;

		mGlobalConstantBuffer.m_StartColor[1] = DirectX::XMVectorSet(0.5f, 0.5f, 0.5f, 1.0f);
		mGlobalConstantBuffer.m_EndColor[1] = DirectX::XMVectorSet(0.6f, 0.6f, 0.65f, 1.0f);
		mGlobalConstantBuffer.m_EmitterLightingCenter[1] = spawnPosition;

		D3D11_MAPPED_SUBRESOURCE MappedResource;
		mDevice->GetDeviceContext()->Map(mPerFrameCB.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
		memcpy(MappedResource.pData, &mGlobalConstantBuffer, sizeof(mGlobalConstantBuffer));
		mDevice->GetDeviceContext()->Unmap(mPerFrameCB.Get(), 0);
	}

	void D3D11ParticleManager::BindFrameCB()
	{
		mDevice->GetDeviceContext()->VSSetConstantBuffers(0, 1, mPerFrameCB.GetAddressOf());
		mDevice->GetDeviceContext()->PSSetConstantBuffers(0, 1, mPerFrameCB.GetAddressOf());
		mDevice->GetDeviceContext()->GSSetConstantBuffers(0, 1, mPerFrameCB.GetAddressOf());
		mDevice->GetDeviceContext()->CSSetConstantBuffers(0, 1, mPerFrameCB.GetAddressOf());

		mLinearWrap->Bind(mDevice, 0, ED3D11ShaderType::Pixelshader);
		mLinearWrap->Bind(mDevice, 0, ED3D11ShaderType::ComputeShader);

		mPointClamp->Bind(mDevice, 1, ED3D11ShaderType::Pixelshader);
		mPointClamp->Bind(mDevice, 1, ED3D11ShaderType::ComputeShader);
	}

	void D3D11ParticleManager::Emit()
	{
		if (mbIsReset)
		{
			Reset();
		}

		// numToEmit update
		if (!mbIsUdpatedFrameTime)
		{
			return;
		}

		mbIsUdpatedFrameTime = false;

		for (const auto& [key, emissionRate] : mEmissionRates)
		{
			auto emitter = mEmitters.find(key);
			assert(emitter != mEmitters.end());

			CalculateNumToEmit(emitter->second, emissionRate, mFrameTime);
		}

		ID3D11UnorderedAccessView* uavs[] = { mParticleBufferAUAV.Get(), mParticleBufferBUAV.Get(), mDeadListUAV.Get() };
		UINT initialCounts[] = { (UINT)-1, (UINT)-1, (UINT)-1 };
		ID3D11Buffer* buffers[] = { mEmitterCB.Get(), mDeadListCB.Get() };
		ID3D11ShaderResourceView* srvs[] = { mRandomTextureSRV.Get() };

		mEmitCS->Bind(mDevice);
		mDevice->GetDeviceContext()->CSSetUnorderedAccessViews(0, ARRAYSIZE(uavs), uavs, initialCounts);
		mDevice->GetDeviceContext()->CSSetConstantBuffers(1, ARRAYSIZE(buffers), buffers);
		mDevice->GetDeviceContext()->CSSetShaderResources(0, ARRAYSIZE(srvs), srvs);

		size_t index = 0u;
		for (const auto& [key, emitter] : mEmitters)
		{
			if (emitter->NumToEmit <= 0)
			{
				continue;
			}

			D3D11_MAPPED_SUBRESOURCE mappedResource;
			mDevice->GetDeviceContext()->Map(mEmitterCB.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
			EmitterConstantBuffer* data = (EmitterConstantBuffer*)mappedResource.pData;
			data->EmitterPosition = emitter->Position;
			data->EmitterVelocity = emitter->Velocity;
			data->MaxParticlesThisFrame = emitter->NumToEmit;
			data->ParticleLifeSpan = emitter->ParticleLifeSpan;
			data->StartSize = emitter->StartSize;
			data->EndSize = emitter->EndSize;
			data->PositionVariance = emitter->PositionVariance;
			data->VelocityVariance = emitter->VelocityVariance;
			data->Mass = emitter->Mass;
			data->Index = index++;
			data->Streaks = emitter->Streaks ? 1 : 0;
			data->TextureIndex = emitter->TextureIndex;
			mDevice->GetDeviceContext()->Unmap(mEmitterCB.Get(), 0);

			mDevice->GetDeviceContext()->CopyStructureCount(mDeadListCB.Get(), 0, mDeadListUAV.Get());

			int numThreadGroups = Align(emitter->NumToEmit, 1024) / 1024;
			mDevice->GetDeviceContext()->Dispatch(numThreadGroups, 1, 1);
		}

#if _DEBUG
		mNumDeadParticlesAfterEmit = readCounter(mDeadListUAV.Get());
#endif
	}

	void D3D11ParticleManager::Simulate()
	{
		mSimulateCS->Bind(mDevice);

		// depathStencil bind

		ID3D11UnorderedAccessView* uavs[] = { mParticleBufferAUAV.Get(), mParticleBufferBUAV.Get(), mDeadListUAV.Get(), mAliveIndexBufferUAV.Get(),
			mViewSpaceParticlePositionsUAV.Get(), mMaxRadiusBufferUAV.Get(), mIndirectDrawArgsBufferUAV.Get() };
		UINT initialCounts[] = { (UINT)-1, (UINT)-1, (UINT)-1, 0, (UINT)-1, (UINT)-1, (UINT)-1 };
		ID3D11ShaderResourceView* srvs[] = { mDepthSRV.Get() };

		mDevice->GetDeviceContext()->CSSetUnorderedAccessViews(0, ARRAYSIZE(uavs), uavs, initialCounts);
		mDevice->GetDeviceContext()->CSSetShaderResources(0, ARRAYSIZE(srvs), srvs);
		mDevice->GetDeviceContext()->Dispatch(Align(MAX_PARTICLE, 256) / 256, 1, 1);

		// depthSRV unbind

		ZeroMemory(srvs, sizeof(srvs));
		mDevice->GetDeviceContext()->CSSetShaderResources(0, ARRAYSIZE(srvs), srvs);
		ZeroMemory(uavs, sizeof(uavs));
		mDevice->GetDeviceContext()->CSSetUnorderedAccessViews(0, ARRAYSIZE(uavs), uavs, nullptr);

		mDevice->GetDeviceContext()->CopyStructureCount(mActiveListCB.Get(), 0, mAliveIndexBufferUAV.Get());

		// 디버그일 경우 카운트 읽어오기
#if _DEBUG
		mNumDeadParticlesAfterSimulation = readCounter(mDeadListUAV.Get());
		mNumActiveParticlesAfterSimulation = readCounter(mAliveIndexBufferUAV.Get());
#endif
	}

	void D3D11ParticleManager::Render()
	{
		// sort

		// renderTarget binding
		mSwapChainRTV->Bind(mDevice, mDSV);
		mRenderProgram->Bind(mDevice);

		ID3D11Buffer* vb = nullptr;
		UINT stride = 0;
		UINT offset = 0;
		mDevice->GetDeviceContext()->IASetVertexBuffers(0, 1, &vb, &stride, &offset); // null VB 바인딩
		mDevice->GetDeviceContext()->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);
		mDevice->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

		ID3D11ShaderResourceView* vs_srv[] = { mParticleBufferASRV.Get(), mViewSpaceParticlePositionsSRV.Get(), mAliveIndexBufferSRV.Get() };
		ID3D11ShaderResourceView* ps_srv[] = { mDepthSRV.Get() };

		mDevice->GetDeviceContext()->VSSetShaderResources(0, ARRAYSIZE(vs_srv), vs_srv);
		mDevice->GetDeviceContext()->PSSetShaderResources(1, ARRAYSIZE(ps_srv), ps_srv);
		mDevice->GetDeviceContext()->VSSetConstantBuffers(3, 1, mActiveListCB.GetAddressOf());

		mDevice->GetDeviceContext()->DrawInstancedIndirect(mIndirectDrawArgsBuffer.Get(), 0);

		ZeroMemory(vs_srv, sizeof(vs_srv));
		mDevice->GetDeviceContext()->VSSetShaderResources(0, ARRAYSIZE(vs_srv), vs_srv);
		ZeroMemory(ps_srv, sizeof(ps_srv));
		mDevice->GetDeviceContext()->PSSetShaderResources(1, ARRAYSIZE(ps_srv), ps_srv);
	}

	void D3D11ParticleManager::AddEmitter(size_t id, EmitterParams emitter, EmissionRate emissionRate)
	{
		mEmitters.insert({ id, std::make_shared<EmitterParams>(emitter) });
		mEmissionRates.insert({ id, std::make_shared<EmissionRate>(emissionRate) });
	}

	void D3D11ParticleManager::DeleteEmitter(size_t id)
	{
		mEmitters.erase(id);
		mEmissionRates.erase(id);
	}

	void D3D11ParticleManager::SetFrameTime(float frameTime)
	{
		mbIsUdpatedFrameTime = true;
		mFrameTime = frameTime;
	}

	float D3D11ParticleManager::GetFrameTime() const
	{
		return mFrameTime;
	}

	int D3D11ParticleManager::readCounter(ID3D11UnorderedAccessView* uav)
	{
		int count = 0;

		mDevice->GetDeviceContext()->CopyStructureCount(mDebugCounterBuffer.Get(), 0, uav);

		D3D11_MAPPED_SUBRESOURCE mappedResource;
		mDevice->GetDeviceContext()->Map(mDebugCounterBuffer.Get(), 0, D3D11_MAP_READ, 0, &mappedResource);
		count = *(int*)mappedResource.pData;
		mDevice->GetDeviceContext()->Unmap(mDebugCounterBuffer.Get(), 0);

		return count;
	}
}