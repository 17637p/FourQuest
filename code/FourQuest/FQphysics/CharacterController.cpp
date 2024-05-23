#include "CharacterController.h"

#include "CharacterMovement.h"

namespace fq::physics
{
	CharacterController::CharacterController()
		: mID()
		, mLayerNumber()
		, mInputMove()
		, mCharacterMovement(nullptr)
		, mMaterial(nullptr)
		, mPxController(nullptr)
	{
	}

	CharacterController::~CharacterController()
	{
		mPxController->release();
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

		mCharacterMovement = std::make_shared<CharacterMovement>();
		mCharacterMovement->initialize(movementInfo);

		return true;
	}

	bool CharacterController::Update(float deltaTime)
	{
		// �̵��� ���� ����
		physx::PxVec3 dispVector;

		// ĳ���� �����Ʈ ������Ʈ�� �ӵ� ��� �� 
		mCharacterMovement->Update(deltaTime, mInputMove);
		mCharacterMovement->CopyDirectionToPxVec3(dispVector);

		// physx CCT �̵�
		physx::PxControllerCollisionFlags collisionFlags = mPxController->move(dispVector, 0.01f, deltaTime, NULL);

		// �ٴڰ� �浹�� ���Ѵٸ� ������ ���·� üũ
		if (collisionFlags & physx::PxControllerCollisionFlag::eCOLLISION_DOWN)
			mCharacterMovement->SetIsFall(false);
		else
			mCharacterMovement->SetIsFall(true);

		// �Է¹��� �� �ʱ�ȭ
		mInputMove = {};

		return true;
	}

	void CharacterController::AddMovementInput(const DirectX::SimpleMath::Vector3& input)
	{
		if (std::abs(input.x) > 0)
			mInputMove.x = input.x;
		else if (std::abs(input.y) > 0)
			mInputMove.y = input.y;
		else if (std::abs(input.z) > 0)
			mInputMove.z = input.z;
	}
}