#pragma once

#include <directxtk/SimpleMath.h>

#include "../FQGameModule/EventHandler.h"
#include "../FQGameModule/Event.h"
#include "../FQGameModule/CollisionMatrix.h"

namespace fq::game_engine
{
	class GameProcess;

	class PhysicsSystem
	{
		using EventHandler = fq::game_module::EventHandler;
	public:
		PhysicsSystem();
		~PhysicsSystem();

		void Initialize(GameProcess* game);

		/// <summary>
		/// 씬을 로드할때 랜더링에 관련된 리소스를 로드합니다
		/// </summary>
		void OnLoadScene(const fq::event::OnLoadScene event);

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

		fq::physics::CollisionMatrix GetCollisionMatrix() const { return mCollisionMatrix; }
		void SetCollisionMatrix(fq::physics::CollisionMatrix val) { mCollisionMatrix = val; }

	private:
		GameProcess* mGameProcess;
		
		DirectX::SimpleMath::Vector3 mGravity;
		fq::physics::CollisionMatrix mCollisionMatrix;

		EventHandler mOnLoadSceneHandler;
		EventHandler mOnUnloadSceneHandler;
		EventHandler mOnAddGameObjectHandler;
		EventHandler mDestroyedGameObjectHandler;

		bool mbIsGameLoaded;
	};


}