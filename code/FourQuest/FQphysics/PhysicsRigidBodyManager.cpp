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
		mRigidBodys.clear();
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
				//UpdateFilterData(dynamicBody, collisionMatrix);
				scene->addActor(*dynamicBody->GetPxRigidDynamic());
			}
			std::shared_ptr<StaticRigidBody> staticBody = std::dynamic_pointer_cast<StaticRigidBody>(body);
			if (staticBody)
			{
				//UpdateFilterData(staticBody, collisionMatrix);
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

	void PhysicsRigidBodyManager::GetRigidBodyMatrix(unsigned int id, DirectX::SimpleMath::Matrix& dxMatrix)
	{
		auto body = mRigidBodys.find(id)->second;

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
		auto body = mRigidBodys.find(id)->second;

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
		auto body = mRigidBodys.find(id)->second;

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

#pragma region UpdateFilterData

	bool PhysicsRigidBodyManager::UpdateFilterData(std::shared_ptr<StaticRigidBody> body, int* collisionMatrix)
	{
		physx::PxShape* shape;
		physx::PxRigidStatic* rigidStatic = body->GetPxRigidStatic();

		rigidStatic->getShapes(&shape, sizeof(physx::PxShape));

		physx::PxFilterData data;
		int layerNumber = body->GetLayerNumber();
		data.word0 = layerNumber;
		data.word1 = collisionMatrix[layerNumber];

		shape->setSimulationFilterData(data);

		return true;
	}

	bool PhysicsRigidBodyManager::UpdateFilterData(std::shared_ptr<DynamicRigidBody> body, int* collisionMatrix)
	{
		physx::PxShape* shape;
		physx::PxRigidDynamic* rigidStatic = body->GetPxRigidDynamic();

		rigidStatic->getShapes(&shape, sizeof(physx::PxShape));

		physx::PxFilterData data;
		int layerNumber = body->GetLayerNumber();
		data.word0 = layerNumber;
		data.word1 = collisionMatrix[layerNumber];

		shape->setSimulationFilterData(data);

		return true;
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

		mRigidBodys.insert(std::make_pair(staticBody->GetID(), staticBody));
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

		mRigidBodys.insert(std::make_pair(staticBody->GetID(), staticBody));
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

		mRigidBodys.insert(std::make_pair(staticBody->GetID(), staticBody));
		mUpcomingActors.push_back(staticBody);

		return true;
	}

	bool PhysicsRigidBodyManager::CreateStaticBody(const ConvexMeshColliderInfo& info, const EColliderType& colliderType, int* collisionMatrix)
	{
		physx::PxConvexMesh* mesh = mCookingMeshTool->CookingConvexMesh(info.vertices, info.vertexSize, info.convexVertexLimit, info.convexPolygonLimit);
		physx::PxMaterial* material = mPhysics->createMaterial(info.colliderInfo.staticFriction, info.colliderInfo.dynamicFriction, info.colliderInfo.restitution);
		physx::PxShape* shape = mPhysics->createShape(physx::PxConvexMeshGeometry(mesh), *material);
		physx::PxFilterData data;
		data.word0 = info.colliderInfo.layerNumber;
		data.word1 = collisionMatrix[info.colliderInfo.layerNumber];
		shape->setSimulationFilterData(data);

		std::shared_ptr<StaticRigidBody> staticBody = std::make_shared<StaticRigidBody>(colliderType, info.colliderInfo.id, info.colliderInfo.layerNumber);
		if (!staticBody->Initialize(info.colliderInfo, shape, mPhysics))
			return false;

		mRigidBodys.insert(std::make_pair(staticBody->GetID(), staticBody));
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

		mRigidBodys.insert(std::make_pair(dynamicBody->GetID(), dynamicBody));
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

		mRigidBodys.insert(std::make_pair(dynamicBody->GetID(), dynamicBody));
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

		mRigidBodys.insert(std::make_pair(dynamicBody->GetID(), dynamicBody));
		mUpcomingActors.push_back(dynamicBody);

		return true;
	}

	bool PhysicsRigidBodyManager::CreateDynamicBody(const ConvexMeshColliderInfo& info, const EColliderType& colliderType, int* collisionMatrix)
	{
		physx::PxConvexMesh* mesh = mCookingMeshTool->CookingConvexMesh(info.vertices, info.vertexSize, info.convexVertexLimit, info.convexPolygonLimit);
		physx::PxMaterial* material = mPhysics->createMaterial(info.colliderInfo.staticFriction, info.colliderInfo.dynamicFriction, info.colliderInfo.restitution);
		physx::PxShape* shape = mPhysics->createShape(physx::PxConvexMeshGeometry(mesh), *material);
		physx::PxFilterData data;
		data.word0 = info.colliderInfo.layerNumber;
		data.word1 = collisionMatrix[info.colliderInfo.layerNumber];
		shape->setSimulationFilterData(data);

		std::shared_ptr<DynamicRigidBody> dynamicBody = std::make_shared<DynamicRigidBody>(colliderType, info.colliderInfo.id, info.colliderInfo.layerNumber);
		if (!dynamicBody->Initialize(info.colliderInfo, shape, mPhysics))
			return false;

		mRigidBodys.insert(std::make_pair(dynamicBody->GetID(), dynamicBody));
		mUpcomingActors.push_back(dynamicBody);

		return true;
	}

#pragma endregion

#pragma region RemoveRigidBody

	bool PhysicsRigidBodyManager::RemoveRigidBody(const unsigned int& id, physx::PxScene* scene)
	{
		std::shared_ptr<RigidBody> body = mRigidBodys.find(id)->second;

		if (body)
		{
			std::shared_ptr<DynamicRigidBody> dynamicBody = std::dynamic_pointer_cast<DynamicRigidBody>(body);
			if (dynamicBody)
			{
				scene->removeActor(*dynamicBody->GetPxRigidDynamic());
				mRigidBodys.erase(mRigidBodys.find(id));

			}
			std::shared_ptr<StaticRigidBody> staticBody = std::dynamic_pointer_cast<StaticRigidBody>(body);
			if (staticBody)
			{
				scene->removeActor(*staticBody->GetPxRigidStatic());
				mRigidBodys.erase(mRigidBodys.find(id));
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
		for (const auto& body : mRigidBodys)
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
		mRigidBodys.clear();

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

		for (const auto& body : mRigidBodys)
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
}