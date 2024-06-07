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

		mBackBufferRTV = mResourceManager->Get<D3D11RenderTargetView>(ED3D11RenderTargetViewType::Offscreen);
		mNoneDSV = mResourceManager->Get<D3D11DepthStencilView>(ED3D11DepthStencilViewType::None);
		mDSV = mResourceManager->Get<D3D11DepthStencilView>(ED3D11DepthStencilViewType::Default);
		mDepthSRV = std::make_shared<D3D11ShaderResourceView>(mDevice, mDSV)->GetSRV();

		mLinearWrap = mResourceManager->Create<D3D11SamplerState>(ED3D11SamplerState::LinearWrap);
		mPointClamp = mResourceManager->Create<D3D11SamplerState>(ED3D11SamplerState::PointClamp);

		mInitDeadListCS = std::make_shared<D3D11ComputeShader>(mDevice, L"./resource/internal/shader/ParticleDeadListInitCS.hlsl");
		mInitParticlesCS = std::make_shared<D3D11ComputeShader>(mDevice, L"./resource/internal/shader/ParticleInitCS.hlsl");
		mEmitCS = std::make_shared<D3D11ComputeShader>(mDevice, L"./resource/internal/shader/ParticleEmitCS.hlsl");
		mSimulateCS = std::make_shared<D3D11ComputeShader>(mDevice, L"./resource/internal/shader/ParticleSimulateCS.hlsl");

		auto disableDepthWriteState = resourceManager->Create<D3D11DepthStencilState>(ED3D11DepthStencilState::DisableDepthWirte);
		auto OITRenderState = resourceManager->Create<D3D11BlendState>(ED3D11BlendState::OITRender);
		auto additiveState = resourceManager->Create<D3D11BlendState>(ED3D11BlendState::Additive);
		auto subtractiveState = resourceManager->Create<D3D11BlendState>(ED3D11BlendState::Subtractive);
		auto modulateState = resourceManager->Create<D3D11BlendState>(ED3D11BlendState::Modulate);
		auto additivePipelieState = std::make_shared<PipelineState>(nullptr, nullptr, additiveState);
		auto subtractivePipelieState = std::make_shared<PipelineState>(nullptr, nullptr, subtractiveState);
		auto modulatePipelieState = std::make_shared<PipelineState>(nullptr, nullptr, modulateState);
		auto particleRenderVS = std::make_shared<D3D11VertexShader>(mDevice, L"./resource/internal/shader/ParticleVS.hlsl");
		auto particleRednerPS = std::make_shared<D3D11PixelShader>(mDevice, L"./resource/internal/shader/ParticlePS.hlsl");
		auto particleRednerGS = std::make_shared<D3D11GeometryShader>(mDevice, L"./resource/internal/shader/ParticleGS.hlsl");
		mAdditiveRenderProgram = std::make_shared<ShaderProgram>(mDevice, particleRenderVS, particleRednerGS, particleRednerPS, additivePipelieState);
		mSubtractiveRenderProgram = std::make_shared<ShaderProgram>(mDevice, particleRenderVS, particleRednerGS, particleRednerPS, subtractivePipelieState);
		mModulateRenderProgram = std::make_shared<ShaderProgram>(mDevice, particleRenderVS, particleRednerGS, particleRednerPS, modulatePipelieState);

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
#if defined(DEBUG) || defined(_DEBUG)
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

		mDevice->GetDeviceContext()->GSSetShader(NULL, NULL, NULL);
	}

	IParticleObject* D3D11ParticleManager::CreateParticleObject(const ParticleInfo& particleInfo)
	{

		IParticleObject* particleObjectInferface = new ParticleObject(mDevice, mResourceManager, particleInfo, DirectX::SimpleMath::Matrix::Identity);
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
		particleObjectData.InstanceData.RandomSeed = particleObject->mRandomSeed;

		particleObjectData.MainData.StartColor[0] = particleInfo.MainData.StartColor0;
		particleObjectData.MainData.StartColor[1] = particleInfo.MainData.StartColorOption == ParticleInfo::EOption::Constant ? particleInfo.MainData.StartColor0 : particleInfo.MainData.StartColor1;
		particleObjectData.MainData.StartLifeTime[0] = particleInfo.MainData.StartLifeTime.x;
		particleObjectData.MainData.StartLifeTime[1] = particleInfo.MainData.StartLifeTimeOption == ParticleInfo::EOption::Constant ? particleInfo.MainData.StartLifeTime.x : particleInfo.MainData.StartLifeTime.y;
		particleObjectData.MainData.StartSpeed[0] = particleInfo.MainData.StartSpeed.x;
		particleObjectData.MainData.StartSpeed[1] = particleInfo.MainData.StartSpeedOption == ParticleInfo::EOption::Constant ? particleInfo.MainData.StartSpeed.x : particleInfo.MainData.StartSpeed.y;
		particleObjectData.MainData.StartRotation[0] = particleInfo.MainData.StartRotation.x;
		particleObjectData.MainData.StartRotation[1] = particleInfo.MainData.StartRotationOption == ParticleInfo::EOption::Constant ? particleInfo.MainData.StartRotation.x : particleInfo.MainData.StartRotation.y;;
		particleObjectData.MainData.StartSize[0] = particleInfo.MainData.StartSize.x;
		particleObjectData.MainData.StartSize[1] = particleInfo.MainData.StartSizeOption == ParticleInfo::EOption::Constant ? particleInfo.MainData.StartSize.x : particleInfo.MainData.StartSize.y;;
		particleObjectData.MainData.GravityModfier[0] = particleInfo.MainData.GravityModifier.x;
		particleObjectData.MainData.GravityModfier[1] = particleInfo.MainData.GravityModifierOption == ParticleInfo::EOption::Constant ? particleInfo.MainData.GravityModifier.x : particleInfo.MainData.GravityModifier.y;
		particleObjectData.MainData.SimulationSpeed = particleInfo.MainData.SimulationSpeed;
		particleObjectData.MainData.MaxParticleSize = particleInfo.MainData.MaxParticleCount;

		particleObjectData.ShapeData.Transform =
			DirectX::SimpleMath::Matrix::CreateScale(particleInfo.ShapeData.Scale) *
			DirectX::SimpleMath::Matrix::CreateFromYawPitchRoll(particleInfo.ShapeData.Rotation) *
			DirectX::SimpleMath::Matrix::CreateTranslation(particleInfo.ShapeData.Position);
		particleObjectData.ShapeData.Transform = particleObjectData.ShapeData.Transform.Transpose();

		particleObjectData.ShapeData.Position = particleInfo.ShapeData.Position;
		particleObjectData.ShapeData.Rotation = particleInfo.ShapeData.Rotation;
		particleObjectData.ShapeData.Scale = particleInfo.ShapeData.Scale;
		particleObjectData.ShapeData.ShapeType = (int)particleInfo.ShapeData.ShapeType;
		particleObjectData.ShapeData.ModeType = (int)particleInfo.ShapeData.ModeType;
		particleObjectData.ShapeData.AngleInRadian = particleInfo.ShapeData.AngleInDegree * 3.141592 / 180;
		particleObjectData.ShapeData.Radius = particleInfo.ShapeData.Radius;
		particleObjectData.ShapeData.DountRadius = particleInfo.ShapeData.DountRadius;
		particleObjectData.ShapeData.ArcInRadian = particleInfo.ShapeData.ArcInDegree * 3.141592 / 180;
		particleObjectData.ShapeData.Speed = particleInfo.ShapeData.Speed;
		particleObjectData.ShapeData.Spread = particleInfo.ShapeData.Spread;
		particleObjectData.ShapeData.RadiusThickness = particleInfo.ShapeData.RadiusThickness;

		particleObjectData.VelocityOverLifetimeData.Velocity = particleInfo.VelocityOverLifetimeData.Velocity;
		particleObjectData.VelocityOverLifetimeData.Orbital = particleInfo.VelocityOverLifetimeData.Orbital;
		particleObjectData.VelocityOverLifetimeData.Offset = particleInfo.VelocityOverLifetimeData.Offset;
		particleObjectData.VelocityOverLifetimeData.bIsUsed = particleInfo.VelocityOverLifetimeData.bIsUsed;

		particleObjectData.LimitVelocityOverLifetimeData.Speed = particleInfo.LimitVelocityOverLifetimeData.Speed;
		particleObjectData.LimitVelocityOverLifetimeData.Dampen = particleInfo.LimitVelocityOverLifetimeData.Dampen;
		particleObjectData.LimitVelocityOverLifetimeData.bIsUsed = particleInfo.LimitVelocityOverLifetimeData.bIsUsed;

		particleObjectData.ForceOverLifeTimeData.Force = particleInfo.ForceOverLifeTimeData.Force;
		particleObjectData.ForceOverLifeTimeData.bIsUsed = particleInfo.ForceOverLifeTimeData.bIsUsed;

		for (size_t i = 0; i < 8; ++i)
		{
			if (particleInfo.ColorOverLifetimeData.ColorRatioCount > i)
			{
				particleObjectData.ColorOverLifetimeData.ColorRatios[i] = particleInfo.ColorOverLifetimeData.ColorRatios[i];
			}
			else
			{
				particleObjectData.ColorOverLifetimeData.ColorRatios[i] = DirectX::SimpleMath::Vector4{ 0, 0, 0, 0 };
			}

			if (particleInfo.ColorOverLifetimeData.AlphaRatioCount > i)
			{
				particleObjectData.ColorOverLifetimeData.AlphaRatios[i] = DirectX::SimpleMath::Vector4{ particleInfo.ColorOverLifetimeData.AlphaRatios[i].x, particleInfo.ColorOverLifetimeData.AlphaRatios[i].y, 0, 0 };
			}
			else
			{
				particleObjectData.ColorOverLifetimeData.AlphaRatios[i] = DirectX::SimpleMath::Vector4{ 0, 0, 0, 0 };
			}
		}
		particleObjectData.ColorOverLifetimeData.AlphaRatioCount = particleInfo.ColorOverLifetimeData.AlphaRatioCount;
		particleObjectData.ColorOverLifetimeData.ColorRatioCount = particleInfo.ColorOverLifetimeData.ColorRatioCount;
		particleObjectData.ColorOverLifetimeData.bIsUsed = particleInfo.ColorOverLifetimeData.bIsUsed;

		particleObjectData.SizeOverLifetimeData.PointA = particleInfo.SizeOverLifetimeData.PointA;
		particleObjectData.SizeOverLifetimeData.PointB = particleInfo.SizeOverLifetimeData.PointB;
		particleObjectData.SizeOverLifetimeData.PointC = particleInfo.SizeOverLifetimeData.PointC;
		particleObjectData.SizeOverLifetimeData.PointD = particleInfo.SizeOverLifetimeData.PointD;
		particleObjectData.SizeOverLifetimeData.bIsUsed = particleInfo.SizeOverLifetimeData.bIsUsed;

		particleObjectData.RotationOverLifetimeData.AngularVelocityInRadian = particleInfo.RotationOverLifetimeData.AngularVelocityInDegree * 3.141592 / 180;
		particleObjectData.RotationOverLifetimeData.bIsUsed = particleInfo.RotationOverLifetimeData.bIsUsed;

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

#if defined(DEBUG) || defined(_DEBUG)
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

		ZeroMemory(buffers, sizeof(buffers));
		mDevice->GetDeviceContext()->CSSetConstantBuffers(1, ARRAYSIZE(buffers), buffers);

#if defined(DEBUG) || defined(_DEBUG)
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
		ID3D11Buffer* buffers[] = { mParticleObjectCB->GetBuffer() };

		mDevice->GetDeviceContext()->CSSetUnorderedAccessViews(0, ARRAYSIZE(uavs), uavs, initialCounts);
		mDevice->GetDeviceContext()->CSSetShaderResources(0, ARRAYSIZE(srvs), srvs);
		mDevice->GetDeviceContext()->CSSetConstantBuffers(1, ARRAYSIZE(buffers), buffers);

		size_t threadX = align(ParticleInfo::MAX_PARTICLE_COUNT, 256) / 256;
		mDevice->GetDeviceContext()->Dispatch(threadX, 1, 1);

		// depthSRV unbind
		ZeroMemory(srvs, sizeof(srvs));
		mDevice->GetDeviceContext()->CSSetShaderResources(0, ARRAYSIZE(srvs), srvs);
		ZeroMemory(uavs, sizeof(uavs));
		mDevice->GetDeviceContext()->CSSetUnorderedAccessViews(0, ARRAYSIZE(uavs), uavs, nullptr);
		ZeroMemory(buffers, sizeof(buffers));
		mDevice->GetDeviceContext()->CSSetConstantBuffers(1, ARRAYSIZE(buffers), buffers);

		mDevice->GetDeviceContext()->CopyStructureCount(mActiveListCB.Get(), 0, particleObject->mAliveIndexBufferUAV.Get());

		// 디버그일 경우 카운트 읽어오기
#if defined(DEBUG) || defined(_DEBUG)
		particleObject->mDebugInfo.NumDeadParticlesAfterSimulation = readCounter(particleObject->mDeadListUAV.Get());
		particleObject->mDebugInfo.NumActiveParticlesAfterSimulation = readCounter(particleObject->mAliveIndexBufferUAV.Get());
#endif
	}

	void D3D11ParticleManager::render(IParticleObject* particleObjectInterface)
	{
		ParticleObject* particleObject = static_cast<ParticleObject*>(particleObjectInterface);


		switch (particleObjectInterface->GetInfo().RenderData.BlendMode)
		{
		case ParticleInfo::Render::EBlendMode::Additive:
			mAdditiveRenderProgram->Bind(mDevice);
			break;
			//case ParticleInfo::Render::EBlendMode::Subtractive: // 정렬 필요
			//	mSubtractiveRenderProgram->Bind(mDevice);
			//	break;
			//case ParticleInfo::Render::EBlendMode::Moudulate:
			//	mModulateRenderProgram->Bind(mDevice);
			//	break;
		}

		mBackBufferRTV->Bind(mDevice, mNoneDSV);

		ID3D11Buffer* vb = nullptr;
		UINT stride = 0;
		UINT offset = 0;
		mDevice->GetDeviceContext()->IASetVertexBuffers(0, 1, &vb, &stride, &offset); // null VB 바인딩
		mDevice->GetDeviceContext()->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);
		mDevice->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

		ID3D11ShaderResourceView* vs_srv[] = { particleObject->mParticleBufferSRV.Get(), particleObject->mAliveIndexBufferSRV.Get() };
		ID3D11ShaderResourceView* ps_srv[] = { particleObject->mTexture->GetSRV().Get(),mDepthSRV.Get() };

		mDevice->GetDeviceContext()->VSSetShaderResources(0, ARRAYSIZE(vs_srv), vs_srv);
		mDevice->GetDeviceContext()->PSSetShaderResources(0, ARRAYSIZE(ps_srv), ps_srv);
		mDevice->GetDeviceContext()->VSSetConstantBuffers(3, 1, mActiveListCB.GetAddressOf());

		mDevice->GetDeviceContext()->DrawInstancedIndirect(mIndirectDrawArgsBuffer.Get(), 0);

		ZeroMemory(vs_srv, sizeof(vs_srv));
		mDevice->GetDeviceContext()->VSSetShaderResources(0, ARRAYSIZE(vs_srv), vs_srv);
		ZeroMemory(ps_srv, sizeof(ps_srv));
		mDevice->GetDeviceContext()->PSSetShaderResources(0, ARRAYSIZE(ps_srv), ps_srv);
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