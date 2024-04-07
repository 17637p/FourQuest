#pragma once


namespace fq::game_module
{
	class Scene;

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