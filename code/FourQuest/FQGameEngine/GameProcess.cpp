#include "FQGameEnginePCH.h"
#include "GameProcess.h"

#include "WindowSystem.h"

fq::game_engine::GameProcess::GameProcess()
	:mWindowSystem(std::make_unique<WindowSystem>())
	, mInputManager(std::make_unique<fq::game_module::InputManager>())
	, mTimeManager(std::make_unique<fq::game_module::TimeManager>())
	, mSceneManager(std::make_unique<fq::game_module::SceneManager>())
	, mEventManager(std::make_unique<fq::game_module::EventManager>())
{}

fq::game_engine::GameProcess::~GameProcess()
{

}

