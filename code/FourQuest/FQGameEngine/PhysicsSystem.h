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
		/// ���� �ý����� �ʱ�ȭ�մϴ�
		/// </summary>
		/// <param name="game"></param>
		void Initialize(GameProcess* game);

		/// <summary>
		/// �ݸ��� �ݹ��� ó���մϴ�.
		/// </summary>
		void ProcessCallBack();

		/// <summary>
		/// �ݶ��̴� �������� ó���� �մϴ� 
		/// </summary>
		void PostUpdate();

		/// <summary>
		/// �������� �ùķ��̼� ������ ���������� ���Ӱ����� �������ݴϴ�
		/// </summary>
		void SinkToPhysicsScene();

		/// <summary>
		/// �������� �ùķ��̼����Ŀ� ���������� ���Ӱ����� �������ݴϴ�
		/// </summary>
		void SinkToGameScene();

		/// <summary>
		/// ���� �ε��Ҷ� �������� ���õ� ���ҽ��� �ε��մϴ�
		/// </summary>
		void OnLoadScene(const fq::event::OnLoadScene event);

		/// <summary>
		/// ���� ��ε��Ҷ� �������� ���õ� ���ҽ��� ��ε��մϴ�.
		/// </summary>
		void OnUnLoadScene();

		/// <summary>
		/// ���ӿ�����Ʈ �߰� �̺�Ʈ
		/// </summary>
		void OnAddGameObject(const fq::event::AddGameObject& event);

		/// <summary>
		/// ���ӿ�����Ʈ ���� �̺�Ʈ
		/// </summary>
		void OnDestroyedGameObject(const fq::event::OnDestoryedGameObject& event);

		/// <summary>
		/// AddComponent �̺�Ʈ
		/// </summary>
		void AddComponent(const fq::event::AddComponent& event);

		/// <summary>
		/// RemoveCompnoent �̺�Ʈ
		/// </summary>
		void RemoveComponent(const fq::event::RemoveComponent& event);

		void CleanUp(const fq::event::OnCleanUp& event);
		
		fq::physics::CollisionMatrix GetCollisionMatrix() const { return mCollisionMatrix; }
		void SetCollisionMatrix(fq::physics::CollisionMatrix matrix);

		/// <summary>
		/// �ݶ��̴��� ��ȯ�մϴ� 
		/// </summary>
		fq::game_module::Component* GetCollider(ColliderID id)const;

		/// <summary>
		/// ĳ���� ��Ʈ�ѷ� �Է�ó�� �̺�Ʈ
		/// </summary>
		void AddInputMove(const fq::event::AddInputMove& event);

		void AddTerrainCollider(fq::game_module::GameObject* object);

		/// <summary>
		/// Raycast �̺�Ʈ ó��
		/// </summary>
		void Raycast(const fq::event::RayCast& event);

	private:
		void addCollider(fq::game_module::GameObject* object);
		void removeCollider(fq::game_module::GameObject* object);
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
		entt::id_type mTerrainTypeID;
		entt::id_type mArticulationTypeID;
		entt::id_type mRigidTypeID;

		bool mbIsGameLoaded;
		ColliderContainer mColliderContainer;
		ColliderID mLastColliderID;
		CallbackContainer mCallbacks;

		// Articulation
		fq::game_module::ArticulationLoader mArticulationLoader;
	};
}