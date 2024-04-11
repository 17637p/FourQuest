#pragma once

#include <memory>
#include <string>

namespace fq::game_module
{
	class Scene;

	class InputManager;
	class EventManager;

	/// <summary>
	/// 씬을 관리하는 클래스 
	/// </summary>
	class SceneManager
	{
	public:
		SceneManager();
		~SceneManager();

		/// <summary>
		/// 씬에 관련된 초기화를 합니다
		/// </summary>
		/// <param name="startSceneName">시작 씬의 이름</param>
		/// <param name="eventMgr">이벤트 매니져</param>
		void Initialize(const std::string& startSceneName, EventManager* eventMgr, InputManager* inputMgr);

		/// <summary>
		/// 물리 처리를 위한 고정프레임 업데이트
		/// </summary>
		/// <param name="dt"></param>
		void FixedUpdate(float dt);

		/// <summary>
		/// 씬 업데이트
		/// </summary>
		/// <param name="dt"></param>
		void Update(float dt);

		/// <summary>
		/// 씬의 모든 오브젝트 업데이트 이후에 호출
		/// </summary>
		/// <param name="dt"></param>
		void LateUpdate(float dt);

		/// <summary>
		/// 씬에 관련된 종료처리를 합니다
		/// </summary>
		void Finalize();

		/// <summary>
		/// 씬을 변경합니다.
		/// </summary>
		/// <param name="sceneName">다음 씬의 이름</param>
		void ChangeScene(const std::string& nextSceneName);

		/// <summary>
		/// 현재 씬을 반환합니다
		/// </summary>
		/// <returns>현재 씬</returns>
		Scene* GetCurrentScene()const { return mCurrentScene.get(); };

	private:
		void loadScene();

	private:
		std::unique_ptr<Scene> mCurrentScene;
		EventManager* mEventManager;
		
	};


}