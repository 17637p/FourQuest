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
{}

fq::game_module::CharacterController::~CharacterController()
{}

void fq::game_module::CharacterController::SetControllerInfo(fq::physics::CharacterControllerInfo info)
{
	mControllerInfo = info;

	mControllerInfo.contactOffset = std::max(mControllerInfo.contactOffset, 0.0001f);
}

void fq::game_module::CharacterController::OnUpdate(float dt)
{
	if (!mbCanMoveCharater)
	{
		mbOnMove = false;
		return;
	}

	using namespace DirectX::SimpleMath;

	auto inputMgr = GetScene()->GetInputManager();
	Vector3 input = Vector3::Zero;

	// 컨트롤러
	input.x = inputMgr->GetStickInfomation(mControllerID, EPadStick::leftX);
	input.z = inputMgr->GetStickInfomation(mControllerID, EPadStick::leftY);

	if (input == Vector3::Zero)
	{
		mbOnMove = false;
		GetScene()->GetEventManager()
			->FireEvent<fq::event::AddInputMove>({ mControllerInfo.id, input });
		return;
	}

	mbOnMove = true;
	GetScene()->GetEventManager()
		->FireEvent<fq::event::AddInputMove>({ mControllerInfo.id, input });

	float lengthSq = input.LengthSquared();

	// 컨트롤러 스틱을 조작하 땔때 반동으로 생기는 미세한 방향설정을 무시하는 값
	constexpr float rotationOffsetSq = 0.5f * 0.5f;
	if (mbOnRotation && lengthSq >= rotationOffsetSq)
	{
		// 바라보는 방향 설정 
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
}

void fq::game_module::CharacterController::SetControllerID(ControllerID id)
{
	mControllerID = std::clamp(id, 0u, 3u);
}

void fq::game_module::CharacterController::OnStart()
{
	mTransform = GetComponent<Transform>();
}


void fq::game_module::CharacterController::OnCollisionEnter(const Collision& collision)
{
	++mCollisionCount;
}

void fq::game_module::CharacterController::OnCollisionExit(const Collision& collision)
{
	--mCollisionCount;
}

void fq::game_module::CharacterController::OnTriggerEnter(const Collision& collision)
{
	++mCollisionCount;
}

void fq::game_module::CharacterController::OnTriggerExit(const Collision& collision)
{
	--mCollisionCount;
}

