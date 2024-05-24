#include "D3D11ParticleManager.h"
#include "D3D11Common.h"
#include "ManagementCommon.h"
#include "RenderObject.h"

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

	void D3D11ParticleManager::CalculateNumToEmit(std::shared_ptr<ParticleObject> particleObject, float frameTime)
	{
		ParticleInfo& particleInfo = particleObject->GetInfo();
		particleInfo.MainModuleData.NumToEmit = 0;

		if (particleInfo.EmissionModuleData.ParticlesPerSecond < 0.f)
		{
			return;
		}

		particleInfo.EmissionModuleData.Accumlation += particleInfo.EmissionModuleData.ParticlesPerSecond * frameTime;

		if (particleInfo.EmissionModuleData.Accumlation > 1.f)
		{
			double integerPart;

			particleInfo.EmissionModuleData.Accumlation = modf(particleInfo.EmissionModuleData.Accumlation, &integerPart);
			particleInfo.MainModuleData.NumToEmit = (int)integerPart;
		}
	}

	void D3D11ParticleManager::Excute()
	{
		if (!mbIsUdpatedFrameTime)
		{
			return;
		}

		mbIsUdpatedFrameTime = false;

		UpdatePerFrame();
		BindFrameCB();

		for (auto& [key, particleObject] : mParticleObjects)
		{
			Emit(particleObject);
			Simulate(particleObject);
			Render(particleObject);
		}
	}

	void D3D11ParticleManager::Initialize(const std::shared_ptr<D3D11Device> device, std::shared_ptr<D3D11ResourceManager> resourceManager, std::shared_ptr<D3D11CameraManager> cameraManager)
	{
		mDevice = device;
		mResourceManager = resourceManager;
		mCameraManager = cameraManager;

		mSwapChainRTV = mResourceManager->Get<D3D11RenderTargetView>(ED3D11RenderTargetViewType::Default);
		mNoneDSV = mResourceManager->Get<D3D11DepthStencilView>(ED3D11DepthStencilViewType::None);
		mDSV = mResourceManager->Get<D3D11DepthStencilView>(ED3D11DepthStencilViewType::Default);
		mDepthSRV = std::make_shared<D3D11ShaderResourceView>(mDevice, mDSV)->GetSRV();

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

		m_pCSSortStep = std::make_shared<D3D11ComputeShader>(mDevice, L"./resource/internal/shader/ParticleBitonicSortStep.hlsl");
		m_pCSSort512 = std::make_shared<D3D11ComputeShader>(mDevice, L"./resource/internal/shader/ParticleBitonicSortLDS.hlsl");
		m_pCSSortInner512 = std::make_shared<D3D11ComputeShader>(mDevice, L"./resource/internal/shader/ParticleBitonicInnerSort.hlsl");
		m_pCSInitArgs = std::make_shared<D3D11ComputeShader>(mDevice, L"./resource/internal/shader/ParticleInitDispatchArgs.hlsl");

		auto additive = mResourceManager->Create<D3D11BlendState>(ED3D11BlendState::Additive);
		std::shared_ptr<PipelineState> pipelineState = std::make_shared<PipelineState>(nullptr, nullptr, additive);
		auto particleRenderVS = std::make_shared<D3D11VertexShader>(mDevice, L"./resource/internal/shader/ParticleVS.hlsl");
		auto particleRednerPS = std::make_shared<D3D11PixelShader>(mDevice, L"./resource/internal/shader/ParticlePS.hlsl");
		auto particleRednerGS = std::make_shared<D3D11GeometryShader>(mDevice, L"./resource/internal/shader/ParticleGS.hlsl");
		mRenderProgram = std::make_shared<ShaderProgram>(mDevice, particleRenderVS, particleRednerGS, particleRednerPS, pipelineState);

		mRandomTextureSRV = CreateRandomTexture2DSRV(mDevice->GetDevice().Get());
		mAtlasSRV = mResourceManager->Create<D3D11Texture>(L"./resource/example/texture/atlas.dds");

		D3D11_BUFFER_DESC desc;
		D3D11_UNORDERED_ACCESS_VIEW_DESC uav;
		D3D11_SHADER_RESOURCE_VIEW_DESC srv;

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

		ZeroMemory(&desc, sizeof(desc));
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.ByteWidth = sizeof(int4);
		mDevice->GetDevice()->CreateBuffer(&desc, nullptr, mDispatchInfoCB.GetAddressOf());

		ZeroMemory(&desc, sizeof(desc));
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.ByteWidth = sizeof(PER_FRAME_CONSTANT_BUFFER);
		mDevice->GetDevice()->CreateBuffer(&desc, nullptr, mPerFrameCB.GetAddressOf());

		// 정렬 자원
		ZeroMemory(&desc, sizeof(desc));
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
		desc.ByteWidth = 4 * sizeof(UINT);
		desc.MiscFlags = D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS;
		mDevice->GetDevice()->CreateBuffer(&desc, nullptr, mIndirectSortArgsBuffer.GetAddressOf());

		ZeroMemory(&uav, sizeof(uav));
		uav.Format = DXGI_FORMAT_R32_UINT;
		uav.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		uav.Buffer.FirstElement = 0;
		uav.Buffer.NumElements = 4;
		uav.Buffer.Flags = 0;
		mDevice->GetDevice()->CreateUnorderedAccessView(mIndirectSortArgsBuffer.Get(), &uav, mIndirectSortArgsBufferUAV.GetAddressOf());
	}

	void D3D11ParticleManager::Reset(std::shared_ptr<ParticleObject> particleObject)
	{
		mInitDeadListCS->Bind(mDevice);
		UINT initialCount[] = { 0 };
		mDevice->GetDeviceContext()->CSSetUnorderedAccessViews(0, 1, particleObject->mDeadListUAV.GetAddressOf(), initialCount);
		mDevice->GetDeviceContext()->Dispatch(Align(ParticleInfo::MAX_PARTICLE_COUNT, 256) / 256, 1, 1);

#if _DEBUG
		particleObject->mDebugInfo.NumDeadParticlesOnInit = readCounter(particleObject->mDeadListUAV.Get());
#endif
		ID3D11UnorderedAccessView* uavs[] = { particleObject->mParticleBufferAUAV.Get(), particleObject->mParticleBufferBUAV.Get() };
		UINT initialCounts[] = { (UINT)-1, (UINT)-1 };

		mInitParticlesCS->Bind(mDevice);
		mDevice->GetDeviceContext()->CSSetUnorderedAccessViews(0, ARRAYSIZE(uavs), uavs, initialCounts);
		mDevice->GetDeviceContext()->Dispatch(Align(ParticleInfo::MAX_PARTICLE_COUNT, 256) / 256, 1, 1);

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

		mGlobalConstantBuffer.m_SunDirectionView = DirectX::XMVector4Transform(mGlobalConstantBuffer.m_SunDirection, mView);

		mGlobalConstantBuffer.m_EyePosition = mCameraManager->GetPosition(ECameraType::Player);

		mGlobalConstantBuffer.m_FrameTime = mFrameTime;

		mGlobalConstantBuffer.m_AlphaThreshold = (float)97 / 100.0f;
		mGlobalConstantBuffer.m_CollisionThickness = (float)40 * 0.1f;

		mGlobalConstantBuffer.m_CollisionsEnabled = 0;
		mGlobalConstantBuffer.m_EnableSleepState = 0;
		mGlobalConstantBuffer.m_ShowSleepingParticles = 0;

		mGlobalConstantBuffer.m_ScreenWidth = mDevice->GetWidth();
		mGlobalConstantBuffer.m_ScreenHeight = mDevice->GetHeight();
		mGlobalConstantBuffer.m_SunDirection = { 0, -1, 0 };
		mGlobalConstantBuffer.m_SunColor = { 1, 1,1, 1 };
		mGlobalConstantBuffer.m_AmbientColor = { 0.1f, 0.1f, 0.1f };

		mGlobalConstantBuffer.m_CollisionsEnabled = true;
		mGlobalConstantBuffer.m_CollisionThickness = 4.f;

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

	void D3D11ParticleManager::Emit(std::shared_ptr<ParticleObject> particleObject)
	{
		using namespace DirectX::SimpleMath;

		if (particleObject->GetIsReset())
		{
			Reset(particleObject);
			particleObject->SetIsReset(false);
		}

		CalculateNumToEmit(particleObject, mFrameTime);

		const auto& particleEmitter = particleObject->GetInfo();

		if (particleEmitter.MainModuleData.NumToEmit <= 0)
		{
			return;
		}

		ID3D11UnorderedAccessView* uavs[] = { particleObject->mParticleBufferAUAV.Get(), particleObject->mParticleBufferBUAV.Get(), particleObject->mDeadListUAV.Get() };
		UINT initialCounts[] = { (UINT)-1, (UINT)-1, (UINT)-1 };
		ID3D11Buffer* buffers[] = { mEmitterCB.Get(), mDeadListCB.Get() };
		ID3D11ShaderResourceView* srvs[] = { mRandomTextureSRV.Get() };

		mEmitCS->Bind(mDevice);
		mDevice->GetDeviceContext()->CSSetUnorderedAccessViews(0, ARRAYSIZE(uavs), uavs, initialCounts);
		mDevice->GetDeviceContext()->CSSetConstantBuffers(1, ARRAYSIZE(buffers), buffers);
		mDevice->GetDeviceContext()->CSSetShaderResources(0, ARRAYSIZE(srvs), srvs);

		D3D11_MAPPED_SUBRESOURCE mappedResource;
		mDevice->GetDeviceContext()->Map(mEmitterCB.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		EmitterConstantBuffer* data = (EmitterConstantBuffer*)mappedResource.pData;
		data->Transform = particleObject->GetTransform();
		data->StartLifeTimeStartSpeed.x = particleEmitter.MainModuleData.StartLifeTime[0];
		data->StartLifeTimeStartSpeed.y = particleEmitter.MainModuleData.StartLifeTimeOption == ParticleInfo::EOption::Constant ? particleEmitter.MainModuleData.StartLifeTime[0] : particleEmitter.MainModuleData.StartLifeTime[1];
		data->StartLifeTimeStartSpeed.z = particleEmitter.MainModuleData.StartSpeed[0];
		data->StartLifeTimeStartSpeed.w = particleEmitter.MainModuleData.StartSpeedOption == ParticleInfo::EOption::Constant ? particleEmitter.MainModuleData.StartSpeed[0] : particleEmitter.MainModuleData.StartSpeed[1];
		data->StartRotationStartSize.x = particleEmitter.MainModuleData.StartRotation[0];
		data->StartRotationStartSize.y = particleEmitter.MainModuleData.StartRotationOption == ParticleInfo::EOption::Constant ? particleEmitter.MainModuleData.StartRotation[0] : particleEmitter.MainModuleData.StartRotation[1];
		data->StartRotationStartSize.z = particleEmitter.MainModuleData.StartSize[0];
		data->StartRotationStartSize.w = particleEmitter.MainModuleData.StartSizeOption == ParticleInfo::EOption::Constant ? particleEmitter.MainModuleData.StartSize[0] : particleEmitter.MainModuleData.StartSize[1];
		data->StartColor0 = particleEmitter.MainModuleData.StartColor[0];
		data->StartColor1 = particleEmitter.MainModuleData.StartColorOption == ParticleInfo::EOption::Constant ? particleEmitter.MainModuleData.StartColor[0] : particleEmitter.MainModuleData.StartColor[1];
		data->GravitySimulationSpeedMaxParticleThisFrame.x = particleEmitter.MainModuleData.GravityModifier[0];
		data->GravitySimulationSpeedMaxParticleThisFrame.y = particleEmitter.MainModuleData.GravityModifierOption == ParticleInfo::EOption::Constant ? particleEmitter.MainModuleData.GravityModifier[0] : particleEmitter.MainModuleData.GravityModifier[1];
		data->GravitySimulationSpeedMaxParticleThisFrame.z = particleEmitter.MainModuleData.SimulationSpeed;
		data->GravitySimulationSpeedMaxParticleThisFrame.w = particleEmitter.MainModuleData.MaxParticleCount;

		mDevice->GetDeviceContext()->Unmap(mEmitterCB.Get(), 0);
		mDevice->GetDeviceContext()->CopyStructureCount(mDeadListCB.Get(), 0, particleObject->mDeadListUAV.Get());

		int numThreadGroups = Align(particleEmitter.MainModuleData.NumToEmit, 1024) / 1024;
		mDevice->GetDeviceContext()->Dispatch(numThreadGroups, 1, 1);

#if _DEBUG
		particleObject->mDebugInfo.NumDeadParticlesAfterEmit = readCounter(particleObject->mDeadListUAV.Get());
#endif
	}

	void D3D11ParticleManager::Simulate(std::shared_ptr<ParticleObject> particleObject)
	{
		mDevice->GetDeviceContext()->OMSetRenderTargets(0, nullptr, nullptr);

		mSimulateCS->Bind(mDevice);

		// depathStencil bind

		ID3D11UnorderedAccessView* uavs[] = { particleObject->mParticleBufferAUAV.Get(), particleObject->mParticleBufferBUAV.Get(), particleObject->mDeadListUAV.Get(), particleObject->mAliveIndexBufferUAV.Get(),
			particleObject->mViewSpaceParticlePositionsUAV.Get(), mIndirectDrawArgsBufferUAV.Get() };
		UINT initialCounts[] = { (UINT)-1, (UINT)-1, (UINT)-1, 0, (UINT)-1, (UINT)-1, (UINT)-1 };
		ID3D11ShaderResourceView* srvs[] = { mDepthSRV.Get() };

		mDevice->GetDeviceContext()->CSSetUnorderedAccessViews(0, ARRAYSIZE(uavs), uavs, initialCounts);
		mDevice->GetDeviceContext()->CSSetShaderResources(0, ARRAYSIZE(srvs), srvs);
		mDevice->GetDeviceContext()->Dispatch(Align(ParticleInfo::MAX_PARTICLE_COUNT, 256) / 256, 1, 1);

		// depthSRV unbind

		ZeroMemory(srvs, sizeof(srvs));
		mDevice->GetDeviceContext()->CSSetShaderResources(0, ARRAYSIZE(srvs), srvs);
		ZeroMemory(uavs, sizeof(uavs));
		mDevice->GetDeviceContext()->CSSetUnorderedAccessViews(0, ARRAYSIZE(uavs), uavs, nullptr);

		mDevice->GetDeviceContext()->CopyStructureCount(mActiveListCB.Get(), 0, particleObject->mAliveIndexBufferUAV.Get());

		// 디버그일 경우 카운트 읽어오기
#if _DEBUG
		particleObject->mDebugInfo.NumDeadParticlesAfterSimulation = readCounter(particleObject->mDeadListUAV.Get());
		particleObject->mDebugInfo.NumActiveParticlesAfterSimulation = readCounter(particleObject->mAliveIndexBufferUAV.Get());
#endif
	}

	void D3D11ParticleManager::Render(std::shared_ptr<ParticleObject> particleObject)
	{
		// renderTarget binding
		mSwapChainRTV->Bind(mDevice, mNoneDSV);
		mRenderProgram->Bind(mDevice);

		ID3D11Buffer* vb = nullptr;
		UINT stride = 0;
		UINT offset = 0;
		mDevice->GetDeviceContext()->IASetVertexBuffers(0, 1, &vb, &stride, &offset); // null VB 바인딩
		mDevice->GetDeviceContext()->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);
		mDevice->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

		ID3D11ShaderResourceView* vs_srv[] = { particleObject->mParticleBufferASRV.Get(), particleObject->mViewSpaceParticlePositionsSRV.Get(), particleObject->mAliveIndexBufferSRV.Get() };
		ID3D11ShaderResourceView* ps_srv[] = { mDepthSRV.Get() };

		mDevice->GetDeviceContext()->VSSetShaderResources(0, ARRAYSIZE(vs_srv), vs_srv);
		mAtlasSRV->Bind(mDevice, 0, ED3D11ShaderType::Pixelshader);
		mDevice->GetDeviceContext()->PSSetShaderResources(1, ARRAYSIZE(ps_srv), ps_srv);
		mDevice->GetDeviceContext()->VSSetConstantBuffers(3, 1, mActiveListCB.GetAddressOf());

		mDevice->GetDeviceContext()->DrawInstancedIndirect(mIndirectDrawArgsBuffer.Get(), 0);

		ZeroMemory(vs_srv, sizeof(vs_srv));
		mDevice->GetDeviceContext()->VSSetShaderResources(0, ARRAYSIZE(vs_srv), vs_srv);
		ZeroMemory(ps_srv, sizeof(ps_srv));
		mDevice->GetDeviceContext()->PSSetShaderResources(1, ARRAYSIZE(ps_srv), ps_srv);
	}

	void D3D11ParticleManager::AddEmitter(size_t id, ParticleInfo particleInfo)
	{
		std::shared_ptr<D3D11Texture> texture = mResourceManager->Create<D3D11Texture>(particleInfo.RenderModuleData.TexturePath);

		mParticleObjects.insert({ id, std::make_shared<ParticleObject>(mDevice, texture, particleInfo, DirectX::SimpleMath::Matrix::Identity) });
	}

	void D3D11ParticleManager::DeleteEmitter(size_t id)
	{
		mParticleObjects.erase(id);
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

	bool D3D11ParticleManager::sortInitial(unsigned int maxSize)
	{
		const int MAX_NUM_TG = 1024; //128; // max 128 * 512 elements = 64k elements

		bool bDone = true;
		unsigned int numThreadGroups = Align(maxSize, 512) / 512;
		assert(numThreadGroups == ((maxSize - 1) >> 9) + 1);
		assert(numThreadGroups <= MAX_NUM_TG);

		if (numThreadGroups > 1)
		{
			bDone = false;
		}

		// 512크기의 모든 버퍼를 정렬하고 더 큰 버퍼를 미리 정렬한다.?
		m_pCSSort512->Bind(mDevice);
		mDevice->GetDeviceContext()->DispatchIndirect(mIndirectSortArgsBuffer.Get(), 0);

		return bDone;
	}
	bool D3D11ParticleManager::sortIncremental(unsigned int presorted, unsigned int maxSize)
	{
		bool bDone = true;
		m_pCSSortStep->Bind(mDevice);

		unsigned int numThreadGroups = 0;

		if (maxSize > presorted)
		{
			if (maxSize > presorted * 2)
			{
				bDone = false;
			}

			unsigned int pow2 = presorted;
			while (pow2 < maxSize)
			{
				pow2 *= 2;
			}

			numThreadGroups = pow2 / 512;
		}

		unsigned int mergeSize = presorted * 2;
		for (unsigned int mergeSubSize = mergeSize / 2; mergeSubSize > 256; mergeSubSize /= 2)
		{
			D3D11_MAPPED_SUBRESOURCE MappedResource;

			mDevice->GetDeviceContext()->Map(mDispatchInfoCB.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
			SortConstants* sc = (SortConstants*)MappedResource.pData;
			sc->x = mergeSubSize;
			if (mergeSubSize == mergeSize / 2)
			{
				sc->y = (2 * mergeSubSize - 1);
				sc->z = -1;
			}
			else
			{
				sc->y = mergeSubSize;
				sc->z = 1;
			}
			sc->w = 0;
			mDevice->GetDeviceContext()->Unmap(mDispatchInfoCB.Get(), 0);

			mDevice->GetDeviceContext()->Dispatch(numThreadGroups, 1, 1);
		}

		m_pCSSortInner512->Bind(mDevice);
		mDevice->GetDeviceContext()->Dispatch(numThreadGroups, 1, 1);

		return bDone;
	}
}