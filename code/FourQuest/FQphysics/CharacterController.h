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

		void Initialize(const CharacterControllerInfo& info, physx::PxControllerManager* CCTManager);
		void Update(float deltaTime);

		void AddMovementInput(DirectX::SimpleMath::Vector3 input);

		inline physx::PxController* GetPxController();
		inline const unsigned int& GetID();
		inline const unsigned int& GetLayerNumber();
		inline const std::shared_ptr<CharacterMovement> GetCharacterMovement();

	private:
		unsigned int mId;
		unsigned int mLayerNumber;

		DirectX::SimpleMath::Vector3 mInput;

		std::shared_ptr<CharacterMovement> mCharacterMovement;
		physx::PxController* mPxController;
	};


	physx::PxController* CharacterController::GetPxController()
	{
		return mPxController;
	}
	const unsigned int& CharacterController::GetID()
	{
		return mId;
	}
	const unsigned int& CharacterController::GetLayerNumber()
	{
		return mLayerNumber;
	}
	const std::shared_ptr<CharacterMovement> CharacterController::GetCharacterMovement()
	{
		return mCharacterMovement;
	}
}