#pragma once

#include <memory>
#include <physx\PxPhysicsAPI.h>
#include <directxtk/SimpleMath.h>

#include "FQPhysics.h"

namespace fq::physics
{
	class CharacterMovement;

	class CharacterController
	{
	public:
		CharacterController();
		~CharacterController();

		/// <summary>
		/// ĳ���� ��Ʈ�ѷ� �ʱ�ȭ �Լ�
		/// </summary>
		/// <param name="info"> ĳ���� ��Ʈ�ѷ� �ʱ�ȭ ������ </param>
		/// <param name="movementInfo"> ĳ���� �����Ʈ �ʱ�ȭ ������ </param>
		/// <param name="CCTManager"> PxController�� ������ PxControllerManager </param>
		/// <param name="material"> PxController�� �����ϱ� ���� material </param>
		/// <param name="collisionMatrix"> �浹 ��Ʈ���� </param>
		/// <returns></returns>
		virtual bool Initialize( const CharacterControllerInfo& info
			, const CharacterMovementInfo& movementInfo
			, physx::PxControllerManager* CCTManager
			, physx::PxMaterial* material
			, std::shared_ptr<CollisionData> collisionData
			, int* collisionMatrix);
		
		/// <summary>
		/// �Է� ���� �Է� ���� ������ �̵��ϰų� �����ϴ� �Լ�
		/// </summary>
		/// <param name="deltaTime"> ��� �ð� </param>
		/// <returns></returns>
		virtual bool Update(float deltaTime);

		/// <summary>
		/// ���� Ű�� �Է� ������ mInputMove�� �����ϴ� �Լ�
		/// </summary>
		/// <param name="input"> �̵��ϰų� ������ �ϱ� ���� �Է� �޴� Ű �� (x,y,z) </param>
		void AddMovementInput(const DirectX::SimpleMath::Vector3& input);


		inline physx::PxController* GetPxController();
		inline const unsigned int& GetID();
		inline const unsigned int& GetLayerNumber();
		inline const std::shared_ptr<CharacterMovement> GetCharacterMovement();

	protected:
		unsigned int mID;
		unsigned int mLayerNumber;

		DirectX::SimpleMath::Vector3 mInputMove;

		std::shared_ptr<CharacterMovement> mCharacterMovement;
		physx::PxMaterial* mMaterial;
		physx::PxController* mPxController;
	};

#pragma region GetSet

	physx::PxController* CharacterController::GetPxController()
	{
		return mPxController;
	}
	const unsigned int& CharacterController::GetID()
	{
		return mID;
	}
	const unsigned int& CharacterController::GetLayerNumber()
	{
		return mLayerNumber;
	}
	const std::shared_ptr<CharacterMovement> CharacterController::GetCharacterMovement()
	{
		return mCharacterMovement;
	}

#pragma endregion
}