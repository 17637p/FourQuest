#include "PhysicsSystem.h"

#include <spdlog/spdlog.h>

#include "../FQCommon/FQPath.h"
#include "../FQphysics/IFQPhysics.h"
#include "../FQGameModule/GameModule.h"
#include "GameProcess.h"

fq::game_engine::PhysicsSystem::PhysicsSystem()
	:mGameProcess(nullptr)
	, mScene(nullptr)
	, mCollisionMatrix{}
	, mbIsGameLoaded(false)
	, mGravity{ 0.f,-10.f,0.f }
	, mPhysicsEngine(nullptr)
	, mLastColliderID(physics::unregisterID)
	, mBoxID(entt::resolve<fq::game_module::BoxCollider>().id())
	, mSphereID(entt::resolve<fq::game_module::SphereCollider>().id())
	, mCapsuleID(entt::resolve<fq::game_module::CapsuleCollider>().id())
	, mMeshID(entt::resolve<fq::game_module::MeshCollider>().id())
{}

fq::game_engine::PhysicsSystem::~PhysicsSystem()
{}

void fq::game_engine::PhysicsSystem::Initialize(GameProcess* game)
{
	mGameProcess = game;
	mPhysicsEngine = mGameProcess->mPhysics;
	mScene = mGameProcess->mSceneManager->GetCurrentScene();

	mPhysicsEngine->SetCallBackFunction([this](fq::physics::CollisionData data
		, fq::physics::ECollisionEventType type)
		{
			this->callBackEvent(data, type);
		}
	);

	// EventHandle µî·Ï
	auto& eventMgr = mGameProcess->mEventManager;
	mOnLoadSceneHandler = eventMgr->
		RegisterHandle<fq::event::OnLoadScene>(this, &PhysicsSystem::OnLoadScene);
	mOnUnloadSceneHandler = eventMgr->
		RegisterHandle<fq::event::OnUnloadScene>(this, &PhysicsSystem::OnUnLoadScene);
	mOnAddGameObjectHandler = mGameProcess->mEventManager->
		RegisterHandle<fq::event::AddGameObject>(this, &PhysicsSystem::OnAddGameObject);
	mDestroyedGameObjectHandler = mGameProcess->mEventManager->
		RegisterHandle<fq::event::OnDestoryedGameObject>(this, &PhysicsSystem::OnDestroyedGameObject);
	mAddComponentHandler = mGameProcess->mEventManager->
		RegisterHandle<fq::event::AddComponent>(this, &PhysicsSystem::AddComponent);
	mRemoveComponentHandler = mGameProcess->mEventManager->
		RegisterHandle<fq::event::RemoveComponent>(this, &PhysicsSystem::RemoveComponent);
}


void fq::game_engine::PhysicsSystem::OnUnLoadScene()
{
	mbIsGameLoaded = false;

}

void fq::game_engine::PhysicsSystem::OnLoadScene(const fq::event::OnLoadScene event)
{
	auto scenePath = fq::path::GetScenePath() / event.sceneName / "collision_matrix.txt";
	mCollisionMatrix.Load(scenePath);

	setPhysicsEngineinfo();

	for (auto& object : mScene->GetObjectView())
	{
		addCollider(&object);
	}

}

void fq::game_engine::PhysicsSystem::OnDestroyedGameObject(const fq::event::OnDestoryedGameObject& event)
{
	removeCollider(event.object);
}

void fq::game_engine::PhysicsSystem::OnAddGameObject(const fq::event::AddGameObject& event)
{
	if (!mbIsGameLoaded)
	{
		return;
	}

	addCollider(event.object);
}


void fq::game_engine::PhysicsSystem::AddComponent(const fq::event::AddComponent& event)
{
	if (event.id != entt::resolve<fq::game_module::BoxCollider>().id()
		|| event.id != entt::resolve<fq::game_module::SphereCollider>().id()
		|| event.id != entt::resolve<fq::game_module::MeshCollider>().id())
		//		|| event.id != entt::resolve<fq::game_module::CapsuleColldier>().id()
	{
		addCollider(event.component->GetGameObject());
	}
}

