#include "PhysicsRigidBodyManager.h"

#include "PhysicsCookingMeshTool.h"
#include "RigidBody.h"
#include "DynamicRigidBody.h"
#include "StaticRigidBody.h"

namespace fq::physics
{
	PhysicsRigidBodyManager::PhysicsRigidBodyManager()
		: mCookingMeshTool(std::make_shared<PhysicsCookingMeshTool>())
	{
	}

	PhysicsRigidBodyManager::~PhysicsRigidBodyManager()
	{
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
				UpdateFilterData(dynamicBody, collisionMatrix);
				scene->addActor(*dynamicBody->GetRigidDynamic());
			}
			std::shared_ptr<StaticRigidBody> staticBody = std::dynamic_pointer_cast<StaticRigidBody>(body);
			if (staticBody)
			{
				UpdateFilterData(staticBody, collisionMatrix);
				scene->addActor(*staticBody->GetRigidStatic());
			}
		}
		mUpcomingActors.clear();

		return true;
	}

#pragma region UpdateFilterData

	bool PhysicsRigidBodyManager::UpdateFilterData(std::shared_ptr<StaticRigidBody> body, int* collisionMatrix)
	{
		physx::PxShape* shape;
		physx::PxRigidStatic* rigidStatic = body->GetRigidStatic();

		rigidStatic->getShapes(&shape, sizeof(physx::PxShape));

		physx::PxFilterData data;
		int layerNumber = body->GetLayerNumber();
		data.word0 = layerNumber;
		data.word1 = collisionMatrix[layerNumber];

		shape[0].setSimulationFilterData(data);

		return true;
	}

	bool PhysicsRigidBodyManager::UpdateFilterData(std::shared_ptr<DynamicRigidBody> body, int* collisionMatrix)
	{
		physx::PxShape* shape;
		physx::PxRigidDynamic* rigidStatic = body->GetRigidDynamic();

		rigidStatic->getShapes(&shape, sizeof(physx::PxShape));

		physx::PxFilterData data;
		int layerNumber = body->GetLayerNumber();
		data.word0 = layerNumber;
		data.word1 = collisionMatrix[layerNumber];

		shape[0].setSimulationFilterData(data);

		return true;
	}

#pragma endregion

