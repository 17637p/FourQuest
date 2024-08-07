#pragma once

#include <memory>
#include <string>

#include "EventHandler.h"
#include "Event.h"

namespace fq::game_module
{
	class Scene;

	class PrefabManager;
	class InputManager;
	class EventManager;
	class ScreenManager;
	class TimeManager;

	/// <summary>
	/// 씬을 관리하는 클래스 
	/// </summary>
	class SceneManager
	{
	public:
		SceneManager();
		~SceneManager();

		/// <summary>
		/// 씬매니져를 초기화를 합니다
		/// </summary>
		/// <param name="startSceneName">시작 씬의 이름</param>
		/// <param name="eventMgr">이벤트 매니져</param>
		void Initialize(const std::string& startSceneName
			, EventManager* eventMgr
			, InputManager* inputMgr
			, PrefabManager* prefabMgr
			, ScreenManager* screenMgr
			, TimeManager* timeMgr);

		/// <summary>
		/// 씬 매니져를 종료합니다
		/// </summary>
		void Finalize();

		/// <summary>
		/// 씬을 로드합니다
		/// </summary>
		void LoadScene();

		/// <summary>
		/// 씬을 언로드합니다
		/// </summary>
		void UnloadScene();

		/// <summary>
		/// Scene을 시작합니다
		/// </summary>
		void StartScene();

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
		/// 씬을 변경합니다.
		/// </summary>
		void ChangeScene();

		/// <summary>
		/// 후처리 이벤트를 처리합니다
		/// </summary>
		void PostUpdate();

		/// <summary>
		/// 현재 씬을 반환합니다
		/// </summary>
		/// <returns>현재 씬</returns>
		Scene* GetCurrentScene()const { return mCurrentScene.get(); };

		/// <summary>
		/// 게임이 종료여부를 반환합니다
		/// </summary>
		/// <returns>종료되면 true, 그렇지않으면 false</returns>
		bool IsEnd() const { return mbIsEnd; }

		/// <summary>
		/// 씬 병경 요청을 씬매니져에게 보냅니다
		/// </summary>
		/// <param name="event">요청에 필요한 정보</param>
		void RequestChangeScene(fq::event::RequestChangeScene event);
		
		/// <summary>
		/// 게임 종료 요청을 보냅니다
		/// </summary>
		/// <param name="event">요청에 필요한 정보</param>
		void RequestExitGame(fq::event::RequestExitGame event);
		
		bool IsChangeScene()const;

	private:
		std::unique_ptr<Scene> mCurrentScene;
		
		EventManager* mEventManager;
		PrefabManager* mPrefabManager;
		TimeManager* mTimeManager;

		EventHandler mRequestChangeSceneHandler;
		EventHandler mRequestExitGameHadler;

		std::string mNextSceneName;
		bool mbIsInvokeStartScene;
		bool mbIsEnd;
	};


}