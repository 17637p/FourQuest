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

		struct ColliderInfo
		{
			entt::id_type enttID;
			std::shared_ptr<game_module::Component> component;
			std::shared_ptr<game_module::GameObject> gameObject;
			game_module::ICollider* collider;
			bool bIsDestroyed = false;
			bool bIsRemoveBody = false;
		};

		struct CollisionCallBackInfo
		{
			fq::physics::CollisionData data;
			fq::physics::ECollisionEventType type;
		};

		using ColliderContainer = std::unordered_map<ColliderID, ColliderInfo>;
		using CallbackContainer = std::vector<CollisionCallBackInfo>;
		using EventHandler = fq::game_module::EventHandler;
	public:
		PhysicsSystem();
		~PhysicsSystem();

		/// <summary>
		/// 물리 시스템을 초기화합니다
		/// </summary>
		/// <param name="game"></param>
		void Initialize(GameProcess* game);

		/// <summary>
		/// 콜리전 콜백을 처리합니다.
		/// </summary>
		void ProcessCallBack();

		/// <summary>
		/// 콜라이더 삭제관련 처리를 합니다 
		/// </summary>
		void PostUpdate();

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

		void CleanUp(const fq::event::OnCleanUp& event);
		
		fq::physics::CollisionMatrix GetCollisionMatrix() const { return mCollisionMatrix; }
		void SetCollisionMatrix(fq::physics::CollisionMatrix matrix);

		/// <summary>
		/// 콜라이더를 반환합니다 
		/// </summary>
		fq::game_module::Component* GetCollider(ColliderID id)const;

		/// <summary>
		/// 캐릭터 컨트롤러 입력처리 이벤트
		/// </summary>
		void AddInputMove(const fq::event::AddInputMove& event);

		void AddTerrainCollider(fq::game_module::GameObject* object);

		/// <summary>
		/// Raycast 이벤트 처리
		/// </summary>
		void Raycast(const fq::event::RayCast& event);

	private:
		void addCollider(fq::game_module::GameObject* object);
		void removeCollider(fq::game_module::GameObject* object);
		void removeArticulation(fq::game_module::GameObject* object);
		void setPhysicsEngineinfo();
		void calculateOffset(common::Transform& t, DirectX::SimpleMath::Vector3 offset);
		void callBackEvent(fq::physics::CollisionData data, fq::physics::ECollisionEventType type);

	private:
		GameProcess* mGameProcess;
		fq::game_module::Scene* mScene;
		physics::IFQPhysics* mPhysicsEngine;
		
		DirectX::SimpleMath::Vector3 mGravity;
		fq::physics::CollisionMatrix mCollisionMatrix;
		fq::physics::PhysicsEngineInfo mPhysicsEngineInfomation;
		int mOneFrameRagdollCreateCount;

		EventHandler mAddComponentHandler;
		EventHandler mRemoveComponentHandler;
		EventHandler mOnLoadSceneHandler;
		EventHandler mOnCleanUpSceneHandler;
		EventHandler mOnUnloadSceneHandler;
		EventHandler mOnAddGameObjectHandler;
		EventHandler mDestroyedGameObjectHandler;
		EventHandler mAddInputMoveHandler;
		EventHandler mRaycastHandler;

		entt::id_type mBoxTypeID;
		entt::id_type mSphereTypeID;
		entt::id_type mCapsuleTypeID;
		entt::id_type mMeshTypeID;
		entt::id_type mCharactorControllerTypeID;
		entt::id_type mControllerTypeID;
		entt::id_type mTerrainTypeID;
		entt::id_type mArticulationTypeID;
		entt::id_type mRigidTypeID;
		entt::id_type mTriangleTypeID;
		entt::id_type mClothTypeID;

		bool mbIsGameLoaded;
		int mAddControllerID;
		ColliderContainer mColliderContainer;
		ColliderID mLastColliderID;
		CallbackContainer mCallbacks;

		// Articulation
		fq::game_module::ArticulationLoader mArticulationLoader;
	};
}