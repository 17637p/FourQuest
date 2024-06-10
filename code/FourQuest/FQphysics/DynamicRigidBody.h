#pragma once

#include "RigidBody.h"
#include <memory>

namespace fq::physics
{
	class DynamicRigidBody : public RigidBody
	{
	public:
		/// <summary>
		/// ���� �������� �ݶ��̴� Ÿ�԰� ID, CollisionMatrix�� layerNumber�� �Է� �޽��ϴ�.
		/// </summary>
		/// <param name="colliderType"> �ݶ��̴� Ÿ�� </param>
		/// <param name="id"> ���ӿ��� ���� ���̵� </param>
		/// <param name="layerNumber"> �ݸ��� ��Ʈ������ ���̾� �ѹ� </param>
		DynamicRigidBody(fq::physics::EColliderType colliderType, unsigned int id, unsigned int layerNumber);
		virtual ~DynamicRigidBody();

		/// <summary>
		/// ������ �ٵ� ������ �����͸� �޾Ƽ� ������ �ٵ� �����մϴ�.
		/// </summary>
		/// <param name="colliderInfo"> �ݶ��̴� Ÿ�� </param>
		/// <param name="shape"> ���� </param>
		/// <param name="physics"> physx���� </param>
		bool Initialize(ColliderInfo colliderInfo, physx::PxShape* shape, physx::PxPhysics* physics, std::shared_ptr<CollisionData> data, bool isKinematic);

		/// <summary>
		/// ���̳��� ������ �ٵ� ��ȯ�մϴ�.
		/// </summary>
		inline physx::PxRigidDynamic* GetPxRigidDynamic();

		virtual void SetConvertScale(const DirectX::SimpleMath::Vector3& scale, physx::PxPhysics* physics, int* collisionMatrix) override;

	private:
		physx::PxRigidDynamic* mRigidDynamic;
	};

	physx::PxRigidDynamic* DynamicRigidBody::GetPxRigidDynamic()
	{
		return mRigidDynamic;
	}

}