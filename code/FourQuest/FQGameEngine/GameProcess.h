#pragma once

#include <memory>

namespace fq::game_module
{
	class SceneManager;
	class EventManager;
	class InputManager;
	class TimeManager;
}

namespace fq::game_engine
{
	class WindowSystem;

	/// <summary>
	/// GameProcess에서 사용되는 클래스 집합
	/// </summary>
	class GameProcess
	{
	public:
		GameProcess();
		~GameProcess();

		// Window 
		std::unique_ptr<WindowSystem> mWindowSystem;

		// GameModule
		std::unique_ptr<fq::game_module::InputManager> mInputManager;
		std::unique_ptr<fq::game_module::TimeManager>  mTimeManager;
		std::unique_ptr<fq::game_module::SceneManager> mSceneManager;
		std::unique_ptr<fq::game_module::EventManager> mEventManager;

		// Graphics
		

		// Physics

	};
}