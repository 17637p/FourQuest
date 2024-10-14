#define  NOMINMAX
#include "Controller.h"

#include "Scene.h"
#include "InputManager.h"
#include "EventManager.h"
#include "Event.h"

namespace fq::game_module
{
	Controller::Controller()
	{
	}
	Controller::~Controller()
	{
	}
	void Controller::SetControllerInfo(fq::physics::CharacterControllerInfo info)
	{
		mControllerInfo = info;

		mControllerInfo.contactOffset = std::max(mControllerInfo.contactOffset, 0.0001f);
	}
	void Controller::SetControllerID(unsigned int id)
	{
		mControllerID = std::clamp(id, 0u, 3u);
	}
	void Controller::SetMoveRestriction(std::array<bool, 4> restriction)
	{
		mbMoveRestriction = restriction;
	}

	void Controller::OnStart()
	{
		mTransform = GetComponent<Transform>();
	}

	void Controller::OnFixedUpdate(float dt)
	{
		using namespace DirectX::SimpleMath;

		auto inputMgr = GetScene()->GetInputManager();
		Vector3 input = Vector3::Zero;

		// ��Ʈ�ѷ� �Է�
		input.x = inputMgr->GetStickInfomation(mControllerID, EPadStick::leftX);
		input.z = inputMgr->GetStickInfomation(mControllerID, EPadStick::leftY);

		mbOnMove = input != Vector3::Zero;

		if (mbOnMove)
		{
			input.Normalize();
			GetScene()->GetEventManager()
				->FireEvent<fq::event::AddInputMove>({ mControllerInfo.id, input , true });
		}
	}

	void fq::game_module::Controller::OnCollisionEnter(const Collision& collision)
	{
		++mCollisionCount;
	}

	void fq::game_module::Controller::OnCollisionExit(const Collision& collision)
	{
		if (mCollisionCount == 0) return;

		--mCollisionCount;
	}

	void fq::game_module::Controller::OnTriggerEnter(const Collision& collision)
	{
		++mCollisionCount;
	}

	void fq::game_module::Controller::OnTriggerExit(const Collision& collision)
	{
		if (mCollisionCount == 0) return;

		--mCollisionCount;
	}

	std::shared_ptr<Component> Controller::Clone(std::shared_ptr<Component> clone) const
	{
		auto cloneController = std::dynamic_pointer_cast<Controller>(clone);

		if (cloneController == nullptr) // ���� �����ؼ� ���纻�� �ش�
		{
			cloneController = ObjectPool::GetInstance()->Assign<Controller>(*this);
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
}
