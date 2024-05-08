#pragma once

#include <vector>
#include <memory>
#include <spdlog/spdlog.h>

namespace fq::game_module
{
	class Component;

	class IComponentPool
	{
	public:
	};

	/// <summary>
	/// 컴포넌트 풀
	/// </summary>
	template <typename T>
	class ComponentPool : public IComponentPool
	{
		using ComponentContainer = std::vector<std::shared_ptr<T>>;
	public:
		ComponentPool(){}
		~ComponentPool(){}

		/// <summary>
		/// 새로운 컴포넌트를 풀에서 가져옵니다 
		/// </summary>
		template <typename ...Args>
		std::shared_ptr<T> Assign(Args&& ...args)
		{
			// 재사용 가능한 컴포넌트가 있는 경우 
			for (std::shared_ptr<T>& component : mContainer)
			{
				if (component.use_count() == 1)
				{
					*component = T(std::forward<Args>(args)...);
					return component;
				}
			}

			// 새로 할당합니다 
			auto component = std::make_shared<T>(std::forward<Args>(args)...);
			mContainer.push_back(component);
			return component;
		}

		/// <summary>
		/// 풀사이즈를 반환합니다 
		/// </summary>
		size_t GetSize()const { return mContainer.size(); }

		/// <summary>
		/// 풀을 초기화합니다 
		/// </summary>
		void Clear() { mContainer.clear(); }

	private:
		ComponentContainer mContainer;
	};
}