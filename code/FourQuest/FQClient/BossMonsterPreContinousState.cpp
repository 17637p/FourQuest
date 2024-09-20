#include "BossMonsterPreContinousState.h"

#include "../FQGameModule/GameModule.h"
#include "../FQGameModule/NavigationAgent.h"
#include "BossMonster.h"

fq::client::BossMonsterPreContinousState::BossMonsterPreContinousState()
{

}

fq::client::BossMonsterPreContinousState::~BossMonsterPreContinousState()
{

}

void fq::client::BossMonsterPreContinousState::OnStateEnter(game_module::Animator& animator, game_module::AnimationStateNode& state)
{
	animator.GetComponent<game_module::NavigationAgent>()->Stop();

	auto boss = animator.GetComponent<BossMonster>();
	boss->StepBack();
}

void fq::client::BossMonsterPreContinousState::OnStateUpdate(game_module::Animator& animator, game_module::AnimationStateNode& state, float dt)
{
	auto boss = animator.GetComponent<BossMonster>();
	boss->HomingTarget();
}

std::shared_ptr<fq::game_module::IStateBehaviour> fq::client::BossMonsterPreContinousState::Clone()
{
	return fq::game_module::ObjectPool::GetInstance()->Assign<BossMonsterPreContinousState>(*this);
}
