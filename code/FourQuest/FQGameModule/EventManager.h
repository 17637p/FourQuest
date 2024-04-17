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
	/// Event와 EventHandle을 연결하는 클래스
	/// </summary>
	class EventManager
	{
	public:
		EventManager() = default;
	public:
		/// <summary>
		/// 등록한 이벤트 핸들을 제거
		/// </summary>
		/// <param name="eventHandler">등록한 이벤트를 관리하는 eventHandler</param>
		/// <returns>true : handle 삭제, false : 삭제 실패</returns>
		bool RemoveHandle(const EventHandler& eventHandler) noexcept;

		/// <summary>
		/// 현재 등록해둔 이벤트 핸들을 모두 삭제
		/// </summary>
		void RemoveAllHandles() noexcept;

		/// <summary>
		/// 현재 등록한 이벤트핸들의 갯수를 반환
		/// </summary>
		/// <returns>등록한 이벤트핸들의 갯수</returns>
		[[nodiscard]] std::size_t GetHandlerCount() noexcept ;

		/// <summary>
		/// EventType에 해당하는 이벤트핸들을 등록
		/// 등록한 함수객체는 EventType의 이벤트가 발생하면
		/// 등록한 함수를 호출
		/// </summary>
		/// <typeparam name="EventType">이벤트 타입</typeparam>
		/// <typeparam name="FunctionType">함수객체 타입</typeparam>
		/// <param name="function">호출 함수</param>
		/// <returns>핸들을 소유하는 핸들러 객체</returns>
		template <typename EventType, typename FunctionType, typename = std::enable_if<std::is_invocable_v<FunctionType> || std::is_invocable_v<FunctionType, EventType>>>
		[[nodiscard]] EventHandler RegisterHandle(FunctionType&& function);

		/// <summary>
		/// 클래스 멤버 함수를 이벤트 핸들로 등록
		/// </summary>
		/// <typeparam name="EventType">이벤트 타입</typeparam>
		/// <typeparam name="ClassType">클래스 타입</typeparam>
		/// <typeparam name="MemberFunction">멤버 함수</typeparam>
		/// <param name="classInstance">클래스의 인스턴스</param>
		/// <param name="function">호출 함수</param>
		/// <returns>핸들을 소유하는 핸들러 객체</returns>
		template <typename EventType, typename ClassType, typename MemberFunction>
		[[nodiscard]] EventHandler RegisterHandle(ClassType* classInstance, MemberFunction&& function) noexcept;

		/// <summary>
		/// EventType에 해당하는 핸들의 함수들을 호출.
		/// </summary>
		/// <typeparam name="EventType">이벤트 타입</typeparam>
		/// <param name="event">전달할 이벤트 정보</param>
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