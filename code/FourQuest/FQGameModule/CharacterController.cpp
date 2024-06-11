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

	// ��Ʈ�ѷ�
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

	// ��Ʈ�ѷ� ��ƽ�� ������ ���� �ݵ����� ����� �̼��� ���⼳���� �����ϴ� ��
	constexpr float rotationOffsetSq = 0.5f * 0.5f;
	if (mbOnRotation && lengthSq >= rotationOffsetSq)
	{
		// �ٶ󺸴� ���� ���� 
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

