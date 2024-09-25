#define  NOMINMAX
#include "CharacterController.h"

#include <algorithm>

#include "Transform.h"
#include "Scene.h"
#include "InputManager.h"
#include "EventManager.h"
#include "Event.h"
#include "RigidBody.h"

std::shared_ptr<fq::game_module::Component> fq::game_module::CharacterController::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	auto cloneController = std::dynamic_pointer_cast<CharacterController>(clone);

	if (cloneController == nullptr) // ���� �����ؼ� ���纻�� �ش�
	{
		cloneController = ObjectPool::GetInstance()->Assign<CharacterController>(*this);
	}
	else // clone�� �����͸� �����Ѵ�.
	{
		// �⺻ ���� ������ ȣ���Ѵ�.
		*cloneController = *this;
	}

	cloneController->mControllerInfo.id = fq::physics::unregisterID;
	cloneController->mCollisionCount = 0;

	return cloneController;
}

fq::game_module::CharacterController::CharacterController()
	:mControllerInfo{}
	, mMovementInfo{}
	, mOffset{}
	, mbIsFalling(false)
	, mControllerID(0)
	, mTransform(nullptr)
	, mbOnMove(true)
	, mbOnRotation(true)
	, mbCanMoveCharater(true)
	, mCollisionCount(0)
	, mbMoveRestriction{}
	, mFinalSpeedMultiplier(1.f)

{}

fq::game_module::CharacterController::~CharacterController()
{}

void fq::game_module::CharacterController::SetControllerInfo(fq::physics::CharacterControllerInfo info)
{
	mControllerInfo = info;

	mControllerInfo.contactOffset = std::max(mControllerInfo.contactOffset, 0.0001f);
}

void fq::game_module::CharacterController::OnFixedUpdate(float dt)
{
	using namespace DirectX::SimpleMath;

	auto inputMgr = GetScene()->GetInputManager();
	Vector3 input = Vector3::Zero;

	// ��Ʈ�ѷ� �Է�
	input.x = inputMgr->GetStickInfomation(mControllerID, EPadStick::leftX);
	input.z = inputMgr->GetStickInfomation(mControllerID, EPadStick::leftY);

	// ĳ���� ��Ʈ�ѷ� �̵� ó�� 
	if (!mbCanMoveCharater)
	{
		mbOnMove = false;

		if (mbHasDashInput)
		{
			GetScene()->GetEventManager()
				->FireEvent<fq::event::AddInputMove>({ mControllerInfo.id, Vector3::Zero , true });
		}
	}
	else
	{
		mbOnMove = input != Vector3::Zero;
		input.Normalize();
		GetScene()->GetEventManager()
			->FireEvent<fq::event::AddInputMove>({ mControllerInfo.id, input , true });
	}

	float lengthSq = input.LengthSquared();

	// ��Ʈ�ѷ� ��ƽ�� ������ ���� �ݵ����� ����� �̼��� ���⼳���� �����ϴ� ��
	constexpr float rotationOffsetSq = 0.5f * 0.5f;

	// ĳ���� ��Ʈ�ѷ� ȸ�� ó��
	if (mbOnRotation && lengthSq >= rotationOffsetSq)
	{
		// �ٶ󺸴� ���� ���� 
		input.Normalize();

		if (input == Vector3::Backward)
		{
			mTransform->SetWorldRotation(Quaternion::LookRotation(input, { 0.f,-1.f,0.f }));
		}
		else if (input != Vector3::Zero)
		{
			mTransform->SetWorldRotation(Quaternion::LookRotation(input, { 0.f,1.f,0.f }));
		}
	}
}


void fq::game_module::CharacterController::SetControllerID(ControllerID id)
{
	mControllerID = std::clamp(id, 0u, 3u);
}

void fq::game_module::CharacterController::OnStart()
{
	mTransform = GetComponent<Transform>();

	mBaseSpeed = mMovementInfo.maxSpeed;
	mBaseAcceleration = mMovementInfo.acceleration;
}

void fq::game_module::CharacterController::OnUpdate(float dt)
{
	mMovementInfo.maxSpeed = mBaseSpeed * mFinalSpeedMultiplier;
	mMovementInfo.acceleration = mBaseAcceleration * mFinalSpeedMultiplier;

	mFinalSpeedMultiplier = 1.f;
}

void fq::game_module::CharacterController::OnCollisionEnter(const Collision& collision)
{
	++mCollisionCount;
}

void fq::game_module::CharacterController::OnCollisionExit(const Collision& collision)
{
	if (mCollisionCount == 0) return;

	--mCollisionCount;
}

void fq::game_module::CharacterController::OnTriggerEnter(const Collision& collision)
{
	++mCollisionCount;
}

void fq::game_module::CharacterController::OnTriggerExit(const Collision& collision)
{
	if (mCollisionCount == 0) return;

	--mCollisionCount;
}

void fq::game_module::CharacterController::SetPadInputRotation(EPadStickType padStickType/* = EPadStickType::Left */)
{
	using namespace DirectX::SimpleMath;

	auto inputMgr = GetScene()->GetInputManager();
	Vector3 input = Vector3::Zero;

	// ��Ʈ�ѷ�
	switch (padStickType)
	{
	case fq::game_module::EPadStickType::Left:
		input.x = inputMgr->GetStickInfomation(mControllerID, EPadStick::leftX);
		input.z = inputMgr->GetStickInfomation(mControllerID, EPadStick::leftY);
		break;
	case fq::game_module::EPadStickType::Right: // ���� ����Ű�� ȸ�� ���� ������ ��찡 ���� �ش� �κ� ����
		input.x = inputMgr->GetStickInfomation(mControllerID, EPadStick::rightX);
		input.z = inputMgr->GetStickInfomation(mControllerID, EPadStick::rightY);
		break;
	default:
		assert(false);
		break;
	}
	input.Normalize();

	if (input == Vector3::Backward)
	{
		mTransform->SetLocalRotation(Quaternion::LookRotation(input, { 0.f,-1.f,0.f }));
	}
	else if (input != Vector3::Zero)
	{
		mTransform->SetLocalRotation(Quaternion::LookRotation(input, { 0.f,1.f,0.f }));
	}
}

void fq::game_module::CharacterController::SetMoveRestriction(std::array<bool, 4> restriction)
{
	mbMoveRestriction = restriction;
}

