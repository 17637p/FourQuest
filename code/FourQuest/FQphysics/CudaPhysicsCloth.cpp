#include "CudaPhysicsCloth.h"

#include "EngineDataConverter.h"
#include "CudaClothTool.h"

#include <cudamanager\PxCudaContextManager.h>
#include <cudamanager\PxCudaContext.h>
#include <PxPhysicsAPI.h>

#include <iostream>
#include <math.h>

#pragma comment (lib, "cudart.lib")

void MulMatrixVector3(const DirectX::SimpleMath::Matrix& mat, DirectX::SimpleMath::Vector3& vec)
{
	DirectX::SimpleMath::Vector3 result;
	result.x = mat._11 * vec.x + mat._21 * vec.y + mat._31 * vec.z + mat._41;
	result.y = mat._12 * vec.x + mat._22 * vec.y + mat._32 * vec.z + mat._42;
	result.z = mat._13 * vec.x + mat._23 * vec.y + mat._33 * vec.z + mat._43;
	vec = result;
}

namespace fq::physics
{
	CudaPhysicsCloth::CudaPhysicsCloth(unsigned int id, unsigned int layerNumber)
		: mID(id)
		, mLayerNumber(layerNumber)
		, mWorldTransform{}
		, mVertices()
		, mUV()
		, mIndices()
		, mSprings()
		, mSameVertices()
		, mTotalClothMass(10.f)
		, mPBDMaterial(nullptr)
		, mParticleSystem(nullptr)
		, mClothBuffer(nullptr)
		, mClothBufferHelper(nullptr)
		, mCudaVertexResource(nullptr)
		, mCudaIndexResource(nullptr)
	{
	}

	CudaPhysicsCloth::~CudaPhysicsCloth()
	{
		PX_RELEASE(mPBDMaterial);
		PX_RELEASE(mClothBuffer);
		PX_RELEASE(mClothBufferHelper);

		CollisionData* data = (CollisionData*)mParticleSystem->userData;
		data->isDead = true;
	}

