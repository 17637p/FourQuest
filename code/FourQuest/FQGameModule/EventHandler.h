#pragma once

namespace fq::game_module
{
	class EventManager;

	/// <summary>
	/// Event Handle의 소유권을 관리하는 객체이다.
	/// 생성, 복사가 불가능하고 객체의 소유권은 이동을 통해서 관리한다.
	/// </summary>
	class EventHandler
	{
	public:
		EventHandler() = delete;
		EventHandler(const EventHandler& other) = delete;
		EventHandler& operator=(const EventHandler& other) = delete;
		
		EventHandler(EventHandler&& other) noexcept;
		EventHandler& operator=(EventHandler&& other) noexcept;
		~EventHandler();

		/// <summary>
		/// 핸들러가 소유하는 핸들 주소를 반환
		/// </summary>
		/// <returns>핸들객체의 주소</returns>
		[[nodiscard]] const void* Handle() const;

		/// <summary>
		/// 이벤트 매니져에서 이벤트 핸들을 해제
		/// </summary>
		void Unregister() noexcept;

		/// <summary>
		/// 현재 이벤트 핸들이 등록여부
		/// </summary>
		/// <returns>등록 여부</returns>
		bool IsRegistered()const;

	private:
		EventHandler(const void* handle, EventManager* eventMgr);

	private:
		EventManager* mEventManager;
		const void* mHandle;

		friend class EventManager;
	};
}