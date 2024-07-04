#include "D3D11ParticleManager.h"
#include "D3D11Common.h"
#include "ManagementCommon.h"
#include "RenderObject.h"
#include "ParticleObject.h"
#include "D3D11Util.h"
#include "Material.h"

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
		mDepthSRV = std::make_shared<D3D11ShaderResourceView>(mDevice, mDSV, DXGI_FORMAT_R24_UNORM_X8_TYPELESS)->GetSRV();

		mLinearWrap = mResourceManager->Create<D3D11SamplerState>(ED3D11SamplerState::LinearWrap);
		mPointClamp = mResourceManager->Create<D3D11SamplerState>(ED3D11SamplerState::PointClamp);

		mInitDeadListCS = std::make_shared<D3D11ComputeShader>(mDevice, L"ParticleDeadListInitCS.cso");
		mInitParticlesCS = std::make_shared<D3D11ComputeShader>(mDevice, L"ParticleInitCS.cso");
		mEmitCS = std::make_shared<D3D11ComputeShader>(mDevice, L"ParticleEmitCS.cso");
		mSimulateCS = std::make_shared<D3D11ComputeShader>(mDevice, L"ParticleSimulateCS.cso");

		mDefaultRasterizer = mResourceManager->Create<D3D11RasterizerState>(ED3D11RasterizerState::Default);
		mCullOffRasterizer = mResourceManager->Create<D3D11RasterizerState>(ED3D11RasterizerState::CullOff);
		mAdditiveState = resourceManager->Create<D3D11BlendState>(ED3D11BlendState::Additive);
		mSubtractiveState = resourceManager->Create<D3D11BlendState>(ED3D11BlendState::Subtractive);
		mModulateState = resourceManager->Create<D3D11BlendState>(ED3D11BlendState::Modulate);
		mAlphaBlendState = resourceManager->Create<D3D11BlendState>(ED3D11BlendState::AlphaBlend);
		auto pipelieState = std::make_shared<PipelineState>(nullptr, nullptr, nullptr);

		auto particleRenderVS = std::make_shared<D3D11VertexShader>(mDevice, L"ParticleVS.cso");
		auto particleRednerPS = std::make_shared<D3D11PixelShader>(mDevice, L"ParticlePS.cso");
		auto particleRednerGS = std::make_shared<D3D11GeometryShader>(mDevice, L"ParticleGS.cso");
		mRenderProgram = std::make_shared<ShaderProgram>(mDevice, particleRenderVS, particleRednerGS, particleRednerPS, pipelieState);

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

	void D3D11ParticleManager::OnResize(unsigned short width, unsigned short height)
	{
		mBackBufferRTV = mResourceManager->Get<D3D11RenderTargetView>(ED3D11RenderTargetViewType::Offscreen);
		mNoneDSV = mResourceManager->Get<D3D11DepthStencilView>(ED3D11DepthStencilViewType::None);
		mDSV = mResourceManager->Get<D3D11DepthStencilView>(ED3D11DepthStencilViewType::Default);
		mDepthSRV = std::make_shared<D3D11ShaderResourceView>(mDevice, mDSV, DXGI_FORMAT_R24_UNORM_X8_TYPELESS)->GetSRV();
	}

	void D3D11ParticleManager::BeginRender()
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
	}
	void D3D11ParticleManager::Render(IParticleObject* particleObject)
	{
		emit(particleObject);
		simulate(particleObject);
		render(particleObject);
	}
	void D3D11ParticleManager::EndRender()
	{
		mDevice->GetDeviceContext()->GSSetShader(NULL, NULL, NULL);
	}

	void D3D11ParticleManager::updateParticleObjectCB(IParticleObject* particleObjectInterface)
	{
		ParticleObject* particleObject = static_cast<ParticleObject*>(particleObjectInterface);
		const std::shared_ptr<ParticleMaterial> particleMaterial = std::static_pointer_cast<ParticleMaterial>(particleObject->GetIParticleMaterial());
		const ParticleInfo& particleInfo = particleObject->GetInfo();
		const ParticleMaterialInfo& materialInfo = particleMaterial->GetInfo();

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

		particleObjectData.ColorOverLifetimeData.AlphaRatioCount = std::min<size_t>(particleInfo.ColorOverLifetimeData.AlphaRatios.size(), 8);
		particleObjectData.ColorOverLifetimeData.ColorRatioCount = std::min<size_t>(particleInfo.ColorOverLifetimeData.ColorRatios.size(), 8);
		for (size_t i = 0; i < particleObjectData.ColorOverLifetimeData.AlphaRatioCount; ++i)
		{
			particleObjectData.ColorOverLifetimeData.AlphaRatios[i] = DirectX::SimpleMath::Vector4{ particleInfo.ColorOverLifetimeData.AlphaRatios[i].x, particleInfo.ColorOverLifetimeData.AlphaRatios[i].y, 0, 0 };
		}
		for (size_t i = 0; i < particleObjectData.ColorOverLifetimeData.ColorRatioCount; ++i)
		{
			particleObjectData.ColorOverLifetimeData.ColorRatios[i] = particleInfo.ColorOverLifetimeData.ColorRatios[i];
		}
		particleObjectData.ColorOverLifetimeData.bIsUsed = particleInfo.ColorOverLifetimeData.bIsUsed;

		particleObjectData.SizeOverLifetimeData.PointA = particleInfo.SizeOverLifetimeData.PointA;
		particleObjectData.SizeOverLifetimeData.PointB = particleInfo.SizeOverLifetimeData.PointB;
		particleObjectData.SizeOverLifetimeData.PointC = particleInfo.SizeOverLifetimeData.PointC;
		particleObjectData.SizeOverLifetimeData.PointD = particleInfo.SizeOverLifetimeData.PointD;
		particleObjectData.SizeOverLifetimeData.bIsUsed = particleInfo.SizeOverLifetimeData.bIsUsed;

		particleObjectData.RotationOverLifetimeData.AngularVelocityInRadian = particleInfo.RotationOverLifetimeData.AngularVelocityInDegree * 3.141592 / 180;
		particleObjectData.RotationOverLifetimeData.bIsUsed = particleInfo.RotationOverLifetimeData.bIsUsed;

		particleObjectData.ParticleMaterialData.BaseColor = materialInfo.BaseColor;
		particleObjectData.ParticleMaterialData.EmissiveColor = materialInfo.EmissiveColor;
		particleObjectData.ParticleMaterialData.TexTransform = DirectX::SimpleMath::Matrix::CreateScale(materialInfo.Tiling.x, materialInfo.Tiling.y, 0) * DirectX::SimpleMath::Matrix::CreateTranslation(materialInfo.Offset.x, materialInfo.Offset.y, 0);
		particleObjectData.ParticleMaterialData.RenderMode = (int)materialInfo.RenderModeType;
		particleObjectData.ParticleMaterialData.ColorMode = (int)materialInfo.ColorModeType;
		particleObjectData.ParticleMaterialData.bUseAlbedoMap = particleMaterial->GetHasBaseColor() && materialInfo.bIsUsedBaseColor;;
		particleObjectData.ParticleMaterialData.bUseEmissiveMap = particleMaterial->GetHasEmissive() && materialInfo.bIsUsedEmissive;
		particleObjectData.ParticleMaterialData.AlphaCutoff = materialInfo.AlphaCutoff;
		particleObjectData.ParticleMaterialData.bUseMultiplyAlpha = materialInfo.bUseMultiplyAlpha;

		mParticleObjectCB->Update(mDevice, particleObjectData);
	}

	void D3D11ParticleManager::emit(IParticleObject* particleObjectInterface)
	{
		ParticleObject* particleObject = static_cast<ParticleObject*>(particleObjectInterface);
		const auto& info = particleObject->GetInfo();

		if (info.InstanceData.bIsReset)
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
			//return;
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
		std::shared_ptr<ParticleMaterial> particleMaterial = std::static_pointer_cast<ParticleMaterial>(particleObject->GetIParticleMaterial());

		const auto& materialInfo = particleMaterial->GetInfo();
		mRenderProgram->Bind(mDevice);

		if (materialInfo.bIsTwoSide)
		{
			mCullOffRasterizer->Bind(mDevice);
		}
		else
		{
			mDefaultRasterizer->Bind(mDevice);
		}

		switch (materialInfo.RenderModeType)
		{
		case ParticleMaterialInfo::ERenderMode::Additive:
			mAdditiveState->Bind(mDevice);
			break;
		case ParticleMaterialInfo::ERenderMode::Subtractive:
			mSubtractiveState->Bind(mDevice);
			break;
		case ParticleMaterialInfo::ERenderMode::Modulate:
			mModulateState->Bind(mDevice);
			break;
		case ParticleMaterialInfo::ERenderMode::AlphaBlend:
			mAlphaBlendState->Bind(mDevice);
			break;
		default:
			assert(false);
			break;
		}

		mBackBufferRTV->Bind(mDevice, mNoneDSV);

		ID3D11Buffer* vb = nullptr;
		UINT stride = 0;
		UINT offset = 0;
		mDevice->GetDeviceContext()->IASetVertexBuffers(0, 1, &vb, &stride, &offset); // null VB 바인딩
		mDevice->GetDeviceContext()->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);
		mDevice->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

		ID3D11ShaderResourceView* vs_srv[] = { particleObject->mParticleBufferSRV.Get(), particleObject->mAliveIndexBufferSRV.Get() };
		ID3D11ShaderResourceView* ps_srv[] = {
			!particleMaterial->GetHasBaseColor() ? nullptr : particleMaterial->GetBaseColor()->GetSRV().Get(),
			!particleMaterial->GetHasEmissive() ? nullptr : particleMaterial->GetEmissive()->GetSRV().Get(),
			mDepthSRV.Get() };

		mParticleFrameCB->Bind(mDevice, ED3D11ShaderType::PixelShader, 0);
		mParticleObjectCB->Bind(mDevice, ED3D11ShaderType::PixelShader, 1);
		mParticleObjectCB->Bind(mDevice, ED3D11ShaderType::GeometryShader, 1);

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