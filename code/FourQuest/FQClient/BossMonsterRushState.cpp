#include "BossMonsterRushState.h"

#include "../FQGameModule/GameModule.h"
#include "BossMonster.h"

fq::client::BossMonsterRushState::BossMonsterRushState()
	:mRushHitBox(nullptr)
{}

fq::client::BossMonsterRushState::~BossMonsterRushState()
{}

void fq::client::BossMonsterRushState::OnStateEnter(game_module::Animator& animator, game_module::AnimationStateNode& state)
{
	animator.GetGameObject()->SetTag(game_module::ETag::Dash);
	mRushHitBox = animator.GetComponent<BossMonster>()->Rush();
}

void fq::client::BossMonsterRushState::OnStateExit(game_module::Animator& animator, game_module::AnimationStateNode& state)
{
	if (mRushHitBox)
	{
		animator.GetScene()->DestroyGameObject(mRushHitBox.get());
		mRushHitBox = nullptr;
	}

	animator.GetGameObject()->SetTag(game_module::ETag::Monster);
	animator.GetComponent<BossMonster>()->EndPattern();
}

std::shared_ptr<fq::game_module::IStateBehaviour> fq::client::BossMonsterRushState::Clone()
{
	return fq::game_module::ObjectPool::GetInstance()->Assign<BossMonsterRushState>(*this);
}
