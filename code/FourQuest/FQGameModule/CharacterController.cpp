#include "CharacterController.h"

#include "Scene.h"
#include "InputManager.h"

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
	,mMovementInfo{}
	,mOffset{}
	,mbIsFalling(false)
{}

fq::game_module::CharacterController::~CharacterController()
{

}

