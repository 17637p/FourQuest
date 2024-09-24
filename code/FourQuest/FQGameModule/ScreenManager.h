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
		/// ������ �ػ󵵸� �����մϴ� 
		/// </summary>
		void SetScreenSize(UINT width, UINT height);

		/// <summary>
		/// ������ ��üȭ�� ������ �մϴ� 
		/// </summary>
		void SetFullScreen(bool isFull);

		/// <summary>
		/// ���� �����찡 ��üȭ�� ���������� ��ȯ�մϴ� 
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