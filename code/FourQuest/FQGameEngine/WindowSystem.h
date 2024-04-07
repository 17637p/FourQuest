#pragma once


namespace fq::game_engine
{
	/// <summary>
	/// 윈도우 창에 대한 처리를 담당
	/// </summary>
	class WindowSystem
	{
	public:
		WindowSystem();
		~WindowSystem();

		/// <summary>
		/// 윈도우 창을 생성한다.
		/// </summary>
		void Initialize();

		/// <summary>
		/// 윈도우 종료
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