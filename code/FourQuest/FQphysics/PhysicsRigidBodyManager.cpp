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
				mCollisionDataContainer.insert(std::make_pair(dynamicBody->GetID(), (CollisionData*)dynamicBody->GetPxRigidDynamic()->userData));
			}
			std::shared_ptr<StaticRigidBody> staticBody = std::dynamic_pointer_cast<StaticRigidBody>(body);
			if (staticBody)
			{
				scene->addActor(*staticBody->GetPxRigidStatic());
				mCollisionDataContainer.insert(std::make_pair(staticBody->GetID(), (CollisionData*)staticBody->GetPxRigidStatic()->userData));
			}
		}
		mUpcomingActors.clear();

		return true;
	}

	bool PhysicsRigidBodyManager::FinalUpdate(physx::PxScene* scene)
	{
#ifdef _DEBUG
		ExtractDebugData();
#endif
		UpdateUserData(scene);

		return true;
	}

#pragma region GetSetRigidBodyMatrix

	void PhysicsRigidBodyManager::GetRigidBodyMatrix(unsigned int id, DirectX::SimpleMath::Matrix& dxMatrix)
	{
		auto body = mRigidBodyContainer.find(id)->second;

		std::shared_ptr<DynamicRigidBody> dynamicBody = std::dynamic_pointer_cast<DynamicRigidBody>(body);
		if (dynamicBody)
		{
			physx::PxTransform pxTransform = dynamicBody->GetPxRigidDynamic()->getGlobalPose();
			CopyPxTransformToDirectXMatrix(pxTransform, dxMatrix);
		}
		std::shared_ptr<StaticRigidBody> staticBody = std::dynamic_pointer_cast<StaticRigidBody>(body);
		if (staticBody)
		{
			physx::PxTransform pxTransform = staticBody->GetPxRigidStatic()->getGlobalPose();
			CopyPxTransformToDirectXMatrix(pxTransform, dxMatrix);
		}
	}

	bool PhysicsRigidBodyManager::SetRigidBodyMatrix(const unsigned int& id, const DirectX::SimpleMath::Matrix& worldTransform)
	{
		auto body = mRigidBodyContainer.find(id)->second;

		std::shared_ptr<DynamicRigidBody> dynamicBody = std::dynamic_pointer_cast<DynamicRigidBody>(body);
		if (dynamicBody)
		{
			physx::PxTransform pxTransform;
			CopyDirectXMatrixToPxTransform(worldTransform, pxTransform);
			dynamicBody->GetPxRigidDynamic()->setGlobalPose(pxTransform);

			return true;
		}
		std::shared_ptr<StaticRigidBody> staticBody = std::dynamic_pointer_cast<StaticRigidBody>(body);
		if (staticBody)
		{
			physx::PxTransform pxTransform;
			CopyDirectXMatrixToPxTransform(worldTransform, pxTransform);
			staticBody->GetPxRigidStatic()->setGlobalPose(pxTransform);

			return true;
		}

		return false;
	}

	bool PhysicsRigidBodyManager::AddRigidBodyVelocity(const unsigned int& id, const DirectX::SimpleMath::Vector3& velocity)
	{
		auto body = mRigidBodyContainer.find(id)->second;

		std::shared_ptr<DynamicRigidBody> dynamicBody = std::dynamic_pointer_cast<DynamicRigidBody>(body);
		if (dynamicBody)
		{
			physx::PxVec3 pxVelocity = dynamicBody->GetPxRigidDynamic()->getLinearVelocity();
			pxVelocity.x += velocity.x;
			pxVelocity.y += velocity.y;
			pxVelocity.z -= velocity.z;

			dynamicBody->GetPxRigidDynamic()->setLinearVelocity(pxVelocity);

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
		physx::PxFilterData data;
		data.word0 = info.colliderInfo.layerNumber;
		data.word1 = collisionMatrix[info.colliderInfo.layerNumber];
		shape->setSimulationFilterData(data);

		std::shared_ptr<StaticRigidBody> staticBody = std::make_shared<StaticRigidBody>(colliderType, info.colliderInfo.id, info.colliderInfo.layerNumber);
		if (!staticBody->Initialize(info.colliderInfo, shape, mPhysics)) 
			return false;

		mRigidBodyContainer.insert(std::make_pair(staticBody->GetID(), staticBody));
		mUpcomingActors.push_back(staticBody);

		return true;
	}

	bool PhysicsRigidBodyManager::CreateStaticBody(const SphereColliderInfo& info, const EColliderType& colliderType, int* collisionMatrix)
	{
		physx::PxMaterial* material = mPhysics->createMaterial(info.colliderInfo.staticFriction, info.colliderInfo.dynamicFriction, info.colliderInfo.restitution);
		physx::PxShape* shape = mPhysics->createShape(physx::PxSphereGeometry(info.raidus), *material);
		physx::PxFilterData data;
		data.word0 = info.colliderInfo.layerNumber;
		data.word1 = collisionMatrix[info.colliderInfo.layerNumber];
		shape->setSimulationFilterData(data);

		std::shared_ptr<StaticRigidBody> staticBody = std::make_shared<StaticRigidBody>(colliderType, info.colliderInfo.id, info.colliderInfo.layerNumber);
		if (!staticBody->Initialize(info.colliderInfo, shape, mPhysics))
			return false;

		mRigidBodyContainer.insert(std::make_pair(staticBody->GetID(), staticBody));
		mUpcomingActors.push_back(staticBody);

		return true;
	}

	bool PhysicsRigidBodyManager::CreateStaticBody(const CapsuleColliderInfo& info, const EColliderType& colliderType, int* collisionMatrix)
	{
		physx::PxMaterial* material = mPhysics->createMaterial(info.colliderInfo.staticFriction, info.colliderInfo.dynamicFriction, info.colliderInfo.restitution);
		physx::PxShape* shape = mPhysics->createShape(physx::PxCapsuleGeometry(info.raidus, info.halfHeight), *material);
		physx::PxFilterData data;
		data.word0 = info.colliderInfo.layerNumber;
		data.word1 = collisionMatrix[info.colliderInfo.layerNumber];
		shape->setSimulationFilterData(data);

		std::shared_ptr<StaticRigidBody> staticBody = std::make_shared<StaticRigidBody>(colliderType, info.colliderInfo.id, info.colliderInfo.layerNumber);
		if (!staticBody->Initialize(info.colliderInfo, shape, mPhysics))
			return false;

		mRigidBodyContainer.insert(std::make_pair(staticBody->GetID(), staticBody));
		mUpcomingActors.push_back(staticBody);

		return true;
	}

	bool PhysicsRigidBodyManager::CreateStaticBody(const ConvexMeshColliderInfo& info, const EColliderType& colliderType, int* collisionMatrix)
	{
		physx::PxConvexMesh* mesh = mCookingMeshTool->CookingConvexMesh(info.vertices, info.vertexSize, info.convexPolygonLimit);
		physx::PxMaterial* material = mPhysics->createMaterial(info.colliderInfo.staticFriction, info.colliderInfo.dynamicFriction, info.colliderInfo.restitution);
		physx::PxShape* shape = mPhysics->createShape(physx::PxConvexMeshGeometry(mesh), *material);
		physx::PxFilterData data;
		data.word0 = info.colliderInfo.layerNumber;
		data.word1 = collisionMatrix[info.colliderInfo.layerNumber];
		shape->setSimulationFilterData(data);

		std::shared_ptr<StaticRigidBody> staticBody = std::make_shared<StaticRigidBody>(colliderType, info.colliderInfo.id, info.colliderInfo.layerNumber);
		if (!staticBody->Initialize(info.colliderInfo, shape, mPhysics))
			return false;

		mRigidBodyContainer.insert(std::make_pair(staticBody->GetID(), staticBody));
		mUpcomingActors.push_back(staticBody);

		return true;
	}

	bool PhysicsRigidBodyManager::CreateDynamicBody(const BoxColliderInfo& info, const EColliderType& colliderType, int* collisionMatrix)
	{
		physx::PxMaterial* material = mPhysics->createMaterial(info.colliderInfo.staticFriction, info.colliderInfo.dynamicFriction, info.colliderInfo.restitution);
		physx::PxShape* shape = mPhysics->createShape(physx::PxBoxGeometry(info.boxExtent.x, info.boxExtent.y, info.boxExtent.z), *material);
		physx::PxFilterData data;
		data.word0 = info.colliderInfo.layerNumber;
		data.word1 = collisionMatrix[info.colliderInfo.layerNumber];
		shape->setSimulationFilterData(data);

		std::shared_ptr<DynamicRigidBody> dynamicBody = std::make_shared<DynamicRigidBody>(colliderType, info.colliderInfo.id, info.colliderInfo.layerNumber);
		if (!dynamicBody->Initialize(info.colliderInfo, shape, mPhysics))
			return false;

		mRigidBodyContainer.insert(std::make_pair(dynamicBody->GetID(), dynamicBody));
		mUpcomingActors.push_back(dynamicBody);

		return true;
	}

	bool PhysicsRigidBodyManager::CreateDynamicBody(const SphereColliderInfo& info, const EColliderType& colliderType, int* collisionMatrix)
	{
		physx::PxMaterial* material = mPhysics->createMaterial(info.colliderInfo.staticFriction, info.colliderInfo.dynamicFriction, info.colliderInfo.restitution);
		physx::PxShape* shape = mPhysics->createShape(physx::PxSphereGeometry(info.raidus), *material);
		physx::PxFilterData data;
		data.word0 = info.colliderInfo.layerNumber;
		data.word1 = collisionMatrix[info.colliderInfo.layerNumber];
		shape->setSimulationFilterData(data);

		std::shared_ptr<DynamicRigidBody> dynamicBody = std::make_shared<DynamicRigidBody>(colliderType, info.colliderInfo.id, info.colliderInfo.layerNumber);
		if (!dynamicBody->Initialize(info.colliderInfo, shape, mPhysics))
			return false;

		mRigidBodyContainer.insert(std::make_pair(dynamicBody->GetID(), dynamicBody));
		mUpcomingActors.push_back(dynamicBody);

		return true;
	}

	bool PhysicsRigidBodyManager::CreateDynamicBody(const CapsuleColliderInfo& info, const EColliderType& colliderType, int* collisionMatrix)
	{
		physx::PxMaterial* material = mPhysics->createMaterial(info.colliderInfo.staticFriction, info.colliderInfo.dynamicFriction, info.colliderInfo.restitution);
		physx::PxShape* shape = mPhysics->createShape(physx::PxCapsuleGeometry(info.raidus, info.halfHeight), *material);
		physx::PxFilterData data;
		data.word0 = info.colliderInfo.layerNumber;
		data.word1 = collisionMatrix[info.colliderInfo.layerNumber];
		shape->setSimulationFilterData(data);

		std::shared_ptr<DynamicRigidBody> dynamicBody = std::make_shared<DynamicRigidBody>(colliderType, info.colliderInfo.id, info.colliderInfo.layerNumber);
		if (!dynamicBody->Initialize(info.colliderInfo, shape, mPhysics))
			return false;

		mRigidBodyContainer.insert(std::make_pair(dynamicBody->GetID(), dynamicBody));
		mUpcomingActors.push_back(dynamicBody);

		return true;
	}

	bool PhysicsRigidBodyManager::CreateDynamicBody(const ConvexMeshColliderInfo& info, const EColliderType& colliderType, int* collisionMatrix)
	{
		physx::PxConvexMesh* mesh = mCookingMeshTool->CookingConvexMesh(info.vertices, info.vertexSize, info.convexPolygonLimit);
		physx::PxMaterial* material = mPhysics->createMaterial(info.colliderInfo.staticFriction, info.colliderInfo.dynamicFriction, info.colliderInfo.restitution);
		physx::PxShape* shape = mPhysics->createShape(physx::PxConvexMeshGeometry(mesh), *material);
		physx::PxFilterData data;
		data.word0 = info.colliderInfo.layerNumber;
		data.word1 = collisionMatrix[info.colliderInfo.layerNumber];
		shape->setSimulationFilterData(data);

		std::shared_ptr<DynamicRigidBody> dynamicBody = std::make_shared<DynamicRigidBody>(colliderType, info.colliderInfo.id, info.colliderInfo.layerNumber);
		if (!dynamicBody->Initialize(info.colliderInfo, shape, mPhysics))
			return false;

		mRigidBodyContainer.insert(std::make_pair(dynamicBody->GetID(), dynamicBody));
		mUpcomingActors.push_back(dynamicBody);

		return true;
	}

#pragma endregion

#pragma region RemoveRigidBody

	bool PhysicsRigidBodyManager::RemoveRigidBody(const unsigned int& id, physx::PxScene* scene)
	{
		std::shared_ptr<RigidBody> body = mRigidBodyContainer.find(id)->second;

		if (body)
		{
			std::shared_ptr<DynamicRigidBody> dynamicBody = std::dynamic_pointer_cast<DynamicRigidBody>(body);
			if (dynamicBody)
			{
				scene->removeActor(*dynamicBody->GetPxRigidDynamic());
				mRigidBodyContainer.erase(mRigidBodyContainer.find(id));

			}
			std::shared_ptr<StaticRigidBody> staticBody = std::dynamic_pointer_cast<StaticRigidBody>(body);
			if (staticBody)
			{
				scene->removeActor(*staticBody->GetPxRigidStatic());
				mRigidBodyContainer.erase(mRigidBodyContainer.find(id));
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
				physx::PxShape* shpae;
				physx::PxFilterData filterData;

				filterData.word0 = dynamicBody->GetLayerNumber();
				filterData.word1 = collisionMatrix[dynamicBody->GetLayerNumber()];

				continue;
			}
			std::shared_ptr<StaticRigidBody> staticBody = std::dynamic_pointer_cast<StaticRigidBody>(body.second);
			if (staticBody)
			{
				physx::PxShape* shpae;
				physx::PxFilterData filterData;

				filterData.word0 = staticBody->GetLayerNumber();
				filterData.word1 = collisionMatrix[staticBody->GetLayerNumber()];

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

		for (const auto& body : mRigidBodyContainer)
		{
			std::shared_ptr<DynamicRigidBody> dynamicBody = std::dynamic_pointer_cast<DynamicRigidBody>(body.second);
			if (dynamicBody)
			{
				physx::PxShape* shape;
				physx::PxRigidActor* actor = dynamicBody->GetPxRigidDynamic();
				actor->getShapes(&shape, sizeof(physx::PxShape));

				if (shape->getGeometry().getType() == physx::PxGeometryType::eCONVEXMESH)
				{
					shared_ptr<vector<vector<DirectX::SimpleMath::Vector3>>> polygon = make_shared<vector<vector<DirectX::SimpleMath::Vector3>>>();
					ExtractDebugConvexMesh(actor, shape, *polygon.get());

					mDebugPolygon.insert(polygon);
				}
			}
			std::shared_ptr<StaticRigidBody> staticBody = dynamic_pointer_cast<StaticRigidBody>(body.second);
			if (staticBody)
			{
				physx::PxShape* shape;
				physx::PxRigidActor* actor = staticBody->GetPxRigidStatic();
				actor->getShapes(&shape, sizeof(physx::PxShape));

				if (shape->getGeometry().getType() == physx::PxGeometryType::eCONVEXMESH)
				{
					shared_ptr<vector<vector<DirectX::SimpleMath::Vector3>>> polygon = make_shared<vector<vector<DirectX::SimpleMath::Vector3>>>();
					ExtractDebugConvexMesh(actor, shape, *polygon.get());

					mDebugPolygon.insert(polygon);
				}
			}
		}
	}

#pragma endregion

#pragma region UpdateUserData

	void PhysicsRigidBodyManager::UpdateUserData(physx::PxScene* scene)
	{
		// 물리 공간의 리지드 바디들을 검색해서 해당 userData를 가지고 있는 리지드 바디가 없으면 userData 메모리 삭제
		unsigned int ActorSize = scene->getNbActors(physx::PxActorTypeFlags(physx::PxActorTypeFlag::eRIGID_DYNAMIC | physx::PxActorTypeFlag::eRIGID_STATIC));

		physx::PxActor** pxActor = new physx::PxActor * [ActorSize];;
		scene->getActors(physx::PxActorTypeFlags(physx::PxActorTypeFlag::eRIGID_DYNAMIC | physx::PxActorTypeFlag::eRIGID_STATIC), pxActor, ActorSize);

		std::vector<decltype(mCollisionDataContainer)::iterator> itemsToDelete;

		for (auto it = mCollisionDataContainer.begin(); it != mCollisionDataContainer.end(); ++it)
		{
			bool IsDead = true;

			for (int i = 0; i < ActorSize; i++)
			{
				CollisionData* collisiondata = (CollisionData*)pxActor[i]->userData;

				if (it->first == collisiondata->myId)
				{
					IsDead = false;
					break;
				}
			}

			if (IsDead)
			{
				delete it->second;
				itemsToDelete.push_back(it);
			}
		}

		for (auto it : itemsToDelete)
		{
			mCollisionDataContainer.erase(it);
		}
	}

#pragma endregion

}