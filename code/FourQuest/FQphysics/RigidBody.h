#pragma once

#include "FQCommonPhysics.h"
#include <physx\PxPhysicsAPI.h>

#include "FQCommonPhysics.h"

namespace fq::physics
{
	class RigidBody
	{
	public:
		RigidBody(fq::physics::EColliderType colliderType, unsigned int id, unsigned int layerNumber);
		virtual ~RigidBody();

		inline const unsigned int GetID() const;
		inline const unsigned int GetLayerNumber() const;
		inline const EColliderType& GetColliderType() const;

	private:
		unsigned int mID;
		unsigned int mLayerNumber;
		fq::physics::EColliderType	mColliderType;
	};

	const unsigned int RigidBody::GetID() const
	{
		return mID;
	}
	const unsigned int RigidBody::GetLayerNumber() const
	{
		return mLayerNumber;
	}
	const EColliderType& RigidBody::GetColliderType() const
	{
		return mColliderType;
	}
}