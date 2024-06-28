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

	class TrailSystem
	{
		using EventHandler = fq::game_module::EventHandler;

	public:
		TrailSystem();
		~TrailSystem();

		void Initialize(GameProcess* gameProcess);
		void Update(float dt);

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

	private:
		void loadTrail(fq::game_module::GameObject* object);
		void unloadTrail(fq::game_module::GameObject* object);

	private:
		GameProcess* mGameProcess;
		EventHandler mOnLoadSceneHandler;
		EventHandler mOnUnloadSceneHandler;
		EventHandler mOnAddGameObjectHandler;
		EventHandler mDestroyedGameObjectHandler;
		EventHandler mAddComponentHandler;
		EventHandler mRemoveComponentHandler;

		bool mbIsGameLoaded;
	};
}