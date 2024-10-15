#pragma once

#include <vector>
#include <memory>
#include <spdlog/spdlog.h>

namespace fq::game_module
{
	class Component;

	class IPool
	{
	public:
		virtual ~IPool(){}

		virtual void Clear() abstract;
		virtual void CleanUp() abstract;
		virtual size_t GetSize()const abstract;
	};

	/// <summary>
	/// 컴포넌트 풀
	/// </summary>
	template <typename T>
	class Pool : public IPool
	{
		using ComponentContainer = std::vector<std::shared_ptr<T>>;
	public:
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
		size_t GetSize()const override { return mContainer.size(); }

		/// <summary>
		/// 풀을 초기화합니다 
		/// </summary>
		void Clear() override {
			mContainer.clear();
		}

		void CleanUp() override
		{
			mContainer.erase(std::remove_if(mContainer.begin(), mContainer.end()
				, [](std::shared_ptr<T>& element)
				{
					return element.use_count() == 1;	
				}), mContainer.end());
		}

	private:
		ComponentContainer mContainer;
	};
}