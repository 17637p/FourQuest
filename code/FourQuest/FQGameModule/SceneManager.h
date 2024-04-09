#pragma once

#include <memory>
#include <string>

namespace fq::game_module
{
	class Scene;

	class EventManager;

	/// <summary>
	/// 씬을 관리하는 클래스 
	/// </summary>
	class SceneManager
	{
	public:
		SceneManager();
		~SceneManager();

		void Initialize(const std::string& startSceneName, EventManager* eventMgr);
		
		/// <summary>
		/// 씬을 변경합니다.
		/// </summary>
		/// <param name="sceneName">다음 씬의 이름</param>
		void ChangeScene(const std::string& nextSceneName);

		void Finalize();

	private:
		void loadScene();

	private:
		std::unique_ptr<Scene> mCurrentScene;
		EventManager* mEventManager;
	};


}