#include "D3D11ParticleManager.h"
#include "D3D11Common.h"

namespace fq::graphics
{
	ParticleSystem::ParticleSystem(const std::shared_ptr<D3D11Device>& device, const ParticleSystemInfo& info)
		: mInfo(info)
		, mParticleCount(0u)
		, mAccumulateTime(0.f)
		, mEmissionRemainTime(0.f)
	{
		mInfo.MaxParticleCount = std::min<size_t>(mInfo.MaxParticleCount, ParticleSystemInfo::MAX_PARTICLE_COUNT);

		mAppendBuffer = std::make_shared<D3D11StructuredBuffer>(device, sizeof(Particle), mInfo.MaxParticleCount);
		mConsumeBuffer = std::make_shared<D3D11StructuredBuffer>(device, sizeof(Particle), mInfo.MaxParticleCount);
		mAppendUAV = std::make_shared<D3D11UnorderedAccessView>(device, mAppendBuffer, eUAVType::ComsumeAppend);
		mConsumeUAV = std::make_shared<D3D11UnorderedAccessView>(device, mConsumeBuffer, eUAVType::ComsumeAppend);
		mAppendSRV = std::make_shared<D3D11ShaderResourceView>(device, mAppendUAV);
		mConsumeSRV = std::make_shared<D3D11ShaderResourceView>(device, mConsumeUAV);

		mEmissionTime = mInfo.RateOverTime;
		
	}

	void ParticleSystem::Swap()
	{
		std::swap(mAppendUAV, mConsumeUAV);
		std::swap(mAppendBuffer, mConsumeBuffer);
		std::swap(mAppendSRV, mConsumeSRV);
	}

	void D3D11ParticleManager::AddDeltaTime(float deltaTime)
	{
		for (auto& [key, system] : mParticleSystems)
		{
			system->AddDeltaTime(deltaTime);
		}
	}
	void D3D11ParticleManager::AddParticleSystem(size_t id, const std::shared_ptr<D3D11Device>& device, ParticleSystemInfo emitterInfo)
	{
		auto find = mParticleSystems.find(id);

		if (find != mParticleSystems.end())
		{
			return;
		}

		std::shared_ptr<ParticleSystem> emitter = std::make_shared<ParticleSystem>(device, emitterInfo);
		mParticleSystems.insert({ id, emitter });
	}
	void D3D11ParticleManager::DeleteParticleSystem(size_t id)
	{
		mParticleSystems.erase(id);
	}
}