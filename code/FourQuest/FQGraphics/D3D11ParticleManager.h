#include "CommonHeader.h"
#include "../FQCommon/FQCommonGraphics.h"

/*
파티클 매니저 진행 방식
1. CPU 자원을 쓰는 방식으로 기능 구현
2. GPU 자원을 써서 최적화하기
3. 유니티 인터페이스 참조해서 최대한 많은 기능 지원하기
*/

namespace fq::graphics
{
	struct Particle
	{
		DirectX::SimpleMath::Vector3 Position;
		DirectX::SimpleMath::Vector2 Size;
		DirectX::SimpleMath::Vector3 Velocity;
		float TimeToLive;
	};
	class D3D11UnorderedAccessView;
	class D3D11StructuredBuffer;
	class D3D11Device;
	class D3D11ShaderResourceView;

	class ParticleEmitter
	{
	public:
		ParticleEmitter(const std::shared_ptr<D3D11Device>& device, const ParticleEmitterInfo& info);

		void Swap();

		inline void SetParticleCount(size_t count) { mParticleCount = count; }

		inline size_t GetParticleCount() const { return mParticleCount; }
		inline const ParticleEmitterInfo& GetInfo() const;
		inline const std::shared_ptr<D3D11UnorderedAccessView>& GetAppendUAV() const;
		inline const std::shared_ptr<D3D11UnorderedAccessView>& GetConsumeUAV() const;
		inline const std::shared_ptr<D3D11StructuredBuffer>& GetAppendBuffer() const;
		inline const std::shared_ptr<D3D11StructuredBuffer>& GetConsumeBuffer() const;
		inline const std::shared_ptr<D3D11ShaderResourceView>& GetAppendSRV() const;
		inline const std::shared_ptr<D3D11ShaderResourceView>& GetConsumeSRV() const;

	private:
		ParticleEmitterInfo mInfo;

		size_t mParticleCount;
		std::shared_ptr<D3D11UnorderedAccessView> mAppendUAV;
		std::shared_ptr<D3D11UnorderedAccessView> mConsumeUAV;
		std::shared_ptr<D3D11StructuredBuffer> mAppendBuffer;
		std::shared_ptr<D3D11StructuredBuffer> mConsumeBuffer;
		std::shared_ptr<D3D11ShaderResourceView> mAppendSRV;
		std::shared_ptr<D3D11ShaderResourceView> mConsumeSRV;
	};

	inline const ParticleEmitterInfo& ParticleEmitter::GetInfo() const
	{
		return mInfo;
	}
	inline const std::shared_ptr<D3D11UnorderedAccessView>& ParticleEmitter::GetAppendUAV() const
	{
		return mAppendUAV;
	}
	inline const std::shared_ptr<D3D11UnorderedAccessView>& ParticleEmitter::GetConsumeUAV() const
	{
		return mConsumeUAV;
	}
	inline const std::shared_ptr<D3D11StructuredBuffer>& ParticleEmitter::GetAppendBuffer() const
	{
		return mAppendBuffer;
	}
	inline const std::shared_ptr<D3D11StructuredBuffer>& ParticleEmitter::GetConsumeBuffer() const
	{
		return mConsumeBuffer;
	}
	inline const std::shared_ptr<D3D11ShaderResourceView>& ParticleEmitter::GetAppendSRV() const
	{
		return mAppendSRV;
	}
	inline const std::shared_ptr<D3D11ShaderResourceView>& ParticleEmitter::GetConsumeSRV() const
	{
		return mConsumeSRV;
	}

	class D3D11Device;

	class D3D11ParticleManager
	{
	public:
		D3D11ParticleManager() = default;

		void AddParticleEmitter(size_t id, const std::shared_ptr<D3D11Device>& device, ParticleEmitterInfo emitter);
		void DeleteParticleEmitter(size_t id);

		inline const std::map<size_t, std::shared_ptr<ParticleEmitter>>& GetParticleEmitters() const;

	private:
		std::map<size_t, std::shared_ptr<ParticleEmitter>> mParticleEmitters;
	};

	inline const std::map<size_t, std::shared_ptr<ParticleEmitter>>& D3D11ParticleManager::GetParticleEmitters() const
	{
		return mParticleEmitters;
	}
}