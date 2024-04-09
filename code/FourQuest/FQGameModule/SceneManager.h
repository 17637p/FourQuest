#pragma once

#include <memory>
#include <string>

namespace fq::game_module
{
	class Scene;

	class EventManager;

	/// <summary>
	/// ���� �����ϴ� Ŭ���� 
	/// </summary>
	class SceneManager
	{
	public:
		SceneManager();
		~SceneManager();

		void Initialize(const std::string& startSceneName, EventManager* eventMgr);
		
		/// <summary>
		/// ���� �����մϴ�.
		/// </summary>
		/// <param name="sceneName">���� ���� �̸�</param>
		void ChangeScene(const std::string& nextSceneName);

		void Finalize();

	private:
		void loadScene();

	private:
		std::unique_ptr<Scene> mCurrentScene;
		EventManager* mEventManager;
	};


}