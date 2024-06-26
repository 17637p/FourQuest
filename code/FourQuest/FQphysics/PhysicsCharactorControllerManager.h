#pragma once

#include <memory>
#include <map>
#include <set>
#include <PxPhysicsAPI.h>

#include "FQCommonPhysics.h"

namespace fq::physics
{
	class CharacterController;
	class PhysicsCollisionDataManager;

	class PhysicsCharactorControllerManager
	{
	public:
		PhysicsCharactorControllerManager();
		~PhysicsCharactorControllerManager();

		/// <summary>
		/// ĳ���� ��Ʈ�ѷ� �Ŵ��� ���� �� �ʱ�ȭ �Լ�
		/// </summary>
		bool initialize(physx::PxScene* scene, physx::PxPhysics* physics, std::shared_ptr<PhysicsCollisionDataManager> collisionDataManager);

		/// <summary>
		/// ĳ���� ��Ʈ�ѷ� �Ŵ����� �����ϰ� �ִ� ĳ���� ��Ʈ�ѷ� ������Ʈ �Լ�
		/// </summary>
		bool Update(float deltaTime);

		/// <summary>
		/// ĳ���� ��Ʈ�ѷ� ���� ������ Ŭ����� ������Ʈ
		/// </summary>
		bool FinalUpdate();

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
		bool AddInputMove(const unsigned int& id, const DirectX::SimpleMath::Vector3& input);

		/// <summary>
		/// ĳ���� ��Ʈ�ѷ� ���� �Լ�
		/// </summary>
		/// <param name="id">  </param>
		/// <returns></returns>
		bool RemoveController(const unsigned int& id);

		/// <summary>
		/// ĳ���� ��Ʈ�ѷ� ���� �Լ�
		/// </summary>
		bool RemoveAllController();

		/// <summary>
		/// �浹 ��Ʈ������ ������ �ȴٸ� ��� ������ �ٵ���� �浹 ��Ʈ������ ������Ʈ�մϴ�.
		/// </summary>
		/// <param name="collisionMatrix"></param>
		void UpdateCollisionMatrix(int* collisionMatrix);

		/// <summary>
		/// ĳ���� ��Ʈ�ѷ��� ĳ���� �����Ʈ�� �����͸� Get Set�մϴ�.
		/// </summary>
		void GetCharacterControllerData(const unsigned int& id, CharacterControllerGetSetData& data);
		void GetCharacterMovementData(const unsigned int& id, CharacterMovementGetSetData& data);
		void SetCharacterControllerData(const unsigned int& id, const CharacterControllerGetSetData& controllerData, int* collisionMatrix);
		void SetCharacterMovementData(const unsigned int& id, const CharacterMovementGetSetData& movementData);

	private:
		std::weak_ptr<PhysicsCollisionDataManager> mCollisionDataManager;
		std::unordered_map<unsigned int, std::shared_ptr<CharacterController>>	mCCTmap;

		physx::PxPhysics*				mPhysics;
		physx::PxMaterial*				mMaterial;
		physx::PxControllerManager*		mCCTManager;
	};
}