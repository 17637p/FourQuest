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

		void Initialize(EventManager* eventMgr);

		UINT GetScreenWidth() const { return mScreenWidth; }
		UINT GetScreenHeight() const { return mScreenHeight; }

	private:
		UINT mScreenWidth;
		UINT mScreenHeight;

		game_module::EventHandler mSetScreenSizeHanlder;
	};


}