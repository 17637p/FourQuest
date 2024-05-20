#include "PhysicsRigidBodyManager.h"

#include "PhysicsCookingMeshTool.h"
#include "RigidBody.h"
#include "DynamicRigidBody.h"
#include "StaticRigidBody.h"
#include "EngineDataConverter.h"

namespace fq::physics
{
	PhysicsRigidBodyManager::PhysicsRigidBodyManager()
		: mCookingMeshTool(std::make_shared<PhysicsCookingMeshTool>())
		, mPhysics(nullptr)
	{
	}

	PhysicsRigidBodyManager::~PhysicsRigidBodyManager()
	{
		mRigidBodyContainer.clear();
		mUpcomingActors.clear();
		mDebugPolygon.clear();
		mCollisionDataContainer.clear();
	}

	bool PhysicsRigidBodyManager::Initialize(physx::PxPhysics* physics)
	{
		mPhysics = physics;

		if (!mCookingMeshTool->Initialize(physics)) 
			return false;

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
		UserDataClear();

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
			CopyPxTransformToDirectXMatrix(pxBody->getGlobalPose(), rigidBodyData.transform);
			CopyPxVec3ToDxVec3(pxBody->getLinearVelocity(), rigidBodyData.linearVelocity);
			CopyPxVec3ToDxVec3(pxBody->getAngularVelocity(), rigidBodyData.angularVelocity);

		}
		std::shared_ptr<StaticRigidBody> staticBody = std::dynamic_pointer_cast<StaticRigidBody>(body);
		if (staticBody)
		{
			physx::PxRigidStatic* pxBody = staticBody->GetPxRigidStatic();
			CopyPxTransformToDirectXMatrix(pxBody->getGlobalPose(), rigidBodyData.transform);
		}
	}

	bool PhysicsRigidBodyManager::SetRigidBodyData(const unsigned int& id, const RigidBodyGetSetData& rigidBodyData)
	{
		auto body = mRigidBodyContainer.find(id)->second;

		std::shared_ptr<DynamicRigidBody> dynamicBody = std::dynamic_pointer_cast<DynamicRigidBody>(body);
		if (dynamicBody)
		{
			physx::PxTransform pxTransform;
			physx::PxVec3 pxLinearVelocity;
			physx::PxVec3 pxangularVelocity;
			CopyDirectXMatrixToPxTransform(rigidBodyData.transform, pxTransform);
			CopyDxVec3ToPxVec3(rigidBodyData.linearVelocity, pxLinearVelocity);
			CopyDxVec3ToPxVec3(rigidBodyData.angularVelocity, pxangularVelocity);

			physx::PxRigidDynamic* pxBody = dynamicBody->GetPxRigidDynamic();
			pxBody->setGlobalPose(pxTransform);
			pxBody->setLinearVelocity(pxLinearVelocity);
			pxBody->setAngularVelocity(pxangularVelocity);

			return true;
		}
		std::shared_ptr<StaticRigidBody> staticBody = std::dynamic_pointer_cast<StaticRigidBody>(body);
		if (staticBody)
		{
			physx::PxTransform pxTransform;
			CopyDirectXMatrixToPxTransform(rigidBodyData.transform, pxTransform);

			physx::PxRigidStatic* pxBody = staticBody->GetPxRigidStatic();
			pxBody->setGlobalPose(pxTransform);

			return true;
		}

		return false;
	}

#pragma endregion

