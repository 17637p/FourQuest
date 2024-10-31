#include "BossMonsterVigilantCountingState.h"
#include "BossMonster.h"

#include "../FQGameModule/GameModule.h"
#include "../FQGameModule/NavigationAgent.h"
#include "../FQGameModule/CapsuleCollider.h"

fq::client::BossMonsterVigilantCountingState::BossMonsterVigilantCountingState()
{}

fq::client::BossMonsterVigilantCountingState::~BossMonsterVigilantCountingState()
{
}

std::shared_ptr<fq::game_module::IStateBehaviour> fq::client::BossMonsterVigilantCountingState::Clone()
{
	return fq::game_module::ObjectPool::GetInstance()->Assign<BossMonsterVigilantCountingState>(*this);
}

void fq::client::BossMonsterVigilantCountingState::OnStateEnter(game_module::Animator& animator, game_module::AnimationStateNode& state)
{
	auto currentVigilantCount = animator.GetController().GetParameter("VigilantCount").cast<int>() + 1;
	auto bossMonster = animator.GetComponent<BossMonster>();

	auto isAngry = animator.GetController().GetParameter("IsAngry").cast<bool>();

	if (bossMonster != nullptr)
	{
		if (!isAngry)
		{
			if (currentVigilantCount < bossMonster->GetEnteringVigilantCount())
			{
				animator.SetParameterInt("VigilantCount", currentVigilantCount);
			}
			else
			{
				animator.SetParameterBoolean("OnVigilant", true);
				animator.SetParameterInt("VigilantCount", 0);
				bossMonster->GenerateEnteringVigilantCount();
			}
		}
		else
		{
			if (currentVigilantCount < bossMonster->GetAngryEnteringVigilantCount())
			{
				animator.SetParameterInt("VigilantCount", currentVigilantCount);
			}
			else
			{
				animator.SetParameterBoolean("OnVigilant", true);
				animator.SetParameterInt("VigilantCount", 0);
				bossMonster->GenerateAngryEnteringVigilantCount();
			}
		}
	}
}