#pragma region CreateDynamicAndStaticBody

	bool PhysicsRigidBodyManager::CreateStaticBody(const BoxColliderInfo& info, const EColliderType& colliderType)
	{
		physx::PxMaterial* material = mPhysics->createMaterial(info.colliderInfo.staticFriction, info.colliderInfo.dynamicFriction, info.colliderInfo.restitution);
		physx::PxShape* shape = mPhysics->createShape(physx::PxBoxGeometry(info.boxExtent.x, info.boxExtent.y, info.boxExtent.z), *material);

		std::shared_ptr<StaticRigidBody> staticBody = std::make_shared<StaticRigidBody>(colliderType, info.colliderInfo.id, info.colliderInfo.layerNumber);
		if (!staticBody->Initialize(info.colliderInfo, shape, mPhysics)) 
			return false;

		mRigidBodys.insert(std::make_pair(staticBody->GetID(), staticBody));
		mUpcomingActors.push_back(staticBody);

		return true;
	}

	bool PhysicsRigidBodyManager::CreateStaticBody(const SphereColliderInfo& info, const EColliderType& colliderType)
	{
		physx::PxMaterial* material = mPhysics->createMaterial(info.colliderInfo.staticFriction, info.colliderInfo.dynamicFriction, info.colliderInfo.restitution);
		physx::PxShape* shape = mPhysics->createShape(physx::PxSphereGeometry(info.raidus), *material);

		std::shared_ptr<StaticRigidBody> staticBody = std::make_shared<StaticRigidBody>(colliderType, info.colliderInfo.id, info.colliderInfo.layerNumber);
		if (!staticBody->Initialize(info.colliderInfo, shape, mPhysics))
			return false;

		mRigidBodys.insert(std::make_pair(staticBody->GetID(), staticBody));
		mUpcomingActors.push_back(staticBody);

		return true;
	}

	bool PhysicsRigidBodyManager::CreateStaticBody(const CapsuleColliderInfo& info, const EColliderType& colliderType)
	{
		physx::PxMaterial* material = mPhysics->createMaterial(info.colliderInfo.staticFriction, info.colliderInfo.dynamicFriction, info.colliderInfo.restitution);
		physx::PxShape* shape = mPhysics->createShape(physx::PxCapsuleGeometry(info.raidus, info.halfHeight), *material);

		std::shared_ptr<StaticRigidBody> staticBody = std::make_shared<StaticRigidBody>(colliderType, info.colliderInfo.id, info.colliderInfo.layerNumber);
		if (!staticBody->Initialize(info.colliderInfo, shape, mPhysics))
			return false;

		mRigidBodys.insert(std::make_pair(staticBody->GetID(), staticBody));
		mUpcomingActors.push_back(staticBody);

		return true;
	}

	bool PhysicsRigidBodyManager::CreateStaticBody(const ConvexMeshColliderInfo& info, const EColliderType& colliderType)
	{
		physx::PxConvexMesh* mesh = mCookingMeshTool->CookingConvexMesh(info.vertices, info.vertexSize, info.convexVertexLimit, info.convexPolygonLimit);
		physx::PxMaterial* material = mPhysics->createMaterial(info.colliderInfo.staticFriction, info.colliderInfo.dynamicFriction, info.colliderInfo.restitution);
		physx::PxShape* shape = mPhysics->createShape(physx::PxConvexMeshGeometry(mesh), *material);

		std::shared_ptr<StaticRigidBody> staticBody = std::make_shared<StaticRigidBody>(colliderType, info.colliderInfo.id, info.colliderInfo.layerNumber);
		if (!staticBody->Initialize(info.colliderInfo, shape, mPhysics))
			return false;

		mRigidBodys.insert(std::make_pair(staticBody->GetID(), staticBody));
		mUpcomingActors.push_back(staticBody);

		return true;
	}

	bool PhysicsRigidBodyManager::CreateDynamicBody(const BoxColliderInfo& info, const EColliderType& colliderType)
	{
		physx::PxMaterial* material = mPhysics->createMaterial(info.colliderInfo.staticFriction, info.colliderInfo.dynamicFriction, info.colliderInfo.restitution);
		physx::PxShape* shape = mPhysics->createShape(physx::PxBoxGeometry(info.boxExtent.x, info.boxExtent.y, info.boxExtent.z), *material);

		std::shared_ptr<StaticRigidBody> dynamicBody = std::make_shared<StaticRigidBody>(colliderType, info.colliderInfo.id, info.colliderInfo.layerNumber);
		if (!dynamicBody->Initialize(info.colliderInfo, shape, mPhysics))
			return false;

		mRigidBodys.insert(std::make_pair(dynamicBody->GetID(), dynamicBody));
		mUpcomingActors.push_back(dynamicBody);

		return true;
	}

	bool PhysicsRigidBodyManager::CreateDynamicBody(const SphereColliderInfo& info, const EColliderType& colliderType)
	{
		physx::PxMaterial* material = mPhysics->createMaterial(info.colliderInfo.staticFriction, info.colliderInfo.dynamicFriction, info.colliderInfo.restitution);
		physx::PxShape* shape = mPhysics->createShape(physx::PxSphereGeometry(info.raidus), *material);

		std::shared_ptr<StaticRigidBody> dynamicBody = std::make_shared<StaticRigidBody>(colliderType, info.colliderInfo.id, info.colliderInfo.layerNumber);
		if (!dynamicBody->Initialize(info.colliderInfo, shape, mPhysics))
			return false;

		mRigidBodys.insert(std::make_pair(dynamicBody->GetID(), dynamicBody));
		mUpcomingActors.push_back(dynamicBody);

		return true;
	}

	bool PhysicsRigidBodyManager::CreateDynamicBody(const CapsuleColliderInfo& info, const EColliderType& colliderType)
	{

		physx::PxMaterial* material = mPhysics->createMaterial(info.colliderInfo.staticFriction, info.colliderInfo.dynamicFriction, info.colliderInfo.restitution);
		physx::PxShape* shape = mPhysics->createShape(physx::PxCapsuleGeometry(info.raidus, info.halfHeight), *material);

		std::shared_ptr<StaticRigidBody> dynamicBody = std::make_shared<StaticRigidBody>(colliderType, info.colliderInfo.id, info.colliderInfo.layerNumber);
		if (!dynamicBody->Initialize(info.colliderInfo, shape, mPhysics))
			return false;

		mRigidBodys.insert(std::make_pair(dynamicBody->GetID(), dynamicBody));
		mUpcomingActors.push_back(dynamicBody);

		return true;
	}

	bool PhysicsRigidBodyManager::CreateDynamicBody(const ConvexMeshColliderInfo& info, const EColliderType& colliderType)
	{
		physx::PxConvexMesh* mesh = mCookingMeshTool->CookingConvexMesh(info.vertices, info.vertexSize, info.convexVertexLimit, info.convexPolygonLimit);
		physx::PxMaterial* material = mPhysics->createMaterial(info.colliderInfo.staticFriction, info.colliderInfo.dynamicFriction, info.colliderInfo.restitution);
		physx::PxShape* shape = mPhysics->createShape(physx::PxConvexMeshGeometry(mesh), *material);

		std::shared_ptr<StaticRigidBody> dynamicBody = std::make_shared<StaticRigidBody>(colliderType, info.colliderInfo.id, info.colliderInfo.layerNumber);
		if (!dynamicBody->Initialize(info.colliderInfo, shape, mPhysics))
			return false;

		mRigidBodys.insert(std::make_pair(dynamicBody->GetID(), dynamicBody));
		mUpcomingActors.push_back(dynamicBody);

		return true;
	}

