#include "CharacterController.h"

#include "CharacterMovement.h"
#include "CharacterQueryFilterCallback.h"
#include "PhysicsCollisionDataManager.h"

namespace fq::physics
{
	CharacterController::CharacterController()
		: mID()
		, mLayerNumber()
		, mInputMove()
		, mCharacterMovement(nullptr)
		, mMaterial(nullptr)
		, mPxController(nullptr)
		, mFilters(nullptr)
		, mFilterData(nullptr)
		, mCharacterQueryFilterCallback(nullptr)
		, mbIsDynamic(false)
	{
	}

	CharacterController::~CharacterController()
	{
		CollisionData* data = (CollisionData*)mPxController->getActor()->userData;
		data->isDead = true;

		PX_RELEASE(mPxController);
	}

	bool CharacterController::Initialize(const CharacterControllerInfo& info
		, const CharacterMovementInfo& movementInfo
		, physx::PxControllerManager* CCTManager
		, physx::PxMaterial* material
		, std::shared_ptr<CollisionData> collisionData
		, int* collisionMatrix)
	{
		mID = info.id;
		mLayerNumber = info.layerNumber;
		mMaterial = material;

		mFilterData = std::make_shared<physx::PxFilterData>();
		mFilterData->word0 = 0;

		std::shared_ptr<physx::PxFilterData> data = std::make_shared<physx::PxFilterData>();
		data->word0 = mLayerNumber;
		data->word1 = collisionMatrix[mLayerNumber];
		mCharacterQueryFilterCallback = std::make_shared<CharacterQueryFilterCallback>(data);
		mFilters = std::make_shared<physx::PxControllerFilters>(mFilterData.get(), mCharacterQueryFilterCallback.get());

		mCharacterMovement = std::make_shared<CharacterMovement>();
		mCharacterMovement->initialize(movementInfo);

		return true;
	}

	bool CharacterController::Update(float deltaTime)
	{
		// 이동할 변위 벡터
		physx::PxVec3 dispVector;

		// 캐릭터 무브먼트 업데이트로 속도 계산 후 
		mCharacterMovement->Update(deltaTime, mInputMove, mbIsDynamic);
		mCharacterMovement->CopyDirectionToPxVec3(dispVector);

		// physx CCT 이동
  		physx::PxControllerCollisionFlags collisionFlags = mPxController->move(dispVector, 0.01f, deltaTime, *mFilters.get());

		// 바닥과 충돌을 안한다면 떨어짐 상태로 체크
		if (collisionFlags & physx::PxControllerCollisionFlag::eCOLLISION_DOWN)
		{
			mCharacterMovement->SetIsFall(false);
		}
		else
		{
			mCharacterMovement->SetIsFall(true);
		}

		// 입력받은 값 초기화
		mInputMove = {};
		mbIsDynamic = false;

		return	true;
	}

	void CharacterController::AddMovementInput(const DirectX::SimpleMath::Vector3& input, bool isDynamic)
	{
		if (std::abs(input.x) > 0)
		{
			mInputMove.x = input.x;
		}
		if (std::abs(input.y) > 0)
		{
			mInputMove.y = input.y;
		}
		if (std::abs(input.z) > 0)
		{
			mInputMove.z = input.z;
		}

		mbIsDynamic = isDynamic;
	}

	bool CharacterController::ChangeLayerNumber(const unsigned int& newLayerNumber, int* collisionMatrix, std::weak_ptr<PhysicsCollisionDataManager> collisionDataManager)
	{
		if (newLayerNumber == UINT_MAX)
		{
			return false;
		}

		mLayerNumber = newLayerNumber;

		std::shared_ptr<physx::PxFilterData> newFilterData = std::make_shared<physx::PxFilterData>();
		newFilterData->word0 = mLayerNumber;
		newFilterData->word1 = collisionMatrix[mLayerNumber];
		mCharacterQueryFilterCallback->SetFilterData(newFilterData);

		physx::PxShape* shape;
		mPxController->getActor()->getShapes(&shape, 1);
		shape->setSimulationFilterData(*newFilterData.get());

		CollisionData* data = (CollisionData*)mPxController->getActor()->userData;
		data->myId = mID;
		data->myLayerNumber = mLayerNumber;

		return true;
	}
}