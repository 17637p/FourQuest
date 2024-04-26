#pragma once

#include <memory>

namespace fq::game_module
{
	class SceneManager;
	class EventManager;
	class InputManager;
	class TimeManager;
	class ObjectManager;
}

namespace fq::graphics
{
	class IFQGraphics;
}

namespace fq::physics
{
	class IFQPhysics;
}

namespace fq::game_engine
{
	class WindowSystem;
	class CameraSystem;
	class RenderingSystem;
	class PhysicsSystem;
}

namespace fq::game_engine
{
	/// <summary>
	/// GameProcess에서 사용되는 클래스 집합
	/// </summary>
	class GameProcess
	{
	public:
		GameProcess();
		~GameProcess();

		// GameModule
		std::unique_ptr<fq::game_module::InputManager> mInputManager;
		std::unique_ptr<fq::game_module::TimeManager>  mTimeManager;
		std::unique_ptr<fq::game_module::SceneManager> mSceneManager;
		std::unique_ptr<fq::game_module::EventManager> mEventManager;
		std::unique_ptr<fq::game_module::ObjectManager> mObjectManager;

		// Graphics
		fq::graphics::IFQGraphics* mGraphics;
		
		// Physics
		fq::physics::IFQPhysics* mPhysics;

		// System
		std::unique_ptr<WindowSystem> mWindowSystem;
		std::unique_ptr<RenderingSystem> mRenderingSystem;
		std::unique_ptr<CameraSystem> mCameraSystem;
		std::unique_ptr<PhysicsSystem> mPhysicsSystem;
	};
}