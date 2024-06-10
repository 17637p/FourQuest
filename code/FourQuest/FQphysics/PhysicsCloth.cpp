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

		// 입자 및 스프링, 삼각형의 개수 계산
		const physx::PxU32 numParticles = info.particleNumberX * info.particleNumberZ;	// 입자 갯수
		const physx::PxU32 numSprings = (mParticleNumberX - 1) * (mParticleNumberZ - 1) * 4 + (mParticleNumberX - 1) + (mParticleNumberZ - 1);	// 입자 하나당 이웃하는 입자들에 스프링 값을 가지는데, 그 스프링 갯수
		const physx::PxU32 numTriangles = (mParticleNumberX - 1) * (mParticleNumberZ - 1) * 2;	// 삼각형 갯수

		const physx::PxReal restOffset = mParticleSpacing;

		// 재료(Material) 설정
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

		// 입자 시스템 생성
		mParticleSystem = physics->createPBDParticleSystem(*cudaContextManager);

		// 입자 시스템의 설정
		const physx::PxReal particleMass = mTotalClothMass / numParticles;
		mParticleSystem->setRestOffset(restOffset);
		mParticleSystem->setContactOffset(restOffset + 0.02f);
		mParticleSystem->setParticleContactOffset(restOffset + 0.02f);
		mParticleSystem->setSolidRestOffset(restOffset);
		mParticleSystem->setFluidRestOffset(0.0f);

		// 씬에 입자 시스템 추가
		scene->addActor(*mParticleSystem);

		// 입자 상태를 저장하는 버퍼 생성
		const physx::PxU32 particlePhase = mParticleSystem->createPhase(defaultMat, 
			physx::PxParticlePhaseFlags(physx::PxParticlePhaseFlag::eParticlePhaseSelfCollideFilter | physx::PxParticlePhaseFlag::eParticlePhaseSelfCollide));

		physx::ExtGpu::PxParticleClothBufferHelper* clothBuffers = physx::ExtGpu::PxCreateParticleClothBufferHelper(1, numTriangles, numSprings, numParticles, cudaContextManager);

		// 입자, 스프링, 삼각형의 상태를 저장하기 위한 버퍼 할당
		physx::PxU32* phase = cudaContextManager->allocDeviceBuffer<physx::PxU32>(numParticles);
		physx::PxVec4* positionInvMass = cudaContextManager->allocPinnedHostBuffer<physx::PxVec4>(numParticles);
		physx::PxVec4* velocity = cudaContextManager->allocPinnedHostBuffer<physx::PxVec4>(numParticles);

		// 스프링 및 삼각형 정의
		physx::PxArray<physx::PxParticleSpring> springs;
		springs.reserve(numSprings);
		physx::PxArray<physx::PxU32> triangles;
		triangles.reserve(numTriangles * 3);

		// 천의 버텍스 버퍼에 스피링 적용
		if (!createClothBuffer(phase, positionInvMass, velocity, particlePhase, particleMass, springs, triangles))
			return false;

		// 천막의 버퍼에 데이터 추가
		clothBuffers->addCloth(0.f, 0.f, 0.f, triangles.begin(), numTriangles, springs.begin(), numSprings, positionInvMass, numParticles);

		// 입자의 상태를 나타내는 버퍼 설명
		physx::ExtGpu::PxParticleBufferDesc bufferDesc;
		bufferDesc.maxParticles = numParticles;
		bufferDesc.numActiveParticles = numParticles;
		bufferDesc.positions = velocity;
		bufferDesc.phases = phase;

		// 천막의 설명 가져오기
		const physx::PxParticleClothDesc& clothDesc = clothBuffers->getParticleClothDesc();

		// 입자 천막의 전처리기 생성
		physx::PxParticleClothPreProcessor* clothPreProcessor = PxCreateParticleClothPreProcessor(cudaContextManager);

		// 입자 천막 분할 및 처리
		physx::PxPartitionedParticleCloth output;
		clothPreProcessor->partitionSprings(clothDesc, output);
		clothPreProcessor->release();

		// 천막 버퍼 생성
		mClothBuffer = physx::ExtGpu::PxCreateAndPopulateParticleClothBuffer(bufferDesc, clothDesc, output, cudaContextManager);
		mParticleSystem->addParticleBuffer(mClothBuffer);

		// 버퍼 해제
		clothBuffers->release();

		// 할당된 메모리 해제
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

				// 입자의 위치와 상태 설정
				physx::PxVec4 pos(0.f, 0.f, 0.f, 1.f / particleMass);
				phase[index] = particlePhase;
				positionInvMass[index] = pos;
				velocity[index] = physx::PxVec4(0.f);

				// 스프링 추가
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

				// 삼각형 추가
				if (i > 0 && j > 0)
				{
					physx::PxParticleSpring spring0 = { id(i - 1, j - 1,  mParticleNumberZ), id(i, j, mParticleNumberZ), physx::PxSqrt(2.f) * mParticleSpacing, shearStiffness, springDamping, 0 };
					springs.pushBack(spring0);
					physx::PxParticleSpring spring1 = { id(i - 1, j, mParticleNumberZ), id(i, j - 1, mParticleNumberZ), physx::PxSqrt(2.f) * mParticleSpacing, shearStiffness, springDamping, 0 };
					springs.pushBack(spring1);

					// 삼각형은 천막이 아래로 떨어질 때 근사적인 공기 저항력 계산에 사용됩니다.
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

		// 생성된 스프링 및 삼각형 수가 예상대로 생성되었는지 확인
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