#pragma region CreateDynamicAndStaticBody
	bool PhysicsRigidBodyManager::CreateStaticBody(const BoxColliderInfo& info, const EColliderType& colliderType, int* collisionMatrix)
	{
		physx::PxMaterial* material = mPhysics->createMaterial(info.colliderInfo.staticFriction, info.colliderInfo.dynamicFriction, info.colliderInfo.restitution);
		physx::PxShape* shape = mPhysics->createShape(physx::PxBoxGeometry(info.boxExtent.x, info.boxExtent.y, info.boxExtent.z), *material);

		if (!SettingStaticBody(shape, info.colliderInfo, colliderType, collisionMatrix)) return false;

		return true;
	}

	bool PhysicsRigidBodyManager::CreateStaticBody(const SphereColliderInfo& info, const EColliderType& colliderType, int* collisionMatrix)
	{
		physx::PxMaterial* material = mPhysics->createMaterial(info.colliderInfo.staticFriction, info.colliderInfo.dynamicFriction, info.colliderInfo.restitution);
		physx::PxShape* shape = mPhysics->createShape(physx::PxSphereGeometry(info.raidus), *material);

		if (!SettingStaticBody(shape, info.colliderInfo, colliderType, collisionMatrix)) return false;

		return true;
	}

	bool PhysicsRigidBodyManager::CreateStaticBody(const CapsuleColliderInfo& info, const EColliderType& colliderType, int* collisionMatrix)
	{
		physx::PxMaterial* material = mPhysics->createMaterial(info.colliderInfo.staticFriction, info.colliderInfo.dynamicFriction, info.colliderInfo.restitution);
		physx::PxShape* shape = mPhysics->createShape(physx::PxCapsuleGeometry(info.raidus, info.halfHeight), *material);

		if (!SettingStaticBody(shape, info.colliderInfo, colliderType, collisionMatrix)) return false;

		return true;
	}

	bool PhysicsRigidBodyManager::CreateStaticBody(const ConvexMeshColliderInfo& info, const EColliderType& colliderType, int* collisionMatrix)
	{
		physx::PxConvexMesh* mesh = mCookingMeshTool->CookingConvexMesh(info.vertices, info.vertexSize, info.convexPolygonLimit);
		physx::PxMaterial* material = mPhysics->createMaterial(info.colliderInfo.staticFriction, info.colliderInfo.dynamicFriction, info.colliderInfo.restitution);
		physx::PxShape* shape = mPhysics->createShape(physx::PxConvexMeshGeometry(mesh), *material);

		if (!SettingStaticBody(shape, info.colliderInfo, colliderType, collisionMatrix)) return false;

		return true;
	}

	bool PhysicsRigidBodyManager::CreateDynamicBody(const BoxColliderInfo& info, const EColliderType& colliderType, int* collisionMatrix)
	{
		physx::PxMaterial* material = mPhysics->createMaterial(info.colliderInfo.staticFriction, info.colliderInfo.dynamicFriction, info.colliderInfo.restitution);
		physx::PxShape* shape = mPhysics->createShape(physx::PxBoxGeometry(info.boxExtent.x, info.boxExtent.y, info.boxExtent.z), *material);

		if (!SettingDynamicBody(shape, info.colliderInfo, colliderType, collisionMatrix)) return false;

		return true;
	}

	bool PhysicsRigidBodyManager::CreateDynamicBody(const SphereColliderInfo& info, const EColliderType& colliderType, int* collisionMatrix)
	{
		physx::PxMaterial* material = mPhysics->createMaterial(info.colliderInfo.staticFriction, info.colliderInfo.dynamicFriction, info.colliderInfo.restitution);
		physx::PxShape* shape = mPhysics->createShape(physx::PxSphereGeometry(info.raidus), *material);

		if (!SettingDynamicBody(shape, info.colliderInfo, colliderType, collisionMatrix)) return false;

		return true;
	}

	bool PhysicsRigidBodyManager::CreateDynamicBody(const CapsuleColliderInfo& info, const EColliderType& colliderType, int* collisionMatrix)
	{
		physx::PxMaterial* material = mPhysics->createMaterial(info.colliderInfo.staticFriction, info.colliderInfo.dynamicFriction, info.colliderInfo.restitution);
		physx::PxShape* shape = mPhysics->createShape(physx::PxCapsuleGeometry(info.raidus, info.halfHeight), *material);

		if (!SettingDynamicBody(shape, info.colliderInfo, colliderType, collisionMatrix)) return false;

		return true;
	}

	bool PhysicsRigidBodyManager::CreateDynamicBody(const ConvexMeshColliderInfo& info, const EColliderType& colliderType, int* collisionMatrix)
	{
		physx::PxConvexMesh* mesh = mCookingMeshTool->CookingConvexMesh(info.vertices, info.vertexSize, 255);
		physx::PxMaterial* material = mPhysics->createMaterial(info.colliderInfo.staticFriction, info.colliderInfo.dynamicFriction, info.colliderInfo.restitution);
		physx::PxShape* shape = mPhysics->createShape(physx::PxConvexMeshGeometry(mesh), *material);

		if (!SettingDynamicBody(shape, info.colliderInfo, colliderType, collisionMatrix)) return false;

		return true;
	}

	bool PhysicsRigidBodyManager::SettingStaticBody(physx::PxShape* shape, const ColliderInfo& info, const EColliderType& colliderType, int* collisionMatrix)
	{
		physx::PxFilterData filterdata;
		filterdata.word0 = info.layerNumber;
		filterdata.word1 = collisionMatrix[info.layerNumber];
		shape->setSimulationFilterData(filterdata);

		std::shared_ptr<StaticRigidBody> dynamicBody = std::make_shared<StaticRigidBody>(colliderType, info.id, info.layerNumber);
		std::shared_ptr<CollisionData> collisiondata = std::make_shared<CollisionData>();

		if (!dynamicBody->Initialize(info, shape, mPhysics, collisiondata)) return false;

		mCollisionDataContainer.insert(std::make_pair(info.id, collisiondata));
		mRigidBodyContainer.insert(std::make_pair(dynamicBody->GetID(), dynamicBody));
		mUpcomingActors.push_back(dynamicBody);

		return true;
	}

	bool PhysicsRigidBodyManager::SettingDynamicBody(physx::PxShape* shape, const ColliderInfo& info, const EColliderType& colliderType, int* collisionMatrix)
	{
		physx::PxFilterData filterdata;
		filterdata.word0 = info.layerNumber;
		filterdata.word1 = collisionMatrix[info.layerNumber];
		shape->setSimulationFilterData(filterdata);

		std::shared_ptr<DynamicRigidBody> dynamicBody = std::make_shared<DynamicRigidBody>(colliderType, info.id, info.layerNumber);
		std::shared_ptr<CollisionData> collisiondata = std::make_shared<CollisionData>();

		if (!dynamicBody->Initialize(info, shape, mPhysics, collisiondata)) return false;

		mCollisionDataContainer.insert(std::make_pair(info.id, collisiondata));
		mRigidBodyContainer.insert(std::make_pair(dynamicBody->GetID(), dynamicBody));
		mUpcomingActors.push_back(dynamicBody);

		return true;
	}
