#pragma once

#include <memory>
#include <unordered_map>
#include <spdlog/spdlog.h>

#include "../FQReflect/FQReflect.h"
#include "../FQReflect/entt.hpp"
#include "Pool.h"

namespace fq::game_module
{
	class GameObject;

	/// <summary>
	/// �޸� ���� Ǯ���� ����մϴ� 
	/// </summary>
	class ObjectPool
	{
		using ComponentPool = std::unique_ptr<IPool>;
		using ComponentID = entt::id_type;
		using PoolContainer = std::unordered_map<ComponentID, ComponentPool>;

	public:
		/// <summary>
		/// �̱��� Ŭ���� �ν��Ͻ��� ��ȯ�մϴ� 
		/// </summary>
		static ObjectPool* GetInstance();

		/// <summary>
		/// ������ƮǮ�� �����մϴ� 
		/// </summary>
		static void CleanUp();

		/// <summary>
		/// ������ƮǮ�� �����մϴ� 
		/// </summary>
		static void Finalize();

		/// <summary>
		/// T Ÿ�Կ� �ش��ϴ� ������Ʈ�� ��ȯ�մϴ� 
		/// </summary>
		template <typename T, typename ...Args>
		std::shared_ptr<T> Assign(Args&& ...args);

	private:
		ObjectPool() {}
		~ObjectPool() {}

	private:
		inline static ObjectPool* mInstance = nullptr;
		PoolContainer mPools;
	};

	template <typename T, typename ...Args>
	std::shared_ptr<T>
		fq::game_module::ObjectPool::Assign(Args&& ...args)
	{
		ComponentID id = entt::resolve<T>().id();

		auto iter = mPools.find(id);

		// ���ο� Ÿ���� ������Ʈ Ǯ�� �Ҵ��մϴ� 
		if (iter == mPools.end())
		{
			mPools.insert({ id, std::make_unique<Pool<T>>() });
			iter = mPools.find(id);
		}

		Pool<T>* pool = static_cast<Pool<T>*>(iter->second.get());
		auto component = pool->Assign(std::forward<Args>(args)...);

		return component;
	}
}