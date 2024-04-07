#pragma once

namespace fq::game_engine
{
	/// <summary>
	/// 윈도우 창에 대한 처리를 담당
	/// </summary>
	class WindowSystem
	{
	public:
		static UINT Width;
		static UINT Height;

	public:
		WindowSystem();
		~WindowSystem();

		/// <summary>
		/// 윈도우 창을 생성한다.
		/// </summary>
		void Initialize();

		/// <summary>
		/// 윈도우 창 크기 변경 확인
		/// </summary>
		/// <returns>변경 여부</returns>
		bool IsResizedWindow();

		/// <summary>
		/// 윈도우 창 크기 변경을 호출한다.
		/// </summary>
		void OnResize();

		/// <summary>
		/// 윈도우 종료
		/// </summary>
		void Finalize();

		/// <summary>
		/// 윈도우 핸들 반환
		/// </summary>
		/// <returns>윈도우 핸들</returns>
		HWND GetHWND()const { return mHWND; }
		
		/// <summary>
		///  HINSTANCE 반환
		/// </summary>
		/// <returns>HINSTANCE</returns>
		HINSTANCE GetHINSTANCE()const { return mHInstance; }

	private:
		UINT mScreenWidth;
		UINT mScreenHeight;
		UINT mScreenLeft;
		UINT mScreenTop;
		HWND mHWND;
		HINSTANCE mHInstance;
	};


}