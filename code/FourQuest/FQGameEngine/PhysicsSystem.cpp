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
	, mGravity{ 0.f,-10.f,0.f }
	, mPhysicsEngine(nullptr)
	, mLastColliderID(physics::unregisterID)
	, mBoxID(0)
	, mSphereID()
	, mCapsuleID(0)
	, mMeshID(0)
	, mCharactorControllerID(0)
	, mRigidID(0)
	, mAddInputMoveHandler{}
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
	mAddInputMoveHandler = mGameProcess->mEventManager->
		RegisterHandle<fq::event::AddInputMove>(this, &PhysicsSystem::AddInputMove);

	mBoxID = entt::resolve<fq::game_module::BoxCollider>().id();
	mSphereID = entt::resolve<fq::game_module::SphereCollider>().id();
	mCapsuleID = entt::resolve<fq::game_module::CapsuleCollider>().id();
	mMeshID = entt::resolve<fq::game_module::MeshCollider>().id();
	mCharactorControllerID = entt::resolve<fq::game_module::CharacterController>().id();
	mRigidID = entt::resolve<game_module::RigidBody>().id();
}


void fq::game_engine::PhysicsSystem::OnUnLoadScene()
{
	mbIsGameLoaded = false;
	mPhysicsEngine->RemoveAllRigidBody();
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
		|| event.id == mCapsuleID || event.id == mMeshID
		|| event.id == mRigidID)
	{
		addCollider(event.component->GetGameObject());
	}
}

