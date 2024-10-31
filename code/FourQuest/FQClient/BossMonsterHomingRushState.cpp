#include "BossMonsterHomingRushState.h"
#include "BossMonster.h"

#include "../FQGameModule/Scene.h"

fq::client::BossMonsterHomingRushState::BossMonsterHomingRushState()
{

}

fq::client::BossMonsterHomingRushState::~BossMonsterHomingRushState()
{

}

void fq::client::BossMonsterHomingRushState::OnStateEnter(game_module::Animator& animator, game_module::AnimationStateNode& state)
{
	auto bossMonster = animator.GetComponent<BossMonster>();

	if (bossMonster != nullptr)
	{
		mRushDecalEffect = bossMonster->EmitRushDecalEffect();
	}
}

void fq::client::BossMonsterHomingRushState::OnStateExit(game_module::Animator& animator, game_module::AnimationStateNode& state)
{
	if (mRushDecalEffect != nullptr)
	{
		animator.GetScene()->DestroyGameObject(mRushDecalEffect.get());
		mRushDecalEffect = nullptr;
	}
}

void fq::client::BossMonsterHomingRushState::OnStateUpdate(game_module::Animator& animator, game_module::AnimationStateNode& state, float dt)
{

}

std::shared_ptr<fq::game_module::IStateBehaviour> fq::client::BossMonsterHomingRushState::Clone()
{
	return fq::game_module::ObjectPool::GetInstance()->Assign<BossMonsterHomingRushState>(*this);
}
