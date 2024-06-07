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
	/// 파티클 관련 바인딩 처리를 담당합니다.
	/// </summary>
	class ParticleSystem
	{
		using EventHandler = fq::game_module::EventHandler;

	public:
		ParticleSystem();
		~ParticleSystem();

		void Initialize(GameProcess* gameProcess);
		void Update(float dt);

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