#pragma once

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

		/// Window 
		std::unique_ptr<WindowSystem> mWindowSystem;

		/// GameModule

		/// Graphics

		/// Physics

	};
}