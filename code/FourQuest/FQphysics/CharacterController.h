#pragma once

#include <memory>
#include <physx\PxPhysicsAPI.h>
#include <directxtk/SimpleMath.h>

namespace fq::physics
{
	class CharactorMovement;

	class CharacterController
	{
	public:
		CharacterController();
		~CharacterController();


	private:
		DirectX::SimpleMath::Vector3 m_InputMove;

		std::shared_ptr<CharactorMovement> m_CharactorMovement = nullptr;

	};

}