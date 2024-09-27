#include "PhysicsRigidBodyManager.h"

#include "PhysicsResourceManager.h"
#include "PhysicsCollisionDataManager.h"
#include "RigidBody.h"
#include "DynamicRigidBody.h"
#include "StaticRigidBody.h"
#include "EngineDataConverter.h"
#include "ConvexMeshResource.h"
#include "HeightFieldResource.h"
#include "TriangleMeshResource.h"
#include "DebugData.h"

#include "PhysicsCookingMeshTool.h"

namespace fq::physics
{
	PhysicsRigidBodyManager::PhysicsRigidBodyManager()
		: mPhysics(nullptr)
		, mResourceManager()
		, mCollisionDataManager()
		, mRigidBodyContainer()
		, mUpcomingActors()
		, mDebugData(std::make_shared<DebugData>())
	{
	}

	PhysicsRigidBodyManager::~PhysicsRigidBodyManager()
	{
		mRigidBodyContainer.clear();
		mUpcomingActors.clear();
	}

	bool PhysicsRigidBodyManager::Initialize(physx::PxPhysics* physics, std::shared_ptr<PhysicsResourceManager> resourceManager, std::shared_ptr<PhysicsCollisionDataManager> collisionDataManager)
	{
		mPhysics = physics;
		mResourceManager = resourceManager;
		mCollisionDataManager = collisionDataManager;

		return true;
	}

	bool PhysicsRigidBodyManager::Update(physx::PxScene* scene, physx::PxScene* gpuScene)
	{
		/// <summary>
		/// 물리 공간 씬에 바디 액터 추가하기
		/// <summary>
		for (const std::shared_ptr<RigidBody>& body : mUpcomingActors)
		{
			std::shared_ptr<DynamicRigidBody> dynamicBody = std::dynamic_pointer_cast<DynamicRigidBody>(body);
			if (dynamicBody)
			{
				scene->addActor(*dynamicBody->GetPxRigidDynamic());
			}
			std::shared_ptr<StaticRigidBody> staticBody = std::dynamic_pointer_cast<StaticRigidBody>(body);
			if (staticBody)
			{
				scene->addActor(*staticBody->GetPxRigidStatic());
			}
		}

		/// <summary>
		/// GPU 물리 공간 씬에 바디 액터 추가하기
		/// <summary>
		for (const std::shared_ptr<RigidBody>& body : mUpcomingGpuActors)
		{
			std::shared_ptr<DynamicRigidBody> dynamicBody = std::dynamic_pointer_cast<DynamicRigidBody>(body);
			if (dynamicBody)
			{
				gpuScene->addActor(*dynamicBody->GetPxRigidDynamic());
			}
			std::shared_ptr<StaticRigidBody> staticBody = std::dynamic_pointer_cast<StaticRigidBody>(body);
			if (staticBody)
			{
				gpuScene->addActor(*staticBody->GetPxRigidStatic());
			}
		}

		mUpcomingActors.clear();
		mUpcomingGpuActors.clear();

		return true;
	}
		
	bool PhysicsRigidBodyManager::FinalUpdate()
	{
#ifdef _DEBUG
		ExtractDebugData();
#endif

		return true;
	}

#pragma region GetSetRigidBodyData
	void PhysicsRigidBodyManager::GetRigidBodyData(unsigned int id, RigidBodyGetSetData& rigidBodyData)
	{
		auto body = mRigidBodyContainer.find(id)->second;

		std::shared_ptr<DynamicRigidBody> dynamicBody = std::dynamic_pointer_cast<DynamicRigidBody>(body);
		if (dynamicBody)
		{
			physx::PxRigidDynamic* pxBody = dynamicBody->GetPxRigidDynamic();
			DirectX::SimpleMath::Matrix dxMatrix;
			CopyPxTransformToDirectXMatrix(pxBody->getGlobalPose(), dxMatrix);
			rigidBodyData.transform = DirectX::SimpleMath::Matrix::CreateScale(dynamicBody->GetScale()) * dxMatrix * dynamicBody->GetOffsetTranslation();
			CopyPxVec3ToDxVec3(pxBody->getLinearVelocity(), rigidBodyData.linearVelocity);
			CopyPxVec3ToDxVec3(pxBody->getAngularVelocity(), rigidBodyData.angularVelocity);
		}
		std::shared_ptr<StaticRigidBody> staticBody = std::dynamic_pointer_cast<StaticRigidBody>(body);
		if (staticBody)
		{
			physx::PxRigidStatic* pxBody = staticBody->GetPxRigidStatic();
			DirectX::SimpleMath::Matrix dxMatrix;
			CopyPxTransformToDirectXMatrix(pxBody->getGlobalPose(), dxMatrix);
			rigidBodyData.transform = 
				DirectX::SimpleMath::Matrix::CreateScale(staticBody->GetScale()) 
				* staticBody->GetOffsetRotation()
				* dxMatrix
				* staticBody->GetOffsetTranslation();
		}
	}

