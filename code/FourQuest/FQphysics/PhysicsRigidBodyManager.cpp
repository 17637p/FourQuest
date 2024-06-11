#include "PhysicsRigidBodyManager.h"

#include "PhysicsResourceManager.h"
#include "PhysicsCollisionDataManager.h"
#include "RigidBody.h"
#include "DynamicRigidBody.h"
#include "StaticRigidBody.h"
#include "EngineDataConverter.h"
#include "ConvexMeshResource.h"

#include "PhysicsCookingMeshTool.h"

namespace fq::physics
{
	PhysicsRigidBodyManager::PhysicsRigidBodyManager()
		: mPhysics(nullptr)
		, mResourceManager()
		, mCollisionDataManager()
		, mRigidBodyContainer()
		, mUpcomingActors()
		, mDebugPolygon()
	{
	}

	PhysicsRigidBodyManager::~PhysicsRigidBodyManager()
	{
		mRigidBodyContainer.clear();
		mUpcomingActors.clear();
		mDebugPolygon.clear();
	}

	bool PhysicsRigidBodyManager::Initialize(physx::PxPhysics* physics, std::shared_ptr<PhysicsResourceManager> resourceManager, std::shared_ptr<PhysicsCollisionDataManager> collisionDataManager)
	{
		mPhysics = physics;
		mResourceManager = resourceManager;
		mCollisionDataManager = collisionDataManager;
		
		return true;
	}

