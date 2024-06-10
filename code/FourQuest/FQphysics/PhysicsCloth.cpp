#include "PhysicsCloth.h"

#include <extensions/PxParticleExt.h>
#include <cudamanager\PxCudaContext.h>

namespace fq::physics
{
	PhysicsCloth::PhysicsCloth(unsigned int id, unsigned int layerNumber)
		: mID()
		, mLayNumber()
		, mParticleNumberX()
		, mParticleNumberZ()
		, mWorldTransform{}
		, mParticleSpacing()
		, mTotalClothMass()
		, mParticleSystem(nullptr)
		, mClothBuffer(nullptr)
	{
	}

	PhysicsCloth::~PhysicsCloth()
	{
	}

	bool PhysicsCloth::Initialize(const PhysicsClothInfo& info, physx::PxPhysics* physics, physx::PxScene* scene, physx::PxCudaContextManager* cudaContextManager)
	{
		mID = info.id;
		mLayNumber = info.layerNumber;

		mParticleNumberX = info.particleNumberX;
		mParticleNumberZ = info.particleNumberZ;
		mWorldTransform = info.worldTransform;
		mParticleSpacing = info.particleSpacing;
		mTotalClothMass = info.totalClothMass;

		if (cudaContextManager == nullptr)
			return false;

		// ���� �� ������, �ﰢ���� ���� ���
		const physx::PxU32 numParticles = info.particleNumberX * info.particleNumberZ;	// ���� ����
		const physx::PxU32 numSprings = (mParticleNumberX - 1) * (mParticleNumberZ - 1) * 4 + (mParticleNumberX - 1) + (mParticleNumberZ - 1);	// ���� �ϳ��� �̿��ϴ� ���ڵ鿡 ������ ���� �����µ�, �� ������ ����
		const physx::PxU32 numTriangles = (mParticleNumberX - 1) * (mParticleNumberZ - 1) * 2;	// �ﰢ�� ����

		const physx::PxReal restOffset = mParticleSpacing;

		// ���(Material) ����
		physx::PxPBDMaterial* defaultMat = physics->createPBDMaterial(
			info.materialInfo.friction, 
			info.materialInfo.damping, 
			info.materialInfo.adhesion, 
			info.materialInfo.viscosity, 
			info.materialInfo.vorticityConfinement, 
			info.materialInfo.surfaceTension, 
			info.materialInfo.cohesion, 
			info.materialInfo.lift, 
			info.materialInfo.drag, 
			info.materialInfo.cflCoefficient, 
			info.materialInfo.gravityScale);

		// ���� �ý��� ����
		mParticleSystem = physics->createPBDParticleSystem(*cudaContextManager);

		// ���� �ý����� ����
		const physx::PxReal particleMass = mTotalClothMass / numParticles;
		mParticleSystem->setRestOffset(restOffset);
		mParticleSystem->setContactOffset(restOffset + 0.02f);
		mParticleSystem->setParticleContactOffset(restOffset + 0.02f);
		mParticleSystem->setSolidRestOffset(restOffset);
		mParticleSystem->setFluidRestOffset(0.0f);

		// ���� ���� �ý��� �߰�
		scene->addActor(*mParticleSystem);

		// ���� ���¸� �����ϴ� ���� ����
		const physx::PxU32 particlePhase = mParticleSystem->createPhase(defaultMat, 
			physx::PxParticlePhaseFlags(physx::PxParticlePhaseFlag::eParticlePhaseSelfCollideFilter | physx::PxParticlePhaseFlag::eParticlePhaseSelfCollide));

		physx::ExtGpu::PxParticleClothBufferHelper* clothBuffers = physx::ExtGpu::PxCreateParticleClothBufferHelper(1, numTriangles, numSprings, numParticles, cudaContextManager);

		// ����, ������, �ﰢ���� ���¸� �����ϱ� ���� ���� �Ҵ�
		physx::PxU32* phase = cudaContextManager->allocDeviceBuffer<physx::PxU32>(numParticles);
		physx::PxVec4* positionInvMass = cudaContextManager->allocPinnedHostBuffer<physx::PxVec4>(numParticles);
		physx::PxVec4* velocity = cudaContextManager->allocPinnedHostBuffer<physx::PxVec4>(numParticles);

		// ������ �� �ﰢ�� ����
		physx::PxArray<physx::PxParticleSpring> springs;
		springs.reserve(numSprings);
		physx::PxArray<physx::PxU32> triangles;
		triangles.reserve(numTriangles * 3);

		// õ�� ���ؽ� ���ۿ� ���Ǹ� ����
		if (!createClothBuffer(phase, positionInvMass, velocity, particlePhase, particleMass, springs, triangles))
			return false;

		// õ���� ���ۿ� ������ �߰�
		clothBuffers->addCloth(0.f, 0.f, 0.f, triangles.begin(), numTriangles, springs.begin(), numSprings, positionInvMass, numParticles);

		// ������ ���¸� ��Ÿ���� ���� ����
		physx::ExtGpu::PxParticleBufferDesc bufferDesc;
		bufferDesc.maxParticles = numParticles;
		bufferDesc.numActiveParticles = numParticles;
		bufferDesc.positions = velocity;
		bufferDesc.phases = phase;

		// õ���� ���� ��������
		const physx::PxParticleClothDesc& clothDesc = clothBuffers->getParticleClothDesc();

		// ���� õ���� ��ó���� ����
		physx::PxParticleClothPreProcessor* clothPreProcessor = PxCreateParticleClothPreProcessor(cudaContextManager);

		// ���� õ�� ���� �� ó��
		physx::PxPartitionedParticleCloth output;
		clothPreProcessor->partitionSprings(clothDesc, output);
		clothPreProcessor->release();

		// õ�� ���� ����
		mClothBuffer = physx::ExtGpu::PxCreateAndPopulateParticleClothBuffer(bufferDesc, clothDesc, output, cudaContextManager);
		mParticleSystem->addParticleBuffer(mClothBuffer);

		// ���� ����
		clothBuffers->release();

		// �Ҵ�� �޸� ����
		cudaContextManager->freePinnedHostBuffer(positionInvMass);
		cudaContextManager->freePinnedHostBuffer(velocity);
		cudaContextManager->freePinnedHostBuffer(phase);

		return true;
	}