	bool areTransformsDifferent(const physx::PxTransform& transform1, const physx::PxTransform& transform2, float positionTolerance, float rotationTolerance) 
	{
		// Compare positions
		if (!(transform1.p).isFinite() || !(transform2.p).isFinite() || (transform1.p - transform2.p).magnitude() > positionTolerance) {
			return true;
		}

		// Compare rotations
		if (!(transform1.q).isFinite() || !(transform2.q).isFinite() || (physx::PxAbs(transform1.q.w - transform2.q.w) > rotationTolerance) ||
			(physx::PxAbs(transform1.q.x - transform2.q.x) > rotationTolerance) ||
			(physx::PxAbs(transform1.q.y - transform2.q.y) > rotationTolerance) ||
			(physx::PxAbs(transform1.q.z - transform2.q.z) > rotationTolerance)) {
			return true;
		}

		return false;
	}

	bool PhysicsRigidBodyManager::SetRigidBodyData(const unsigned int& id, const RigidBodyGetSetData& rigidBodyData, int* collisionMatrix)
	{
		using namespace DirectX::SimpleMath;

		if (mRigidBodyContainer.find(id) == mRigidBodyContainer.end())
			return false;

		auto body = mRigidBodyContainer.find(id)->second;

		std::shared_ptr<DynamicRigidBody> dynamicBody = std::dynamic_pointer_cast<DynamicRigidBody>(body);
		if (dynamicBody)
		{
			Matrix dxTransform = rigidBodyData.transform;
			physx::PxTransform pxTransform;
			physx::PxVec3 pxLinearVelocity;
			physx::PxVec3 pxangularVelocity;
			CopyDxVec3ToPxVec3(rigidBodyData.linearVelocity, pxLinearVelocity);
			CopyDxVec3ToPxVec3(rigidBodyData.angularVelocity, pxangularVelocity);

			physx::PxRigidDynamic* pxBody = dynamicBody->GetPxRigidDynamic();
			if (!(pxBody->getRigidBodyFlags() & physx::PxRigidBodyFlag::eKINEMATIC))
			{
				pxBody->setLinearVelocity(pxLinearVelocity);
				pxBody->setAngularVelocity(pxangularVelocity);
			}

			Vector3 position;
			Vector3 scale;
			Quaternion rotation;
			dxTransform.Decompose(scale, rotation, position);
			dxTransform = Matrix::CreateScale(1.f) 
				* Matrix::CreateFromQuaternion(rotation) 
				* Matrix::CreateTranslation(position);

			CopyDirectXMatrixToPxTransform(dxTransform, pxTransform);

			pxBody->setGlobalPose(pxTransform);
			dynamicBody->SetConvertScale(scale, mPhysics, collisionMatrix);
			dynamicBody->ChangeLayerNumber(rigidBodyData.myLayerNumber, collisionMatrix, mCollisionDataManager);

			return true;
		}
		std::shared_ptr<StaticRigidBody> staticBody = std::dynamic_pointer_cast<StaticRigidBody>(body);
		if (staticBody)
		{
			physx::PxRigidStatic* pxBody = staticBody->GetPxRigidStatic();
			DirectX::SimpleMath::Matrix dxTransform = rigidBodyData.transform;
			physx::PxTransform pxPrevTransform = pxBody->getGlobalPose();
			physx::PxTransform pxCurrentTransform;

			DirectX::SimpleMath::Vector3 position;
			DirectX::SimpleMath::Vector3 scale;
			DirectX::SimpleMath::Quaternion rotation;

			dxTransform.Decompose(scale, rotation, position);
			dxTransform = Matrix::CreateScale(1.f)
				* Matrix::CreateFromQuaternion(rotation)
				* staticBody->GetOffsetRotation().Invert()
				* Matrix::CreateTranslation(position)
				* staticBody->GetOffsetTranslation().Invert();

			CopyDirectXMatrixToPxTransform(dxTransform, pxCurrentTransform);

			if (areTransformsDifferent(pxPrevTransform, pxCurrentTransform, 0.001f, 0.001f))
			{
				pxBody->setGlobalPose(pxCurrentTransform);
			}
			staticBody->SetConvertScale(scale, mPhysics, collisionMatrix);
			staticBody->ChangeLayerNumber(rigidBodyData.myLayerNumber, collisionMatrix, mCollisionDataManager);

			return true;
		}

		return false;
	}
#pragma endregion

#pragma region CreateDynamicAndStaticBody
	bool PhysicsRigidBodyManager::CreateStaticBody(const BoxColliderInfo& info, const EColliderType& colliderType, int* collisionMatrix)
	{
		physx::PxMaterial* pxMaterial = mPhysics->createMaterial(info.colliderInfo.staticFriction, info.colliderInfo.dynamicFriction, info.colliderInfo.restitution);
		physx::PxShape* shape = mPhysics->createShape(physx::PxBoxGeometry(info.boxExtent.x, info.boxExtent.y, info.boxExtent.z), *pxMaterial);

		StaticRigidBody* rigidBody = SettingStaticBody(shape, info.colliderInfo, colliderType, collisionMatrix);

		shape->release();

		if (rigidBody != nullptr)
		{
			rigidBody->SetExtent(info.boxExtent.x, info.boxExtent.y, info.boxExtent.z);
			return true;
		}

		return false;
	}

