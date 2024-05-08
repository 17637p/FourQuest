#pragma once

#include <memory>
#include <unordered_map>

#include "../FQReflect/entt.hpp"
#include "ComponentPool.h"

namespace fq::game_module
{
	/// <summary>
	/// GameObject�� �޸𸮸� �����ϴ� Ǯ
	/// </summary>
	class ObjectPool
	{
		using Pool = std::unique_ptr<IComponentPool>;
		using ComponentID = entt::id_type;
	public:
		/// <summary>
		/// �̱��� Ŭ���� �ν��Ͻ��� ��ȯ�մϴ� 
		/// </summary>
		static ObjectPool* GetInstance();

		/// <summary>
		/// T Ÿ�Կ� �ش��ϴ� ������Ʈ�� ��ȯ�մϴ� 
		/// </summary>
		template <typename T, typename ...Args>
		std::shared_ptr<T> AssignComponent(Args&& ...args);

	private:
		ObjectPool();
		~ObjectPool();

		inline static ObjectPool* mPool = nullptr;
	private:
		std::unordered_map<ComponentID, Pool> mComponentPools;
	};

	template <typename T, typename ...Args>
	std::shared_ptr<T>
		fq::game_module::ObjectPool::AssignComponent(Args&& ...args)
	{
		ComponentID id = entt::resolve<T>().id();

		auto iter = mComponentPools.find(id);

		// ���ο� Ÿ���� ������Ʈ Ǯ�� �Ҵ��մϴ� 
		if (iter == mComponentPools.end())
		{
			mComponentPools.insert({ id, std::make_unique<ComponentPool<T>>() });
			iter = mComponentPools.find(id);
		}

		ComponentPool<T>* pool = static_cast<ComponentPool<T>*>(iter->second.get());
		auto component = pool->Assign(std::forward<Args>(args)...);

		return component;
	}
}