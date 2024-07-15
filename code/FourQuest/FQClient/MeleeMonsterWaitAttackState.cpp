#include "MeleeMonsterWaitAttackState.h"

#include "../FQGameModule/GameModule.h"
#include "MeleeMonster.h"

std::shared_ptr<fq::game_module::IStateBehaviour> fq::client::MeleeMonsterWaitAttackState::Clone()
{
	return fq::game_module::ObjectPool::GetInstance()->Assign<MeleeMonsterWaitAttackState>(*this);
}

fq::client::MeleeMonsterWaitAttackState::MeleeMonsterWaitAttackState()
{

}

fq::client::MeleeMonsterWaitAttackState::~MeleeMonsterWaitAttackState()
{

}

void fq::client::MeleeMonsterWaitAttackState::OnStateUpdate(game_module::Animator& animator, game_module::AnimationStateNode& state, float dt)
{
	auto monster = animator.GetComponent<MeleeMonster>();

	monster->CheckTargetInAttackRange();
	monster->CheckAttackAble();
}

void fq::client::MeleeMonsterWaitAttackState::OnStateEnter(game_module::Animator& animator, game_module::AnimationStateNode& state)
{
	auto agent = animator.GetComponent<game_module::NavigationAgent>();
	agent->Stop();
}
