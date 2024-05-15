
#include "CommonHeader.h"

/*
파티클 매니저 진행 방식
1. CPU 자원을 쓰는 방식으로 기능 구현
2. GPU 자원을 써서 최적화하기
3. 유니티 인터페이스 참조해서 최대한 많은 기능 지원하기
*/

namespace fq::graphics
{
	// 입자 인스턴스
	struct Particle
	{
		DirectX::SimpleMath::Vector3 Position;
		DirectX::SimpleMath::Vector3 Velocity;
		float TimeToLive;
	};

	// 파티클의 초기 위치와 변화량에 대한 처리
	struct ParticleEmitterInfo
	{
		// 방출기 데이터
		size_t MaxParticleCount;

		// 입자 초기 데이터
		DirectX::SimpleMath::Vector3 InitPosition;
		DirectX::SimpleMath::Vector3 InitVelocity;
		float InitTimeToLive;

		// 입자마다 무작위성 부여
		DirectX::SimpleMath::Vector3 RandomRangePosition;
		DirectX::SimpleMath::Vector3 RandomRangeVelocity;
		float RandomRangeTimeToLive;
	};

	class ParticleEmitter
	{
	public:
		ParticleEmitter(const ParticleEmitterInfo& info)
			: mInfo(info)
		{
		}

		void Update(float delta)
		{

		}

		const std::vector<Particle*>& GetParticles() const { return mParticles; }

	private:
		ParticleEmitterInfo mInfo;
		std::vector<Particle*> mParticles;
	};

	template <typename T>
	class ObjectPool
	{
	public:
		ObjectPool(size_t poolSize)
			: mPoolSize(poolSize)
		{
		}

		~ObjectPool()
		{
			while (!mPool.empty())
			{
				T* obj = mPool.front();
				mPool.pop();

				delete obj;
			}
		}

		void Resize(size_t poolSize)
		{
			mPoolSize = poolSize;

			while (mPool.size() > mPoolSize)
			{
				delete mPool.front();
				mPool.pop();
			}
		}

		T* Get()
		{
			T* obj = nullptr;

			if (mPool.empty())
			{
				obj = new T();
			}
			else
			{
				obj = mPool.front();
				mPool.pop();
			}

			assert(obj != nullptr);
			return obj;
		}

		void Return(T* object)
		{
			if (mPool.size() > mPoolSize)
			{
				delete object;
			}

			mPool.push(object);
		}

		const std::queue<T*> GetPool() const
		{
			return mPool;
		}

	private:
		size_t mPoolSize;
		std::queue<T*> mPool;
	};

	class D3D11ParticleManager
	{
	public:
		D3D11ParticleManager() = default;

		void Update(float deltaTime)
		{

		}

		void SetParticleEmiiter(size_t id, ParticleEmitterInfo emitter)
		{

		}

		void AddParticleEmitter(size_t id, ParticleEmitterInfo emitter)
		{
			
		}

		void DeleteParticleEmitter(size_t id)
		{
			mParticleEmitters.erase(id);
		}

		const std::map<size_t, ParticleEmitter>& GetParticleEmitters() const
		{
			return mParticleEmitters;
		}

	private:
		std::map<size_t, ParticleEmitter> mParticleEmitters;
		ObjectPool<Particle> ObjectPool;
	};
}