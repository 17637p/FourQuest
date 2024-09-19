#include "BossMonsterWaitTargetState.h"

#include "BossMonster.h"

fq::client::BossMonsterWaitTargetState::BossMonsterWaitTargetState()
{

}

fq::client::BossMonsterWaitTargetState::~BossMonsterWaitTargetState()
{

}

void fq::client::BossMonsterWaitTargetState::OnStateUpdate(game_module::Animator& animator, game_module::AnimationStateNode& state, float dt)
{
	animator.GetComponent<BossMonster>()->SetRandomTarget();
}

std::shared_ptr<fq::game_module::IStateBehaviour> fq::client::BossMonsterWaitTargetState::Clone()
{
	return fq::game_module::ObjectPool::GetInstance()->Assign<BossMonsterWaitTargetState>(*this);
}