	bool PhysicsRigidBodyManager::CreateStaticBody(const SphereColliderInfo& info, const EColliderType& colliderType, int* collisionMatrix)
	{
		physx::PxMaterial* pxMaterial = mPhysics->createMaterial(info.colliderInfo.staticFriction, info.colliderInfo.dynamicFriction, info.colliderInfo.restitution);
		physx::PxShape* shape = mPhysics->createShape(physx::PxSphereGeometry(info.raidus), *pxMaterial);

		StaticRigidBody* rigidBody = SettingStaticBody(shape, info.colliderInfo, colliderType, collisionMatrix);

		shape->release();

		if (rigidBody != nullptr)
		{
			rigidBody->SetRadius(info.raidus);
			return true;
		}


		return false;
	}

	bool PhysicsRigidBodyManager::CreateStaticBody(const CapsuleColliderInfo& info, const EColliderType& colliderType, int* collisionMatrix)
	{
		physx::PxMaterial* pxMaterial = mPhysics->createMaterial(info.colliderInfo.staticFriction, info.colliderInfo.dynamicFriction, info.colliderInfo.restitution);
		physx::PxShape* shape = mPhysics->createShape(physx::PxCapsuleGeometry(info.raidus, info.halfHeight), *pxMaterial);

		StaticRigidBody* rigidBody = SettingStaticBody(shape, info.colliderInfo, colliderType, collisionMatrix);
		
		shape->release();

		if (rigidBody != nullptr)
		{
			rigidBody->SetRadius(info.raidus);
			rigidBody->SetHalfHeight(info.halfHeight);
			return true;
		}

		return false;
	}

	bool PhysicsRigidBodyManager::CreateStaticBody(const ConvexMeshColliderInfo& info, const EColliderType& colliderType, int* collisionMatrix)
	{
		std::weak_ptr<ConvexMeshResource> convexMesh = mResourceManager.lock()->Create<ConvexMeshResource>(info.convexMeshHash, info.vertices, info.vertexSize, info.convexPolygonLimit);
		physx::PxConvexMesh* pxConvexMesh = convexMesh.lock()->GetConvexMesh();

		physx::PxMaterial* pxMaterial = mPhysics->createMaterial(info.colliderInfo.staticFriction, info.colliderInfo.dynamicFriction, info.colliderInfo.restitution);
		physx::PxShape* shape = mPhysics->createShape(physx::PxConvexMeshGeometry(pxConvexMesh), *pxMaterial);

		StaticRigidBody* rigidBody = SettingStaticBody(shape, info.colliderInfo, colliderType, collisionMatrix);

		shape->release();

		if (rigidBody != nullptr)
		{
			return true;
		}

		return false;
	}

