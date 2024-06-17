#include "ScreenManager.h"


#include "EventManager.h"
#include "Event.h"

fq::game_module::ScreenManager::ScreenManager()
	:mScreenHeight(0)
	, mScreenWidth(0)
{

}

fq::game_module::ScreenManager::~ScreenManager()
{

}

void fq::game_module::ScreenManager::Initialize(EventManager* eventMgr)
{
	mSetScreenSizeHanlder = eventMgr->RegisterHandle<fq::event::SetScreenSize>(
		[this](fq::event::SetScreenSize event) {
			mScreenHeight = event.height;
			mScreenWidth = event.width;
		});


}
