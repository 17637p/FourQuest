#pragma once


namespace fq::game_engine
{
	/// <summary>
	/// ������ â�� ���� ó���� ���
	/// </summary>
	class WindowSystem
	{
	public:
		WindowSystem();
		~WindowSystem();

		/// <summary>
		/// ������ â�� �����Ѵ�.
		/// </summary>
		void Initialize();

		/// <summary>
		/// ������ ����
		/// </summary>
		void Finalize();

		HWND GetHWND()const { return mHWND; }
		HINSTANCE GetHINSTANCE()const { return mHInstance; }

	private:
		RECT mWindowRect;

		HWND mHWND;
		HINSTANCE mHInstance;
	};


}