#pragma endregion

#pragma region RemoveRigidBody

	bool PhysicsRigidBodyManager::RemoveRigidBody(unsigned int id, physx::PxScene* scene)
	{
		std::shared_ptr<RigidBody> body = mRigidBodys.find(id)->second;

		if (body)
		{
			std::shared_ptr<DynamicRigidBody> dynamicBody = std::dynamic_pointer_cast<DynamicRigidBody>(body);
			if (dynamicBody)
			{
				scene->removeActor(*dynamicBody->GetRigidDynamic());
				mRigidBodys.erase(mRigidBodys.find(id));

				return true;
			}
			std::shared_ptr<StaticRigidBody> staticBody = std::dynamic_pointer_cast<StaticRigidBody>(body);
			if (staticBody)
			{
				scene->removeActor(*staticBody->GetRigidStatic());
				mRigidBodys.erase(mRigidBodys.find(id));

				return true;
			}
		}

		return false;
	}

	bool PhysicsRigidBodyManager::RemoveAllRigidBody(physx::PxScene* scene)
	{
		for (const auto& body : mRigidBodys)
		{
			std::shared_ptr<DynamicRigidBody> dynamicBody = std::dynamic_pointer_cast<DynamicRigidBody>(body.second);
			if (dynamicBody)
			{
				scene->removeActor(*dynamicBody->GetRigidDynamic());
				continue;
			}
			std::shared_ptr<StaticRigidBody> staticBody = std::dynamic_pointer_cast<StaticRigidBody>(body.second);
			if (staticBody)
			{
				scene->removeActor(*staticBody->GetRigidStatic());
				continue;
			}
		}
		mRigidBodys.clear();

		for (const auto& body : mUpcomingActors)
		{
			std::shared_ptr<DynamicRigidBody> dynamicBody = std::dynamic_pointer_cast<DynamicRigidBody>(body);
			if (dynamicBody)
			{
				scene->removeActor(*dynamicBody->GetRigidDynamic());
				continue;
			}
			std::shared_ptr<StaticRigidBody> staticBody = std::dynamic_pointer_cast<StaticRigidBody>(body);
			if (staticBody)
			{
				scene->removeActor(*staticBody->GetRigidStatic());
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
		for (const auto& body : mRigidBodys)
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

				filterData.word0 = dynamicBody->GetLayerNumber();
				filterData.word1 = collisionMatrix[dynamicBody->GetLayerNumber()];

				continue;
			}
		}
	}

#pragma endregion

}