void fq::game_engine::PhysicsSystem::RemoveComponent(const fq::event::RemoveComponent& event)
{
	if (event.id == mBoxID || event.id == mSphereID
		|| event.id == mCapsuleID || event.id == mMeshID
		|| event.id == mRigidID)
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
		auto offset = boxCollider->GetOffset();

		ColliderID id = ++mLastColliderID;
		boxInfo.colliderInfo.id = id;
		boxInfo.colliderInfo.layerNumber = static_cast<int>(object->GetTag());
		boxInfo.colliderInfo.collisionTransform = transform->GetTransform();
		calculateOffset(boxInfo.colliderInfo.collisionTransform, offset);

		boxCollider->SetBoxInfomation(boxInfo);

		if (isStatic)
		{
			bool check = mPhysicsEngine->CreateStaticBody(boxInfo, type);
			assert(check);
			mColliderContainer.insert({ id, {mBoxID, boxCollider->shared_from_this(), boxCollider} });
		}
		else
		{
			bool check = mPhysicsEngine->CreateDynamicBody(boxInfo, type);
			assert(check);
			mColliderContainer.insert({ id, {mBoxID, boxCollider->shared_from_this(), boxCollider} });
		}
	}

	// 2. Sphere Collider
	if (object->HasComponent<SphereCollider>())
	{
 		auto sphereCollider = object->GetComponent<SphereCollider>();
		auto type = sphereCollider->GetType();
		auto sphereInfo = sphereCollider->GetSphereInfomation();
		auto offset = sphereCollider->GetOffset();

		ColliderID id = ++mLastColliderID;
		sphereInfo.colliderInfo.id = id;
		sphereInfo.colliderInfo.layerNumber = static_cast<int>(object->GetTag());
		sphereInfo.colliderInfo.collisionTransform = transform->GetTransform();
		calculateOffset(sphereInfo.colliderInfo.collisionTransform, offset);
		sphereCollider->SetSphereInfomation(sphereInfo);

		if (isStatic)
		{
			bool check = mPhysicsEngine->CreateStaticBody(sphereInfo, type);
			assert(check);
			mColliderContainer.insert({ id, {mSphereID, sphereCollider->shared_from_this(),sphereCollider} });
		}
		else
		{
			bool check = mPhysicsEngine->CreateDynamicBody(sphereInfo, type);
			assert(check);
			mColliderContainer.insert({ id, {mSphereID, sphereCollider->shared_from_this(),sphereCollider} });
		}
	}

	// 3. Capsule Collider
	if (object->HasComponent<CapsuleCollider>())
	{
		auto capsuleCollider = object->GetComponent<CapsuleCollider>();
		auto type = capsuleCollider->GetType();
		auto capsuleInfo = capsuleCollider->GetCapsuleInfomation();
		auto offset = capsuleCollider->GetOffset();

		ColliderID id = ++mLastColliderID;
		capsuleInfo.colliderInfo.id = id;
		capsuleInfo.colliderInfo.layerNumber = static_cast<int>(object->GetTag());
		capsuleInfo.colliderInfo.collisionTransform = transform->GetTransform();;
		capsuleCollider->SetCapsuleInfomation(capsuleInfo);
		calculateOffset(capsuleInfo.colliderInfo.collisionTransform, offset);

		if (isStatic)
		{
			bool check = mPhysicsEngine->CreateStaticBody(capsuleInfo, type);
			assert(check);
			mColliderContainer.insert({ id, {mCapsuleID, capsuleCollider->shared_from_this(),capsuleCollider} });
		}
		else
		{
			bool check = mPhysicsEngine->CreateDynamicBody(capsuleInfo, type);
			assert(check);
			mColliderContainer.insert({ id, {mCapsuleID, capsuleCollider->shared_from_this(),capsuleCollider} });
		}
	}

	// 5.CharacterController
	if (object->HasComponent<CharacterController>())
	{
		auto controller = object->GetComponent<CharacterController>();
		auto controllerInfo = controller->GetControllerInfo();
		auto movementInfo = controller->GetMovementInfo();
		auto offset = controller->GetOffset();
		auto charaterTransform = transform->GetTransform();

		ColliderID id = ++mLastColliderID;
		controllerInfo.id = id;
		controllerInfo.layerNumber = static_cast<int>(object->GetTag());
		controllerInfo.position = transform->GetWorldPosition() + controller->GetOffset();

		bool check = mPhysicsEngine->CreateCCT(controllerInfo, movementInfo);
		assert(check);

		mColliderContainer.insert({ id, {mCharactorControllerID, controller->shared_from_this(),controller} });
		controller->SetControllerInfo(controllerInfo);
	}

	bool hasStaticMesh = object->HasComponent<StaticMeshRenderer>();
	bool hasSkinnedMesh = object->HasComponent<SkinnedMeshRenderer>();

	// 5. Mesh Collider
	if (object->HasComponent<MeshCollider>() &&
		(hasStaticMesh || hasSkinnedMesh))
	{
		auto meshCollider = object->GetComponent<MeshCollider>();
		auto type = meshCollider->GetType();
		auto convexMeshInfo = meshCollider->GetConvexMeshInfomation();
		auto offset = meshCollider->GetOffset();

		ColliderID id = ++mLastColliderID;
		convexMeshInfo.colliderInfo.id = id;
		convexMeshInfo.colliderInfo.layerNumber = static_cast<int>(object->GetTag());
		convexMeshInfo.colliderInfo.collisionTransform = transform->GetTransform();
		calculateOffset(convexMeshInfo.colliderInfo.collisionTransform, offset);

		if (hasStaticMesh)
		{
			auto staticMeshRenderer = object->GetComponent<StaticMeshRenderer>();
			auto meshName = staticMeshRenderer->GetMeshObjectInfomation().MeshName;
			auto modelPath = staticMeshRenderer->GetMeshObjectInfomation().ModelPath;

			bool check = mGameProcess->mRenderingSystem->IsLoadedModel(modelPath);
			assert(check);

			mGameProcess->mGraphics->GetModel(modelPath);

			const auto& model = mGameProcess->mGraphics->GetModel(modelPath);
			const auto& mesh = ModelSystem::GetMesh(model, meshName);
			convexMeshInfo.vertices = new DirectX::SimpleMath::Vector3[mesh.Vertices.size()];
			convexMeshInfo.vertexSize = mesh.Vertices.size();

			for (int i = 0; i < mesh.Vertices.size(); ++i)
			{
				convexMeshInfo.vertices[i] = -mesh.Vertices[i].Pos;
			}
		}
		else // skinned mesh 
		{
			auto skinnedMeshRenderer = object->GetComponent<SkinnedMeshRenderer>();
			auto modelPath = skinnedMeshRenderer->GetMeshObjectInfomation().ModelPath;
			auto meshName = skinnedMeshRenderer->GetMeshObjectInfomation().MeshName;

			bool check = mGameProcess->mRenderingSystem->IsLoadedModel(modelPath);
			assert(check);

			const auto& model = mGameProcess->mGraphics->GetModel(modelPath);

		}
		meshCollider->SetConvexMeshInfomation(convexMeshInfo);

		if (isStatic)
		{
			bool check = mPhysicsEngine->CreateStaticBody(convexMeshInfo, type);
			assert(check);
			mColliderContainer.insert({ id, {mCapsuleID, meshCollider->shared_from_this(),meshCollider} });
		}
		else
		{
			bool check = mPhysicsEngine->CreateDynamicBody(convexMeshInfo, type);
			assert(check);
			mColliderContainer.insert({ id, {mCapsuleID, meshCollider->shared_from_this(),meshCollider} });
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

	// 4. CharacterController
	if (object->HasComponent<CharacterController>())
	{
		auto controller = object->GetComponent<CharacterController>();
		auto id = controller->GetControllerInfo().id;
		assert(id != physics::unregisterID);

		mPhysicsEngine->RemoveController(id);
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

	assert(data.myId != data.otherId);

	if (!isLfsVaild)
	{
		return;
	}

	auto lhsObject = lfs->second.component->GetGameObject();
	auto rhsObject = isRfsVaild ? rhs->second.component->GetGameObject() : nullptr;

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
	using namespace  DirectX::SimpleMath;

	for (auto& [id, colliderInfo] : mColliderContainer)
	{
		auto transform = colliderInfo.component->GetComponent<fq::game_module::Transform>();
		auto rigid = colliderInfo.component->GetComponent<fq::game_module::RigidBody>();
		auto offset = colliderInfo.collider->GetOffset();

		if (colliderInfo.id == mCharactorControllerID)
		{
			auto controller = colliderInfo.component->GetComponent<fq::game_module::CharacterController>();
			auto controll = mPhysicsEngine->GetCharacterControllerData(id);
			auto movement = mPhysicsEngine->GetCharacterMovementData(id);
			auto localPos = controll.position - controller->GetOffset();

			controller->SetFalling(movement.isFall);
			rigid->SetLinearVelocity(movement.velocity);
			transform->SetLocalPosition(localPos);
		}
		else
		{
			auto data = mPhysicsEngine->GetRigidBodyData(id);
			rigid->SetLinearVelocity(data.linearVelocity);
			rigid->SetAngularVelocity(data.angularVelocity);

			auto matrix = data.transform;

			if (offset != Vector3::Zero)
			{
				Vector3 pos, scale;
				Quaternion rotation;
				matrix.Decompose(scale, rotation, pos);
				matrix._41 = 0.f;
				matrix._42 = 0.f;
				matrix._43 = 0.f;
				offset = Vector3::Transform(offset, matrix);
				pos -= offset;

				matrix._41 = pos.x;
				matrix._42 = pos.y;
				matrix._43 = pos.z;
			}

			transform->SetWorldMatrix(matrix);
		}
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
	using namespace DirectX::SimpleMath;

	for (auto& [id, colliderInfo] : mColliderContainer)
	{
		auto transform = colliderInfo.component->GetComponent<fq::game_module::Transform>();
		auto rigid = colliderInfo.component->GetComponent<fq::game_module::RigidBody>();
		auto offset = colliderInfo.collider->GetOffset();

		if (colliderInfo.id == mCharactorControllerID)
		{
			auto controller = colliderInfo.component->GetComponent<fq::game_module::CharacterController>();

			fq::physics::CharacterControllerGetSetData data;
			data.position = transform->GetWorldPosition() + controller->GetOffset();
			data.rotation = transform->GetWorldRotation();
			data.scale = transform->GetWorldScale();

			mPhysicsEngine->SetCharacterControllerData(id, data);

			fq::physics::CharacterMovementGetSetData moveData;

			moveData.velocity = rigid->GetLinearVelocity();
			moveData.isFall = controller->IsFalling();
			mPhysicsEngine->SetCharacterMovementData(id, moveData);
		}
		else
		{
			fq::physics::RigidBodyGetSetData data;
			data.transform = transform->GetWorldMatrix();
			data.angularVelocity = rigid->GetAngularVelocity();
			data.linearVelocity = rigid->GetLinearVelocity();

			if (offset != Vector3::Zero)
			{
				data.transform._41 = 0.f;
				data.transform._42 = 0.f;
				data.transform._43 = 0.f;

				auto pos = transform->GetWorldPosition();
				offset = Vector3::Transform(offset, data.transform);

				data.transform._41 = pos.x + offset.x;
				data.transform._42 = pos.y + offset.y;
				data.transform._43 = pos.z + offset.z;
			}

			mPhysicsEngine->SetRigidBodyData(id, data);
		}
	}
}

fq::game_module::Component* fq::game_engine::PhysicsSystem::GetCollider(ColliderID id) const
{
	auto iter = mColliderContainer.find(id);

	return iter == mColliderContainer.end() ? nullptr : iter->second.component.get();
}

void fq::game_engine::PhysicsSystem::AddInputMove(const fq::event::AddInputMove& event)
{
	mPhysicsEngine->AddInputMove(event.colliderID, event.input);
}

void fq::game_engine::PhysicsSystem::calculateOffset(common::Transform& t, DirectX::SimpleMath::Vector3 offset)
{
	using namespace DirectX::SimpleMath;

	if (offset == Vector3::Zero) return;

	// 이동성분 제거
	t.worldMatrix._41 = 0.f;
	t.worldMatrix._42 = 0.f;
	t.worldMatrix._43 = 0.f;

	offset = Vector3::Transform(offset, t.worldMatrix);

	t.worldPosition += offset;

	t.worldMatrix._41 = t.worldPosition.x;
	t.worldMatrix._42 = t.worldPosition.y;
	t.worldMatrix._43 = t.worldPosition.z;
}