void fq::game_engine::PhysicsSystem::RemoveComponent(const fq::event::RemoveComponent& event)
{
	if (event.id != entt::resolve<fq::game_module::BoxCollider>().id()
		|| event.id != entt::resolve<fq::game_module::SphereCollider>().id()
		|| event.id != entt::resolve<fq::game_module::MeshCollider>().id())
		//		|| event.id != entt::resolve<fq::game_module::CapsuleColldier>().id()
	{
		removeCollider(event.component->GetGameObject());
	}
}

void fq::game_engine::PhysicsSystem::SetCollisionMatrix(fq::physics::CollisionMatrix matrix)
{
	mCollisionMatrix = matrix;
	setPhysicsEngineinfo();
}

void fq::game_engine::PhysicsSystem::addCollider(fq::game_module::GameObject* object)
{
	using namespace fq::game_module;

	if (!object->HasComponent<RigidBody>())
	{
		return;
	}
	auto rigid = object->GetComponent<RigidBody>();
	auto transform = object->GetComponent<Transform>();
	bool isStatic = rigid->IsStatic();

	// 1. Box Colllider
	if (object->HasComponent<BoxCollider>())
	{
		auto boxCollider = object->GetComponent<BoxCollider>();
		auto type = boxCollider->GetType();
		auto boxInfo = boxCollider->GetBoxInfomation();

		ColliderID id = ++mLastColliderID;
		boxInfo.colliderInfo.id = id;
		boxInfo.colliderInfo.layerNumber = static_cast<int>(object->GetTag());
		boxInfo.colliderInfo.collisionTransform.worldMatrix = transform->GetWorldMatrix();
		boxCollider->SetBoxInfomation(boxInfo);

		if (isStatic)
		{
			bool check = mPhysicsEngine->CreateStaticBody(boxInfo, type);
			assert(check);
			mColliderContainer.insert({ id, {mBoxID, boxCollider} });
		}
		else
		{
			bool check = mPhysicsEngine->CreateDynamicBody(boxInfo, type);
			assert(check);
			mColliderContainer.insert({ id, {mBoxID, boxCollider} });
		}
		
	}
	// 2. Sphere Collider
	if (object->HasComponent<SphereCollider>())
	{
		auto sphereCollider = object->GetComponent<SphereCollider>();
		auto type = sphereCollider->GetType();
		auto sphereInfo = sphereCollider->GetSphereInfomation();

		ColliderID id = ++mLastColliderID;
		sphereInfo.colliderInfo.id = id;
		sphereInfo.colliderInfo.layerNumber = static_cast<int>(object->GetTag());
		sphereInfo.colliderInfo.collisionTransform.worldMatrix = transform->GetWorldMatrix();
		sphereCollider->SetSphereInfomation(sphereInfo);

		if (isStatic)
		{
			bool check = mPhysicsEngine->CreateStaticBody(sphereInfo, type);
			assert(check);
			mColliderContainer.insert({ id, {mSphereID, sphereCollider} });
		}
		else
		{
			bool check = mPhysicsEngine->CreateDynamicBody(sphereInfo, type);
			assert(check);
			mColliderContainer.insert({ id, {mSphereID, sphereCollider} });
		}
	}
	// 3. Capsule Collider
	if (object->HasComponent<CapsuleCollider>())
	{
		auto capsuleCollider = object->GetComponent<CapsuleCollider>();
		auto type = capsuleCollider->GetType();
		auto capsuleInfo = capsuleCollider->GetCapsuleInfomation();

		ColliderID id = ++mLastColliderID;
		capsuleInfo.colliderInfo.id = id;
		capsuleInfo.colliderInfo.layerNumber = static_cast<int>(object->GetTag());
		capsuleInfo.colliderInfo.collisionTransform.worldMatrix = transform->GetWorldMatrix();
		capsuleCollider->SetCapsuleInfomation(capsuleInfo);

		if (isStatic)
		{
			bool check = mPhysicsEngine->CreateStaticBody(capsuleInfo, type);
			assert(check);
			mColliderContainer.insert({ id, {mCapsuleID, capsuleCollider} });
		}
		else
		{
			bool check = mPhysicsEngine->CreateDynamicBody(capsuleInfo, type);
			assert(check);
			mColliderContainer.insert({ id, {mCapsuleID, capsuleCollider} });
		}
	}


	// 4. Mesh Collider

}

