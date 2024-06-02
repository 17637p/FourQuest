#include "D3D11ParticleManager.h"
#include "D3D11Common.h"
#include "ManagementCommon.h"
#include "RenderObject.h"
#include "ParticleObject.h"
#include "D3D11Util.h"

namespace fq::graphics
{
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

		mInitDeadListCS = std::make_shared<D3D11ComputeShader>(mDevice, L"./resource/internal/shader/ParticleDeadListInitCS.hlsl");
		mInitParticlesCS = std::make_shared<D3D11ComputeShader>(mDevice, L"./resource/internal/shader/ParticleInitCS.hlsl");
		mEmitCS = std::make_shared<D3D11ComputeShader>(mDevice, L"./resource/internal/shader/ParticleEmitCS.hlsl");
		mSimulateCS = std::make_shared<D3D11ComputeShader>(mDevice, L"./resource/internal/shader/ParticleSimulateCS.hlsl");

		auto additive = mResourceManager->Create<D3D11BlendState>(ED3D11BlendState::Additive);
		std::shared_ptr<PipelineState> pipelineState = std::make_shared<PipelineState>(nullptr, nullptr, additive);
		auto particleRenderVS = std::make_shared<D3D11VertexShader>(mDevice, L"./resource/internal/shader/ParticleVS.hlsl");
		auto particleRednerPS = std::make_shared<D3D11PixelShader>(mDevice, L"./resource/internal/shader/ParticlePS.hlsl");
		auto particleRednerGS = std::make_shared<D3D11GeometryShader>(mDevice, L"./resource/internal/shader/ParticleGS.hlsl");
		mRenderProgram = std::make_shared<ShaderProgram>(mDevice, particleRenderVS, particleRednerGS, particleRednerPS, pipelineState);

		D3D11Util::CreateRandomTexture2DSRV(mDevice->GetDevice().Get(), mRandomTextureSRV.GetAddressOf());

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
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = 0;
		desc.ByteWidth = 4 * sizeof(UINT);
		mDevice->GetDevice()->CreateBuffer(&desc, nullptr, mDeadListCB.GetAddressOf());
		mDevice->GetDevice()->CreateBuffer(&desc, nullptr, mActiveListCB.GetAddressOf());

