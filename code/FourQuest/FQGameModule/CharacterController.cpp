#define  NOMINMAX
#include "CharacterController.h"

#include <algorithm>

#include "Transform.h"
#include "Scene.h"
#include "InputManager.h"
#include "TimeManager.h"
#include "EventManager.h"
#include "Event.h"
#include "RigidBody.h"

std::shared_ptr<fq::game_module::Component> fq::game_module::CharacterController::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	auto cloneController = std::dynamic_pointer_cast<CharacterController>(clone);

	if (cloneController == nullptr) // 새로 생성해서 복사본을 준다
	{
		cloneController = ObjectPool::GetInstance()->Assign<CharacterController>(*this);
	}
	else // clone에 데이터를 복사한다.
	{
		// 기본 대입 연산자 호출한다.
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

	if (mControllerInfo.id == fq::physics::unregisterID)
		return;


	auto inputMgr = GetScene()->GetInputManager();
	Vector3 input = Vector3::Zero;

	// 컨트롤러 입력
	input.x = inputMgr->GetStickInfomation(mControllerID, EPadStick::leftX);
	input.z = inputMgr->GetStickInfomation(mControllerID, EPadStick::leftY);

	// 캐릭터 컨트롤러 이동 처리 
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

	// 컨트롤러 스틱을 조작하 땔때 반동으로 생기는 미세한 방향설정을 무시하는 값
	constexpr float rotationOffsetSq = 0.5f * 0.5f;

	// 캐릭터 컨트롤러 회전 처리
	if (mbOnRotation && lengthSq >= rotationOffsetSq)
	{
		// 바라보는 방향 설정 
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

void fq::game_module::CharacterController::OnLateUpdate(float dt)
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


void fq::game_module::CharacterController::SetPadInputRotationBySpeed(EPadStickType padStickType, float rotationSpeed, float dt)
{
	using namespace DirectX::SimpleMath;

	// 일시정지 예외처리 
	if (dt == 0.f)
		return;

	auto inputMgr = GetScene()->GetInputManager();
	Vector3 input = Vector3::Zero;

	// 컨트롤러 입력 
	switch (padStickType)
	{
		case fq::game_module::EPadStickType::Left:
			input.x = inputMgr->GetStickInfomation(mControllerID, EPadStick::leftX);
			input.z = inputMgr->GetStickInfomation(mControllerID, EPadStick::leftY);
			break;
		case fq::game_module::EPadStickType::Right: // 우측 방향키로 회전 값을 가지는 경우가 생겨 해당 부분 수정
			input.x = inputMgr->GetStickInfomation(mControllerID, EPadStick::rightX);
			input.z = inputMgr->GetStickInfomation(mControllerID, EPadStick::rightY);
			break;
	}

	// 입력 최소치 보정 값 
	constexpr float rotationOffsetSq = 0.5f * 0.5f;
	if (input.LengthSquared() <= rotationOffsetSq)
		return;

	input.Normalize();
	Vector3 look = mTransform->GetLookAtVector();
	float inputAngle = atan2f(-input.x, -input.z);

	Quaternion inputRotation = Quaternion::CreateFromYawPitchRoll(inputAngle, 0.f, 0.f);
	Quaternion lookRotation = mTransform->GetWorldRotation();

	float diffAngle = Quaternion::Angle(inputRotation, lookRotation);

	// 미세한 각도차이는 무시합니다. (무시하지 않으면 미세한 떨림 발생)
	constexpr float IgnoreAngle = 6.2f;
	if (diffAngle >= IgnoreAngle)
	{
		mTransform->SetWorldRotation(inputRotation);
		return;
	}

	float deltaSpeed = rotationSpeed * DirectX::XM_2PI * dt;

	// 가까운 회전 방향 판단 
	if (diffAngle > DirectX::XM_PI)
	{
		deltaSpeed *= -1.f;
	}

	lookRotation.RotateTowards(inputRotation, deltaSpeed);
	mTransform->SetWorldRotation(lookRotation);
}



void fq::game_module::CharacterController::SetPadInputRotation(EPadStickType padStickType/* = EPadStickType::Left */)
{
	// 일시정지 예외처리 
	if (GetScene()->GetTimeManager()->GetTimeScale() == 0.f)
		return;

	using namespace DirectX::SimpleMath;

	auto inputMgr = GetScene()->GetInputManager();
	Vector3 input = Vector3::Zero;

	// 컨트롤러
	switch (padStickType)
	{
		case fq::game_module::EPadStickType::Left:
			input.x = inputMgr->GetStickInfomation(mControllerID, EPadStick::leftX);
			input.z = inputMgr->GetStickInfomation(mControllerID, EPadStick::leftY);
			break;
		case fq::game_module::EPadStickType::Right: // 우측 방향키로 회전 값을 가지는 경우가 생겨 해당 부분 수정
			input.x = inputMgr->GetStickInfomation(mControllerID, EPadStick::rightX);
			input.z = inputMgr->GetStickInfomation(mControllerID, EPadStick::rightY);
			break;
		default:
			assert(false);
			break;
	}
	input.Normalize();

	if (input.z >= 1.f)
	{
		auto rotation = Quaternion::LookRotation({ 0.f,0.f,1.f }, { 0.f,-1.f,0.f });
		rotation.Normalize();
		mTransform->SetLocalRotation(rotation);
	}
	else if (input != Vector3::Zero)
	{
		auto rotation = Quaternion::LookRotation(input, { 0.f,1.f,0.f });
		rotation.Normalize();
		mTransform->SetLocalRotation(rotation);
	}
}

void fq::game_module::CharacterController::SetMoveRestriction(std::array<bool, 4> restriction)
{
	mbMoveRestriction = restriction;
}

