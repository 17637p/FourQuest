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
		DirectX::SimpleMath::Vector4 Color;
		float TimeToLive;
		DirectX::SimpleMath::Vector3 Velocity;
		float Rotation;
		float Gravity;
	};
	class D3D11UnorderedAccessView;
	class D3D11StructuredBuffer;
	class D3D11Device;
	class D3D11ShaderResourceView;
	class D3D11VertexBuffer;

	class ParticleSystem
	{
	public:
		ParticleSystem(const std::shared_ptr<D3D11Device>& device, const ParticleSystemInfo& info);

		void Swap();

		bool Emission()
		{
			if (mEmissionRemainTime > mEmissionTime)
			{
				mEmissionRemainTime -= mEmissionTime;

				return true;
			}

			return false;
		}

		inline void AddDeltaTime(float deltaTime) {
			mAccumulateTime += deltaTime;
			mEmissionRemainTime += deltaTime;
		}
		inline void SetParticleCount(size_t count) { mParticleCount = count; }

		inline  ParticleSystemInfo& GetInfo() { return mInfo; }
		inline const ParticleSystemInfo& GetInfo() const;
		inline size_t GetParticleCount() const { return mParticleCount; }
		inline float GetAccumlateTime() const { return mAccumulateTime; }
		inline const std::shared_ptr<D3D11UnorderedAccessView>& GetAppendUAV() const;
		inline const std::shared_ptr<D3D11UnorderedAccessView>& GetConsumeUAV() const;
		inline const std::shared_ptr<D3D11StructuredBuffer>& GetAppendBuffer() const;
		inline const std::shared_ptr<D3D11StructuredBuffer>& GetConsumeBuffer() const;
		inline const std::shared_ptr<D3D11ShaderResourceView>& GetAppendSRV() const;
		inline const std::shared_ptr<D3D11ShaderResourceView>& GetConsumeSRV() const;

	private:
		ParticleSystemInfo mInfo;
		size_t mParticleCount;
		float mAccumulateTime;
		float mEmissionRemainTime;
		float mEmissionTime;
		std::shared_ptr<D3D11UnorderedAccessView> mAppendUAV;
		std::shared_ptr<D3D11UnorderedAccessView> mConsumeUAV;
		std::shared_ptr<D3D11StructuredBuffer> mAppendBuffer;
		std::shared_ptr<D3D11StructuredBuffer> mConsumeBuffer;
		std::shared_ptr<D3D11ShaderResourceView> mAppendSRV;
		std::shared_ptr<D3D11ShaderResourceView> mConsumeSRV;
	};

	inline const ParticleSystemInfo& ParticleSystem::GetInfo() const
	{
		return mInfo;
	}
	inline const std::shared_ptr<D3D11UnorderedAccessView>& ParticleSystem::GetAppendUAV() const
	{
		return mAppendUAV;
	}
	inline const std::shared_ptr<D3D11UnorderedAccessView>& ParticleSystem::GetConsumeUAV() const
	{
		return mConsumeUAV;
	}
	inline const std::shared_ptr<D3D11StructuredBuffer>& ParticleSystem::GetAppendBuffer() const
	{
		return mAppendBuffer;
	}
	inline const std::shared_ptr<D3D11StructuredBuffer>& ParticleSystem::GetConsumeBuffer() const
	{
		return mConsumeBuffer;
	}
	inline const std::shared_ptr<D3D11ShaderResourceView>& ParticleSystem::GetAppendSRV() const
	{
		return mAppendSRV;
	}
	inline const std::shared_ptr<D3D11ShaderResourceView>& ParticleSystem::GetConsumeSRV() const
	{
		return mConsumeSRV;
	}

	class D3D11Device;

	class D3D11ParticleManager
	{
	public:
		D3D11ParticleManager() = default;

		void AddDeltaTime(float deltaTime);
		void AddParticleSystem(size_t id, const std::shared_ptr<D3D11Device>& device, ParticleSystemInfo emitter);
		void DeleteParticleSystem(size_t id);

		inline const std::map<size_t, std::shared_ptr<ParticleSystem>>& GetParticleSystems() const;
		float GetLastDeltaTime() const { return mLastDeltaTime; }

	private:
		std::map<size_t, std::shared_ptr<ParticleSystem>> mParticleSystems;
		float mLastDeltaTime;
	};

	inline const std::map<size_t, std::shared_ptr<ParticleSystem>>& D3D11ParticleManager::GetParticleSystems() const
	{
		return mParticleSystems;
	}
}