#include "D3D11ParticleManager.h"
#include "D3D11Common.h"

namespace fq::graphics
{
	ParticleEmitter::ParticleEmitter(const std::shared_ptr<D3D11Device>& device, const ParticleEmitterInfo& info)
		: mInfo(info)
		, mParticleCount(0u)
	{
		mAppendBuffer = std::make_shared<D3D11StructuredBuffer>(device, sizeof(Particle), mInfo.MaxParticleCount);
		mConsumeBuffer = std::make_shared<D3D11StructuredBuffer>(device, sizeof(Particle), mInfo.MaxParticleCount);
		mAppendUAV = std::make_shared<D3D11UnorderedAccessView>(device, mAppendBuffer, eUAVType::ComsumeAppend);
		mConsumeUAV = std::make_shared<D3D11UnorderedAccessView>(device, mConsumeBuffer, eUAVType::ComsumeAppend);
		mAppendSRV = std::make_shared<D3D11ShaderResourceView>(device, mAppendUAV);
		mConsumeSRV = std::make_shared<D3D11ShaderResourceView>(device, mConsumeUAV);
	}

	void ParticleEmitter::Swap()
	{
		std::swap(mAppendUAV, mConsumeUAV);
		std::swap(mAppendBuffer, mConsumeBuffer);
		std::swap(mAppendSRV, mConsumeSRV);
	}

	void D3D11ParticleManager::AddParticleEmitter(size_t id, const std::shared_ptr<D3D11Device>& device, ParticleEmitterInfo emitterInfo)
	{
		auto find = mParticleEmitters.find(id);

		if (find != mParticleEmitters.end())
		{
			return;
		}

		std::shared_ptr<ParticleEmitter> emitter = std::make_shared<ParticleEmitter>(device, emitterInfo);
		mParticleEmitters.insert({ id, emitter });
	}
	void D3D11ParticleManager::DeleteParticleEmitter(size_t id)
	{
		mParticleEmitters.erase(id);
	}
}