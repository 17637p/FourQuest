#include "PhysicsSystem.h"

#include <spdlog/spdlog.h>

#include "../FQGraphics/IFQGraphics.h"
#include "../FQCommon/FQPath.h"
#include "../FQphysics/IFQPhysics.h"
#include "../FQReflect/entt.hpp"
#include "../FQGameModule/GameModule.h"
#include "../FQGameModule/RigidBody.h"
#include "../FQGameModule/CharacterController.h"
#include "../FQGameModule/BoxCollider.h"
#include "../FQGameModule/SphereCollider.h"
#include "../FQGameModule/MeshCollider.h"
#include "../FQGameModule/CapsuleCollider.h"
#include "../FQGameModule/TerrainCollider.h"
#include "../FQGameModule/Terrain.h"
#include "../FQGameModule/Articulation.h"
#include "../FQGameModule/Transform.h"
#include "../FQGameModule/SkinnedMeshRenderer.h"
#include "../FQGameModule/StaticMeshRenderer.h"
#include "../FQClient/MonsterVariable.h"

#include "ResourceSystem.h"
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
	, mBoxTypeID(0)
	, mSphereTypeID()
	, mCapsuleTypeID(0)
	, mMeshTypeID(0)
	, mCharactorControllerTypeID(0)
	, mTerrainTypeID(0)
	, mRigidTypeID(0)
	, mRaycastHandler{}
	, mArticulationTypeID(0)
	, mAddInputMoveHandler{}
	, mOneFrameRagdollCreateCount(0)
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
	mOnCleanUpSceneHandler = mGameProcess->mEventManager->
		RegisterHandle<fq::event::OnCleanUp>(this, &PhysicsSystem::CleanUp);
	mRaycastHandler = mGameProcess->mEventManager->
		RegisterHandle<fq::event::RayCast>(this, &PhysicsSystem::Raycast);

	mBoxTypeID = entt::resolve<fq::game_module::BoxCollider>().id();
	mSphereTypeID = entt::resolve<fq::game_module::SphereCollider>().id();
	mCapsuleTypeID = entt::resolve<fq::game_module::CapsuleCollider>().id();
	mMeshTypeID = entt::resolve<fq::game_module::MeshCollider>().id();
	mCharactorControllerTypeID = entt::resolve<fq::game_module::CharacterController>().id();
	mRigidTypeID = entt::resolve<game_module::RigidBody>().id();
	mTerrainTypeID = entt::resolve<game_module::TerrainCollider>().id();
	mArticulationTypeID = entt::resolve<game_module::Articulation>().id();
}


void fq::game_engine::PhysicsSystem::OnUnLoadScene()
{
	mbIsGameLoaded = false;

	for (auto& [id, info] : mColliderContainer)
	{
		info.bIsDestroyed = true;
	}

	PostUpdate();
	mPhysicsEngine->RemoveAllRigidBody();
	mPhysicsEngine->Update(1.f);
	mPhysicsEngine->FinalUpdate();
	mCallbacks.clear();
}

void fq::game_engine::PhysicsSystem::OnLoadScene(const fq::event::OnLoadScene event)
{
	CleanUp({});

	auto scenePath = fq::path::GetScenePath() / event.sceneName / "collision_matrix.txt";
	mCollisionMatrix.Load(scenePath);

	setPhysicsEngineinfo();

	for (auto& object : mScene->GetObjectView())
	{
		addCollider(&object);
	}

	mbIsGameLoaded = true;
}

void fq::game_engine::PhysicsSystem::OnDestroyedGameObject(const fq::event::OnDestoryedGameObject& event)
{
	removeCollider(event.object);
	removeArticulation(event.object);
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
	if (event.id == mBoxTypeID || event.id == mSphereTypeID
		|| event.id == mCapsuleTypeID || event.id == mMeshTypeID
		|| event.id == mRigidTypeID || event.id == mTerrainTypeID)
	{
		addCollider(event.component->GetGameObject());
	}
}

void fq::game_engine::PhysicsSystem::RemoveComponent(const fq::event::RemoveComponent& event)
{
	if (event.id == mBoxTypeID || event.id == mSphereTypeID
		|| event.id == mCapsuleTypeID || event.id == mMeshTypeID
		|| event.id == mRigidTypeID || event.id == mTerrainTypeID)
	{
		removeCollider(event.component->GetGameObject());
	}
}

