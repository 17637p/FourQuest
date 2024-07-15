#include "MeleeMonsterDeadState.h"

#include "../FQGameModule/GameModule.h"

fq::client::MeleeMonsterDeadState::MeleeMonsterDeadState()
{

}

fq::client::MeleeMonsterDeadState::~MeleeMonsterDeadState()
{

}

std::shared_ptr<fq::game_module::IStateBehaviour> fq::client::MeleeMonsterDeadState::Clone()
{
	return fq::game_module::ObjectPool::GetInstance()->Assign<MeleeMonsterDeadState>(*this);

}

void fq::client::MeleeMonsterDeadState::OnStateExit(game_module::Animator& animator, game_module::AnimationStateNode& state)
{
	auto scene = animator.GetScene();
	scene->DestroyGameObject(animator.GetGameObject());
}

void fq::client::MeleeMonsterDeadState::OnStateEnter(game_module::Animator& animator, game_module::AnimationStateNode& state)
{
	auto agent = animator.GetComponent<game_module::NavigationAgent>();
	agent->Stop();

	auto gameObject = animator.GetGameObject();

	gameObject->RemoveComponent<game_module::NavigationAgent>();
	gameObject->RemoveComponent<game_module::CapsuleCollider>();
}