	bool PhysicsRigidBodyManager::CreateStaticBody(const TriangleMeshColliderInfo& info, const EColliderType& colliderType, int* collisionMatrix)
	{
		std::weak_ptr<TriangleMeshResource> triangleMesh = mResourceManager.lock()->Create<TriangleMeshResource>(info.triangleMeshHash, info.vertices, info.vertexSize, info.indices, info.indexSize);
		physx::PxTriangleMesh* pxTriangleMesh = triangleMesh.lock()->GetTriangleMesh();

		physx::PxMaterial* pxMaterial = mPhysics->createMaterial(info.colliderInfo.staticFriction, info.colliderInfo.dynamicFriction, info.colliderInfo.restitution);
		physx::PxShape* shape = mPhysics->createShape(physx::PxTriangleMeshGeometry(pxTriangleMesh, physx::PxMeshScale(), physx::PxMeshGeometryFlag::eDOUBLE_SIDED), *pxMaterial);

		StaticRigidBody* rigidBody = SettingStaticBody(shape, info.colliderInfo, colliderType, collisionMatrix, true);

		shape->release();

		if (rigidBody != nullptr)
		{
			return true;
		}

		return false;
	}

	bool PhysicsRigidBodyManager::CreateStaticBody(const HeightFieldColliderInfo& info, const EColliderType& colliderType, int* collisionMatrix)
	{
		std::weak_ptr<HeightFieldResource> heightField = mResourceManager.lock()->Create<HeightFieldResource>(info.heightFieldMeshHash, info.height, info.numCols, info.numRows);
		physx::PxHeightField* pxHeightField = heightField.lock()->GetHeightField();

		physx::PxMaterial* pxMaterial = mPhysics->createMaterial(info.colliderInfo.staticFriction, info.colliderInfo.dynamicFriction, info.colliderInfo.restitution);
		physx::PxShape* shape = mPhysics->createShape(physx::PxHeightFieldGeometry(pxHeightField, physx::PxMeshGeometryFlag::eDOUBLE_SIDED, info.heightScale, info.rowScale, info.colScale), *pxMaterial);

		StaticRigidBody* rigidBody = SettingStaticBody(shape, info.colliderInfo, colliderType, collisionMatrix);
		rigidBody->SetOffsetRotation(DirectX::SimpleMath::Matrix::CreateRotationZ(180.f / 180.f * 3.14f));
		rigidBody->SetOffsetTranslation(
			DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(info.rowScale * info.numRows * 0.5f, 0.f, -info.colScale * info.numCols * 0.5f)));

		shape->release();

		if (rigidBody != nullptr)
		{
			return true;
		}

