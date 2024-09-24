#pragma once

#include <windows.h>

#include "EventHandler.h"

namespace fq::game_module
{
	class EventManager;

	class ScreenManager
	{
	public:
		ScreenManager();
		~ScreenManager();

		void Initialize(EventManager* eventMgr, UINT screenWidth, UINT screenHeight , HWND hwnd);

		UINT GetScreenWidth() const { return mScreenWidth; }
		UINT GetScreenHeight() const { return mScreenHeight; }

		UINT GetFixScreenWidth() const { return mFixScreenWidth; }
		UINT GetFixScreenHeight() const { return mFixScreenHeight; }

		/// <summary>
		/// 윈도우 해상도를 설정합니다 
		/// </summary>
		void SetScreenSize(UINT width, UINT height);

		/// <summary>
		/// 윈도우 전체화면 설정을 합니다 
		/// </summary>
		void SetFullScreen(bool isFull);

		/// <summary>
		/// 현재 윈도우가 전체화면 설정인지를 반환합니다 
		/// </summary>
		bool IsFullScreen()const { return mbIsFullScreen; }

	private:
		UINT mScreenWidth;
		UINT mScreenHeight;

		UINT mFixScreenWidth;
		UINT mFixScreenHeight;

		HWND mHwnd;
		RECT mPrevRect;

		bool mbIsFullScreen;

		game_module::EventHandler mSetScreenSizeHanlder;
	};


}