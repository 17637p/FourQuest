#pragma once

#include "../FQGameModule/EventManager.h"
#include "../FQGameModule/Event.h"

namespace fq::game_module
{
	class GameObject;
}

namespace fq::game_engine
{
	class GameProcess;

	/// <summary>
	/// UI 관련 바인딩 처리를 담당합니다.
	/// </summary>
	class UISystem
	{
		using EventHandler = fq::game_module::EventHandler;

	public:
		UISystem();
		~UISystem();

		void Initialize(GameProcess* gameProcess);
		void Finalize();

		void Update(float dt);

		/// <summary>
		/// 씬을 로드할때 랜더링에 관련된 리소스를 로드합니다
		/// </summary>
		void OnLoadScene();

		/// <summary>
		/// 씬을 언로드할때 랜더링에 관련된 리소스를 언로드합니다.
		/// </summary>
		void OnUnLoadScene();

		/// <summary>
		/// 게임오브젝트 추가 이벤트
		/// </summary>
		void OnAddGameObject(const fq::event::AddGameObject& event);

		/// <summary>
		/// 게임오브젝트 삭제 이벤트
		/// </summary>
		void OnDestroyedGameObject(const fq::event::OnDestoryedGameObject& event);

		/// <summary>
		/// AddComponent 이벤트
		/// </summary>
		void AddComponent(const fq::event::AddComponent& event);

		/// <summary>
		/// RemoveCompnoent 이벤트
		/// </summary>
		void RemoveComponent(const fq::event::RemoveComponent& event);

		/// <summary>
		/// UI정보 변경 이벤트
		/// </summary>
		void SetUIInfomations(const fq::event::SetUIInfomations& event);

		/// <summary>
		/// Text정보 변경 이벤트
		/// </summary>
		void SetTextInformation(const fq::event::SetTextInformation& event);

		/// <summary>
		/// SpriteAnimation정보 변경 이벤트
		/// </summary>
		void SetSpriteAnimationInformation(const fq::event::SetSpriteAnimationInformation& event);

		/// <summary>
		/// Video정보 변경 이벤트
		/// </summary>
		void SetVideoInformation(const fq::event::SetVideoInformation& event);

		void LoadImageUI(game_module::GameObject* object);
		void UnloadImageUI(game_module::GameObject* object);

		void LoadTextUI(game_module::GameObject* object);
		void UnloadTextUI(game_module::GameObject* object);

		void LoadSpriteAnimationUI(game_module::GameObject* object);
		void UnloadSpriteAnimationUI(game_module::GameObject* object);

		void LoadVideoUI(game_module::GameObject* object);
		void UnloadVideoUI(game_module::GameObject* object);

	private:
		GameProcess* mGameProcess;

		EventHandler mOnLoadSceneHandler;
		EventHandler mOnUnloadSceneHandler;
		EventHandler mOnAddGameObjectHandler;
		EventHandler mDestroyedGameObjectHandler;
		EventHandler mAddComponentHandler;
		EventHandler mRemoveComponentHandler;
		EventHandler mSetUIInfomationsHandler;
		EventHandler mSetTextInformationHandler;
		EventHandler mSetSpriteInformationHandler;
		EventHandler mSetVideoInformationHandler;
		EventHandler mSetScreenSizeHandler;
		EventHandler mSetViewportSizeHandler;

		UINT mScreenWidth;
		UINT mScreenHeight;
		UINT mViewporWidth;
		UINT mViewportHeight;

		bool mbIsGameLoaded;
	};



}