#pragma endregion

#pragma region RemoveRigidBody
	bool PhysicsRigidBodyManager::RemoveRigidBody(const unsigned int& id, physx::PxScene* scene)
	{
		if (mRigidBodyContainer.find(id) == mRigidBodyContainer.end())
			return false;

		std::shared_ptr<RigidBody> body = mRigidBodyContainer.find(id)->second;

		if (body)
		{
			std::shared_ptr<DynamicRigidBody> dynamicBody = std::dynamic_pointer_cast<DynamicRigidBody>(body);
			if (dynamicBody)
			{
				scene->removeActor(*dynamicBody->GetPxRigidDynamic());
				mRigidBodyContainer.erase(mRigidBodyContainer.find(id));
				mCollisionDataContainer.find(id)->second->isDead = true;
			}
			std::shared_ptr<StaticRigidBody> staticBody = std::dynamic_pointer_cast<StaticRigidBody>(body);
			if (staticBody)
			{
				scene->removeActor(*staticBody->GetPxRigidStatic());
				mRigidBodyContainer.erase(mRigidBodyContainer.find(id));
				mCollisionDataContainer.find(id)->second->isDead = true;
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

	bool PhysicsRigidBodyManager::RemoveAllRigidBody(physx::PxScene* scene)
	{
		for (const auto& body : mRigidBodyContainer)
		{
			std::shared_ptr<DynamicRigidBody> dynamicBody = std::dynamic_pointer_cast<DynamicRigidBody>(body.second);
			if (dynamicBody)
			{
				scene->removeActor(*dynamicBody->GetPxRigidDynamic());
				continue;
			}
			std::shared_ptr<StaticRigidBody> staticBody = std::dynamic_pointer_cast<StaticRigidBody>(body.second);
			if (staticBody)
			{
				scene->removeActor(*staticBody->GetPxRigidStatic());
				continue;
			}
		}
		mRigidBodyContainer.clear();

		for (const auto& body : mUpcomingActors)
		{
			std::shared_ptr<DynamicRigidBody> dynamicBody = std::dynamic_pointer_cast<DynamicRigidBody>(body);
			if (dynamicBody)
			{
				scene->removeActor(*dynamicBody->GetPxRigidDynamic());
				continue;
			}
			std::shared_ptr<StaticRigidBody> staticBody = std::dynamic_pointer_cast<StaticRigidBody>(body);
			if (staticBody)
			{
				scene->removeActor(*staticBody->GetPxRigidStatic());
				continue;
			}
		}
		mUpcomingActors.clear();

		for (auto& data : mCollisionDataContainer)
		{
			data.second->isDead = true;
		}

		return true;
	}

	void PhysicsRigidBodyManager::UserDataClear()
	{
		auto dataIter = mCollisionDataContainer.begin();
		std::vector<std::unordered_map<unsigned int, std::shared_ptr<CollisionData>>::iterator> iterContainer;

		for (; dataIter != mCollisionDataContainer.end(); dataIter++)
		{
			if (dataIter->second->isDead == true)
				iterContainer.push_back(dataIter);
		}

		for (auto& deleteIter : iterContainer)
		{
			mCollisionDataContainer.erase(deleteIter);
		}
		iterContainer.clear();
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
				actor->getShapes(&shape, sizeof(physx::PxShape));

				if (shape->getGeometry().getType() == physx::PxGeometryType::eCONVEXMESH)
				{
					shared_ptr<vector<vector<DirectX::SimpleMath::Vector3>>> polygon = make_shared<vector<vector<DirectX::SimpleMath::Vector3>>>();
					ExtractDebugConvexMesh(actor, shape, *polygon.get());

					mDebugPolygon.insert(std::make_pair(dynamicBody->GetID(), polygon));
				}
			}
			std::shared_ptr<StaticRigidBody> staticBody = dynamic_pointer_cast<StaticRigidBody>(iter.second);
			if (staticBody)
			{
				physx::PxShape* shape;
				physx::PxRigidActor* actor = staticBody->GetPxRigidStatic();
				actor->getShapes(&shape, sizeof(physx::PxShape));

				if (shape->getGeometry().getType() == physx::PxGeometryType::eCONVEXMESH)
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