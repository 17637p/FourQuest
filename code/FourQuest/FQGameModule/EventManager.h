#pragma once

#include <any>
#include <atomic>
#include <functional>
#include <mutex>
#include <thread>
#include <shared_mutex>
#include <unordered_map>
#include <typeindex>
#include <utility>

#include "EventHandler.h"
#include "FunctionTraits.h"

namespace fq::game_module
{
	/// <summary>
	/// Event�� EventHandle�� �����ϴ� Ŭ����
	/// </summary>
	class EventManager
	{
	public:
		EventManager() = default;
	public:
		/// <summary>
		/// ����� �̺�Ʈ �ڵ��� ����
		/// </summary>
		/// <param name="eventHandler">����� �̺�Ʈ�� �����ϴ� eventHandler</param>
		/// <returns>true : handle ����, false : ���� ����</returns>
		bool RemoveHandle(const EventHandler& eventHandler) noexcept;

		/// <summary>
		/// ���� ����ص� �̺�Ʈ �ڵ��� ��� ����
		/// </summary>
		void RemoveAllHandles() noexcept;

		/// <summary>
		/// ���� ����� �̺�Ʈ�ڵ��� ������ ��ȯ
		/// </summary>
		/// <returns>����� �̺�Ʈ�ڵ��� ����</returns>
		[[nodiscard]] std::size_t GetHandlerCount() noexcept ;

		/// <summary>
		/// EventType�� �ش��ϴ� �̺�Ʈ�ڵ��� ���
		/// ����� �Լ���ü�� EventType�� �̺�Ʈ�� �߻��ϸ�
		/// ����� �Լ��� ȣ��
		/// </summary>
		/// <typeparam name="EventType">�̺�Ʈ Ÿ��</typeparam>
		/// <typeparam name="FunctionType">�Լ���ü Ÿ��</typeparam>
		/// <param name="function">ȣ�� �Լ�</param>
		/// <returns>�ڵ��� �����ϴ� �ڵ鷯 ��ü</returns>
		template <typename EventType, typename FunctionType, typename = std::enable_if<std::is_invocable_v<FunctionType> || std::is_invocable_v<FunctionType, EventType>>>
		[[nodiscard]] EventHandler RegisterHandle(FunctionType&& function);

		/// <summary>
		/// Ŭ���� ��� �Լ��� �̺�Ʈ �ڵ�� ���
		/// </summary>
		/// <typeparam name="EventType">�̺�Ʈ Ÿ��</typeparam>
		/// <typeparam name="ClassType">Ŭ���� Ÿ��</typeparam>
		/// <typeparam name="MemberFunction">��� �Լ�</typeparam>
		/// <param name="classInstance">Ŭ������ �ν��Ͻ�</param>
		/// <param name="function">ȣ�� �Լ�</param>
		/// <returns>�ڵ��� �����ϴ� �ڵ鷯 ��ü</returns>
		template <typename EventType, typename ClassType, typename MemberFunction>
		[[nodiscard]] EventHandler RegisterHandle(ClassType* classInstance, MemberFunction&& function) noexcept;

		/// <summary>
		/// EventType�� �ش��ϴ� �ڵ��� �Լ����� ȣ��.
		/// </summary>
		/// <typeparam name="EventType">�̺�Ʈ Ÿ��</typeparam>
		/// <param name="event">������ �̺�Ʈ ����</param>
		template <typename EventType, typename = std::enable_if_t<!std::is_pointer_v<EventType>>>
		void FireEvent(EventType&& event) noexcept;
	
	private:
		template <typename Callable>
		void accessSharedHandler(Callable&& callable);

		template <typename Callable>
		void accessUniqueHandler(Callable&& callable);

	private:
		using mutex_type = std::shared_mutex;

		mutable mutex_type mRegistrationMutex;
		std::unordered_multimap<std::type_index, std::function<void(std::any)>> mEventHandles;
	};
}

template <typename EventType, typename /*= std::enable_if_t<!std::is_pointer_v<EventType>>*/>
void fq::game_module::EventManager::FireEvent(EventType&& event) noexcept
{
	accessSharedHandler([this, localEvent = std::forward<EventType>(event)]()
		{
			for (auto [beginIter, endIter] =
				mEventHandles.equal_range(std::type_index(typeid(EventType)));
				beginIter != endIter; ++beginIter)
			{
				beginIter->second(localEvent);
			}
		});
}

template <typename EventType, typename FunctionType,
	typename /*= std::enable_if<std::is_invocable_v<FunctionType> ||
	std::is_invocable_v<FunctionType, EventType>>*/>
[[nodiscard]] fq::game_module::EventHandler fq::game_module::EventManager::RegisterHandle(FunctionType&& function)
{
	using traits = function_traits::function_traits<FunctionType>;
	const auto typeIndex = std::type_index(typeid(EventType));
	const void* handle = nullptr;

	if constexpr (traits::arity == 0)
	{
		accessUniqueHandler([&]()
			{
				auto iter = mEventHandles.emplace(typeIndex
					, [function = std::forward<FunctionType>(function)](auto) { function(); });

				handle = static_cast<const void*>(&(iter->second));
			});
	}
	else
	{
		accessUniqueHandler([&]()
			{
				auto iter = mEventHandles.emplace(typeIndex
					, [func = std::forward<FunctionType>(function)](auto value)
					{
						func(std::any_cast<EventType>(value));
					});

				handle = static_cast<const void*>(&(iter->second));
			});
	}
	return { handle, this };
}

template<typename EventType, typename ClassType, typename MemberFunction>
inline fq::game_module::EventHandler fq::game_module::EventManager::RegisterHandle(ClassType* classInstance, MemberFunction&& function) noexcept
{
	using traits = function_traits::function_traits<MemberFunction>;
	static_assert(std::is_same_v<ClassType, std::decay_t<typename traits::owner_type>>
		, "Member function pointer must match instance type");

	const auto typeIndex = std::type_index(typeid(EventType));
	const void* handle =nullptr;

	if constexpr (traits::arity == 0)
	{
		accessUniqueHandler([&]()
			{
				auto iter = mEventHandles.emplace(typeIndex
					, [classInstance, function](auto)
					{
						(classInstance->*function)();
					});

				handle = static_cast<const void*>(&(iter->second));
			});
	}
	else
	{
		accessUniqueHandler([&]()
			{
				auto iter = mEventHandles.emplace(typeIndex
					, [classInstance, function](auto value)
					{
						(classInstance->*function)(std::any_cast<EventType>(value));
					});

				handle = static_cast<const void*>(&(iter->second));
			});
	}

	return { handle, this };
}


template <typename Callable>
void fq::game_module::EventManager::accessSharedHandler(Callable&& callable)
{
	try
	{
		std::shared_lock<mutex_type> lock(mRegistrationMutex);
		callable();
	}
	catch (std::system_error&) {}
}

template<typename Callable>
inline void fq::game_module::EventManager::accessUniqueHandler(Callable&& callable)
{
	try
	{
		std::unique_lock<mutex_type> lock(mRegistrationMutex);
		callable();
	}
	catch (const std::system_error&) {}
}