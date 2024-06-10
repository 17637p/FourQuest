#pragma once

#include "FQCommonPhysics.h"

#include <PxPhysicsAPI.h>

namespace fq::physics
{
	class PhysicsCloth
	{
	public:
		PhysicsCloth(unsigned int id, unsigned int layerNumber);
		~PhysicsCloth();

		bool Initialize(const PhysicsClothInfo& info, const physx::PxCudaContextManager* cudaContextManager, physx::PxPhysics* physics);

		inline void SetParticleNumberX(const unsigned int& particleNumberX);
		inline void SetParticleNumberZ(const unsigned int& particleNumberZ);
		inline void SetPosition(const physx::PxVec3& position);
		inline void SetParticleSpacing(const float& particleSpacing);
		inline void SetTotalClothMass(const float& toTalClothMass);
		inline const unsigned int& GetParticleNumberX() const;
		inline const unsigned int& GetParticleNumberZ() const;
		inline const physx::PxVec3& GetPosition() const;
		inline const float& GetParticleSpacing() const;
		inline const float& GetTotalClothMass() const;

		inline const physx::PxPBDParticleSystem* GetParticleSystem() const;
		inline const physx::PxParticleClothBuffer* GetClothBuffer() const;

	private:
		physx::PxU32 id(const physx::PxU32& x, const physx::PxU32& y, const physx::PxU32& numY);

	private:
		unsigned int	mID;
		unsigned int	mLayNumber;

		unsigned int	mParticleNumberX;
		unsigned int	mParticleNumberZ;
		physx::PxVec3	mPosition;
		float			mParticleSpacing;
		float			mTotalClothMass;

		physx::PxPBDParticleSystem* mParticleSystem;
		physx::PxParticleClothBuffer* mClothBuffer;
	};

#pragma region GetSet
	void PhysicsCloth::SetParticleNumberX(const unsigned int& particleNumberX)
	{
		mParticleNumberX = particleNumberX;
	}
	void PhysicsCloth::SetParticleNumberZ(const unsigned int& particleNumberZ)
	{
		mParticleNumberZ = particleNumberZ;
	}
	void PhysicsCloth::SetPosition(const physx::PxVec3& position)
	{
		mPosition = position;
	}
	void PhysicsCloth::SetParticleSpacing(const float& particleSpacing)
	{
		mParticleSpacing = particleSpacing;
	}
	void PhysicsCloth::SetTotalClothMass(const float& toTalClothMass)
	{
		mTotalClothMass = toTalClothMass;
	}
	const unsigned int& PhysicsCloth::GetParticleNumberX() const
	{
		return mParticleNumberX;
	}
	const unsigned int& PhysicsCloth::GetParticleNumberZ() const
	{
		return mParticleNumberZ;
	}
	const physx::PxVec3& PhysicsCloth::GetPosition() const
	{
		return mPosition;
	}
	const float& PhysicsCloth::GetParticleSpacing() const
	{
		return mParticleSpacing;
	}
	const float& PhysicsCloth::GetTotalClothMass() const
	{
		return mTotalClothMass;
	}
	const physx::PxPBDParticleSystem* PhysicsCloth::GetParticleSystem() const
	{
		return mParticleSystem;
	}
	const physx::PxParticleClothBuffer* PhysicsCloth::GetClothBuffer() const
	{
		return mClothBuffer;
	}
#pragma endregion

}