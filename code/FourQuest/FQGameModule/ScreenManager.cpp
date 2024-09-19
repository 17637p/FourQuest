#include "ScreenManager.h"


#include "EventManager.h"
#include "Event.h"

fq::game_module::ScreenManager::ScreenManager()
	:mScreenHeight(0)
	, mScreenWidth(0)
	, mFixScreenWidth(0)
	, mFixScreenHeight(0)
{

}

fq::game_module::ScreenManager::~ScreenManager()
{

}

void fq::game_module::ScreenManager::Initialize(EventManager* eventMgr, UINT screenWidth, UINT screenHeight)
{
	mSetScreenSizeHanlder = eventMgr->RegisterHandle<fq::event::SetScreenSize>(
		[this](fq::event::SetScreenSize event) {
			mScreenHeight = event.height;
			mScreenWidth = event.width;

			// 16:9 비율 유지하기 위한 스케일링 계산
			float renderAspect = 16.0f / 9.0f;
			float windowAspect = (float)mScreenWidth / mScreenHeight;

			if (windowAspect > renderAspect)
			{
				// 창이 더 넓은 경우
				mFixScreenWidth = mScreenHeight * renderAspect;
				mFixScreenHeight = mScreenHeight;
			}
			else
			{
				// 창이 더 높은 경우
				mFixScreenWidth = mScreenWidth;
				mFixScreenHeight = mScreenWidth / renderAspect;
			}
		});

	mScreenHeight = screenHeight;
	mScreenWidth = screenWidth;

	// 16:9 비율 유지하기 위한 스케일링 계산
	float renderAspect = 16.0f / 9.0f;
	float windowAspect = (float)mScreenWidth / mScreenHeight;

	if (windowAspect > renderAspect)
	{
		// 창이 더 넓은 경우
		mFixScreenWidth = mScreenHeight * renderAspect;
		mFixScreenHeight = mScreenHeight;
	}
	else
	{
		// 창이 더 높은 경우
		mFixScreenWidth = mScreenWidth;
		mFixScreenHeight = mScreenWidth / renderAspect;
	}
}

void fq::game_module::ScreenManager::SetScreenSize()
{
//	::SetWindowPos(


}