		mParticleFrameCB = std::make_shared<D3D11ConstantBuffer<ParticleFrameData>>(mDevice, ED3D11ConstantBuffer::Transform);
		mParticleObjectCB = std::make_shared<D3D11ConstantBuffer<ParticleObjectData>>(mDevice, ED3D11ConstantBuffer::Transform);
	}

	void D3D11ParticleManager::Excute()
	{
		// update frameCB
		ParticleFrameData particleFrameData;
		particleFrameData.ViewMatrix = mCameraManager->GetViewMatrix(ECameraType::Player).Transpose();
		particleFrameData.ProjMatrix = mCameraManager->GetProjectionMatrix(ECameraType::Player).Transpose();
		particleFrameData.InvProjMatrix = D3D11Util::InverseTranspose(mCameraManager->GetProjectionMatrix(ECameraType::Player)).Transpose();
		particleFrameData.ScreenWidth = mDevice->GetWidth();
		particleFrameData.ScreenHeight = mDevice->GetHeight();

		mParticleFrameCB->Update(mDevice, particleFrameData);

		// bind
		mParticleFrameCB->Bind(mDevice, ED3D11ShaderType::VertexShader);
		mParticleFrameCB->Bind(mDevice, ED3D11ShaderType::PixelShader);
		mParticleFrameCB->Bind(mDevice, ED3D11ShaderType::ComputeShader);
		mParticleFrameCB->Bind(mDevice, ED3D11ShaderType::GeometryShader);

		mLinearWrap->Bind(mDevice, 0, ED3D11ShaderType::PixelShader);
		mLinearWrap->Bind(mDevice, 0, ED3D11ShaderType::ComputeShader);

		mPointClamp->Bind(mDevice, 1, ED3D11ShaderType::PixelShader);
		mPointClamp->Bind(mDevice, 1, ED3D11ShaderType::ComputeShader);

		for (IParticleObject* particleObjectInterface : mParticleObjects)
		{
			emit(particleObjectInterface);
			simulate(particleObjectInterface);
			render(particleObjectInterface);
		}
	}

	IParticleObject* D3D11ParticleManager::CreateParticleObject(const ParticleInfo& particleInfo)
	{
		std::shared_ptr<D3D11Texture> texture = mResourceManager->Create<D3D11Texture>(particleInfo.RenderModuleData.TexturePath);

		IParticleObject* particleObjectInferface = new ParticleObject(mDevice, texture, particleInfo, DirectX::SimpleMath::Matrix::Identity);
		mParticleObjects.insert(particleObjectInferface);

		return particleObjectInferface;
	}

	void D3D11ParticleManager::DeleteParticleObject(IParticleObject* particleObjectInterface)
	{
		mParticleObjects.erase(particleObjectInterface);

		ParticleObject* particleObject = static_cast<ParticleObject*>(particleObjectInterface);
		delete particleObject;
	}

	void D3D11ParticleManager::updateParticleObjectCB(IParticleObject* particleObjectInterface)
	{
		const ParticleObject* particleObject = static_cast<ParticleObject*>(particleObjectInterface);
		const ParticleInfo& particleInfo = particleObject->GetInfo();

		ParticleObjectData particleObjectData;

		particleObjectData.InstanceData.Transform = particleObject->GetTransform().Transpose();
		particleObjectData.InstanceData.TimePos = particleObject->mTimePos;
		particleObjectData.InstanceData.FrameTime = particleObject->mFrameTime;
		particleObjectData.InstanceData.NumToEmit = particleObject->mNumToEmit;

		particleObjectData.MainData.StartColor[0] = particleInfo.MainModuleData.StartColor[0];
		particleObjectData.MainData.StartColor[1] = particleInfo.MainModuleData.StartColorOption == ParticleInfo::EOption::Constant ? particleInfo.MainModuleData.StartColor[0] : particleInfo.MainModuleData.StartColor[1];;
		particleObjectData.MainData.StartLifeTime[0] = particleInfo.MainModuleData.StartLifeTime[0];
		particleObjectData.MainData.StartLifeTime[1] = particleInfo.MainModuleData.StartLifeTimeOption == ParticleInfo::EOption::Constant ? particleInfo.MainModuleData.StartLifeTime[0] : particleInfo.MainModuleData.StartLifeTime[1];;
		particleObjectData.MainData.StartSpeed[0] = particleInfo.MainModuleData.StartSpeed[0];
		particleObjectData.MainData.StartSpeed[1] = particleInfo.MainModuleData.StartSpeedOption == ParticleInfo::EOption::Constant ? particleInfo.MainModuleData.StartSpeed[0] : particleInfo.MainModuleData.StartSpeed[1];;
		particleObjectData.MainData.StartRotation[0] = particleInfo.MainModuleData.StartRotation[0];
		particleObjectData.MainData.StartRotation[1] = particleInfo.MainModuleData.StartRotationOption == ParticleInfo::EOption::Constant ? particleInfo.MainModuleData.StartRotation[0] : particleInfo.MainModuleData.StartRotation[1];;
		particleObjectData.MainData.StartSize[0] = particleInfo.MainModuleData.StartSize[0];
		particleObjectData.MainData.StartSize[1] = particleInfo.MainModuleData.StartSizeOption == ParticleInfo::EOption::Constant ? particleInfo.MainModuleData.StartSize[0] : particleInfo.MainModuleData.StartSize[1];;
		particleObjectData.MainData.GravityModfier[0] = particleInfo.MainModuleData.GravityModifier[0];
		particleObjectData.MainData.GravityModfier[1] = particleInfo.MainModuleData.GravityModifierOption == ParticleInfo::EOption::Constant ? particleInfo.MainModuleData.GravityModifier[0] : particleInfo.MainModuleData.GravityModifier[1];
		particleObjectData.MainData.SimulationSpeed = particleInfo.MainModuleData.SimulationSpeed;
		particleObjectData.MainData.MaxParticleSize = particleInfo.MainModuleData.MaxParticleCount;

		particleObjectData.ShapeData.ShapeType = (int)particleInfo.ShapeData.ShapeType;
		particleObjectData.ShapeData.ModeType = (int)particleInfo.ShapeData.ModeType;
		particleObjectData.ShapeData.Angle = particleInfo.ShapeData.Angle;
		particleObjectData.ShapeData.Radius = particleInfo.ShapeData.Radius;
		particleObjectData.ShapeData.DountRadius = particleInfo.ShapeData.DountRadius;
		particleObjectData.ShapeData.Arc = particleInfo.ShapeData.Arc;
		particleObjectData.ShapeData.Speed = particleInfo.ShapeData.Speed;
		particleObjectData.ShapeData.Spread = particleInfo.ShapeData.Spread;
		particleObjectData.ShapeData.RadiusThickness = particleInfo.ShapeData.RadiusThickness;

		mParticleObjectCB->Update(mDevice, particleObjectData);
	}

	void D3D11ParticleManager::emit(IParticleObject* particleObjectInterface)
	{
		ParticleObject* particleObject = static_cast<ParticleObject*>(particleObjectInterface);

		if (particleObject->mbIsReset)
		{
			mInitDeadListCS->Bind(mDevice);
			UINT initialCount[] = { 0 };
			mDevice->GetDeviceContext()->CSSetUnorderedAccessViews(0, 1, particleObject->mDeadListUAV.GetAddressOf(), initialCount);
			mDevice->GetDeviceContext()->Dispatch(align(ParticleInfo::MAX_PARTICLE_COUNT, 256) / 256, 1, 1);

#if _DEBUG
			particleObject->mDebugInfo.NumDeadParticlesOnInit = readCounter(particleObject->mDeadListUAV.Get());
#endif
			ID3D11UnorderedAccessView* uavs[] = { particleObject->mParticleBufferUAV.Get() };
			UINT initialCounts[] = { (UINT)-1 };

			mInitParticlesCS->Bind(mDevice);
			mDevice->GetDeviceContext()->CSSetUnorderedAccessViews(0, ARRAYSIZE(uavs), uavs, initialCounts);
			mDevice->GetDeviceContext()->Dispatch(align(ParticleInfo::MAX_PARTICLE_COUNT, 256) / 256, 1, 1);

			particleObject->reset();
		}

		particleObject->caculateNumToEmit();
		updateParticleObjectCB(particleObjectInterface);

		if (particleObject->mNumToEmit <= 0)
		{
			return;
		}

		ID3D11UnorderedAccessView* uavs[] = { particleObject->mParticleBufferUAV.Get(), particleObject->mDeadListUAV.Get() };
		UINT initialCounts[] = { (UINT)-1,  (UINT)-1 };
		ID3D11Buffer* buffers[] = { mParticleObjectCB->GetBuffer(), mDeadListCB.Get() };
		ID3D11ShaderResourceView* srvs[] = { mRandomTextureSRV.Get() };

		mEmitCS->Bind(mDevice);
		mDevice->GetDeviceContext()->CSSetUnorderedAccessViews(0, ARRAYSIZE(uavs), uavs, initialCounts);
		mDevice->GetDeviceContext()->CSSetConstantBuffers(1, ARRAYSIZE(buffers), buffers);
		mDevice->GetDeviceContext()->CSSetShaderResources(0, ARRAYSIZE(srvs), srvs);

		mDevice->GetDeviceContext()->CopyStructureCount(mDeadListCB.Get(), 0, particleObject->mDeadListUAV.Get());

		int numThreadGroups = align(particleObject->mNumToEmit, 1024) / 1024;
		mDevice->GetDeviceContext()->Dispatch(numThreadGroups, 1, 1);

#if _DEBUG
		particleObject->mDebugInfo.NumDeadParticlesAfterEmit = readCounter(particleObject->mDeadListUAV.Get());
#endif
	}

	void D3D11ParticleManager::simulate(IParticleObject* particleObjectInterface)
	{
		ParticleObject* particleObject = static_cast<ParticleObject*>(particleObjectInterface);

		mDevice->GetDeviceContext()->OMSetRenderTargets(0, nullptr, nullptr);

		mSimulateCS->Bind(mDevice);

		// depathStencil bind
		ID3D11UnorderedAccessView* uavs[] = { particleObject->mParticleBufferUAV.Get(),  particleObject->mDeadListUAV.Get(), particleObject->mAliveIndexBufferUAV.Get(), mIndirectDrawArgsBufferUAV.Get() };
		UINT initialCounts[] = { (UINT)-1,  (UINT)-1, 0, (UINT)-1 };
		ID3D11ShaderResourceView* srvs[] = { mDepthSRV.Get() };

		mDevice->GetDeviceContext()->CSSetUnorderedAccessViews(0, ARRAYSIZE(uavs), uavs, initialCounts);
		mDevice->GetDeviceContext()->CSSetShaderResources(0, ARRAYSIZE(srvs), srvs);
		size_t threadX = align(ParticleInfo::MAX_PARTICLE_COUNT, 256) / 256;
		mDevice->GetDeviceContext()->Dispatch(threadX, 1, 1);

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

	void D3D11ParticleManager::render(IParticleObject* particleObjectInterface)
	{
		ParticleObject* particleObject = static_cast<ParticleObject*>(particleObjectInterface);

		mSwapChainRTV->Bind(mDevice, mNoneDSV);
		mRenderProgram->Bind(mDevice);

		ID3D11Buffer* vb = nullptr;
		UINT stride = 0;
		UINT offset = 0;
		mDevice->GetDeviceContext()->IASetVertexBuffers(0, 1, &vb, &stride, &offset); // null VB 바인딩
		mDevice->GetDeviceContext()->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);
		mDevice->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

		ID3D11ShaderResourceView* vs_srv[] = { particleObject->mParticleBufferSRV.Get(), particleObject->mAliveIndexBufferSRV.Get() };
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

	int D3D11ParticleManager::align(int value, int aligment)
	{
		return (value + aligment - 1) / aligment * aligment;
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