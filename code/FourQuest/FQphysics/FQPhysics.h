#pragma once
#include "IFQPhysics.h"

#include <physx\PxPhysicsAPI.h>
#include <memory>

namespace fq::physics
{
	class Physics;
	class PhysicsRigidBodyManager;
	class PhysicsSimulationEventCallback;

	class FQPhysics : public IFQPhysics
	{
	public:
		FQPhysics();
		virtual ~FQPhysics();

		/// <summary>
		/// ���� ������ �����Ͽ� ���� ������ ����ϴ�.
		/// </summary>
		virtual bool Initialize(PhysicsEngineInfo& info) override;

		/// <summary>
		/// ���� ������ ������Ʈ�� ���� ������ �ٵ� �߰��ϰ� �ùķ��̼��� �����մϴ�.
		/// �ùķ��̼��� �������� �ÿ� �浹 �̺�Ʈ�� �ݹ� �Լ��� ���� ����˴ϴ�.
		/// </summary>
		virtual bool Update(float deltaTime) override;

		/// <summary>
		/// ���� ������ �߰��� ����ƽ �ٵ� �� ���̳��� �ٵ� �����մϴ�.
		/// </summary>
		virtual bool CreateStaticBody(const BoxColliderInfo& info, const EColliderType& colliderType) override;
		virtual bool CreateStaticBody(const SphereColliderInfo& info, const EColliderType& colliderType) override;
		virtual bool CreateStaticBody(const CapsuleColliderInfo& info, const EColliderType& colliderType) override;
		virtual bool CreateStaticBody(const ConvexMeshColliderInfo& info, const EColliderType& colliderType) override;
		virtual bool CreateDynamicBody(const BoxColliderInfo& info, const EColliderType& colliderType) override;
		virtual bool CreateDynamicBody(const SphereColliderInfo& info, const EColliderType& colliderType) override;
		virtual bool CreateDynamicBody(const CapsuleColliderInfo& info, const EColliderType& colliderType) override;
		virtual bool CreateDynamicBody(const ConvexMeshColliderInfo& info, const EColliderType& colliderType) override;

		/// <summary>
		/// 
		/// </summary>
		/// <param name="id"></param>
		/// <returns></returns>
		virtual DirectX::SimpleMath::Matrix GetRigidBodyMatrix(unsigned int id) override;

		/// <summary>
		/// ���̵� ���� ���� ������ �ٵ� �����մϴ�.
		/// </summary>
		virtual bool RemoveRigidBody(unsigned int id) override;

		/// <summary>
		/// ���� ������ �ִ� ��� ������ �ٵ���� �����մϴ�.
		/// </summary>
		virtual bool RemoveAllRigidBody() override;

		/// <summary>
		/// �ݹ� �Լ� ���
		/// </summary>
		/// <param name="func"> �ݸ��� ������ ����ü </param>
		/// <param name="eventType"> �ݸ��� �̺�Ʈ Ÿ�� </param>
		virtual void SetCallBackFunction(std::function<void(fq::physics::CollisionData, fq::physics::ECollisionEventType)> func) override;

		/// <summary>
		/// ���� ������ ������ ������ ���� �޽��ϴ�. ( �߷�, �浹 ��Ʈ���� )
		/// </summary>
		/// <param name="info"> ���� ���� ���� </param>
		virtual void SetPhysicsInfo(PhysicsEngineInfo info) override;

	private:
		// ��
		physx::PxScene* mScene;
		std::shared_ptr<Physics> mPhysics;
		std::shared_ptr<PhysicsRigidBodyManager> mRigidBodyManager;

		// �浹 �̺�Ʈ Ŭ����
		std::shared_ptr<PhysicsSimulationEventCallback> mMyEventCallback;

		// �浹 ��Ʈ����
		int* mCollisionMatrix;
	};
}