		return false;
	}

	bool PhysicsRigidBodyManager::CreateDynamicBody(const BoxColliderInfo& info, const EColliderType& colliderType, int* collisionMatrix, bool isKinematic)
	{
		physx::PxMaterial* pxMaterial = mPhysics->createMaterial(info.colliderInfo.staticFriction, info.colliderInfo.dynamicFriction, info.colliderInfo.restitution);
		physx::PxShape* shape = mPhysics->createShape(physx::PxBoxGeometry(info.boxExtent.x, info.boxExtent.y, info.boxExtent.z), *pxMaterial);

		DynamicRigidBody* rigidBody = SettingDynamicBody(shape, info.colliderInfo, colliderType, collisionMatrix, isKinematic);

		shape->release();

		if (rigidBody != nullptr)
		{
			rigidBody->SetExtent(info.boxExtent.x, info.boxExtent.y, info.boxExtent.z);
			return true;
		}


		return false;
	}

	bool PhysicsRigidBodyManager::CreateDynamicBody(const SphereColliderInfo& info, const EColliderType& colliderType, int* collisionMatrix, bool isKinematic)
	{
		physx::PxMaterial* pxMaterial = mPhysics->createMaterial(info.colliderInfo.staticFriction, info.colliderInfo.dynamicFriction, info.colliderInfo.restitution);
		physx::PxShape* shape = mPhysics->createShape(physx::PxSphereGeometry(info.raidus), *pxMaterial);

		DynamicRigidBody* rigidBody = SettingDynamicBody(shape, info.colliderInfo, colliderType, collisionMatrix, isKinematic);

		shape->release();

		if (rigidBody != nullptr)
		{
			rigidBody->SetRadius(info.raidus);
			return true;
		}

		return false;
	}

	bool PhysicsRigidBodyManager::CreateDynamicBody(const CapsuleColliderInfo& info, const EColliderType& colliderType, int* collisionMatrix, bool isKinematic)
	{
		physx::PxMaterial* pxMaterial = mPhysics->createMaterial(info.colliderInfo.staticFriction, info.colliderInfo.dynamicFriction, info.colliderInfo.restitution);
		physx::PxShape* shape = mPhysics->createShape(physx::PxCapsuleGeometry(info.raidus, info.halfHeight), *pxMaterial);

		DynamicRigidBody* rigidBody = SettingDynamicBody(shape, info.colliderInfo, colliderType, collisionMatrix, isKinematic);

		shape->release();

		if (rigidBody != nullptr)
		{
			rigidBody->SetRadius(info.raidus);
			rigidBody->SetHalfHeight(info.halfHeight);
			return true;
		}

		return false;
	}

	bool PhysicsRigidBodyManager::CreateDynamicBody(const ConvexMeshColliderInfo& info, const EColliderType& colliderType, int* collisionMatrix, bool isKinematic)
	{
		static int number = 0;
		number++;

		std::weak_ptr<ConvexMeshResource> convexMesh = mResourceManager.lock()->Create<ConvexMeshResource>(info.convexMeshHash + number, info.vertices, info.vertexSize, info.convexPolygonLimit);
		physx::PxConvexMesh* pxConvexMesh = convexMesh.lock()->GetConvexMesh();

		physx::PxMaterial* pxMaterial = mPhysics->createMaterial(info.colliderInfo.staticFriction, info.colliderInfo.dynamicFriction, info.colliderInfo.restitution);
		physx::PxShape* shape = mPhysics->createShape(physx::PxConvexMeshGeometry(pxConvexMesh), *pxMaterial);

		DynamicRigidBody* rigidBody = SettingDynamicBody(shape, info.colliderInfo, colliderType, collisionMatrix, isKinematic);

		shape->release();

		if (rigidBody != nullptr)
		{
			return true;
		}

		return false;
	}

	bool PhysicsRigidBodyManager::CreateDynamicBody(const TriangleMeshColliderInfo& info, const EColliderType& colliderType, int* collisionMatrix, bool isKinematic)
	{
		std::weak_ptr<TriangleMeshResource> triangleMesh = mResourceManager.lock()->Create<TriangleMeshResource>(info.triangleMeshHash, info.vertices, info.vertexSize, info.indices, info.indexSize);
		physx::PxTriangleMesh* pxTriangleMesh = triangleMesh.lock()->GetTriangleMesh();

		physx::PxMaterial* pxMaterial = mPhysics->createMaterial(info.colliderInfo.staticFriction, info.colliderInfo.dynamicFriction, info.colliderInfo.restitution);
		physx::PxShape* shape = mPhysics->createShape(physx::PxTriangleMeshGeometry(pxTriangleMesh, physx::PxMeshScale(), physx::PxMeshGeometryFlag::eDOUBLE_SIDED), *pxMaterial);

		DynamicRigidBody* rigidBody = SettingDynamicBody(shape, info.colliderInfo, colliderType, collisionMatrix, isKinematic, true);

		shape->release();

		if (rigidBody != nullptr)
		{
			return true;
		}

		return false;
	}

	bool PhysicsRigidBodyManager::CreateDynamicBody(const HeightFieldColliderInfo& info, const EColliderType& colliderType, int* collisionMatrix, bool isKinematic)
	{
		std::weak_ptr<HeightFieldResource> heightField = mResourceManager.lock()->Create<HeightFieldResource>(info.heightFieldMeshHash, info.height, info.numCols, info.numRows);
		physx::PxHeightField* pxHeightField = heightField.lock()->GetHeightField();

		physx::PxMaterial* pxMaterial = mPhysics->createMaterial(info.colliderInfo.staticFriction, info.colliderInfo.dynamicFriction, info.colliderInfo.restitution);
		physx::PxShape* shape = mPhysics->createShape(physx::PxHeightFieldGeometry(pxHeightField, physx::PxMeshGeometryFlag::eDOUBLE_SIDED, info.heightScale , info.rowScale, info.colScale), *pxMaterial);

		DynamicRigidBody* rigidBody = SettingDynamicBody(shape, info.colliderInfo, colliderType, collisionMatrix, isKinematic);
		rigidBody->SetOffsetRotation(DirectX::SimpleMath::Matrix::CreateRotationZ(180.f / 180.f * 3.14f));
		rigidBody->SetOffsetTranslation(
			DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(info.rowScale * info.numRows * 0.5f, 0.f, -info.colScale * info.numCols * 0.5f)));

		shape->release();

		if (rigidBody != nullptr)
		{
			return true;
		}

		return false;
	}

	StaticRigidBody* PhysicsRigidBodyManager::SettingStaticBody(
		physx::PxShape* shape, 
		const ColliderInfo& info,
		const EColliderType& colliderType, 
		int* collisionMatrix, 
		bool isGpuScene)
	{
		physx::PxFilterData filterdata;
		filterdata.word0 = info.layerNumber;
		filterdata.word1 = collisionMatrix[info.layerNumber];
		shape->setSimulationFilterData(filterdata);

		std::shared_ptr<StaticRigidBody> staticBody = std::make_shared<StaticRigidBody>(colliderType, info.id, info.layerNumber);
		std::shared_ptr<CollisionData> collisiondata = std::make_shared<CollisionData>();

		if (!staticBody->Initialize(info, shape, mPhysics, collisiondata))
		{
			return nullptr;
		}

		mCollisionDataManager.lock()->Create(info.id, collisiondata);
		mRigidBodyContainer.insert(std::make_pair(staticBody->GetID(), staticBody));

		if (isGpuScene)
			mUpcomingGpuActors.push_back(staticBody);
		else
			mUpcomingActors.push_back(staticBody);

		return staticBody.get();
	}

	DynamicRigidBody* PhysicsRigidBodyManager::SettingDynamicBody(
		physx::PxShape* shape
		, const ColliderInfo& info
		, const EColliderType& colliderType
		, int* collisionMatrix
		, bool isKinematic
		, bool isGpuScene)
	{
		physx::PxFilterData filterdata;
		filterdata.word0 = info.layerNumber;
		filterdata.word1 = collisionMatrix[info.layerNumber];
		shape->setSimulationFilterData(filterdata);

		std::shared_ptr<DynamicRigidBody> dynamicBody = std::make_shared<DynamicRigidBody>(colliderType, info.id, info.layerNumber);
		std::shared_ptr<CollisionData> collisiondata = std::make_shared<CollisionData>();

		if (!dynamicBody->Initialize(info, shape, mPhysics, collisiondata, isKinematic)) return nullptr;
		         
		mCollisionDataManager.lock()->Create(info.id, collisiondata);
		mRigidBodyContainer.insert(std::make_pair(dynamicBody->GetID(), dynamicBody));

		if (isGpuScene)
			mUpcomingGpuActors.push_back(dynamicBody);
		else
			mUpcomingActors.push_back(dynamicBody);

		return dynamicBody.get();
	}
