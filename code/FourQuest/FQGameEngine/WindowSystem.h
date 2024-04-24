#pragma once

#include <windows.h>
#include <string>
#include <vector>

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
		static std::vector<std::wstring> DropFiles;

	public:
		WindowSystem();
		~WindowSystem();

		/// <summary>
		/// 윈도우 창을 생성한다.
		/// </summary>
		void Initialize();

		/// <summary>
		/// 에디터 전용 윈도우 창을 생성합니다
		/// </summary>
		void InitializeEditorType();

		/// <summary>
		/// 윈도우 창 크기 변경 확인
		/// </summary>
		/// <returns>변경 여부</returns>
		bool IsResizedWindow()const ;

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

		/// <summary>
		/// 윈도우 가로 길이 반환
		/// </summary>
		/// <returns>윈도우 가로</returns>
		UINT GetScreenWidth()const { return mScreenWidth; }
		
		/// <summary>
		/// 윈도우 창 세로 길이 반환
		/// </summary>
		/// <returns>윈도우 세로</returns>
		UINT GetScreenHeight()const { return mScreenHeight; }
		
		/// <summary>
		/// 윈도우 창 좌측 좌표 반환
		/// </summary>
		/// <returns>윈도우 좌픅 좌표</returns>
		UINT GetScreenLeft()const { return mScreenLeft; }

		/// <summary>
		/// 윈도우 창 상단 좌표 반환
		/// </summary>
		/// <returns>윈도우 상단 좌표</returns>
		UINT GetScreenTop()const { return mScreenTop; }

	private:
		UINT mScreenWidth;
		UINT mScreenHeight;
		UINT mScreenLeft;
		UINT mScreenTop;
		HWND mHWND;
		HINSTANCE mHInstance;
	};
}