#include "BossMonsterDeadState.h"

#include "../FQGameModule/GameModule.h"
#include "../FQGameModule/NavigationAgent.h"
#include "BossMonster.h"

fq::client::BossMonsterDeadState::BossMonsterDeadState()
{

}

fq::client::BossMonsterDeadState::~BossMonsterDeadState()
{

}

void fq::client::BossMonsterDeadState::OnStateEnter(game_module::Animator& animator, game_module::AnimationStateNode& state)
{
	auto agent = animator.GetComponent<game_module::NavigationAgent>();
	agent->Stop();
}

void fq::client::BossMonsterDeadState::OnStateExit(game_module::Animator& animator, game_module::AnimationStateNode& state)
{
	auto scene = animator.GetScene();
	scene->DestroyGameObject(animator.GetGameObject());
	animator.GetComponent<BossMonster>()->DestroryHpBar();
}

std::shared_ptr<fq::game_module::IStateBehaviour> fq::client::BossMonsterDeadState::Clone()
{
	return fq::game_module::ObjectPool::GetInstance()->Assign<BossMonsterDeadState>(*this);
}