void fq::game_engine::PhysicsSystem::SetCollisionMatrix(fq::physics::CollisionMatrix matrix)
{
	mCollisionMatrix = matrix;
	setPhysicsEngineinfo();
}

void fq::game_engine::PhysicsSystem::AddTerrainCollider(fq::game_module::GameObject* object)
{
	if (!object->HasComponent<game_module::TerrainCollider>())
		return;

	game_module::TerrainCollider* collider = object->GetComponent<game_module::TerrainCollider>();
	game_module::Transform* transform = object->GetComponent<game_module::Transform>();
	game_module::Terrain* terrain = object->GetComponent<game_module::Terrain>();

	physics::HeightFieldColliderInfo info{};

	ColliderID id = ++mLastColliderID;

	collider->SetColliderID(id);
	info.colliderInfo.id = id;
	info.heightFieldMeshHash = entt::hashed_string(terrain->GetAlphaMap().c_str()).value();
	info.colliderInfo.layerNumber = static_cast<int>(object->GetTag());
	info.colliderInfo.collisionTransform = transform->GetTransform();
	info.height = terrain->GetTerrainMeshObject()->GetHeightData();

	info.heightScale = terrain->GetHeightScale() / 255.f;
	info.numCols = terrain->GetWidth();
	info.numRows = terrain->GetHeight();
	info.rowScale = terrain->GetTextureWidth() / 300.f;
	info.colScale = terrain->GetTextureHeight() / 300.f;

	mPhysicsEngine->CreateStaticBody(info, physics::EColliderType::COLLISION);
	mColliderContainer.insert({ id,
		{mTerrainTypeID
		, collider->shared_from_this()
		,  collider->GetGameObject()->shared_from_this()
		, collider,false} });
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
	auto bodyType = rigid->GetBodyType();

	// 1. Box Colllider
	if (object->HasComponent<BoxCollider>())
	{
		auto boxCollider = object->GetComponent<BoxCollider>();
		auto type = boxCollider->GetType();
		auto boxInfo = boxCollider->GetBoxInfomation();
		auto offset = boxCollider->GetOffset();

		ColliderID colliderID = ++mLastColliderID;
		boxInfo.colliderInfo.id = colliderID;
		boxInfo.colliderInfo.layerNumber = static_cast<int>(object->GetTag());
		boxInfo.colliderInfo.collisionTransform = transform->GetTransform();
		calculateOffset(boxInfo.colliderInfo.collisionTransform, offset);

		boxCollider->SetBoxInfomation(boxInfo);

		if (bodyType == RigidBody::EBodyType::Static)
		{
			bool check = mPhysicsEngine->CreateStaticBody(boxInfo, type);
			assert(check);
			mColliderContainer.insert({ colliderID,
				{mBoxTypeID
				, boxCollider->shared_from_this()
				,  boxCollider->GetGameObject()->shared_from_this()
				, boxCollider,false} });
		}
		else
		{
			bool isKinematic = bodyType == RigidBody::EBodyType::Kinematic;

			bool check = mPhysicsEngine->CreateDynamicBody(boxInfo, type, isKinematic);
			assert(check);
			mColliderContainer.insert({ colliderID,
				{mBoxTypeID
				, boxCollider->shared_from_this()
				, boxCollider->GetGameObject()->shared_from_this()
				, boxCollider
				,false} });
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

		if (bodyType == RigidBody::EBodyType::Static)
		{
			bool check = mPhysicsEngine->CreateStaticBody(sphereInfo, type);
			assert(check);
			mColliderContainer.insert({ id,
				{mSphereTypeID
				, sphereCollider->shared_from_this()
				, sphereCollider->GetGameObject()->shared_from_this()
				,sphereCollider, false} });
		}
		else
		{
			bool isKinematic = bodyType == RigidBody::EBodyType::Kinematic;

			bool check = mPhysicsEngine->CreateDynamicBody(sphereInfo, type, isKinematic);
			assert(check);
			mColliderContainer.insert({ id,
				{mSphereTypeID
				, sphereCollider->shared_from_this()
				, sphereCollider->GetGameObject()->shared_from_this()
				,sphereCollider
				, false} });
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

		if (bodyType == RigidBody::EBodyType::Static)
		{
			bool check = mPhysicsEngine->CreateStaticBody(capsuleInfo, type);
			assert(check);
			mColliderContainer.insert({ id,
				{mCapsuleTypeID
				, capsuleCollider->shared_from_this()
				, object->shared_from_this()
				,capsuleCollider
				,false} });
		}
		else
		{
			bool isKinematic = bodyType == RigidBody::EBodyType::Kinematic;

			bool check = mPhysicsEngine->CreateDynamicBody(capsuleInfo, type, isKinematic);
			assert(check);
			mColliderContainer.insert({ id,
				{mCapsuleTypeID
				, capsuleCollider->shared_from_this()
				,object->shared_from_this()
				,capsuleCollider,false} });
		}
	}

	// 4.CharacterController
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

		mColliderContainer.insert({ id,
			{mCharactorControllerTypeID
			, controller->shared_from_this()
			,object->shared_from_this()
			,controller,false} });
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
			auto meshName = staticMeshRenderer->GetMeshName();
			auto modelPath = staticMeshRenderer->GetModelPath();

			bool check = mGameProcess->mResourceSystem->HasModel(modelPath);
			assert(check);

			const auto& model = mGameProcess->mResourceSystem->GetModel(modelPath);
			const auto& mesh = ModelSystem::GetMesh(model, meshName);
			convexMeshInfo.vertices = new DirectX::SimpleMath::Vector3[mesh.Vertices.size()];
			convexMeshInfo.vertexSize = mesh.Vertices.size();
			convexMeshInfo.convexMeshHash = entt::hashed_string(modelPath.c_str()).value();

			for (int i = 0; i < mesh.Vertices.size(); ++i)
			{
				convexMeshInfo.vertices[i] = mesh.Vertices[i].Pos;
			}
		}
		else // skinned mesh 
		{
			auto skinnedMeshRenderer = object->GetComponent<SkinnedMeshRenderer>();
			auto modelPath = skinnedMeshRenderer->GetModelPath();
			auto meshName = skinnedMeshRenderer->GetMeshName();

			bool check = mGameProcess->mResourceSystem->HasModel(modelPath);
			assert(check);

			const auto& model = mGameProcess->mResourceSystem->GetModel(modelPath);
			const auto& mesh = ModelSystem::GetMesh(model, meshName);
			convexMeshInfo.vertices = new DirectX::SimpleMath::Vector3[mesh.Vertices.size()];
			convexMeshInfo.vertexSize = mesh.Vertices.size();
			convexMeshInfo.convexMeshHash = entt::hashed_string(modelPath.c_str()).value();

			for (int i = 0; i < mesh.Vertices.size(); ++i)
			{	
				convexMeshInfo.vertices[i] = mesh.Vertices[i].Pos;
			}
		}
		meshCollider->SetConvexMeshInfomation(convexMeshInfo);

		if (bodyType == RigidBody::EBodyType::Static)
		{
			bool check = mPhysicsEngine->CreateStaticBody(convexMeshInfo, type);
			assert(check);
			mColliderContainer.insert({ id,
				{mMeshTypeID
				, meshCollider->shared_from_this()
				,object->shared_from_this()
				,meshCollider,false} });
		}
		else
		{
			bool isKinematic = bodyType == RigidBody::EBodyType::Kinematic;

			bool check = mPhysicsEngine->CreateDynamicBody(convexMeshInfo, type, isKinematic);
			assert(check);
			mColliderContainer.insert({ id,
				{mMeshTypeID
				, meshCollider->shared_from_this()
				,object->shared_from_this()
				,meshCollider,false} });
		}
	}

	// 6. Articulation
	if (object->HasComponent<Articulation>())
	{
		ColliderID id = ++mLastColliderID;

		auto articulation = object->GetComponent<Articulation>();
		articulation->Load();
		articulation->SetArticulationID(id);
		auto articulationData = articulation->GetArticulationData();

		ArticulationInfo articulationInfo;
		articulationInfo.id = id;
		articulationInfo.layerNumber = static_cast<int>(object->GetTag());
		articulationInfo.density = articulationData->GetDensity();
		articulationInfo.dynamicFriction = articulationData->GetDynamicFriction();
		articulationInfo.restitution = articulationData->GetRestitution();
		articulationInfo.staticFriction = articulationData->GetStaticFriction();
		articulationInfo.worldTransform = transform->GetWorldMatrix();

		bool check = mPhysicsEngine->CreateCharacterphysics(articulationInfo);
		assert(check);
		mColliderContainer.insert({ id,
			{mArticulationTypeID
			, articulation->shared_from_this()
			, object->shared_from_this()
			, articulation, false} });

		if (object->GetComponent<fq::game_module::Animator>() == nullptr)
			return;

		auto animatorMesh = object->GetComponent<fq::game_module::Animator>();

		if (animatorMesh->GetHasNodeHierarchyInstance() == false)
			return;

		auto objectTransform = object->GetComponent<fq::game_module::Transform>();
		auto objectScale = objectTransform->GetWorldScale();

		auto& nodeHierarchy = animatorMesh->GetNodeHierarchyInstance();
		auto& boneHierarchy = animatorMesh->GetNodeHierarchy();

		nodeHierarchy.SetBindPose();
		nodeHierarchy.UpdateByLocalTransform();

		std::function<void(std::shared_ptr<LinkData>, ColliderID)> loadFunction = [&](std::shared_ptr<LinkData> linkData, ColliderID id)
			{
				fq::physics::LinkInfo linkInfo;

				linkInfo.boneName = linkData->GetBoneName();
				linkInfo.parentBoneName = linkData->GetParentBoneName();
				linkInfo.density = linkData->GetDensity();
				linkInfo.localTransform = linkData->GetLocalTransform();
				linkInfo.worldTransform = linkData->GetWorldTransform();

				if (linkData->GetBoneName() != "root")
				{
					int boneIndex = boneHierarchy.GetBoneIndex(linkData->GetBoneName());
					int parentBoneIndex = boneHierarchy.GetBones()[boneIndex].ParentIndex;
					linkInfo.boneTransform = nodeHierarchy.GetRootTransform(boneIndex);
					linkInfo.jointInfo.localTransform = linkData->GetJointLocalTransform();
				}
				linkInfo.rootTransform = transform->GetWorldMatrix();

				linkInfo.jointInfo.damping = linkData->GetJointDamping();
				linkInfo.jointInfo.maxForce = linkData->GetJointMaxForce();
				linkInfo.jointInfo.stiffness = linkData->GetJointStiffness();
				linkInfo.jointInfo.Swing1AxisInfo.motion = linkData->GetSwing1AxisMotion();
				linkInfo.jointInfo.Swing1AxisInfo.limitsHigh = linkData->GetSwing1LimitHigh();
				linkInfo.jointInfo.Swing1AxisInfo.limitsLow = linkData->GetSwing1LimitLow();
				linkInfo.jointInfo.Swing2AxisInfo.motion = linkData->GetSwing2AxisMotion();
				linkInfo.jointInfo.Swing2AxisInfo.limitsHigh = linkData->GetSwing2LimitHigh();
				linkInfo.jointInfo.Swing2AxisInfo.limitsLow = linkData->GetSwing2LimitLow();
				linkInfo.jointInfo.TwistAxisInfo.motion = linkData->GetTwistAxisMotion();
				linkInfo.jointInfo.TwistAxisInfo.limitsHigh = linkData->GetTwistLimitHigh();
				linkInfo.jointInfo.TwistAxisInfo.limitsLow = linkData->GetTwistLimitLow();

				switch (linkData->GetShapeType())
				{
					case EShapeType::BOX:
					{
						mPhysicsEngine->AddArticulationLink(id, linkInfo, linkData->GetBoxExtent());
					}
					break;
					case EShapeType::SPHERE:
					{
						mPhysicsEngine->AddArticulationLink(id, linkInfo, linkData->GetSphereRadius());
					}
					break;
					case EShapeType::CAPSULE:
					{
						mPhysicsEngine->AddArticulationLink(id, linkInfo, linkData->GetCapsuleHalfHeight(), linkData->GetCapsuleRadius());
					}
					break;
					case EShapeType::END:
					{
						mPhysicsEngine->AddArticulationLink(id, linkInfo);
					}
					break;
				}

				for (auto& [name, childLinkData] : linkData->GetChildrenLinkData())
				{
					loadFunction(childLinkData, id);
				}
			};

		auto rootLinkData = articulationData->GetRootLinkData().lock();

		loadFunction(rootLinkData, id);
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

		mColliderContainer.at(id).bIsDestroyed = true;
	}
	// 2. Sphere Collider
	if (object->HasComponent<SphereCollider>())
	{
		auto sphereCollider = object->GetComponent<SphereCollider>();
		auto id = sphereCollider->GetSphereInfomation().colliderInfo.id;
		assert(id != physics::unregisterID);

		mColliderContainer.at(id).bIsDestroyed = true;
	}
	// 3. Capsule Collider
	if (object->HasComponent<CapsuleCollider>())
	{
		auto sphereCollider = object->GetComponent<CapsuleCollider>();
		auto id = sphereCollider->GetCapsuleInfomation().colliderInfo.id;
		assert(id != physics::unregisterID);

		mColliderContainer.at(id).bIsDestroyed = true;
	}

	// 4. CharacterController
	if (object->HasComponent<CharacterController>())
	{
		auto controller = object->GetComponent<CharacterController>();
		auto id = controller->GetControllerInfo().id;
		assert(id != physics::unregisterID);

		mColliderContainer.at(id).bIsDestroyed = true;
	}

	// 4. Mesh Collider
	if (object->HasComponent<MeshCollider>())
	{
		auto meshCollider = object->GetComponent<MeshCollider>();
		auto id = meshCollider->GetConvexMeshInfomation().colliderInfo.id;
		assert(id != physics::unregisterID);

		mColliderContainer.at(id).bIsDestroyed = true;
	}

	// 5. TerrainCollider 
	if (object->HasComponent<TerrainCollider>())
	{
		auto terrainCollider = object->GetComponent<TerrainCollider>();
		auto id = terrainCollider->GetColliderID();
		assert(id != physics::unregisterID);

		mColliderContainer.at(id).bIsDestroyed = true;
	}
}

