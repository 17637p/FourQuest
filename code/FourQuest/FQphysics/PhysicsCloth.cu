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
// CUDA 커널 함수 정의
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

		// 중간값을 사용하여 덧셈 연산을 분리
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

// 커널 함수
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

	// 이전 worldTransform의 역행렬 적용
	vertex = multiply(previousTransformInverse, vertex);

	// 새로운 worldTransform 적용
	vertex = multiply(newTransform, vertex);

	// 변환된 vertex 저장
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
			std::cerr << "CUDA 디바이스 초기화 실패" << std::endl;
			return false;
		}

		cudaStatus = cudaSetDevice(0); // 첫 번째 CUDA 디바이스 사용
		if (cudaStatus != cudaSuccess) {
			std::cerr << "CUDA 디바이스 설정 실패" << std::endl;
			return false;
		}

		if (cudaContextManager == nullptr)
			return false;

		settingInfoData(info);
		extractSpringsData();

		// 재료(Material) 설정
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
		// 삼각형 단위로 인덱스를 순회
		for (size_t i = 0; i < mIndices.size(); i += 3)
		{
			unsigned int v1 = mIndices[i];
			unsigned int v2 = mIndices[i + 1];
			unsigned int v3 = mIndices[i + 2];

			// 정렬된 순서로 vertex 쌍을 추가하여 중복된 선분 방지
			auto addEdge = [this](unsigned int a, unsigned int b)
				{
					if (a > b) std::swap(a, b);
					mSprings.insert({ a, b });
				};

			addEdge(v1, v2);
			addEdge(v2, v3);
			addEdge(v3, v1);
		}

		// 같은 위치의 버텍스를 묶고 저장
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
		// 입자 및 스프링, 삼각형의 개수 계산
		const physx::PxU32 numParticles = mVertices.size();	// 입자 갯수
		const physx::PxU32 numSprings = mSprings.size();	// 입자 하나당 이웃하는 입자들에 스프링 값을 가지는데, 그 스프링 갯수
		const physx::PxU32 numTriangles = mIndices.size() / 3;	// 삼각형 갯수

		// 입자 시스템의 설정
		const physx::PxReal particleMass = mTotalClothMass / mVertices.size();
		const physx::PxReal restOffset = 2.f;

		// 입자 시스템 생성
		mParticleSystem = physics->createPBDParticleSystem(*cudaContextManager);

		mParticleSystem->setRestOffset(1.f);
		mParticleSystem->setContactOffset(restOffset + 0.02f);
		mParticleSystem->setParticleContactOffset(restOffset + 0.02f);
		mParticleSystem->setSolidRestOffset(restOffset);

		// 씬에 입자 시스템 추가
		scene->addActor(*mParticleSystem);

		// 입자의 상태를 저장하는 버퍼 생성
		const physx::PxU32 particlePhase = mParticleSystem->createPhase(mPBDMaterial, physx::PxParticlePhaseFlags(
			physx::PxParticlePhaseFlag::eParticlePhaseSelfCollideFilter | physx::PxParticlePhaseFlag::eParticlePhaseSelfCollide));

		mClothBufferHelper = physx::ExtGpu::PxCreateParticleClothBufferHelper(1, numTriangles, numSprings, numParticles, cudaContextManager);
		// 입자, 스프링 삼각형의 상태를 저장하기 위한 버퍼 할당
		unsigned int* phase = cudaContextManager->allocPinnedHostBuffer<physx::PxU32>(numParticles);
		physx::PxVec4* positionInvMass = cudaContextManager->allocPinnedHostBuffer<physx::PxVec4>(numParticles);
		physx::PxVec4* velocity = cudaContextManager->allocPinnedHostBuffer<physx::PxVec4>(numParticles);

		// cloth를 생성할 파티클과 스프링 데이터를 기반으로 하는 Cloth Particle Buffer 생성
		settingParticleBuffer(numSprings, numTriangles, numParticles, particlePhase, particleMass, phase, positionInvMass, velocity);

		// cloth 생성
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

		// 파티클 스프링 및 트라이앵글 생성
		physx::PxArray<physx::PxParticleSpring> springs;
		springs.reserve(numSprings);
		physx::PxArray<physx::PxU32> triangles;
		triangles.reserve(numTriangles * 3);

		// 입자 상태 저장
		for (int i = 0; i < numParticles; i++)
		{
			positionInvMass[i] = physx::PxVec4(mVertices[i].x, mVertices[i].y + 300.f, mVertices[i].z, 1.f / particleMass);
			phase[i] = particlePhase;
			velocity[i] = physx::PxVec4(0.f);
		}

		// 스프링 추가
		for (auto line : mSprings)
		{
			physx::PxParticleSpring spring = { line.first, line.second, calculateVectorMagnitude(mVertices[line.first], mVertices[line.second]), stretchStiffness, springDamping, 0 };
			springs.pushBack(spring);
		}

		// 삼각형 추가
		for (int i = 0; i < mIndices.size(); i += 3)
		{
			triangles.pushBack(mIndices[i]);
			triangles.pushBack(mIndices[i + 1]);
			triangles.pushBack(mIndices[i + 2]);
		}

		// 생성된 스프링 및 삼각형 수가 예상대로 생성되었는지 확인
		PX_ASSERT(numSprings == springs.size());
		PX_ASSERT(numTriangles == triangles.size() / 3);

		// 천막의 버퍼에 데이터 추가
		mClothBufferHelper->addCloth(0.f, 0.f, 0.f, triangles.begin(), numTriangles, springs.begin(), numSprings, positionInvMass, numParticles);
	}

	void PhysicsCloth::createCloth(
		const physx::PxU32& numParticles,
		physx::PxCudaContextManager* cudaContextManager,
		unsigned int* phase,
		physx::PxVec4* positionInvMass,
		physx::PxVec4* velocity)
	{
		// 입자의 상태를 나타내는 버퍼 설명
		physx::ExtGpu::PxParticleBufferDesc bufferDesc;
		bufferDesc.maxParticles = numParticles;
		bufferDesc.numActiveParticles = numParticles;
		bufferDesc.positions = positionInvMass;
		bufferDesc.velocities = velocity;
		bufferDesc.phases = phase;

		// 천막의 설명 가져오기
		const physx::PxParticleClothDesc& clothDesc = mClothBufferHelper->getParticleClothDesc();

		// 입자 천막의 전처리기 생성
		physx::PxParticleClothPreProcessor* clothPreProcessor = PxCreateParticleClothPreProcessor(cudaContextManager);

		// 입자 천막 분할 및 처리
		physx::PxPartitionedParticleCloth output;
		clothPreProcessor->partitionSprings(clothDesc, output);
		clothPreProcessor->release();

		// 천막 버퍼 생성
		mClothBuffer = physx::ExtGpu::PxCreateAndPopulateParticleClothBuffer(bufferDesc, clothDesc, output, cudaContextManager);
		mParticleSystem->addParticleBuffer(mClothBuffer);

		// 버파 해제
		mClothBufferHelper->release();

		// 할당된 메모리 해제
		cudaContextManager->freePinnedHostBuffer(positionInvMass);
		cudaContextManager->freePinnedHostBuffer(velocity);
		cudaContextManager->freePinnedHostBuffer(phase);
	}

	void PhysicsCloth::GetPhysicsCloth(PhysicsClothGetData& data)
	{
		data.worldTransform = mWorldTransform;
	}

	physx::PxVec4 multiply(const physx::PxMat44& mat, const physx::PxVec4& vec)  // 4x4 행렬과 PxVec4를 곱하는 함수
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
			std::cerr << "Direct3D 리소스 등록 실패" << std::endl;
			return false;
		}
		return true;
	}

	bool PhysicsCloth::RegisterD3D11IndexBufferWithCUDA(ID3D11Buffer* buffer)
	{
		cudaError_t cudaStatus = cudaGraphicsD3D11RegisterResource(&mCudaIndexResource, buffer, cudaGraphicsRegisterFlagsNone);
		if (cudaStatus != cudaSuccess)
		{
			std::cerr << "Direct3D 리소스 등록 실패" << std::endl;
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

		// CUDA 리소스를 매핑
		cudaError_t cudaStatus = cudaGraphicsMapResources(1, &directXVertexBuffer);
		if (cudaStatus != cudaSuccess) {
			std::cerr << "cudaGraphicsMapResources 실패: " << cudaGetErrorString(cudaStatus) << std::endl;
			return false;
		}

		// CUDA 포인터 가져오기
		void* devPtr = nullptr;
		size_t size = 0;
		cudaGraphicsResourceGetMappedPointer(&devPtr, &size, directXVertexBuffer);
		if (cudaStatus != cudaSuccess) {
			std::cerr << "cudaGraphicsResourceGetMappedPointer 실패: " << cudaGetErrorString(cudaStatus) << std::endl;
			return false;
		}

		// GPU Memory를 할당할 변수
		physx::PxVec2* d_uvs = nullptr;
		unsigned int* d_indices = nullptr;

		// GPU Memory에 할당 및 데이터 복사
		cudaMalloc(&d_uvs, uvs.size() * sizeof(DirectX::SimpleMath::Vector2));
		cudaMalloc(&d_indices, indices.size() * sizeof(unsigned int));
		cudaMemcpy(d_uvs, uvs.data(), uvs.size() * sizeof(DirectX::SimpleMath::Vector2), cudaMemcpyKind::cudaMemcpyHostToDevice);
		cudaMemcpy(d_indices, indices.data(), indices.size() * sizeof(unsigned int), cudaMemcpyKind::cudaMemcpyHostToDevice);

		// CUDA 함수 실행
		UpdateVertex << <blocksPerGrid, threadsPerBlock >> > (
			particle, d_uvs, deviceVertexSize, d_indices, deviceIndexSize, (PhysicsVertex*)devPtr);
		cudaDeviceSynchronize();

		// CUDA 리소스를 언매핑
		cudaGraphicsUnmapResources(1, &directXVertexBuffer);

		// 메모리 해제
		cudaFree(d_uvs);
		cudaFree(d_indices);

		return true;
	}

	bool PhysicsCloth::updateNormalToID3DBuffer(
		std::vector<std::pair<unsigned int, unsigned int>>& sameVertices,
		unsigned int vertexSize,
		cudaGraphicsResource* directXVertexBuffer)
	{
		// CUDA 리소스를 매핑
		cudaError_t cudaStatus = cudaGraphicsMapResources(1, &directXVertexBuffer);
		if (cudaStatus != cudaSuccess) {
			std::cerr << "cudaGraphicsMapResources 실패: " << cudaGetErrorString(cudaStatus) << std::endl;
			return false;
		}

		// CUDA 포인터 가져오기
		void* devPtr = nullptr;
		size_t size = 0;
		cudaGraphicsResourceGetMappedPointer(&devPtr, &size, directXVertexBuffer);
		if (cudaStatus != cudaSuccess) {
			std::cerr << "cudaGraphicsResourceGetMappedPointer 실패: " << cudaGetErrorString(cudaStatus) << std::endl;
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

		// CUDA 리소스를 언매핑
		cudaGraphicsUnmapResources(1, &directXVertexBuffer);

		// 메모리 해제
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