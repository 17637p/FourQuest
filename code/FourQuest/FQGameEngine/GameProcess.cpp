#include "GameProcess.h"

#include "../FQGameModule/GameModule.h"

#include "WindowSystem.h"
#include "ModelSystem.h"
#include "RenderingSystem.h"
#include "CameraSystem.h"
#include "PhysicsSystem.h"

fq::game_engine::GameProcess::GameProcess()
	: mInputManager(std::make_unique<fq::game_module::InputManager>())
	, mTimeManager(std::make_unique<fq::game_module::TimeManager>())
	, mSceneManager(std::make_unique<fq::game_module::SceneManager>())
	, mEventManager(std::make_unique<fq::game_module::EventManager>())
	, mObjectManager(std::make_unique<fq::game_module::ObjectManager>())
	, mWindowSystem(std::make_unique<WindowSystem>())
	, mRenderingSystem(std::make_unique<RenderingSystem>())
	, mCameraSystem(std::make_unique<CameraSystem>())
	, mPhsicsSystem(std::make_unique<PhysicsSystem>())
	, mGraphics(nullptr)
	, mPhysics(nullptr)
{}

fq::game_engine::GameProcess::~GameProcess()
{

}

