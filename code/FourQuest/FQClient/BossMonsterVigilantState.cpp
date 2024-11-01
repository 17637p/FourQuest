#include "BossMonsterVigilantState.h"
#include "BossMonster.h"

#include "../FQGameModule/GameModule.h"
#include "../FQGameModule/NavigationAgent.h"
#include "../FQGameModule/CapsuleCollider.h"

fq::client::BossMonsterVigilantState::BossMonsterVigilantState()
	: mAngryVigilantDuration(1.5f)
	, mVigilantDuration(3.f)
{
}

fq::client::BossMonsterVigilantState::~BossMonsterVigilantState()
{
}

std::shared_ptr<fq::game_module::IStateBehaviour> fq::client::BossMonsterVigilantState::Clone()
{
	return fq::game_module::ObjectPool::GetInstance()->Assign<BossMonsterVigilantState>(*this);
}

void fq::client::BossMonsterVigilantState::OnStateEnter(game_module::Animator& animator, game_module::AnimationStateNode& state)
{
	auto isAngry = animator.GetController().GetParameter("IsAngry").cast<bool>();
	animator.GetComponent<game_module::NavigationAgent>()->Stop();

	mDuration = isAngry ? mAngryVigilantDuration : mVigilantDuration;
	mElapsed = 0.f;
}

void fq::client::BossMonsterVigilantState::OnStateUpdate(game_module::Animator& animator, game_module::AnimationStateNode& state, float dt)
{
	mElapsed += dt;

	// 시간 카운팅
	if (mDuration < mElapsed)
	{
		animator.SetParameterBoolean("OnVigilant", false);
	}
}

void fq::client::BossMonsterVigilantState::OnStateExit(game_module::Animator& animator, game_module::AnimationStateNode& state)
{
}

