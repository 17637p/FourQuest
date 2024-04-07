#pragma once

namespace fq::game_engine
{
	class WindowSystem;

	/// <summary>
	/// GameProcess���� ���Ǵ� Ŭ���� ����
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