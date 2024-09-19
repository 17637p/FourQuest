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

			// 16:9 ���� �����ϱ� ���� �����ϸ� ���
			float renderAspect = 16.0f / 9.0f;
			float windowAspect = (float)mScreenWidth / mScreenHeight;

			if (windowAspect > renderAspect)
			{
				// â�� �� ���� ���
				mFixScreenWidth = mScreenHeight * renderAspect;
				mFixScreenHeight = mScreenHeight;
			}
			else
			{
				// â�� �� ���� ���
				mFixScreenWidth = mScreenWidth;
				mFixScreenHeight = mScreenWidth / renderAspect;
			}
		});

	mScreenHeight = screenHeight;
	mScreenWidth = screenWidth;

	// 16:9 ���� �����ϱ� ���� �����ϸ� ���
	float renderAspect = 16.0f / 9.0f;
	float windowAspect = (float)mScreenWidth / mScreenHeight;

	if (windowAspect > renderAspect)
	{
		// â�� �� ���� ���
		mFixScreenWidth = mScreenHeight * renderAspect;
		mFixScreenHeight = mScreenHeight;
	}
	else
	{
		// â�� �� ���� ���
		mFixScreenWidth = mScreenWidth;
		mFixScreenHeight = mScreenWidth / renderAspect;
	}
}

void fq::game_module::ScreenManager::SetScreenSize()
{
//	::SetWindowPos(


}
