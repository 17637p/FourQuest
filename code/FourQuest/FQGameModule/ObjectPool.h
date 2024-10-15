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
	/// 메모리 관리 풀링을 담당합니다 
	/// </summary>
	class ObjectPool
	{
		using ComponentPool = std::unique_ptr<IPool>;
		using ComponentID = entt::id_type;
		using PoolContainer = std::unordered_map<ComponentID, ComponentPool>;

	public:
		/// <summary>
		/// 싱글톤 클래스 인스턴스를 반환합니다 
		/// </summary>
		static ObjectPool* GetInstance();

		/// <summary>
		/// 오브젝트풀을 정리합니다 
		/// </summary>
		static void CleanUp();

		/// <summary>
		/// 오브젝트풀을 종료합니다 
		/// </summary>
		static void Finalize();

		/// <summary>
		/// T 타입에 해당하는 컴포넌트를 반환합니다 
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

		// 새로운 타입의 컴포넌트 풀을 할당합니다 
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