	physx::PxU32 PhysicsCloth::id(const physx::PxU32& x, const physx::PxU32& y, const physx::PxU32& numY)
	{
		return x * numY + y;
	}

	bool PhysicsCloth::createClothBuffer(
		physx::PxU32* phase, 
		physx::PxVec4* positionInvMass, 
		physx::PxVec4* velocity, 
		const physx::PxU32& particlePhase, 
		const physx::PxReal& particleMass, 
		physx::PxArray<physx::PxParticleSpring>& springs, 
		physx::PxArray<physx::PxU32>& triangles)
	{
		const physx::PxReal stretchStiffness = 100.f;
		const physx::PxReal shearStiffness = 100.f;
		const physx::PxReal springDamping = 0.1f;

		for (physx::PxU32 i = 0; i < mParticleNumberX; i++)
		{
			for (physx::PxU32 j = 0; j < mParticleNumberZ; j++)
			{
				const physx::PxU32 index = 1 * mParticleNumberZ + j;

				// ������ ��ġ�� ���� ����
				physx::PxVec4 pos(0.f, 0.f, 0.f, 1.f / particleMass);
				phase[index] = particlePhase;
				positionInvMass[index] = pos;
				velocity[index] = physx::PxVec4(0.f);

				// ������ �߰�
				if (i > 0)
				{
					physx::PxParticleSpring spring = { id(i - 1, j, mParticleNumberZ), id(i, j, mParticleNumberZ), mParticleSpacing, stretchStiffness, springDamping, 0 };
					springs.pushBack(spring);
				}
				if (j > 0)
				{
					physx::PxParticleSpring spring = { id(i, j - 1, mParticleNumberZ), id(i, j, mParticleNumberZ), mParticleSpacing, stretchStiffness, springDamping, 0 };
					springs.pushBack(spring);
				}

				// �ﰢ�� �߰�
				if (i > 0 && j > 0)
				{
					physx::PxParticleSpring spring0 = { id(i - 1, j - 1,  mParticleNumberZ), id(i, j, mParticleNumberZ), physx::PxSqrt(2.f) * mParticleSpacing, shearStiffness, springDamping, 0 };
					springs.pushBack(spring0);
					physx::PxParticleSpring spring1 = { id(i - 1, j, mParticleNumberZ), id(i, j - 1, mParticleNumberZ), physx::PxSqrt(2.f) * mParticleSpacing, shearStiffness, springDamping, 0 };
					springs.pushBack(spring1);

					// �ﰢ���� õ���� �Ʒ��� ������ �� �ٻ����� ���� ���׷� ��꿡 ���˴ϴ�.
					triangles.pushBack(id(i - 1, j - 1, mParticleNumberZ));
					triangles.pushBack(id(i - 1, j, mParticleNumberZ));
					triangles.pushBack(id(i, j - 1, mParticleNumberZ));

					triangles.pushBack(id(i - 1, j, mParticleNumberZ));
					triangles.pushBack(id(i, j - 1, mParticleNumberZ));
					triangles.pushBack(id(i, j, mParticleNumberZ));
				}

				mParticleNumberZ += mParticleSpacing;
			}

			mParticleNumberZ = 0.f;
			mParticleNumberZ += mParticleSpacing;
		}

		// ������ ������ �� �ﰢ�� ���� ������ �����Ǿ����� Ȯ��
		PX_ASSERT(numSprings == springs.size());
		PX_ASSERT(numSprings == triangles.size() / 3);

		return true;
	}

	void PhysicsCloth::GetPhysicsCloth(physx::PxCudaContext* cudaContext, PhysicsClothGetData& data)
	{
		data.worldTransform = mWorldTransform;
		unsigned int paticleSize = mClothBuffer->getNbActiveParticles();
		physx::PxVec4* particle = mClothBuffer->getPositionInvMasses();

		std::vector<physx::PxVec4> pxVertex;
		pxVertex.resize(paticleSize);

		cudaContext->memcpyDtoH(pxVertex.data(), CUdeviceptr(particle), sizeof(physx::PxVec4) * paticleSize);
		
		for (int i = 0; i < paticleSize; i++)
		{
			mVertices[i].x = pxVertex[i].x;
			mVertices[i].y = pxVertex[i].y;
			mVertices[i].z = -pxVertex[i].z;
		}
	}

	void PhysicsCloth::SetPhysicsCloth(physx::PxCudaContext* cudaContext, const PhysicsClothSetData& data)
	{

	}
}