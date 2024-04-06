#pragma once

namespace fq::game_module
{
	class EventManager;

	/// <summary>
	/// Event Handle�� �������� �����ϴ� ��ü�̴�.
	/// ����, ���簡 �Ұ����ϰ� ��ü�� �������� �̵��� ���ؼ� �����Ѵ�.
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
		/// �ڵ鷯�� �����ϴ� �ڵ� �ּҸ� ��ȯ
		/// </summary>
		/// <returns>�ڵ鰴ü�� �ּ�</returns>
		[[nodiscard]] const void* Handle() const;

		/// <summary>
		/// �̺�Ʈ �Ŵ������� �̺�Ʈ �ڵ��� ����
		/// </summary>
		void Unregister() noexcept;

		/// <summary>
		/// ���� �̺�Ʈ �ڵ��� ��Ͽ���
		/// </summary>
		/// <returns>��� ����</returns>
		bool IsRegistered()const;

	private:
		EventHandler(const void* handle, EventManager* eventMgr);

	private:
		EventManager* mEventManager;
		const void* mHandle;

		friend class EventManager;
	};
}