#pragma endregion

#pragma region RemoveRigidBody 
	bool PhysicsRigidBodyManager::RemoveRigidBody(const unsigned int& id, physx::PxScene* scene, std::vector<physx::PxActor*>& removeActorList)
	{
		if (mRigidBodyContainer.find(id) == mRigidBodyContainer.end())
			return false;

		std::shared_ptr<RigidBody> body = mRigidBodyContainer.find(id)->second;

		if (body)
		{
			std::shared_ptr<DynamicRigidBody> dynamicBody = std::dynamic_pointer_cast<DynamicRigidBody>(body);
			if (dynamicBody)
			{
				if (dynamicBody->GetPxRigidDynamic()->getScene() == scene)
				{
					removeActorList.push_back(dynamicBody->GetPxRigidDynamic());
					mRigidBodyContainer.erase(mRigidBodyContainer.find(id));
				}
			}
			std::shared_ptr<StaticRigidBody> staticBody = std::dynamic_pointer_cast<StaticRigidBody>(body);
			if (staticBody)
			{
				if (staticBody->GetPxRigidStatic()->getScene() == scene)
				{
					removeActorList.push_back(staticBody->GetPxRigidStatic());
					mRigidBodyContainer.erase(mRigidBodyContainer.find(id));
				}
			}
		}

		auto bodyIter = mUpcomingActors.begin();
		for (bodyIter; bodyIter != mUpcomingActors.end(); bodyIter++)
		{
			if ((*bodyIter)->GetID() == id)
			{
				mUpcomingActors.erase(bodyIter);
				break;
			}
		}

		return true;
	}

	bool PhysicsRigidBodyManager::RemoveAllRigidBody(physx::PxScene* scene, std::vector<physx::PxActor*>& removeActorList)
	{
		for (const auto& body : mRigidBodyContainer)
		{
			std::shared_ptr<DynamicRigidBody> dynamicBody = std::dynamic_pointer_cast<DynamicRigidBody>(body.second);
			if (dynamicBody)
			{
				if (dynamicBody->GetPxRigidDynamic()->getScene() == scene)
				{
					removeActorList.push_back(dynamicBody->GetPxRigidDynamic());
					continue;
				}
			}
			std::shared_ptr<StaticRigidBody> staticBody = std::dynamic_pointer_cast<StaticRigidBody>(body.second);
			if (staticBody)
			{
				if (staticBody->GetPxRigidStatic()->getScene() == scene)
				{
					removeActorList.push_back(staticBody->GetPxRigidStatic());
					continue;
				}
			}
		}
		mRigidBodyContainer.clear();

		for (const auto& body : mUpcomingActors)
		{
			std::shared_ptr<DynamicRigidBody> dynamicBody = std::dynamic_pointer_cast<DynamicRigidBody>(body);
			if (dynamicBody)
			{
				if (dynamicBody->GetPxRigidDynamic()->getScene() == scene)
				{
					removeActorList.push_back(dynamicBody->GetPxRigidDynamic());
					continue;
				}
			}
			std::shared_ptr<StaticRigidBody> staticBody = std::dynamic_pointer_cast<StaticRigidBody>(body);
			if (staticBody)
			{
				if (staticBody->GetPxRigidStatic()->getScene() == scene)
				{
					removeActorList.push_back(staticBody->GetPxRigidStatic());
					continue;
				}
			}
		}
		mUpcomingActors.clear();

		return true;
	}
