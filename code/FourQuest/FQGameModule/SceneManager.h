#pragma once

#include <memory>

namespace fq::game_module
{
	class Scene;

	/// <summary>
	/// 씬을 관리하는 클래스 
	/// </summary>
	class SceneManager
	{
	public:
		SceneManager();
		~SceneManager();

		void Initialize();

		void Finalize();
	private:
		std::unique_ptr<Scene> mCurrentScene;
	};


}