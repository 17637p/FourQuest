#pragma once

#include <memory>
#include <map>
#include <set>
#include <physx\PxPhysicsAPI.h>

#include "FQCommonPhysics.h"

namespace fq::physics
{
	class CharacterController;

	class PhysicsCharactorControllerManager
	{
	public:
		PhysicsCharactorControllerManager();
		~PhysicsCharactorControllerManager();

		/// <summary>
		/// ĳ���� ��Ʈ�ѷ� �Ŵ��� ���� �� �ʱ�ȭ �Լ�
		/// </summary>
		bool initialize(physx::PxScene* scene, physx::PxPhysics* physics);

		/// <summary>
		/// ĳ���� ��Ʈ�ѷ� �Ŵ����� �����ϰ� �ִ� ĳ���� ��Ʈ�ѷ� ������Ʈ �Լ�
		/// </summary>
		bool Update(float deltaTime);

		/// <summary>
		/// ĳ���� ��Ʈ�ѷ� ���� �Լ�
		/// </summary>
		/// <param name="controllerInfo"> ĳ���� ��Ʈ�ѷ� ���� ������ </param>
		/// <param name="movementInfo"> ĳ���� �����Ʈ ���� ������ </param>
		/// <returns></returns>
		bool CreateCCT(const CharacterControllerInfo& controllerInfo, const CharacterMovementInfo& movementInfo, int* collisionMatrix);

		/// <summary>
		/// Ư�� ĳ���� ��Ʈ�ѷ��� �Է� �� �߰� �Լ�
		/// </summary>
		/// <param name="id"> ĳ���� ��Ʈ�ѷ� ���̵� </param>
		/// <param name="input"> �Է��� �̵� ���� (ex. {1.f, 0.f, 0.f}) </param>
		void AddInputMove(const unsigned int& id, const DirectX::SimpleMath::Vector3& input);

		/// <summary>
		/// ĳ���� ��Ʈ�ѷ� ���� �Լ�
		/// </summary>
		/// <param name="id">  </param>
		/// <returns></returns>
		bool RemoveController(const unsigned int& id);

		/// <summary>
		/// �浹 ��Ʈ������ ������ �ȴٸ� ��� ������ �ٵ���� �浹 ��Ʈ������ ������Ʈ�մϴ�.
		/// </summary>
		/// <param name="collisionMatrix"></param>
		void UpdateCollisionMatrix(int* collisionMatrix);


	private:
		physx::PxPhysics*				mPhysics;
		physx::PxMaterial*				mMaterial;
		physx::PxControllerManager*		mCCTManager;

		std::unordered_map<unsigned int, std::shared_ptr<CharacterController>>	mCCTmap;
		std::unordered_map<unsigned int, std::shared_ptr<CollisionData>>		mCollisionDataContainer;
	};
}