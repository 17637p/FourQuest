#pragma once

#include <memory>
#include <unordered_map>

#include "../FQReflect/entt.hpp"
#include "ComponentPool.h"

namespace fq::game_module
{
	/// <summary>
	/// GameObject의 메모리를 관리하는 풀
	/// </summary>
	class ObjectPool
	{
		using Pool = std::unique_ptr<IComponentPool>;
		using ComponentID = entt::id_type;
	public:
		/// <summary>
		/// 싱글톤 클래스 인스턴스를 반환합니다 
		/// </summary>
		static ObjectPool* GetInstance();

		/// <summary>
		/// T 타입에 해당하는 컴포넌트를 반환합니다 
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

		// 새로운 타입의 컴포넌트 풀을 할당합니다 
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