#include "BossMonsterHitState.h"
#include "BossMonster.h"

#include "../FQGameModule/GameModule.h"
#include "../FQGameModule/NavigationAgent.h"

fq::client::BossMonsterHitState::BossMonsterHitState()
{}

fq::client::BossMonsterHitState::~BossMonsterHitState()
{
}

std::shared_ptr<fq::game_module::IStateBehaviour> fq::client::BossMonsterHitState::Clone()
{
	return fq::game_module::ObjectPool::GetInstance()->Assign<BossMonsterHitState>(*this);
}

void fq::client::BossMonsterHitState::OnStateEnter(game_module::Animator& animator, game_module::AnimationStateNode& state)
{
	animator.GetScene()->GetEventManager()->FireEvent<fq::event::OnPlaySound>({ "MB_Groggy", false ,  fq::sound::EChannel::SE });

	auto bossMonster = animator.GetComponent< BossMonster>();

	if (bossMonster != nullptr)
	{
		bossMonster->SetRimLightColor(true, { 1, 0, 0, 1 });
	}

}

void fq::client::BossMonsterHitState::OnStateExit(game_module::Animator& animator, game_module::AnimationStateNode& state)
{
	auto bossMonster = animator.GetComponent< BossMonster>();

	if (bossMonster != nullptr)
	{
		bossMonster->SetRimLightColor(true, { 0, 0, 0, 1 });
	}
}

