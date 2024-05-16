#pragma once

#include <memory>
#include <string>
#include <physx\PxPhysicsAPI.h>
#include <directxtk\SimpleMath.h>

namespace fq::physics
{
	class CharacterLink;

	class CharacterPhysics
	{
	public:
		CharacterPhysics();
		~CharacterPhysics();

		void Initialize();

	private:
		std::string mName;
		unsigned int mID;
		unsigned int mLayerNumber;

		std::shared_ptr<CharacterLink> mRootLink;
		DirectX::SimpleMath::Matrix mWorldTransform;

	private:
		physx::PxArticulationReducedCoordinate* mArticulation;
	};
}