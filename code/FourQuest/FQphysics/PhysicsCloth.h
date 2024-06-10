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

		bool Initialize(const PhysicsClothInfo& info, physx::PxPhysics* physics, physx::PxScene* scene, physx::PxCudaContextManager* cudaContextManager);

		void GetPhysicsCloth(physx::PxCudaContext* cudaContext, PhysicsClothGetData& data);
		void SetPhysicsCloth(physx::PxCudaContext* cudaContext, const PhysicsClothSetData& data);

		inline void SetParticleNumberX(const unsigned int& particleNumberX);
		inline void SetParticleNumberZ(const unsigned int& particleNumberZ);
		inline void SetWorldTransform(const DirectX::SimpleMath::Matrix& position);
		inline void SetParticleSpacing(const float& particleSpacing);
		inline void SetTotalClothMass(const float& toTalClothMass);
		inline const unsigned int& GetParticleNumberX() const;
		inline const unsigned int& GetParticleNumberZ() const;
		inline const DirectX::SimpleMath::Matrix& GetWorldTransform() const;
		inline const float& GetParticleSpacing() const;
		inline const float& GetTotalClothMass() const;

		inline const physx::PxPBDParticleSystem* GetParticleSystem() const;
		inline const physx::PxParticleClothBuffer* GetClothBuffer() const;

	private:
		physx::PxU32 id(const physx::PxU32& x, const physx::PxU32& y, const physx::PxU32& numY);

		bool createClothBuffer(
			physx::PxU32* phase,
			physx::PxVec4* positionInvMass,
			physx::PxVec4* velocity,
			const physx::PxU32& particlePhase,
			const physx::PxReal& particleMass,
			physx::PxArray<physx::PxParticleSpring>& springs,
			physx::PxArray<physx::PxU32>& triangles);

	private:
		unsigned int	mID;
		unsigned int	mLayNumber;

		unsigned int	mParticleNumberX;
		unsigned int	mParticleNumberZ;
		float			mParticleSpacing;
		float			mTotalClothMass;

		DirectX::SimpleMath::Matrix mWorldTransform;
		std::vector<DirectX::SimpleMath::Vector4> mVertices;

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
	void PhysicsCloth::SetWorldTransform(const DirectX::SimpleMath::Matrix& position)
	{
		mWorldTransform = position;
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
	const DirectX::SimpleMath::Matrix& PhysicsCloth::GetWorldTransform() const
	{
		return mWorldTransform;
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