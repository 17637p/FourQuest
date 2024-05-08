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
	/// ������Ʈ Ǯ
	/// </summary>
	template <typename T>
	class ComponentPool : public IComponentPool
	{
		using ComponentContainer = std::vector<std::shared_ptr<T>>;
	public:
		ComponentPool(){}
		~ComponentPool(){}

		/// <summary>
		/// ���ο� ������Ʈ�� Ǯ���� �����ɴϴ� 
		/// </summary>
		template <typename ...Args>
		std::shared_ptr<T> Assign(Args&& ...args)
		{
			// ���� ������ ������Ʈ�� �ִ� ��� 
			for (std::shared_ptr<T>& component : mContainer)
			{
				if (component.use_count() == 1)
				{
					*component = T(std::forward<Args>(args)...);
					return component;
				}
			}

			// ���� �Ҵ��մϴ� 
			auto component = std::make_shared<T>(std::forward<Args>(args)...);
			mContainer.push_back(component);
			return component;
		}

		/// <summary>
		/// Ǯ����� ��ȯ�մϴ� 
		/// </summary>
		size_t GetSize()const { return mContainer.size(); }

		/// <summary>
		/// Ǯ�� �ʱ�ȭ�մϴ� 
		/// </summary>
		void Clear() { mContainer.clear(); }

	private:
		ComponentContainer mContainer;
	};
}