	bool PhysicsRigidBodyManager::Update(physx::PxScene* scene, int* collisionMatrix)
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
		mUpcomingActors.clear();

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
			rigidBodyData.transform = DirectX::SimpleMath::Matrix::CreateScale(dynamicBody->GetScale()) * dxMatrix;
			CopyPxVec3ToDxVec3(pxBody->getLinearVelocity(), rigidBodyData.linearVelocity);
			CopyPxVec3ToDxVec3(pxBody->getAngularVelocity(), rigidBodyData.angularVelocity);
		}
		std::shared_ptr<StaticRigidBody> staticBody = std::dynamic_pointer_cast<StaticRigidBody>(body);
		if (staticBody)
		{
			physx::PxRigidStatic* pxBody = staticBody->GetPxRigidStatic();
			DirectX::SimpleMath::Matrix dxMatrix;
			CopyPxTransformToDirectXMatrix(pxBody->getGlobalPose(), dxMatrix);
			rigidBodyData.transform = DirectX::SimpleMath::Matrix::CreateScale(staticBody->GetScale()) * dxMatrix;
		}
	}

	bool PhysicsRigidBodyManager::SetRigidBodyData(const unsigned int& id, const RigidBodyGetSetData& rigidBodyData, int* collisionMatrix)
	{
		auto body = mRigidBodyContainer.find(id)->second;

		std::shared_ptr<DynamicRigidBody> dynamicBody = std::dynamic_pointer_cast<DynamicRigidBody>(body);
		if (dynamicBody)
		{
			DirectX::SimpleMath::Matrix dxTransform = rigidBodyData.transform;
			physx::PxTransform pxTransform; 
			physx::PxVec3 pxLinearVelocity;
			physx::PxVec3 pxangularVelocity;
			CopyDirectXMatrixToPxTransform(dxTransform, pxTransform);
			CopyDxVec3ToPxVec3(rigidBodyData.linearVelocity, pxLinearVelocity);
			CopyDxVec3ToPxVec3(rigidBodyData.angularVelocity, pxangularVelocity);

			physx::PxRigidDynamic* pxBody = dynamicBody->GetPxRigidDynamic();
			pxBody->setGlobalPose(pxTransform);
			pxBody->setLinearVelocity(pxLinearVelocity);
			pxBody->setAngularVelocity(pxangularVelocity);

			DirectX::SimpleMath::Vector3 position;
			DirectX::SimpleMath::Vector3 scale;
			DirectX::SimpleMath::Quaternion rotation;
			dxTransform.Decompose(scale, rotation, position);
			dynamicBody->SetConvertScale(scale, mPhysics, collisionMatrix);

			return true;
		}
		std::shared_ptr<StaticRigidBody> staticBody = std::dynamic_pointer_cast<StaticRigidBody>(body);
		if (staticBody)
		{
			DirectX::SimpleMath::Matrix dxTransform = rigidBodyData.transform;
			physx::PxTransform pxTransform;
			CopyDirectXMatrixToPxTransform(dxTransform, pxTransform);

			physx::PxRigidStatic* pxBody = staticBody->GetPxRigidStatic();
			pxBody->setGlobalPose(pxTransform);

			DirectX::SimpleMath::Vector3 position;
			DirectX::SimpleMath::Vector3 scale;
			DirectX::SimpleMath::Quaternion rotation;
			dxTransform.Decompose(scale, rotation, position);
			staticBody->SetConvertScale(scale, mPhysics, collisionMatrix);

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
		if (rigidBody != nullptr)
			return true;

		return false;
	}

	bool PhysicsRigidBodyManager::CreateDynamicBody(const BoxColliderInfo& info, const EColliderType& colliderType, int* collisionMatrix, bool isKinematic)
	{
		physx::PxMaterial* pxMaterial = mPhysics->createMaterial(info.colliderInfo.staticFriction, info.colliderInfo.dynamicFriction, info.colliderInfo.restitution);
		physx::PxShape* shape = mPhysics->createShape(physx::PxBoxGeometry(info.boxExtent.x, info.boxExtent.y, info.boxExtent.z), *pxMaterial);

		DynamicRigidBody* rigidBody = SettingDynamicBody(shape, info.colliderInfo, colliderType, collisionMatrix, isKinematic);
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
		std::weak_ptr<ConvexMeshResource> convexMesh = mResourceManager.lock()->Create<ConvexMeshResource>(info.convexMeshHash, info.vertices, info.vertexSize, info.convexPolygonLimit);
		physx::PxConvexMesh* pxConvexMesh = convexMesh.lock()->GetConvexMesh();

		physx::PxMaterial* pxMaterial = mPhysics->createMaterial(info.colliderInfo.staticFriction, info.colliderInfo.dynamicFriction, info.colliderInfo.restitution);
		physx::PxShape* shape = mPhysics->createShape(physx::PxConvexMeshGeometry(pxConvexMesh), *pxMaterial);

		DynamicRigidBody* rigidBody = SettingDynamicBody(shape, info.colliderInfo, colliderType, collisionMatrix, isKinematic);
		if (rigidBody != nullptr)
			return true;

		return false;
	}

	StaticRigidBody* PhysicsRigidBodyManager::SettingStaticBody(physx::PxShape* shape, const ColliderInfo& info, const EColliderType& colliderType, int* collisionMatrix)
	{
		physx::PxFilterData filterdata;
		filterdata.word0 = info.layerNumber;
		filterdata.word1 = collisionMatrix[info.layerNumber];
		shape->setSimulationFilterData(filterdata);

		std::shared_ptr<StaticRigidBody> staticBody = std::make_shared<StaticRigidBody>(colliderType, info.id, info.layerNumber);
		std::shared_ptr<CollisionData> collisiondata = std::make_shared<CollisionData>();

		if (!staticBody->Initialize(info, shape, mPhysics, collisiondata)) return nullptr;

		mCollisionDataManager.lock()->Create(info.id, collisiondata);
		mRigidBodyContainer.insert(std::make_pair(staticBody->GetID(), staticBody));
		mUpcomingActors.push_back(staticBody);

		return staticBody.get();
	}

	DynamicRigidBody* PhysicsRigidBodyManager::SettingDynamicBody(physx::PxShape* shape, const ColliderInfo& info, const EColliderType& colliderType, int* collisionMatrix, bool isKinematic)
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

#pragma region ExtractDebugData
	void PhysicsRigidBodyManager::ExtractDebugData()
	{
		using namespace std;

		mDebugPolygon.clear();

		for (const auto& iter : mRigidBodyContainer)
		{
			std::shared_ptr<DynamicRigidBody> dynamicBody = std::dynamic_pointer_cast<DynamicRigidBody>(iter.second);
			if (dynamicBody)
			{
				physx::PxShape* shape;
				physx::PxRigidActor* actor = dynamicBody->GetPxRigidDynamic();
				actor->getShapes(&shape, 1);

				if (shape != nullptr && shape->getGeometry().getType() == physx::PxGeometryType::eCONVEXMESH)
				{
					shared_ptr<vector<vector<DirectX::SimpleMath::Vector3>>> polygon = make_shared<vector<	vector<DirectX::SimpleMath::Vector3>>>();
					ExtractDebugConvexMesh(actor, shape, *polygon.get());

					mDebugPolygon.insert(std::make_pair(dynamicBody->GetID(), polygon));
				}
			}
			std::shared_ptr<StaticRigidBody> staticBody = dynamic_pointer_cast<StaticRigidBody>(iter.second);
			if (staticBody)
			{
				physx::PxShape* shape;
				physx::PxRigidActor* actor = staticBody->GetPxRigidStatic();
				actor->getShapes(&shape, 1);
				
				if (shape != nullptr && shape->getGeometry().getType() == physx::PxGeometryType::eCONVEXMESH)
				{
					shared_ptr<vector<vector<DirectX::SimpleMath::Vector3>>> polygon = make_shared<vector<vector<DirectX::SimpleMath::Vector3>>>();
					ExtractDebugConvexMesh(actor, shape, *polygon.get());

					mDebugPolygon.insert(std::make_pair(dynamicBody->GetID(), polygon));
				}
			}
		}
	}
#pragma endregion

}