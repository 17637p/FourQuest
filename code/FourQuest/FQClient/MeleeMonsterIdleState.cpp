#include "MeleeMonsterIdleState.h"

#include "../FQGameModule/GameModule.h"
#include "MeleeMonster.h"
#include "ClientHelper.h"

std::shared_ptr<fq::game_module::IStateBehaviour> fq::client::MeleeMonsterIdleState::Clone()
{
	return fq::game_module::ObjectPool::GetInstance()->Assign<MeleeMonsterIdleState>(*this);
}

fq::client::MeleeMonsterIdleState::MeleeMonsterIdleState()
	:mElapsedTime(0.f)
{

}

fq::client::MeleeMonsterIdleState::~MeleeMonsterIdleState()
{

}

void fq::client::MeleeMonsterIdleState::OnStateUpdate(game_module::Animator& animator, game_module::AnimationStateNode& state, float dt)
{
	mElapsedTime += dt;

	auto melee = animator.GetComponent<MeleeMonster>();

	if (mElapsedTime >= mChangeStateTime)
	{
		mElapsedTime = 0.f;
		animator.SetParameterBoolean("OnPatrol", true);
	} 
	auto monster = animator.GetComponent<MeleeMonster>();
	monster->DetectTarget();
}

void fq::client::MeleeMonsterIdleState::OnStateEnter(game_module::Animator& animator, game_module::AnimationStateNode& state)
{
	mElapsedTime = 0.f;
	mChangeStateTime = helper::RandomGenerator::GetInstance().GetRandomNumber(3.f, 10.f);

	auto agent = animator.GetComponent<game_module::NavigationAgent>();
	agent->Stop();
}