void fq::game_engine::PhysicsSystem::removeArticulation(fq::game_module::GameObject* object)
{
	using namespace fq::game_module;
	if (!object->HasComponent<RigidBody>())
	{
		return;
	}

	if (object->HasComponent<Articulation>())
	{
		auto articulation = object->GetComponent<Articulation>();
		auto id = articulation->GetID();
		assert(id != physics::unregisterID);

		mColliderContainer.at(id).bIsDestroyed = true;
	}
}

void fq::game_engine::PhysicsSystem::callBackEvent(fq::physics::CollisionData data, fq::physics::ECollisionEventType type)
{
	mCallbacks.push_back({ data,type });
}


void fq::game_engine::PhysicsSystem::setPhysicsEngineinfo()
{
	fq::physics::PhysicsEngineInfo mPhysicsEngineInfomation;

	for (int i = 0; i < (int)fq::game_module::ETag::End; ++i)
	{
		mPhysicsEngineInfomation.collisionMatrix[i] = static_cast<int>(mCollisionMatrix.data[i].to_ulong());
	}
	mPhysicsEngineInfomation.gravity = mGravity;

	mGameProcess->mPhysics->SetPhysicsInfo(mPhysicsEngineInfomation);
}

void fq::game_engine::PhysicsSystem::SinkToGameScene()
{
	using namespace  DirectX::SimpleMath;

	for (auto& [id, colliderInfo] : mColliderContainer)
	{
		if (colliderInfo.bIsDestroyed)
			continue;

		auto rigid = colliderInfo.component->GetComponent<fq::game_module::RigidBody>();
		auto transform = colliderInfo.component->GetComponent<fq::game_module::Transform>();
		auto offset = colliderInfo.collider->GetOffset();

		if (rigid->GetBodyType() != game_module::RigidBody::EBodyType::Dynamic)
			continue;

		if (colliderInfo.enttID == mCharactorControllerTypeID)
		{
			auto controller = colliderInfo.component->GetComponent<fq::game_module::CharacterController>();
			auto controll = mPhysicsEngine->GetCharacterControllerData(id);
			auto movement = mPhysicsEngine->GetCharacterMovementData(id);
			auto position = controll.position - controller->GetOffset();

			controller->SetFalling(movement.isFall);
			rigid->SetLinearVelocity(movement.velocity);
			transform->SetWorldPosition(position);
		}
		else if (colliderInfo.enttID == mCapsuleTypeID)
		{
			auto data = mPhysicsEngine->GetRigidBodyData(id);
			rigid->SetLinearVelocity(data.linearVelocity);
			rigid->SetAngularVelocity(data.angularVelocity);

			auto matrix = data.transform;
			auto capsule = colliderInfo.component->GetComponent<fq::game_module::CapsuleCollider>();
			auto direct = capsule->GetDirection();

			// 캡슐콜라이더 Y방향으로 수정 
			if (direct == game_module::CapsuleCollider::EDirection::YAxis)
			{
				Vector3 pos, scale;
				Quaternion rotation;
				matrix.Decompose(scale, rotation, pos);
				rotation = game_module::CapsuleCollider::YtoXRoation * rotation;
				rotation.Normalize();

				matrix = Matrix::CreateScale(scale)
					* Matrix::CreateFromQuaternion(rotation)
					* Matrix::CreateTranslation(pos);
			}

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
		else if (colliderInfo.enttID == mArticulationTypeID )
		{
			auto articulation = colliderInfo.component->GetComponent<fq::game_module::Articulation>();
			auto data = mPhysicsEngine->GetArticulationData(id);

			bool bIsRagdoll = data.bIsRagdollSimulation;

			articulation->SetIsRagdoll(bIsRagdoll);

			// Animator의 본에 LocalTransform을 수정하기
			if (bIsRagdoll)
			{
				articulation->AddBlendTime(1.f / 60.f);

				if (colliderInfo.component->GetComponent<fq::game_module::Animator>() == nullptr)
					return;

				auto animatorMesh = colliderInfo.component->GetComponent<fq::game_module::Animator>();

				if (animatorMesh->GetHasNodeHierarchyInstance() == false)
					return;

				auto& nodeHierarchy = animatorMesh->GetNodeHierarchyInstance();
				auto& boneHierarchy = animatorMesh->GetNodeHierarchy();

				auto currentAnimation = animatorMesh->GetController().GetSharedCurrentStateAnimation();
				auto currentAnimationTime = animatorMesh->GetController().GetTimePos();

				animatorMesh->SetStopAnimation(true);

				nodeHierarchy.SetBindPose();

				for (auto& linkData : data.linkData)
				{
					DirectX::SimpleMath::Matrix boneTransform = linkData.jointLocalTransform;
					DirectX::SimpleMath::Vector3 position;
					DirectX::SimpleMath::Quaternion rotation;
					DirectX::SimpleMath::Vector3 scale;
					boneTransform.Decompose(scale, rotation, position);

					position.x = position.x / transform->GetLocalScale().x;
					position.y = position.y / transform->GetLocalScale().y;
					position.z = position.z / transform->GetLocalScale().z;
					boneTransform =
						DirectX::SimpleMath::Matrix::CreateScale(1.f)
						* DirectX::SimpleMath::Matrix::CreateFromQuaternion(rotation)
						* DirectX::SimpleMath::Matrix::CreateTranslation(position);

					unsigned int boneIndex = boneHierarchy.GetBoneIndex(linkData.name);
					nodeHierarchy.SetLocalTransform(boneIndex, boneTransform);
				}

				float blendTime = articulation->GetBlendTime();
				float rotationOffsetX = articulation->GetRotationOffset().x / 180.f * 3.14f;
				float rotationOffsetY = articulation->GetRotationOffset().y / 180.f * 3.14f;
				float rotationOffsetZ = articulation->GetRotationOffset().z / 180.f * 3.14f;

				DirectX::SimpleMath::Matrix dxTransform =
					DirectX::SimpleMath::Matrix::CreateRotationX(std::lerp(0.f, rotationOffsetX, std::clamp(blendTime, 0.f, 1.f)))
					* DirectX::SimpleMath::Matrix::CreateRotationY(std::lerp(0.f, rotationOffsetY, std::clamp(blendTime, 0.f, 1.f)))
					* DirectX::SimpleMath::Matrix::CreateRotationZ(std::lerp(0.f, rotationOffsetZ, std::clamp(blendTime, 0.f, 1.f)))
					* data.worldTransform;

				DirectX::SimpleMath::Vector3 scale;
				DirectX::SimpleMath::Quaternion rotation;
				DirectX::SimpleMath::Vector3 position;
				dxTransform.Decompose(scale, rotation, position);

				transform->SetWorldPosition(position);
				transform->SetWorldRotation(rotation);

				nodeHierarchy.UpdateByLocalTransform(currentAnimationTime + blendTime, currentAnimation, std::max<float>(1 - blendTime, 0));
			}
			else
			{
				articulation->SetBlendTime(0.f);
			}
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


void fq::game_engine::PhysicsSystem::SinkToPhysicsScene()
{
	using namespace DirectX::SimpleMath;
	mOneFrameRagdollCreateCount = 0;

	for (auto& [id, colliderInfo] : mColliderContainer)
	{
		if (colliderInfo.bIsDestroyed)
			continue;

		auto transform = colliderInfo.component->GetComponent<fq::game_module::Transform>();
		auto rigid = colliderInfo.component->GetComponent<fq::game_module::RigidBody>();
		auto offset = colliderInfo.collider->GetOffset();

		if (colliderInfo.enttID == mCharactorControllerTypeID)
		{
			auto controller = colliderInfo.component->GetComponent<fq::game_module::CharacterController>();

			fq::physics::CharacterControllerGetSetData data;
			data.position = transform->GetWorldPosition() + controller->GetOffset();
			data.rotation = transform->GetWorldRotation();
			data.scale = transform->GetWorldScale();

			// Tag가 변경된 경우
			auto controllerInfo = controller->GetControllerInfo();
			auto prevLayer = controllerInfo.layerNumber;
			auto currentLayer = static_cast<unsigned int>(colliderInfo.gameObject->GetTag());
			if (prevLayer != currentLayer)
			{
				data.myLayerNumber = currentLayer;
				controllerInfo.layerNumber = currentLayer;
				controller->SetControllerInfo(controllerInfo);
			}

			mPhysicsEngine->SetCharacterControllerData(id, data);

			fq::physics::CharacterMovementGetSetData moveData;

			auto movementInfo = controller->GetMovementInfo();
			moveData.acceleration = movementInfo.acceleration;
			moveData.maxSpeed = movementInfo.maxSpeed;
			moveData.velocity = rigid->GetLinearVelocity();
			moveData.isFall = controller->IsFalling();
			moveData.restriction = controller->GetMoveRestrction();
			moveData.maxSpeed = controller->GetMovementInfo().maxSpeed;
			mPhysicsEngine->SetCharacterMovementData(id, moveData);

		}
		else if (colliderInfo.enttID == mCapsuleTypeID)
		{
			auto capsule = colliderInfo.component->GetComponent<fq::game_module::CapsuleCollider>();
			auto direct = capsule->GetDirection();
			fq::physics::RigidBodyGetSetData data;
			data.transform = transform->GetWorldMatrix();
			data.angularVelocity = rigid->GetAngularVelocity();
			data.linearVelocity = rigid->GetLinearVelocity();

			// 캡슐콜라이더 Y방향으로 수정 
			if (direct == game_module::CapsuleCollider::EDirection::YAxis)
			{
				Quaternion rotation = game_module::CapsuleCollider::XtoYRoation
					* transform->GetWorldRotation();
				rotation.Normalize();

				Vector3 pos = transform->GetWorldPosition();
				Vector3 scale = transform->GetWorldScale();

				data.transform = Matrix::CreateScale(scale)
					* Matrix::CreateFromQuaternion(rotation)
					* Matrix::CreateTranslation(pos);
			}

			if (offset != Vector3::Zero)
			{
				auto worldMat = transform->GetWorldMatrix();
				worldMat._41 = 0.f;
				worldMat._42 = 0.f;
				worldMat._43 = 0.f;

				auto pos = transform->GetWorldPosition();
				offset = Vector3::Transform(offset, worldMat);

				data.transform._41 = pos.x + offset.x;
				data.transform._42 = pos.y + offset.y;
				data.transform._43 = pos.z + offset.z;
			}

			// Tag가 변경된 경우
			auto capsuleInfo = capsule->GetCapsuleInfomation();
			auto prevLayer = capsuleInfo.colliderInfo.layerNumber;
			auto currentLayer = static_cast<unsigned int>(colliderInfo.gameObject->GetTag());
			if (prevLayer != currentLayer)
			{
				data.myLayerNumber = currentLayer;
				capsuleInfo.colliderInfo.layerNumber = currentLayer;
				capsule->SetCapsuleInfomation(capsuleInfo);
			}

			mPhysicsEngine->SetRigidBodyData(id, data);
		}
		else if (colliderInfo.enttID == mArticulationTypeID)
		{
			auto articulation = colliderInfo.component->GetComponent<fq::game_module::Articulation>();

			if (colliderInfo.component->GetComponent<fq::game_module::Animator>() == nullptr)
				return;

			auto animatorMesh = colliderInfo.component->GetComponent<fq::game_module::Animator>();

			if (animatorMesh->GetHasNodeHierarchyInstance() == false)
				return;

			auto& nodeHierarchy = animatorMesh->GetNodeHierarchyInstance();
			auto& boneHierarchy = animatorMesh->GetNodeHierarchy();

			nodeHierarchy.SetBindPose();

			fq::physics::ArticulationSetData data;
			data.bIsRagdollSimulation = articulation->GetIsRagdoll();
			data.worldTransform = transform->GetWorldMatrix();

			// 새로 생성되는 레그돌만 프레임 갯수 제한, 씬에 레그돌 최대 갯수 제한, 한 프레임에 레그돌 생성 갯수 제한, 레그돌 On/Off
			if ((mGameProcess->mTimeManager->GetFPS() <= client::MonsterVariable::MinFrameCountForRagdoll
				|| client::MonsterVariable::MaxRagdollsPerScene <= mPhysicsEngine->GetArticulationCount()
				|| client::MonsterVariable::MaxOneFrameCreateRagdollCount <= mOneFrameRagdollCreateCount
				|| !client::MonsterVariable::OnRagdoll)
				&& !mPhysicsEngine->GetArticulationData(id).bIsRagdollSimulation)
			{
				articulation->SetIsRagdoll(false);
				data.bIsRagdollSimulation = false;
			}

			if (data.bIsRagdollSimulation)
			{
				mOneFrameRagdollCreateCount++;
			}

			mPhysicsEngine->SetArticulationData(id, data);
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
	physics::CharacterControllerInputInfo info;
	info.id = event.colliderID;
	info.input = event.input;
	info.isDynamic = event.isDynamic;

	mPhysicsEngine->AddInputMove(info);
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

void fq::game_engine::PhysicsSystem::CleanUp(const fq::event::OnCleanUp& event)
{
	for (auto iter = mColliderContainer.begin(); iter != mColliderContainer.end(); )
	{
		if (iter->second.bIsDestroyed
			&& iter->second.bIsRemoveBody)
		{
			iter = mColliderContainer.erase(iter);
		}
		else
			++iter;
	}
}

void fq::game_engine::PhysicsSystem::PostUpdate()
{
	for (auto& [colliderID, info] : mColliderContainer)
	{
		if (info.bIsDestroyed)
		{
			if (info.enttID == mArticulationTypeID)
				mPhysicsEngine->RemoveArticulation(colliderID);
			else if (info.enttID == mCharactorControllerTypeID)
				mPhysicsEngine->RemoveController(colliderID);
			else
				mPhysicsEngine->RemoveRigidBody(colliderID);

			info.bIsRemoveBody = true;
		}
	}
}

void fq::game_engine::PhysicsSystem::ProcessCallBack()
{
	for (auto& [data, type] : mCallbacks)
	{
		auto lfs = mColliderContainer.find(data.myId);
		auto rhs = mColliderContainer.find(data.otherId);

		if (data.myId == data.otherId
			|| lfs == mColliderContainer.end()
			|| rhs == mColliderContainer.end())
		{
			spdlog::warn("ColliderContainer CallBack Error");
			continue; 
		}
		
		auto lhsObject = lfs->second.component->GetGameObject();
		auto rhsObject = rhs->second.component->GetGameObject();

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

	mCallbacks.clear();
}

void fq::game_engine::PhysicsSystem::Raycast(const fq::event::RayCast& event)
{
	physics::RayCastInput rayCastInfo;

	rayCastInfo.direction = event.direction;
	rayCastInfo.distance = event.distance;
	rayCastInfo.layerNumber = static_cast<int>(event.tag);
	rayCastInfo.origin = event.origin;

	auto result = mPhysicsEngine->RayCast(rayCastInfo);
	
	// Block 정보
	if (result.hasBlock)
	{
		event.result->hasBlock = result.hasBlock;
		event.result->blockObject = mColliderContainer.find(result.blockID)->second.gameObject.get();
		event.result->blockPosition = result.blockPosition;
	}

	// Hit 정보
	event.result->hitCount = result.hitSize;
	event.result->hitContactPoints.reserve(result.hitSize);
	event.result->hitObjects.reserve(result.hitSize);
	for (unsigned int i = 0; i < result.hitSize; ++i)
	{
		event.result->hitContactPoints.push_back(result.contectPoints[i]);
		auto object = mColliderContainer.find(result.id[i])->second.gameObject.get();
		event.result->hitObjects.push_back(object);
	}

	if (event.bUseDebugDraw)
	{
		auto renderer = mGameProcess->mGraphics;

		fq::graphics::debug::RayInfo  ray;
		ray.Color = (result.hasBlock == 0) ? DirectX::SimpleMath::Color{ 0.f,1.f,0.f,1.f } : DirectX::SimpleMath::Color{ 1.f,0.f,0.f,1.f };
		ray.Direction = event.direction * event.distance;
		ray.Origin = event.origin;
		ray.Normalize = false;

		renderer->DrawRay(ray);
	}
}