	bool CudaPhysicsCloth::Initialize(
		const PhysicsClothInfo& info, 
		physx::PxPhysics* physics, 
		physx::PxScene* scene,
		physx::PxCudaContextManager* cudaContextManager, 
		std::shared_ptr<CollisionData> collisionData,
		int* collisionMatrix)
	{
		int deviceCount;
		cudaError_t cudaStatus = cudaGetDeviceCount(&deviceCount);
		if (cudaStatus != cudaSuccess || deviceCount == 0) {
			std::cerr << "CUDA ����̽� �ʱ�ȭ ����" << std::endl;
			return false;
		}

		cudaStatus = cudaSetDevice(0); // ù ��° CUDA ����̽� ���
		if (cudaStatus != cudaSuccess) {
			std::cerr << "CUDA ����̽� ���� ����" << std::endl;
			return false;
		}

		if (cudaContextManager == nullptr)
			return false;

		settingInfoData(info);
		extractSpringsData();

		// ���(Material) ����
		mPBDMaterial = physics->createPBDMaterial(
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

		createClothParticle(physics, scene, cudaContextManager, collisionMatrix, collisionData);

		return true;
	}

	bool CudaPhysicsCloth::UpdatePhysicsCloth(physx::PxCudaContextManager* cudaContextManager)
	{
		physx::PxVec4* paticle = mClothBuffer->getPositionInvMasses();

		if (!CudaClothTool::UpdatePhysXDataToID3DBuffer(mVertices, mIndices, mUV, mCudaVertexResource, paticle)) return false;
		if (!CudaClothTool::UpdateNormalToID3DBuffer(mSameVertices, mVertices.size(), mCudaVertexResource)) return false;

		return true;
	}

	void CudaPhysicsCloth::settingInfoData(const PhysicsClothInfo& info)
	{
		RegisterD3D11VertexBufferWithCUDA((ID3D11Buffer*)info.vertexBuffer);
		RegisterD3D11IndexBufferWithCUDA((ID3D11Buffer*)info.indexBuffer);

		mWorldTransform = info.worldTransform;
		mTotalClothMass = info.totalClothMass;

		if (!CudaClothTool::copyVertexFromGPUToCPU(mVertices, mUV, mWorldTransform, mCudaVertexResource)) return;
		if (!CudaClothTool::copyIndexFromGPUToCPU(mIndices, mCudaIndexResource)) return;
	}

	bool areVerticesEqual(const DirectX::SimpleMath::Vector3& vertex1, const DirectX::SimpleMath::Vector3& vertex2, float epsilon = 1e-6)
	{
		return (std::abs(vertex1.x - vertex2.x) < epsilon) &&
			(std::abs(vertex1.y - vertex2.y) < epsilon) &&
			(std::abs(vertex1.z - vertex2.z) < epsilon);
	}

	void CudaPhysicsCloth::extractSpringsData()
	{
		// �ﰢ�� ������ �ε����� ��ȸ
		for (size_t i = 0; i < mIndices.size(); i += 3)
		{
			unsigned int v1 = mIndices[i];
			unsigned int v2 = mIndices[i + 1];
			unsigned int v3 = mIndices[i + 2];

			// ���ĵ� ������ vertex ���� �߰��Ͽ� �ߺ��� ���� ����
			auto addEdge = [this](unsigned int a, unsigned int b)
				{
					if (a > b) std::swap(a, b);
					mSprings.insert({ a, b });
				};

			addEdge(v1, v2);
			addEdge(v2, v3);
			addEdge(v3, v1);
		}

		// ���� ��ġ�� ���ؽ��� ���� ����
		mSameVertices.reserve(mVertices.size() / 3);
		for (int i = 0; i < mVertices.size(); i++)
		{
			for (int j = i + 1; j < mVertices.size(); j++)
			{
				if (areVerticesEqual(mVertices[i], mVertices[j]))
				{
					mSprings.insert({ i, j });
					mSameVertices.push_back({ i, j });
				}
			}
		}
	}

	void CudaPhysicsCloth::createClothParticle(
		physx::PxPhysics* physics, 
		physx::PxScene* scene, 
		physx::PxCudaContextManager* cudaContextManager,
		int* collisionMatrix,
		std::shared_ptr<CollisionData> collisionData)
	{
		// ���� �� ������, �ﰢ���� ���� ���
		const physx::PxU32 numParticles = mVertices.size();	// ���� ����
		const physx::PxU32 numSprings = mSprings.size();	// ���� �ϳ��� �̿��ϴ� ���ڵ鿡 ������ ���� �����µ�, �� ������ ����
		const physx::PxU32 numTriangles = mIndices.size() / 3;	// �ﰢ�� ����

		// ���� �ý����� ����
		const physx::PxReal particleMass = mTotalClothMass / mVertices.size();
		const physx::PxReal restOffset = 2.f;

		// ���� �ý��� ����
		mParticleSystem = physics->createPBDParticleSystem(*cudaContextManager);

		mParticleSystem->setRestOffset(1.f);
		mParticleSystem->setContactOffset(restOffset + 0.02f);
		mParticleSystem->setParticleContactOffset(restOffset + 0.02f);
		mParticleSystem->setSolidRestOffset(restOffset);

		physx::PxFilterData filterdata;
		filterdata.word0 = mLayerNumber;
		filterdata.word1 = collisionMatrix[mLayerNumber];
		mParticleSystem->setSimulationFilterData(filterdata);

		collisionData->myId = mID;
		collisionData->myLayerNumber = mLayerNumber;
		mParticleSystem->userData = collisionData.get();

		// ���� ���� �ý��� �߰�
		scene->addActor(*mParticleSystem);

		// ������ ���¸� �����ϴ� ���� ����
		const physx::PxU32 particlePhase = mParticleSystem->createPhase(mPBDMaterial, physx::PxParticlePhaseFlags(
			physx::PxParticlePhaseFlag::eParticlePhaseSelfCollideFilter | physx::PxParticlePhaseFlag::eParticlePhaseSelfCollide));

		mClothBufferHelper = physx::ExtGpu::PxCreateParticleClothBufferHelper(1, numTriangles, numSprings, numParticles, cudaContextManager);
		// ����, ������ �ﰢ���� ���¸� �����ϱ� ���� ���� �Ҵ�
		unsigned int* phase = cudaContextManager->allocPinnedHostBuffer<physx::PxU32>(numParticles);
		physx::PxVec4* positionInvMass = cudaContextManager->allocPinnedHostBuffer<physx::PxVec4>(numParticles);
		physx::PxVec4* velocity = cudaContextManager->allocPinnedHostBuffer<physx::PxVec4>(numParticles);

		// cloth�� ������ ��ƼŬ�� ������ �����͸� ������� �ϴ� Cloth Particle Buffer ����
		settingParticleBuffer(numSprings, numTriangles, numParticles, particlePhase, particleMass, phase, positionInvMass, velocity);

		// cloth ����
		createCloth(numParticles, cudaContextManager, phase, positionInvMass, velocity);
	}


	float calculateVectorMagnitude(const DirectX::SimpleMath::Vector3& point1, const DirectX::SimpleMath::Vector3& point2) 
	{
		float dx = point2.x - point1.x;
		float dy = point2.y - point1.y;
		float dz = point2.z - point1.z;

		return std::sqrt(dx * dx + dy * dy + dz * dz);
	}


	void CudaPhysicsCloth::settingParticleBuffer(
		const physx::PxU32& numSprings,
		const physx::PxU32& numTriangles,
		const physx::PxU32& numParticles,
		const physx::PxU32& particlePhase,
		const physx::PxReal& particleMass,
		unsigned int* phase,
		physx::PxVec4* positionInvMass,
		physx::PxVec4* velocity)
	{
		const physx::PxReal stretchStiffness = 100.f;
		const physx::PxReal shearStiffness = 100.f;
		const physx::PxReal springDamping = 0.1f;

		// ��ƼŬ ������ �� Ʈ���̾ޱ� ����
		physx::PxArray<physx::PxParticleSpring> springs;
		springs.reserve(numSprings);
		physx::PxArray<physx::PxU32> triangles;
		triangles.reserve(numTriangles * 3);

		// ���� ���� ����
		for (int i = 0; i < numParticles; i++)
		{
			positionInvMass[i] = physx::PxVec4(mVertices[i].x, mVertices[i].y, mVertices[i].z, 1.f / particleMass);
			phase[i] = particlePhase;
			velocity[i] = physx::PxVec4(0.f);
		}

		// ������ �߰�
		for (auto line : mSprings)
		{
			physx::PxParticleSpring spring = { line.first, line.second, calculateVectorMagnitude(mVertices[line.first], mVertices[line.second]), stretchStiffness, springDamping, 0 };
			springs.pushBack(spring);
		}

		// �ﰢ�� �߰�
		for (int i = 0; i < mIndices.size(); i += 3)
		{
			triangles.pushBack(mIndices[i]);
			triangles.pushBack(mIndices[i + 1]);
			triangles.pushBack(mIndices[i + 2]);
		}

		// ������ ������ �� �ﰢ�� ���� ������ �����Ǿ����� Ȯ��
		PX_ASSERT(numSprings == springs.size());
		PX_ASSERT(numTriangles == triangles.size() / 3);

		// õ���� ���ۿ� ������ �߰�
		mClothBufferHelper->addCloth(0.f, 0.f, 0.f, triangles.begin(), numTriangles, springs.begin(), numSprings, positionInvMass, numParticles);
	}

	void CudaPhysicsCloth::createCloth(
		const physx::PxU32& numParticles,
		physx::PxCudaContextManager* cudaContextManager,
		unsigned int* phase,
		physx::PxVec4* positionInvMass,
		physx::PxVec4* velocity)
	{
		// ������ ���¸� ��Ÿ���� ���� ����
		physx::ExtGpu::PxParticleBufferDesc bufferDesc;
		bufferDesc.maxParticles = numParticles;
		bufferDesc.numActiveParticles = numParticles;
		bufferDesc.positions = positionInvMass;
		bufferDesc.velocities = velocity;
		bufferDesc.phases = phase;

		// õ���� ���� ��������
		const physx::PxParticleClothDesc& clothDesc = mClothBufferHelper->getParticleClothDesc();

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
		mClothBufferHelper->release();

		// �Ҵ�� �޸� ����
		cudaContextManager->freePinnedHostBuffer(positionInvMass);
		cudaContextManager->freePinnedHostBuffer(velocity);
		cudaContextManager->freePinnedHostBuffer(phase);
	}

	void CudaPhysicsCloth::GetPhysicsCloth(PhysicsClothGetData& data)
	{
		data.worldTransform = mWorldTransform;
	}

	physx::PxVec4 multiply(const physx::PxMat44& mat, const physx::PxVec4& vec)  // 4x4 ��İ� PxVec4�� ���ϴ� �Լ�
	{
		physx::PxVec4 result;
		result.x = mat(0, 0) * vec.x + mat(0, 1) * vec.y + mat(0, 2) * vec.z + mat(0, 3) * vec.w;
		result.y = mat(1, 0) * vec.x + mat(1, 1) * vec.y + mat(1, 2) * vec.z + mat(1, 3) * vec.w;
		result.z = mat(2, 0) * vec.x + mat(2, 1) * vec.y + mat(2, 2) * vec.z + mat(2, 3) * vec.w;
		result.w = mat(3, 0) * vec.x + mat(3, 1) * vec.y + mat(3, 2) * vec.z + mat(3, 3) * vec.w;
		return result;
	}

	bool CudaPhysicsCloth::SetPhysicsCloth(const PhysicsClothSetData& data)
	{
		physx::PxVec4* paticle = mClothBuffer->getPositionInvMasses();

		if (!CudaClothTool::UpdateWorldTransformToID3DBuffer(mWorldTransform, data.worldTransform, mVertices.size(), paticle)) return false;

		// ClothBuffer ������Ʈ
		mClothBuffer->raiseFlags(physx::PxParticleBufferFlag::eUPDATE_POSITION);
		mWorldTransform = data.worldTransform;

		return true;
	}

	bool CudaPhysicsCloth::RegisterD3D11VertexBufferWithCUDA(ID3D11Buffer* buffer)
	{
		cudaError_t cudaStatus = cudaGraphicsD3D11RegisterResource(&mCudaVertexResource, buffer, cudaGraphicsRegisterFlagsNone);
		if (cudaStatus != cudaSuccess)
		{
			std::cerr << "Direct3D ���ҽ� ��� ����" << std::endl;
			return false;
		}
		return true;
	}

	bool CudaPhysicsCloth::RegisterD3D11IndexBufferWithCUDA(ID3D11Buffer* buffer)
	{
		cudaError_t cudaStatus = cudaGraphicsD3D11RegisterResource(&mCudaIndexResource, buffer, cudaGraphicsRegisterFlagsNone);
		if (cudaStatus != cudaSuccess)
		{
			std::cerr << "Direct3D ���ҽ� ��� ����" << std::endl;
			return false;
		}
		return true;
	}
}