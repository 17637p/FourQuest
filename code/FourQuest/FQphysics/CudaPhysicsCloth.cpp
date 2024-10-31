#include "CudaPhysicsCloth.h"

#include "EngineDataConverter.h"
#include "CudaClothTool.h"

#include <cudamanager\PxCudaContextManager.h>
#include <cudamanager\PxCudaContext.h>
#include <PxPhysicsAPI.h>

#include <math.h>
#include <random>
#include <spdlog\spdlog.h>

#pragma comment (lib, "cudart.lib")

void MulMatrixVector3(const DirectX::SimpleMath::Matrix& mat, DirectX::SimpleMath::Vector3& vec)
{
	DirectX::SimpleMath::Vector3 result;
	result.x = mat._11 * vec.x + mat._21 * vec.y + mat._31 * vec.z + mat._41;
	result.y = mat._12 * vec.x + mat._22 * vec.y + mat._32 * vec.z + mat._42;
	result.z = mat._13 * vec.x + mat._23 * vec.y + mat._33 * vec.z + mat._43;
	vec = result;
}

float calculateVectorMagnitude(const DirectX::SimpleMath::Vector3& point1, const DirectX::SimpleMath::Vector3& point2)
{
	float dx = point2.x - point1.x;
	float dy = point2.y - point1.y;
	float dz = point2.z - point1.z;

	return std::sqrt(dx * dx + dy * dy + dz * dz);
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
		, mClothMass(1.f)
		, mRestOffset(1.f)
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
		CollisionData* data = (CollisionData*)mParticleSystem->userData;
		data->isDead = true;

		PX_RELEASE(mPBDMaterial);
		PX_RELEASE(mClothBuffer);
		PX_RELEASE(mParticleSystem);

		if (mCudaIndexResource)
		{
			cudaError_t err = cudaGraphicsUnregisterResource(mCudaIndexResource);
			if (err != cudaSuccess)
			{
				spdlog::error("[CudaPhysics ({})] CUDA Failed Direct3D UnRegister VertexResource", __LINE__);
			}
		}
		if (mCudaVertexResource)
		{
			cudaError_t err = cudaGraphicsUnregisterResource(mCudaVertexResource);
			if (err != cudaSuccess)
			{
				spdlog::error("[CudaPhysics ({})] CUDA Failed Direct3D UnRegister VertexResource", __LINE__);
			}
		}
		mCudaIndexResource = nullptr;
		mCudaVertexResource = nullptr;
	}

	bool CudaPhysicsCloth::Initialize(
		const Cloth::CreateClothData& info, 
		physx::PxPhysics* physics, 
		physx::PxScene* scene,
		physx::PxCudaContextManager* cudaContextManager, 
		std::shared_ptr<CollisionData> collisionData,
		int* collisionMatrix,
		bool isSkinnedMesh)
	{
		mbIsSkinnedMesh = isSkinnedMesh;
		mCudaVertexStride = info.vertexStride;

		int deviceCount;
		cudaError_t cudaStatus = cudaGetDeviceCount(&deviceCount);
		if (cudaStatus != cudaSuccess || deviceCount == 0) {
			spdlog::error("[CudaPhysicsCloth ({})] Failed Initlize the Device", __LINE__);
			return false;
		}

		cudaStatus = cudaSetDevice(0); // ù ��° CUDA ����̽� ���
		if (cudaStatus != cudaSuccess) {
			spdlog::error("[CudaPhysicsCloth ({})] Failed Setting the Device", __LINE__);
			return false;
		}

		if (cudaContextManager == nullptr)
		{
			spdlog::error("[CudaPhysicsCloth ({})] CudaContextManager Is \'nullptr\'", __LINE__);
			return false;
		}

		if (!settingInfoData(info))
			return false;
		if (!extractSpringsData())
			return false;

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

		DirectX::SimpleMath::Matrix InvTransform;
		mWorldTransform.Invert(InvTransform);

		if (!CudaClothTool::UpdatePhysXDataToID3DBuffer(mVertices, mIndices, mUV, InvTransform, mCudaVertexResource, mCudaVertexStride, paticle)) return false;		// õ ���ڷ� Graphics VertexBuffer�� position�� Update�ϴ� cuda�Լ� 
		//if (!CudaClothTool::UpdateNormalToID3DBuffer(mSameVertices, mVertices.size(), mCudaVertexResource, mCudaVertexStride)) return false;	// ��ġ�� ���ؽ� �븻�� ������Ʈ cuda�Լ�
		if (!updateWindToParticle()) return false;

		//if (!updateDebugVertex()) return false;

		return true;
	}


	bool CudaPhysicsCloth::updateDebugVertex()
	{
		physx::PxVec4* particle = mClothBuffer->getPositionInvMasses();

		// ���÷�, particles�� GPU �޸𸮿� �ִٸ�, cudaMemcpy�� ����Ͽ� �����մϴ�.
		physx::PxVec4* hostParticleData = new physx::PxVec4[mVertices.size()];

		// GPU���� CPU�� �����մϴ�. (GPU�� �޸� �ּҿ��� CPU�� �޸𸮷�)
		cudaMemcpy(hostParticleData, particle, sizeof(physx::PxVec4) * mVertices.size(), cudaMemcpyDeviceToHost);

		// ������ �����͸� mVertices�� ����
		for (int i = 0; i < mVertices.size(); i++)
		{
			mVertices[i].x = hostParticleData[i].x;
			mVertices[i].y = hostParticleData[i].y;
			mVertices[i].z = hostParticleData[i].z;
		}

		// �޸� ����
		delete[] hostParticleData;

		return true;
	}

	bool CudaPhysicsCloth::updateWindToParticle()
	{
		DirectX::SimpleMath::Vector3 wind = getRandomWindForce(10.f);
		physx::PxVec3 pxWind;
		pxWind.x = wind.x;
		pxWind.y = wind.y;
		pxWind.z = wind.z;

		mParticleSystem->setWind(pxWind);

		return true;
	}

	bool CudaPhysicsCloth::settingInfoData(const Cloth::CreateClothData& data)
	{
		RegisterD3D11VertexBufferWithCUDA((ID3D11Buffer*)data.vertexBuffer);
		RegisterD3D11IndexBufferWithCUDA((ID3D11Buffer*)data.indexBuffer);

		mWorldTransform = data.worldTransform;
		mClothMass = data.clothMass;
		mIndices = data.clothData.indices;
		mUV = data.clothData.uvs;
		mDisableIndicesIndices = data.clothData.disableIndices;
		
		mVertices.resize(data.clothData.vertices.size());
		for (int i = 0; i < mVertices.size(); i++)
		{
			mVertices[i] = DirectX::SimpleMath::Vector3::Transform(data.clothData.vertices[i], mWorldTransform);
		}

		//bool isSucced = CudaClothTool::copyVertexFromGPUToCPU(mVertices, mUV, mWorldTransform, mCudaVertexResource);
		//if (!isSucced)
		//	return false;

		//isSucced = CudaClothTool::copyIndexFromGPUToCPU(mIndices, mCudaIndexResource);
		//if (!isSucced)
		//	return false;

		return true;
	}

	bool areVerticesEqual(const DirectX::SimpleMath::Vector3& vertex1, const DirectX::SimpleMath::Vector3& vertex2, float epsilon = 1e-6)
	{
		return (std::abs(vertex1.x - vertex2.x) < epsilon) &&
			(std::abs(vertex1.y - vertex2.y) < epsilon) &&
			(std::abs(vertex1.z - vertex2.z) < epsilon);
	}

	bool CudaPhysicsCloth::extractSpringsData()
	{
		if (mIndices.size() == 0 || mVertices.size() == 0)
		{
			spdlog::error("[CudaPhysicsCloth ({})] Indices Or Vertices Size is Zero", __LINE__);
			return false;
		}

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

		return true;
	}

	bool CudaPhysicsCloth::createClothParticle(
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
		const physx::PxReal particleMass = mClothMass / mVertices.size();

		// ���� �ý��� ����
		mParticleSystem = physics->createPBDParticleSystem(*cudaContextManager);

		mParticleSystem->setRestOffset(0.1f);
		mParticleSystem->setContactOffset(0.001 + 0.02f);
		mParticleSystem->setParticleContactOffset(0.001 + 0.02f);
		mParticleSystem->setSolidRestOffset(0.001);

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
		if (!settingParticleBuffer(numSprings, numTriangles, numParticles, particlePhase, particleMass, phase, positionInvMass, velocity))
			return false;

		// cloth ����
		createCloth(numParticles, cudaContextManager, phase, positionInvMass, velocity);
	}

	bool CudaPhysicsCloth::settingParticleBuffer(
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

		// ������ ���� ���� ����
		for (unsigned int disableIndex : mDisableIndicesIndices)
		{
			positionInvMass[disableIndex].w = 0.f;							// �������� 0���� �����Ͽ� ���� ����
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
		if (!(numSprings == springs.size()))
		{
			spdlog::error("[CudaPhysicsCloth ({})] Failed Create Cloth Spring", __LINE__);
			return false;
		}
		if (!(numTriangles == triangles.size() / 3))
		{
			spdlog::error("[CudaPhysicsCloth ({})] Failed Create Cloth Spring", __LINE__);
			return false;
		}

		// õ���� ���ۿ� ������ �߰�
		mClothBufferHelper->addCloth(0.f, 0.f, 0.f, triangles.begin(), numTriangles, springs.begin(), numSprings, positionInvMass, numParticles);

		return true;
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
		PX_RELEASE(mClothBufferHelper);

		// �Ҵ�� �޸� ����
		cudaContextManager->freePinnedHostBuffer(positionInvMass);
		cudaContextManager->freePinnedHostBuffer(velocity);
		cudaContextManager->freePinnedHostBuffer(phase);
	}

	DirectX::SimpleMath::Vector3 CudaPhysicsCloth::getRandomWindForce(float maxWindStrength) // ������ �ٶ� ���� �Լ�
	{
		// ������ ���� �� ���� ����
		static std::random_device rd;
		static std::mt19937 gen(rd());
		std::uniform_real_distribution<float> strengthDist(0.0f, maxWindStrength); // �ٶ� ����
		std::uniform_real_distribution<float> directionDist(-1.0f, 1.0f);           // ����

		// ������ �ٶ��� ����� ������ ����
		float windStrength = strengthDist(gen);
		DirectX::SimpleMath::Vector3 windDirection(directionDist(gen), directionDist(gen), directionDist(gen));
		windDirection.Normalize();

		// ���� �ٶ� ���� = ���� * ����
		return windDirection * windStrength;
	}

	void CudaPhysicsCloth::GetPhysicsCloth(Cloth::GetSetClothData& data)
	{
		data.worldTransform = mWorldTransform;
	}

	bool CudaPhysicsCloth::SetPhysicsCloth(const Cloth::GetSetClothData& data)
	{
		physx::PxVec4* paticle = mClothBuffer->getPositionInvMasses();

		DirectX::SimpleMath::Vector3 prevPosition;
		DirectX::SimpleMath::Quaternion prevRotation;
		DirectX::SimpleMath::Vector3 prevScale;
		DirectX::SimpleMath::Vector3 nextPosition;
		DirectX::SimpleMath::Quaternion nextRotation;
		DirectX::SimpleMath::Vector3 nextScale;
		DirectX::SimpleMath::Matrix prevTransform = mWorldTransform;
		DirectX::SimpleMath::Matrix nextTransform = data.worldTransform;

		prevTransform.Decompose(prevScale, prevRotation, prevPosition);
		nextTransform.Decompose(nextScale, nextRotation, nextPosition);
		prevTransform = DirectX::SimpleMath::Matrix::CreateFromQuaternion(prevRotation) * DirectX::SimpleMath::Matrix::CreateTranslation(prevPosition);
		nextTransform = DirectX::SimpleMath::Matrix::CreateFromQuaternion(nextRotation) * DirectX::SimpleMath::Matrix::CreateTranslation(nextPosition);

		//if (!CudaClothTool::UpdateWorldTransformToID3DBuffer(prevTransform, nextTransform, mVertices.size(), paticle)) return false;

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
			spdlog::error("[CudaPhysicsCloth Warnning({})] Failed Register Vertex ( Error : {} )", __LINE__, cudaGetErrorString(cudaStatus));
			return false;
		}
		return true;
	}

	bool CudaPhysicsCloth::RegisterD3D11IndexBufferWithCUDA(ID3D11Buffer* buffer)
	{
		cudaError_t cudaStatus = cudaGraphicsD3D11RegisterResource(&mCudaIndexResource, buffer, cudaGraphicsRegisterFlagsNone);
		if (cudaStatus != cudaSuccess)
		{
			spdlog::error("[CudaPhysicsCloth Warnning({})] Failed Register Index ( Error : {} )", __LINE__, cudaGetErrorString(cudaStatus));
			return false;
		}
		return true;
	}
}