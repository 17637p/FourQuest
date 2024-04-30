#pragma once

#include "RigidBody.h"

namespace fq::physics
{
	class StaticRigidBody : public RigidBody
	{
	public:
		/// <summary>
		/// �ݶ��̴� Ÿ�԰� ID, CollisionMatrix�� layerNumber�� �Է� �޽��ϴ�.
		/// </summary>
		/// <param name="colliderType"> �ݶ��̴� Ÿ�� </param>
		/// <param name="id"> ���ӿ��� ���� ���̵� </param>
		/// <param name="layerNumber"> �ݸ��� ��Ʈ������ ���̾� �ѹ� </param>
		StaticRigidBody(fq::physics::EColliderType colliderType, unsigned int id, unsigned int layerNumber);
		virtual ~StaticRigidBody();

		/// <summary>
		/// ������ �ٵ� ������ �����͸� �޾Ƽ� ���°� �ִ� ������ �ٵ� �����մϴ�.
		/// </summary>
		/// <param name="colliderInfo"> �ݶ��̴� Ÿ�� </param>
		/// <param name="shape"> ���� </param>
		/// <param name="physics"> physx���� </param>
		bool Initialize(ColliderInfo colliderInfo, physx::PxShape* shape, physx::PxPhysics* physics);

		/// <summary>
		/// ����ƽ ������ �ٵ� ��ȯ�մϴ�.
		/// </summary>
		inline physx::PxRigidStatic* GetPxRigidStatic();

	private:
		physx::PxRigidStatic* mRigidStatic;
	};

	physx::PxRigidStatic* StaticRigidBody::GetPxRigidStatic()
	{
		return mRigidStatic;
	}
}