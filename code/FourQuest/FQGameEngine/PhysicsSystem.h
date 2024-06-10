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
			entt::id_type id;
			std::shared_ptr<game_module::Component> component;
			game_module::ICollider* collider;
			bool bIsDestroyed = false;
		};

		using ColliderContainer = std::unordered_map<ColliderID, ColliderInfo>;

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

		entt::id_type mBoxID;
		entt::id_type mSphereID;
		entt::id_type mCapsuleID;
		entt::id_type mMeshID;
		entt::id_type mCharactorControllerID;
		entt::id_type mRigidID;

		bool mbIsGameLoaded;
		ColliderContainer mColliderContainer;
		ColliderID mLastColliderID;
	};


}