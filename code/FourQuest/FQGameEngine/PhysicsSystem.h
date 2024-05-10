#pragma once

#include <unordered_map>
#include <directxtk/SimpleMath.h>

#include "../FQCommon/FQCommonPhysics.h"
#include "../FQGameModule/GameModule.h"

namespace fq::physics
{
	class IFQPhysics;
}

namespace fq::game_engine
{
	class GameProcess;

	class PhysicsSystem
	{
		using ColliderID = unsigned int;
		using ColliderInfo = std::pair<entt::id_type, fq::game_module::Component*>;
		using ColliderContainer = std::unordered_map<ColliderID, ColliderInfo>;

		using EventHandler = fq::game_module::EventHandler;
	public:
		PhysicsSystem();
		~PhysicsSystem();

		void Initialize(GameProcess* game);

		/// <summary>
		/// 물리엔진 시뮬레이션 이전에 물리공간과 게임공간을 연결해줍니다
		/// </summary>
		void SinkToPhysicsScene();

		/// <summary>
		/// 물리엔진 시뮬레이션이후에 물리공간과 게임공간을 연결해줍니다
		/// </summary>
		void SinkToGameScene();

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

		/// <summary>
		/// AddComponent 이벤트
		/// </summary>
		void AddComponent(const fq::event::AddComponent& event);

		/// <summary>
		/// RemoveCompnoent 이벤트
		/// </summary>
		void RemoveComponent(const fq::event::RemoveComponent& event);

		fq::physics::CollisionMatrix GetCollisionMatrix() const { return mCollisionMatrix; }
		void SetCollisionMatrix(fq::physics::CollisionMatrix matrix);

		/// <summary>
		/// 콜라이더를 반환합니다 
		/// </summary>
		fq::game_module::Component* GetCollider(ColliderID id)const;

	private:
		void addCollider(fq::game_module::GameObject* object);
		void removeCollider(fq::game_module::GameObject* object);
		void setPhysicsEngineinfo();

		void callBackEvent(fq::physics::CollisionData data, fq::physics::ECollisionEventType type);

	private:
		GameProcess* mGameProcess;
		fq::game_module::Scene* mScene;
		physics::IFQPhysics* mPhysicsEngine;
		
		DirectX::SimpleMath::Vector3 mGravity;
		fq::physics::CollisionMatrix mCollisionMatrix;
		fq::physics::PhysicsEngineInfo mPhysicsEngineInfomation;

		EventHandler mAddComponentHandler;
		EventHandler mRemoveComponentHandler;
		EventHandler mOnLoadSceneHandler;
		EventHandler mOnUnloadSceneHandler;
		EventHandler mOnAddGameObjectHandler;
		EventHandler mDestroyedGameObjectHandler;

		const entt::id_type mBoxID;
		const entt::id_type mSphereID;
		const entt::id_type mCapsuleID;
		const entt::id_type mMeshID;
		const entt::id_type mRigidID;

		bool mbIsGameLoaded;
		ColliderContainer mColliderContainer;
		ColliderID mLastColliderID;
	};


}