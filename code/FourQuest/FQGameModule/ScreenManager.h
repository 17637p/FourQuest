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

		void Initialize(EventManager* eventMgr, UINT screenWidth, UINT screenHeight);

		UINT GetScreenWidth() const { return mScreenWidth; }
		UINT GetScreenHeight() const { return mScreenHeight; }

		UINT GetFixScreenWidth() const { return mFixScreenWidth; }
		UINT GetFixScreenHeight() const { return mFixScreenHeight; }

		void SetScreenSize();

	private:
		UINT mScreenWidth;
		UINT mScreenHeight;

		UINT mFixScreenWidth;
		UINT mFixScreenHeight;

		game_module::EventHandler mSetScreenSizeHanlder;
	};


}