#pragma endregion

#pragma region UpdateCollisionMatrix
	void PhysicsRigidBodyManager::UpdateCollisionMatrix(int* collisionMatrix)
	{
		for (const auto& body : mRigidBodyContainer)
		{
			std::shared_ptr<DynamicRigidBody> dynamicBody = std::dynamic_pointer_cast<DynamicRigidBody>(body.second);
			if (dynamicBody)
			{
				physx::PxShape* shape;
				dynamicBody->GetPxRigidDynamic()->getShapes(&shape, 1);
				physx::PxFilterData filterData;

				filterData.word0 = dynamicBody->GetLayerNumber();
				filterData.word1 = collisionMatrix[dynamicBody->GetLayerNumber()];
				shape->setSimulationFilterData(filterData);

				continue;
			}
			std::shared_ptr<StaticRigidBody> staticBody = std::dynamic_pointer_cast<StaticRigidBody>(body.second);
			if (staticBody)
			{
				physx::PxShape* shape;
				staticBody->GetPxRigidStatic()->getShapes(&shape, 1);
				physx::PxFilterData filterData;

				filterData.word0 = staticBody->GetLayerNumber();
				filterData.word1 = collisionMatrix[staticBody->GetLayerNumber()];
				shape->setSimulationFilterData(filterData);

				continue;
			}
		}
	}
#pragma endregion

#pragma region DebugData
	void PhysicsRigidBodyManager::ExtractDebugData()
	{
		for (auto& bodyIter : mRigidBodyContainer)
		{
			mDebugData->UpdateDebugData(bodyIter.second);
		}
	}

	const std::unordered_map<unsigned int, PolygonMesh>& PhysicsRigidBodyManager::GetDebugPolygon()
	{
		return mDebugData->GetDebugPolygon();
	}
	const std::unordered_map<unsigned int, std::vector<unsigned int>>& PhysicsRigidBodyManager::GetDebugTriangleIndiecs()
	{
		return mDebugData->GetDebugTriangleIndiecs();
	}
	const std::unordered_map<unsigned int, std::vector<DirectX::SimpleMath::Vector3>>& PhysicsRigidBodyManager::GetDebugTriangleVertices()
	{
		return mDebugData->GetDebugTriangleVertices();
	}
	const std::unordered_map<unsigned int, std::vector<std::pair<DirectX::SimpleMath::Vector3, DirectX::SimpleMath::Vector3>>>& PhysicsRigidBodyManager::GetDebugHeightField()
	{
		return mDebugData->GetDebugHeightField();
	}
#pragma endregion

}