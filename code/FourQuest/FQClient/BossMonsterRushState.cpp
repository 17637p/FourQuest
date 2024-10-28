#include "BossMonsterRushState.h"

#include "../FQGameModule/GameModule.h"
#include "../FQGameModule/Transform.h"
#include "BossMonster.h"
#include "KnockBack.h"

fq::client::BossMonsterRushState::BossMonsterRushState()
	:mRushHitBox(nullptr)
	, mRushVelocity(5.f)
	, mRushAcceleration(5.f)
	, mRushElapsed(0.f)
	, mRushDuration(0.5f)
{}

fq::client::BossMonsterRushState::~BossMonsterRushState()
{}

void fq::client::BossMonsterRushState::OnStateEnter(game_module::Animator& animator, game_module::AnimationStateNode& state)
{
	animator.GetGameObject()->SetTag(game_module::ETag::Dash);
	mRushHitBox = animator.GetComponent<BossMonster>()->Rush();

	auto look = animator.GetTransform()->GetLookAtVector();
	auto bossMonster = animator.GetComponent<BossMonster>();

	if (bossMonster != nullptr)
	{
		auto knockBack = bossMonster->GetKnockBack();
		knockBack->SetKnockBack(mRushVelocity, look);
	}

	animator.GetScene()->GetEventManager()->FireEvent<fq::event::OnPlaySound>({ "MB_Rush", false , fq::sound::EChannel::SE });

	mRushElapsed = 0.f;

	animator.SetParameterBoolean("OnRush", true);
}

void fq::client::BossMonsterRushState::OnStateUpdate(game_module::Animator& animator, game_module::AnimationStateNode& state, float dt)
{
	mRushElapsed += dt;

	auto bossMonster = animator.GetComponent<BossMonster>();
	auto look = animator.GetTransform()->GetLookAtVector();

	if (bossMonster != nullptr)
	{
		auto knockBack = bossMonster->GetKnockBack();
		knockBack->AddKnockBack(mRushAcceleration * dt, look);
	}

	if (mRushDuration < mRushElapsed)
	{
		animator.SetParameterBoolean("OnRush", false);
	}
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
