#include "Animator.h"

#include "../FQReflect/entt.hpp"
#include "AnimatorController.h"

fq::game_module::Animator::Animator()
{

}


fq::game_module::Animator::~Animator()
{

}

std::shared_ptr<fq::game_module::Component> fq::game_module::Animator::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	auto cloneAnimator = std::dynamic_pointer_cast<Animator>(clone);

	if (cloneAnimator == nullptr) // ���� �����ؼ� ���纻�� �ش�
	{
		cloneAnimator = ObjectPool::GetInstance()->Assign<Animator>(*this);
	}
	else // clone�� �����͸� �����Ѵ�.
	{
		// �⺻ ���� ������ ȣ���Ѵ�.
		*cloneAnimator = *this;
	}

	return cloneAnimator;
}

void fq::game_module::Animator::SetController(std::shared_ptr<AnimatorController> controller)
{
	mController = std::move(controller);
}

void fq::game_module::Animator::SetParameterInt(const std::string& id, int val)
{
	if (mController != nullptr)
		mController->SetParameter(id, val);
}

void fq::game_module::Animator::SetParameterFloat(const std::string& id, float val)
{
	if (mController != nullptr)
		mController->SetParameter(id, val);
}

void fq::game_module::Animator::SetParameterBoolean(const std::string& id, bool val)
{
	if (mController != nullptr)
		mController->SetParameter(id, val);
}

void fq::game_module::Animator::SetParameterTrigger(const std::string& id)
{
	if (mController != nullptr)
		mController->SetParameter(id, AnimatorController::OnTrigger);
}

void fq::game_module::Animator::SetParameterOffTrigger(const std::string& id)
{
	if (mController)
		mController->SetParameter(id, AnimatorController::OffTrigger);
}

void fq::game_module::Animator::UpdateState(float dt)
{
	if (mController)
		mController->UpdateState(dt);
}

void fq::game_module::Animator::UpdateAnimation(float dt)
{
	if (mController)
		mController->UpdateAnimation(dt);
}

bool fq::game_module::Animator::IsInTransition() const
{
	return mController->IsInTransition();
}

void fq::game_module::Animator::OnUpdate(float dt)
{
	if (mController)
		mController->Update(dt);
}

