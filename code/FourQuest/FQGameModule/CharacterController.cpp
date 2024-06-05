#define  NOMINMAX
#include "CharacterController.h"

#include <algorithm>

#include "Transform.h"
#include "Scene.h"
#include "InputManager.h"
#include "EventManager.h"
#include "Event.h"

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
	auto input = GetScene()->GetInputManager();
	DirectX::SimpleMath::Vector3 dir{};

	// Ű����


	// ��Ʈ�ѷ�
	dir.x = input->GetStickInfomation(mControllerID, EPadStick::leftX);
	dir.z = input->GetStickInfomation(mControllerID, EPadStick::leftY);

	GetScene()->GetEventManager()
		->FireEvent<fq::event::AddInputMove>({ mControllerInfo.id, dir });
}

void fq::game_module::CharacterController::SetControllerID(ControllerID id)
{
	mControllerID = std::clamp(id, 0u, 3u);
}

void fq::game_module::CharacterController::OnStart()
{
	mTransform = GetComponent<Transform>();
}

