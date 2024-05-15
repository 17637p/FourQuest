
#include "CommonHeader.h"

/*
��ƼŬ �Ŵ��� ���� ���
1. CPU �ڿ��� ���� ������� ��� ����
2. GPU �ڿ��� �Ἥ ����ȭ�ϱ�
3. ����Ƽ �������̽� �����ؼ� �ִ��� ���� ��� �����ϱ�
*/

namespace fq::graphics
{
	// ���� �ν��Ͻ�
	struct Particle
	{
		DirectX::SimpleMath::Vector3 Position;
		DirectX::SimpleMath::Vector3 Velocity;
		float TimeToLive;
	};

	// ��ƼŬ�� �ʱ� ��ġ�� ��ȭ���� ���� ó��
	struct ParticleEmitterInfo
	{
		// ����� ������
		size_t MaxParticleCount;

		// ���� �ʱ� ������
		DirectX::SimpleMath::Vector3 InitPosition;
		DirectX::SimpleMath::Vector3 InitVelocity;
		float InitTimeToLive;

		// ���ڸ��� �������� �ο�
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