void fq::game_engine::PhysicsSystem::removeCollider(fq::game_module::GameObject* object)
{
	using namespace fq::game_module;
	if (!object->HasComponent<RigidBody>())
	{
		return;
	}

	// 1. Box Colllider
	if (object->HasComponent<BoxCollider>())
	{
		auto boxCollider = object->GetComponent<BoxCollider>();
		auto id = boxCollider->GetBoxInfomation().colliderInfo.id;
		assert(id != physics::unregisterID);

		mPhysicsEngine->RemoveRigidBody(id);
		mColliderContainer.erase(mColliderContainer.find(id));
	}
	// 2. Sphere Collider
	if (object->HasComponent<SphereCollider>())
	{
		auto sphereCollider = object->GetComponent<SphereCollider>();
		auto id = sphereCollider->GetSphereInfomation().colliderInfo.id;
		assert(id != physics::unregisterID);

		mPhysicsEngine->RemoveRigidBody(id);
		mColliderContainer.erase(mColliderContainer.find(id));
	}
	// 3. Capsule Collider
	if (object->HasComponent<CapsuleCollider>())
	{
		auto sphereCollider = object->GetComponent<CapsuleCollider>();
		auto id = sphereCollider->GetCapsuleInfomation().colliderInfo.id;
		assert(id != physics::unregisterID);

		mPhysicsEngine->RemoveRigidBody(id);
		mColliderContainer.erase(mColliderContainer.find(id));
	}

	// 4. Mesh Collider
}

void fq::game_engine::PhysicsSystem::callBackEvent(fq::physics::CollisionData data, fq::physics::ECollisionEventType type)
{
	auto my =  mColliderContainer.find(data.myId);
	auto other = mColliderContainer.find(data.otherId);
	assert(my != mColliderContainer.end() || other != mColliderContainer.end());

	auto object = my->second.second->GetGameObject();
	auto otherObject = other->second.second->GetGameObject();
	
	fq::game_module::Collision collision{ object,otherObject, data.ContectPoints};

	switch (type)
	{
		case fq::physics::ECollisionEventType::ENTER_OVERLAP:
			object->OnTriggerEnter(collision);
			break;
		case fq::physics::ECollisionEventType::ON_OVERLAP:
			object->OnTriggerStay(collision);
			break;
		case fq::physics::ECollisionEventType::END_OVERLAP:
			object->OnTriggerExit(collision);
			break;
		case fq::physics::ECollisionEventType::ENTER_COLLISION:
			object->OnCollisionEnter(collision);
			break;
		case fq::physics::ECollisionEventType::ON_COLLISION:
			object->OnCollisionStay(collision);
			break;
		case fq::physics::ECollisionEventType::END_COLLISION:
			object->OnCollisionExit(collision);
			break;
	}

}

void fq::game_engine::PhysicsSystem::SinkToGameScene()
{
	for (auto& [id, colliderInfo] : mColliderContainer)
	{
		auto transform = colliderInfo.second->GetComponent<fq::game_module::Transform>();
		auto matrix =  mPhysicsEngine->GetRigidBodyMatrix(id);
		transform->SetLocalMatrix(matrix);
	}
}

void fq::game_engine::PhysicsSystem::setPhysicsEngineinfo()
{
	fq::physics::PhysicsEngineInfo mPhysicsEngineInfomation;

	for (int i = 0; i < 16; ++i)
	{
		mPhysicsEngineInfomation.collisionMatrix[i] = static_cast<int>(mCollisionMatrix.data[i].to_ulong());
	}
	mPhysicsEngineInfomation.gravity = mGravity;

	mGameProcess->mPhysics->SetPhysicsInfo(mPhysicsEngineInfomation);
}

void fq::game_engine::PhysicsSystem::SinkToPhysicsScene()
{
	for (auto& [id, colliderInfo] : mColliderContainer)
	{
		auto transform = colliderInfo.second->GetComponent<fq::game_module::Transform>();
		mPhysicsEngine->SetRigidBodyMatrix(id, transform->GetWorldMatrix());
	}
}

