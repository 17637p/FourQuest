#include "PhysicsCloth.h"

#include "EngineDataConverter.h"

__device__ physx::PxVec2 Sub(const physx::PxVec2& lhs, const physx::PxVec2& rhs) {
	return { lhs.x - rhs.x, lhs.y - rhs.y };
}

__device__ physx::PxVec4 Sub(const physx::PxVec4& lhs, const physx::PxVec4& rhs) {
	return { lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z, lhs.w - rhs.w };
}

__device__ physx::PxVec3 cross(const physx::PxVec4& a, const physx::PxVec4& b) {
	return { a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x };
}

__device__ float DotProduct(const physx::PxVec3& a, const physx::PxVec3& b) {
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

__device__ void NormalizeVector(physx::PxVec3& vec) {
	float length = sqrtf(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
	if (length > 0) {
		vec.x /= length;
		vec.y /= length;
		vec.z /= length;
	}
}

#pragma region UpdateID3D11VertexBuffer
// CUDA Ŀ�� �Լ� ����
__global__ void UpdateVertex(
	physx::PxVec4* vertices,
	physx::PxVec2* uvs,
	unsigned int vertexSize,
	unsigned int* indices,
	unsigned int indexSize,
	fq::physics::Vertex* buffer)
{
	int idx = blockIdx.x * blockDim.x + threadIdx.x;
	if (idx >= indexSize / 3) return;

	unsigned int i0 = indices[idx * 3];
	unsigned int i1 = indices[idx * 3 + 1];
	unsigned int i2 = indices[idx * 3 + 2];

	physx::PxVec4 v0 = vertices[i0];
	physx::PxVec4 v1 = vertices[i1];
	physx::PxVec4 v2 = vertices[i2];
	v0.z = -v0.z;
	v1.z = -v1.z;
	v2.z = -v2.z;

	physx::PxVec2 uv0 = uvs[i0];
	physx::PxVec2 uv1 = uvs[i1];
	physx::PxVec2 uv2 = uvs[i2];

	physx::PxVec4 edge1 = Sub(v1, v0);
	physx::PxVec4 edge2 = Sub(v2, v0);

	physx::PxVec2 deltaUV1 = Sub(uv1, uv0);
	physx::PxVec2 deltaUV2 = Sub(uv2, uv0);

	float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

	physx::PxVec3 tangent;
	tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
	tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
	tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
	NormalizeVector(tangent);

	physx::PxVec3 bitangent;
	bitangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
	bitangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
	bitangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
	NormalizeVector(bitangent);

	physx::PxVec3 normal = cross(edge1, edge2);
	NormalizeVector(normal);

	buffer[i0].Pos.x = v0.x;
	buffer[i0].Pos.y = v0.y;
	buffer[i0].Pos.z = v0.z;
	buffer[i1].Pos.x = v1.x;
	buffer[i1].Pos.y = v1.y;
	buffer[i1].Pos.z = v1.z;
	buffer[i2].Pos.x = v2.x;
	buffer[i2].Pos.y = v2.y;
	buffer[i2].Pos.z = v2.z;

	buffer[i0].Normal.x = normal.x;
	buffer[i0].Normal.y = normal.y;
	buffer[i0].Normal.z = normal.z;
	buffer[i1].Normal.x = normal.x;
	buffer[i1].Normal.y = normal.y;
	buffer[i1].Normal.z = normal.z;
	buffer[i2].Normal.x = normal.x;
	buffer[i2].Normal.y = normal.y;
	buffer[i2].Normal.z = normal.z;

	buffer[i0].Tangent.x = tangent.x;
	buffer[i0].Tangent.y = tangent.y;
	buffer[i0].Tangent.z = tangent.z;
	buffer[i1].Tangent.x = tangent.x;
	buffer[i1].Tangent.y = tangent.y;
	buffer[i1].Tangent.z = tangent.z;
	buffer[i2].Tangent.x = tangent.x;
	buffer[i2].Tangent.y = tangent.y;
	buffer[i2].Tangent.z = tangent.z;
}
#pragma endregion

#pragma region SetID3D11BufferVertexNormal
__global__ void processVerticesKernel(unsigned int* sameVerticesFirst, unsigned int* sameVerticesSecond,
	fq::physics::Vertex* buffer, int size) {
	int idx = blockIdx.x * blockDim.x + threadIdx.x;

	if (idx < size) {
		int first = sameVerticesFirst[idx];
		int second = sameVerticesSecond[idx];

		// �߰����� ����Ͽ� ���� ������ �и�
		float3 normalTemp, tangentTemp, biTangentTemp;

		normalTemp.x = buffer[first].Normal.x + buffer[second].Normal.x;
		normalTemp.y = buffer[first].Normal.y + buffer[second].Normal.y;
		normalTemp.z = buffer[first].Normal.z + buffer[second].Normal.z;

		tangentTemp.x = buffer[first].Tangent.x + buffer[second].Tangent.x;
		tangentTemp.y = buffer[first].Tangent.y + buffer[second].Tangent.y;
		tangentTemp.z = buffer[first].Tangent.z + buffer[second].Tangent.z;

		buffer[first].Normal.x = normalTemp.x;
		buffer[first].Normal.y = normalTemp.y;
		buffer[first].Normal.z = normalTemp.z;
		buffer[second].Normal.x = normalTemp.x;
		buffer[second].Normal.y = normalTemp.y;
		buffer[second].Normal.z = normalTemp.z;

		buffer[first].Tangent.x = tangentTemp.x;
		buffer[first].Tangent.y = tangentTemp.y;
		buffer[first].Tangent.z = tangentTemp.z;
		buffer[second].Tangent.x = tangentTemp.x;
		buffer[second].Tangent.y = tangentTemp.y;
		buffer[second].Tangent.z = tangentTemp.z;
	}
}
#pragma endregion

#pragma region SetClothParticleWorldTransform
__device__ DirectX::SimpleMath::Vector3 multiply(DirectX::SimpleMath::Matrix& mat, const DirectX::SimpleMath::Vector3& vec)
{
	DirectX::SimpleMath::Vector3 result;
	result.x = mat.m[0][0] * vec.x + mat.m[1][0] * vec.y + mat.m[2][0] * vec.z + mat.m[3][0] * 1.0f;
	result.y = mat.m[0][1] * vec.x + mat.m[1][1] * vec.y + mat.m[2][1] * vec.z + mat.m[3][1] * 1.0f;
	result.z = mat.m[0][2] * vec.x + mat.m[1][2] * vec.y + mat.m[2][2] * vec.z + mat.m[3][2] * 1.0f;
	return result;
}

// Ŀ�� �Լ�
__global__ void TransformVertices(
	physx::PxVec4* particle,
	DirectX::SimpleMath::Matrix previousTransformInverse,
	DirectX::SimpleMath::Matrix newTransform,
	int vertexCount)
{
	int idx = blockIdx.x * blockDim.x + threadIdx.x;
	if (idx >= vertexCount) return;

	DirectX::SimpleMath::Vector3 vertex;
	vertex.x = particle[idx].x;
	vertex.y = particle[idx].y;
	vertex.z = -particle[idx].z;

	// ���� worldTransform�� ����� ����
	vertex = multiply(previousTransformInverse, vertex);

	// ���ο� worldTransform ����
	vertex = multiply(newTransform, vertex);

	// ��ȯ�� vertex ����
	particle[idx].x = vertex.x;
	particle[idx].y = vertex.y;
	particle[idx].z = -vertex.z;
}
#pragma endregion


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
	PhysicsCloth::PhysicsCloth(unsigned int id, unsigned int layerNumber)
		: mID()
		, mLayNumber()
		, mWorldTransform{}
		, mParticleSystem(nullptr)
		, mClothBuffer(nullptr)
		, mPBDMaterial(nullptr)
	{
	}

	PhysicsCloth::~PhysicsCloth()
	{
	}

	bool PhysicsCloth::Initialize(const PhysicsClothInfo& info, physx::PxPhysics* physics, physx::PxScene* scene, physx::PxCudaContextManager* cudaContextManager)
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

		createClothParticle(physics, scene, cudaContextManager);

		return true;
	}

	bool PhysicsCloth::UpdatePhysicsCloth(physx::PxCudaContextManager* cudaContextManager)
	{
		physx::PxVec4* paticle = mClothBuffer->getPositionInvMasses();

		if (!updatePhysXDataToID3DBuffer(mVertices, mIndices, mUV, mCudaVertexResource, paticle)) return false;
		if (!updateNormalToID3DBuffer(mSameVertices, mVertices.size(), mCudaVertexResource)) return false;

		return true;
	}

	void PhysicsCloth::settingInfoData(const PhysicsClothInfo& info)
	{
		RegisterD3D11VertexBufferWithCUDA((ID3D11Buffer*)info.vertexBuffer);
		RegisterD3D11IndexBufferWithCUDA((ID3D11Buffer*)info.indexBuffer);

		mWorldTransform = info.worldTransform;
		mTotalClothMass = info.totalClothMass;

		mIndices.resize(info.indexSize);
		memcpy(mIndices.data(), info.indices, info.indexSize * sizeof(unsigned int));

		mUV.resize(info.vertexSize);
		mVertices.resize(info.vertexSize);

		for (int i = 0; i < info.vertexSize; i++)
		{
			mVertices[i].x = info.vertices[i].x;
			mVertices[i].y = info.vertices[i].y;
			mVertices[i].z = info.vertices[i].z;
			mUV[i] = info.uvs[i];
		}

		for (auto& vertex : mVertices)
		{
			MulMatrixVector3(mWorldTransform, vertex);
		}

		for (int i = 0; i < info.vertexSize; i++)
		{
			mVertices[i].z = -mVertices[i].z;
		}
	}

	bool areVerticesEqual(const DirectX::SimpleMath::Vector3& vertex1, const DirectX::SimpleMath::Vector3& vertex2, float epsilon = 1e-6) {
		return (std::abs(vertex1.x - vertex2.x) < epsilon) &&
			(std::abs(vertex1.y - vertex2.y) < epsilon) &&
			(std::abs(vertex1.z - vertex2.z) < epsilon);
	}

	void PhysicsCloth::extractSpringsData()
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

	void PhysicsCloth::createClothParticle(physx::PxPhysics* physics, physx::PxScene* scene, physx::PxCudaContextManager* cudaContextManager)
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

	float calculateVectorMagnitude(const DirectX::SimpleMath::Vector3& point1, const DirectX::SimpleMath::Vector3& point2) {
		float dx = point2.x - point1.x;
		float dy = point2.y - point1.y;
		float dz = point2.z - point1.z;

		return std::sqrt(dx * dx + dy * dy + dz * dz);
	}

	void PhysicsCloth::settingParticleBuffer(
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
			positionInvMass[i] = physx::PxVec4(mVertices[i].x, mVertices[i].y + 300.f, mVertices[i].z, 1.f / particleMass);
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

	void PhysicsCloth::createCloth(
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

	void PhysicsCloth::GetPhysicsCloth(PhysicsClothGetData& data)
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

	bool PhysicsCloth::SetPhysicsCloth(const PhysicsClothSetData& data)
	{
		physx::PxVec4* paticle = mClothBuffer->getPositionInvMasses();

		if (!updateWorldTransformToID3DBuffer(mWorldTransform, data.worldTransform, mVertices.size(), paticle)) return false;

		return true;
	}

	bool PhysicsCloth::RegisterD3D11VertexBufferWithCUDA(ID3D11Buffer* buffer)
	{
		cudaError_t cudaStatus = cudaGraphicsD3D11RegisterResource(&mCudaVertexResource, buffer, cudaGraphicsRegisterFlagsNone);
		if (cudaStatus != cudaSuccess)
		{
			std::cerr << "Direct3D ���ҽ� ��� ����" << std::endl;
			return false;
		}
		return true;
	}

	bool PhysicsCloth::RegisterD3D11IndexBufferWithCUDA(ID3D11Buffer* buffer)
	{
		cudaError_t cudaStatus = cudaGraphicsD3D11RegisterResource(&mCudaIndexResource, buffer, cudaGraphicsRegisterFlagsNone);
		if (cudaStatus != cudaSuccess)
		{
			std::cerr << "Direct3D ���ҽ� ��� ����" << std::endl;
			return false;
		}
		return true;
	}

#pragma region CudaFunction
	bool PhysicsCloth::updatePhysXDataToID3DBuffer(
		std::vector<DirectX::SimpleMath::Vector3>& vertices,
		std::vector<unsigned int>& indices,
		std::vector<DirectX::SimpleMath::Vector2> uvs,
		cudaGraphicsResource* directXVertexBuffer,
		physx::PxVec4* particle)
	{
		int threadsPerBlock = 256;
		int blocksPerGrid = (indices.size() / 3 + threadsPerBlock - 1) / threadsPerBlock;

		// CUDA ���ҽ��� ����
		cudaError_t cudaStatus = cudaGraphicsMapResources(1, &directXVertexBuffer);
		if (cudaStatus != cudaSuccess) {
			std::cerr << "cudaGraphicsMapResources ����: " << cudaGetErrorString(cudaStatus) << std::endl;
			return false;
		}

		// CUDA ������ ��������
		void* devPtr = nullptr;
		size_t size = 0;
		cudaGraphicsResourceGetMappedPointer(&devPtr, &size, directXVertexBuffer);
		if (cudaStatus != cudaSuccess) {
			std::cerr << "cudaGraphicsResourceGetMappedPointer ����: " << cudaGetErrorString(cudaStatus) << std::endl;
			return false;
		}

		// GPU Memory�� �Ҵ��� ����
		physx::PxVec2* d_uvs = nullptr;
		unsigned int* d_indices = nullptr;

		// GPU Memory�� �Ҵ� �� ������ ����
		cudaMalloc(&d_uvs, uvs.size() * sizeof(DirectX::SimpleMath::Vector2));
		cudaMalloc(&d_indices, indices.size() * sizeof(unsigned int));
		cudaMemcpy(d_uvs, uvs.data(), uvs.size() * sizeof(DirectX::SimpleMath::Vector2), cudaMemcpyKind::cudaMemcpyHostToDevice);
		cudaMemcpy(d_indices, indices.data(), indices.size() * sizeof(unsigned int), cudaMemcpyKind::cudaMemcpyHostToDevice);

		// CUDA �Լ� ����
		UpdateVertex << <blocksPerGrid, threadsPerBlock >> > (
			particle, d_uvs, deviceVertexSize, d_indices, deviceIndexSize, (PhysicsVertex*)devPtr);
		cudaDeviceSynchronize();

		// CUDA ���ҽ��� �����
		cudaGraphicsUnmapResources(1, &directXVertexBuffer);

		// �޸� ����
		cudaFree(d_uvs);
		cudaFree(d_indices);

		return true;
	}

	bool PhysicsCloth::updateNormalToID3DBuffer(
		std::vector<std::pair<unsigned int, unsigned int>>& sameVertices,
		unsigned int vertexSize,
		cudaGraphicsResource* directXVertexBuffer)
	{
		// CUDA ���ҽ��� ����
		cudaError_t cudaStatus = cudaGraphicsMapResources(1, &directXVertexBuffer);
		if (cudaStatus != cudaSuccess) {
			std::cerr << "cudaGraphicsMapResources ����: " << cudaGetErrorString(cudaStatus) << std::endl;
			return false;
		}

		// CUDA ������ ��������
		void* devPtr = nullptr;
		size_t size = 0;
		cudaGraphicsResourceGetMappedPointer(&devPtr, &size, directXVertexBuffer);
		if (cudaStatus != cudaSuccess) {
			std::cerr << "cudaGraphicsResourceGetMappedPointer ����: " << cudaGetErrorString(cudaStatus) << std::endl;
			return false;
		}

		int threadsPerBlock = 256;
		int blocksPerGrid = (vertexSize / 3 + threadsPerBlock - 1) / threadsPerBlock;

		blocksPerGrid = (size + threadsPerBlock - 1) / threadsPerBlock;

		std::vector<unsigned int> firstVertex;
		std::vector<unsigned int> secondVertex;
		firstVertex.resize(sameVertices.size());
		secondVertex.resize(sameVertices.size());
		for (int i = 0; i < sameVertices.size(); i++)
		{
			firstVertex.push_back(sameVertices[i].first);
			secondVertex.push_back(sameVertices[i].second);
		}

		unsigned int* d_firstVertex;
		unsigned int* d_secondVertex;

		cudaMalloc(&d_firstVertex, firstVertex.size() * sizeof(unsigned int));
		cudaMalloc(&d_secondVertex, secondVertex.size() * sizeof(unsigned int));
		cudaMemcpy(d_firstVertex, firstVertex.data(), firstVertex.size() * sizeof(unsigned int), cudaMemcpyKind::cudaMemcpyHostToDevice);
		cudaMemcpy(d_secondVertex, secondVertex.data(), secondVertex.size() * sizeof(unsigned int), cudaMemcpyKind::cudaMemcpyHostToDevice);

		processVerticesKernel << <blocksPerGrid, threadsPerBlock >> > (d_firstVertex, d_secondVertex, (PhysicsVertex*)devPtr, mVertices.size());
		cudaDeviceSynchronize();

		// CUDA ���ҽ��� �����
		cudaGraphicsUnmapResources(1, &directXVertexBuffer);

		// �޸� ����
		cudaFree(d_firstVertex);
		cudaFree(d_secondVertex);

		return true;
	}

	bool PhysicsCloth::updateWorldTransformToID3DBuffer(
		const DirectX::SimpleMath::Matrix& prevWorldTransform,
		const DirectX::SimpleMath::Matrix& nextWorldTrnasform,
		const unsigned int vertexSize,
		physx::PxVec4* particle)
	{
		int threadsPerBlock = 256;
		int blocksPerGrid = (vertexSize / 3 + threadsPerBlock - 1) / threadsPerBlock;

		DirectX::SimpleMath::Matrix prevTransform = prevWorldTransform.Invert();
		DirectX::SimpleMath::Matrix nextTransform = nextWorldTrnasform;

		int threadsPerBlock = 256;
		int blocksPerGrid = (vertexSize + threadsPerBlock - 1) / threadsPerBlock;

		TransformVertices << <blocksPerGrid, threadsPerBlock >> > (particle, prevMatrix, nextMatrix, vertexSize);
		cudaDeviceSynchronize();

		return true;
	}
#pragma endregion

}