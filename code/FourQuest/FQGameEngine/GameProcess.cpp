#include "GameProcess.h"

#include "../FQGameModule/GameModule.h"

#include "WindowSystem.h"
#include "ModelSystem.h"
#include "RenderingSystem.h"

fq::game_engine::GameProcess::GameProcess()
	: mInputManager(std::make_unique<fq::game_module::InputManager>())
	, mTimeManager(std::make_unique<fq::game_module::TimeManager>())
	, mSceneManager(std::make_unique<fq::game_module::SceneManager>())
	, mEventManager(std::make_unique<fq::game_module::EventManager>())
	, mObjectManager(std::make_unique<fq::game_module::ObjectManager>())
	, mModelSystem(std::make_unique <ModelSystem>())
	, mWindowSystem(std::make_unique<WindowSystem>())
	, mRenderingSystem(std::make_unique<RenderingSystem>())
	, mGraphics(nullptr)
{}

fq::game_engine::GameProcess::~GameProcess()
{

}

