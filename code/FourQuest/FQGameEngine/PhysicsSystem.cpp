#include "PhysicsSystem.h"

#include <spdlog/spdlog.h>

#include "../FQGraphics/IFQGraphics.h"
#include "../FQCommon/FQPath.h"
#include "../FQphysics/IFQPhysics.h"
#include "../FQGameModule/GameModule.h"
#include "GameProcess.h"
#include "ModelSystem.h"
#include "RenderingSystem.h"

fq::game_engine::PhysicsSystem::PhysicsSystem()
	:mGameProcess(nullptr)
	, mScene(nullptr)
	, mCollisionMatrix{}
	, mbIsGameLoaded(false)
	, mGravity{ 0.f,-1.f,0.f }
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

	// EventHandle 등록
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

	mGameProcess->mPhysics->FinalUpdate();

	mbIsGameLoaded = true;
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
	if (event.id == mBoxID || event.id == mSphereID
		|| event.id == mCapsuleID || event.id == mMeshID)
	{
		addCollider(event.component->GetGameObject());
	}
}

void fq::game_engine::PhysicsSystem::RemoveComponent(const fq::event::RemoveComponent& event)
{
	if (event.id == mBoxID || event.id == mSphereID
		|| event.id == mCapsuleID || event.id == mMeshID)
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

	bool hasStaticMesh = object->HasComponent<StaticMeshRenderer>();
	bool hasSkinnedMesh = object->HasComponent<SkinnedMeshRenderer>();

	// 4. Mesh Collider
	if (object->HasComponent<MeshCollider>() &&
		(hasStaticMesh || hasSkinnedMesh))
	{
		auto meshCollider = object->GetComponent<MeshCollider>();
		auto type = meshCollider->GetType();
		auto convexMeshInfo = meshCollider->GetConvexMeshInfomation();

		ColliderID id = ++mLastColliderID;
		convexMeshInfo.colliderInfo.id = id;
		convexMeshInfo.colliderInfo.layerNumber = static_cast<int>(object->GetTag());
		convexMeshInfo.colliderInfo.collisionTransform.worldMatrix = transform->GetWorldMatrix();

		if (hasStaticMesh)
		{
			auto staticMeshRenderer = object->GetComponent<StaticMeshRenderer>();
			auto modelPath = staticMeshRenderer->GetMeshObjectInfomation().ModelPath;
			auto meshName = staticMeshRenderer->GetMeshObjectInfomation().MeshName;

			assert(mGameProcess->mRenderingSystem->IsLoadedModel(modelPath));

			mGameProcess->mGraphics->GetModel(modelPath);

			const auto& model = mGameProcess->mGraphics->GetModel(modelPath);
			const auto& mesh = ModelSystem::GetMesh(model, meshName);
			convexMeshInfo.vertices = new DirectX::SimpleMath::Vector3[mesh.Vertices.size()];
			convexMeshInfo.vertexSize = mesh.Vertices.size();

			for (int i = 0; i < mesh.Vertices.size(); ++i)
			{
				convexMeshInfo.vertices[i] =-mesh.Vertices[i].Pos;
			}
		}
		else // skinned mesh 
		{
			auto skinnedMeshRenderer = object->GetComponent<SkinnedMeshRenderer>();
			auto modelPath = skinnedMeshRenderer->GetMeshObjectInfomation().ModelPath;
			auto meshName = skinnedMeshRenderer->GetMeshObjectInfomation().MeshName;

			assert(mGameProcess->mRenderingSystem->IsLoadedModel(modelPath));

			const auto& model = mGameProcess->mGraphics->GetModel(modelPath);

		}
		meshCollider->SetConvexMeshInfomation(convexMeshInfo);

		if (isStatic)
		{
			bool check = mPhysicsEngine->CreateStaticBody(convexMeshInfo, type);
			assert(check);
			mColliderContainer.insert({ id, {mCapsuleID, meshCollider} });
		}
		else
		{
			bool check = mPhysicsEngine->CreateDynamicBody(convexMeshInfo, type);
			assert(check);
			mColliderContainer.insert({ id, {mCapsuleID, meshCollider} });
		}
	}
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
	if (object->HasComponent<MeshCollider>())
	{
		auto meshCollider = object->GetComponent<MeshCollider>();
		auto id = meshCollider->GetConvexMeshInfomation().colliderInfo.id;
		assert(id != physics::unregisterID);

		mPhysicsEngine->RemoveRigidBody(id);
		mColliderContainer.erase(mColliderContainer.find(id));
	}

}

void fq::game_engine::PhysicsSystem::callBackEvent(fq::physics::CollisionData data, fq::physics::ECollisionEventType type)
{
	auto lfs = mColliderContainer.find(data.myId);
	auto rhs = mColliderContainer.find(data.otherId);

	bool isLfsVaild = lfs != mColliderContainer.end();
	bool isRfsVaild = rhs != mColliderContainer.end();

	if (!isLfsVaild)
	{
		return;
	}

	auto lhsObject = lfs->second.second->GetGameObject();
	auto rhsObject = isRfsVaild ? rhs->second.second->GetGameObject() : nullptr;

	fq::game_module::Collision collision{ lhsObject,rhsObject, data.ContectPoints };

	switch (type)
	{
	case fq::physics::ECollisionEventType::ENTER_OVERLAP:
		lhsObject->OnTriggerEnter(collision);
		break;
	case fq::physics::ECollisionEventType::ON_OVERLAP:
		lhsObject->OnTriggerStay(collision);
		break;
	case fq::physics::ECollisionEventType::END_OVERLAP:
		lhsObject->OnTriggerExit(collision);
		break;
	case fq::physics::ECollisionEventType::ENTER_COLLISION:
		lhsObject->OnCollisionEnter(collision);
		break;
	case fq::physics::ECollisionEventType::ON_COLLISION:
		lhsObject->OnCollisionStay(collision);
		break;
	case fq::physics::ECollisionEventType::END_COLLISION:
		lhsObject->OnCollisionExit(collision);
		break;
	}

}

void fq::game_engine::PhysicsSystem::SinkToGameScene()
{
	for (auto& [id, colliderInfo] : mColliderContainer)
	{
		auto transform = colliderInfo.second->GetComponent<fq::game_module::Transform>();
		auto rigid = colliderInfo.second->GetComponent<fq::game_module::RigidBody>();

		auto data = mPhysicsEngine->GetRigidBodyData(id);

		// 선속도
		rigid->SetLinearVelocity(data.linearVelocity);

		// 각속도
		rigid->SetAngularVelocity(data.angularVelocity);

		// 위치 
		auto matrix = data.transform;
		transform->SetWorldMatrix(matrix);
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
		fq::physics::RigidBodyGetSetData data;

		auto transform = colliderInfo.second->GetComponent<fq::game_module::Transform>();
		auto rigid = colliderInfo.second->GetComponent<fq::game_module::RigidBody>();

		data.transform = transform->GetWorldMatrix();
		data.angularVelocity = rigid->GetAngularVelocity();
		data.linearVelocity = rigid->GetLinearVelocity();
		
		mPhysicsEngine->SetRigidBodyData(id, data);
	}
}

fq::game_module::Component* fq::game_engine::PhysicsSystem::GetCollider(ColliderID id) const
{
	auto iter = mColliderContainer.find(id);

	return iter == mColliderContainer.end() ? nullptr : iter->second.second;
}

