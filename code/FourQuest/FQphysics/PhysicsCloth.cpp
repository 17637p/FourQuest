#include "PhysicsCloth.h"


namespace fq::physics
{
	PhysicsCloth::PhysicsCloth(unsigned int id, unsigned int layerNumber)
		: mID()
		, mLayNumber()
		, mParticleNumberX()
		, mParticleNumberZ()
		, mPosition()
		, mParticleSpacing()
		, mTotalClothMass()
		, mParticleSystem(nullptr)
		, mClothBuffer(nullptr)
	{
	}

	PhysicsCloth::~PhysicsCloth()
	{
	}

	bool PhysicsCloth::Initialize(const PhysicsClothInfo& info, const physx::PxCudaContextManager* cudaContextManager, physx::PxPhysics* physics)
	{
		mID = info.id;
		mLayNumber = info.layerNumber;

		mParticleNumberX = info.particleNumberX;
		mParticleNumberZ = info.particleNumberZ;
		mPosition.x = info.position.x;
		mPosition.y = info.position.y;
		mPosition.z = -info.position.z;
		mParticleSpacing = info.particleSpacing;
		mTotalClothMass = info.totalClothMass;

		if (cudaContextManager == nullptr)
			return false;

		// ���� �� ������, �ﰢ���� ���� ���
		const physx::PxU32 numParticles = info.particleNumberX * info.particleNumberZ;	// ���� ����
		const physx::PxU32 numSprings = (mParticleNumberX - 1) * (mParticleNumberZ - 1) * 4 + (mParticleNumberX - 1) + (mParticleNumberZ - 1);	// ���� �ϳ��� �̿��ϴ� ���ڵ鿡 ������ ���� �����µ�, �� ������ ����
		const physx::PxU32 numTriangles = (mParticleNumberX - 1) * (mParticleNumberZ - 1) * 2;	// �ﰢ�� ����

		const physx::PxReal restOffset = mParticleSpacing;

		const physx::PxReal stretchStiffness = 100.f;
		const physx::PxReal shearStiffness = 100.f;
		const physx::PxReal springDamping = 0.1f;

		// ���(Material) ����
		physx::PxPBDMaterial* defaultMat = physics->createPBDMaterial(0.8f, 0.001f, 1e+7f, 0.001f, 0.5f, 0.005f, 0.05f, 0.f, 0.f, 1.f, 2.f);

		return true;
	}

	physx::PxU32 PhysicsCloth::id(const physx::PxU32& x, const physx::PxU32& y, const physx::PxU32& numY)
	{
		return x * numY + y;
	}
}