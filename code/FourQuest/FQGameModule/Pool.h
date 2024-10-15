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
	/// ������Ʈ Ǯ
	/// </summary>
	template <typename T>
	class Pool : public IPool
	{
		using ComponentContainer = std::vector<std::shared_ptr<T>>;
	public:
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
		size_t GetSize()const override { return mContainer.size(); }

		/// <summary>
		/// Ǯ�� �ʱ�ȭ�մϴ� 
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