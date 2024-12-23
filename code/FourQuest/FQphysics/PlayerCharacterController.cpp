#include "PlayerCharacterController.h"

#include "PhysicsCCTHitCallback.h"
#include "CharacterMovement.h"


namespace fq::physics
{
	PlayerCharacterController::PlayerCharacterController()
	{
	}

	PlayerCharacterController::~PlayerCharacterController()
	{
	}

	bool PlayerCharacterController::Initialize(const CharacterControllerInfo& info
		, const CharacterMovementInfo& movementInfo
		, physx::PxControllerManager* CCTManager
		, physx::PxMaterial* material
		, std::shared_ptr<CollisionData> collisionData
		, int* collisionMatrix)
	{
		__super::Initialize(info, movementInfo, CCTManager, material, collisionData, collisionMatrix);


		physx::PxCapsuleControllerDesc desc;
		desc.height = info.height;
		desc.radius = info.radius;
		desc.contactOffset = info.contactOffset;
		desc.stepOffset = info.stepOffset;
		desc.nonWalkableMode = physx::PxControllerNonWalkableMode::ePREVENT_CLIMBING_AND_FORCE_SLIDING;
		desc.slopeLimit = info.slopeLimit;
		desc.maxJumpHeight = 100.f;
		desc.position.x = info.position.x;
		desc.position.y = info.position.y;
		desc.position.z = info.position.z;
		desc.material = material;
		//desc.reportCallback = dynamic_pointer_cast<physx::PxUserControllerHitReport>(mCCTHitCallback).get();
		mPxController = CCTManager->createController(desc);

		physx::PxRigidDynamic* body = mPxController->getActor();
		physx::PxShape* shape;
		int ShapeSize = body->getNbShapes();
		body->getShapes(&shape, ShapeSize);
		body->setSolverIterationCounts(8, 4);
		shape->setContactOffset(0.02f);
		shape->setRestOffset(0.01f);

		physx::PxFilterData filterData;
		filterData.word0 = info.layerNumber;
		filterData.word1 = collisionMatrix[info.layerNumber];
		shape->setSimulationFilterData(filterData);

		collisionData->myId = info.id;
		collisionData->myLayerNumber = info.layerNumber;
		shape->userData = collisionData.get();
		body->userData = collisionData.get();

		return true;
	}
	bool PlayerCharacterController::Update(float deltaTime)
	{
		__super::Update(deltaTime);

		return true;
	}
}
