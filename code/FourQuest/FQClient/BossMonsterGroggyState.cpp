#include "BossMonsterGroggyState.h"
#include "BossMonster.h"

#include "../FQGameModule/GameModule.h"
#include "../FQGameModule/NavigationAgent.h"
#include "../FQGameModule/CapsuleCollider.h"

fq::client::BossMonsterGroggyState::BossMonsterGroggyState()
{}

fq::client::BossMonsterGroggyState::~BossMonsterGroggyState()
{
}

std::shared_ptr<fq::game_module::IStateBehaviour> fq::client::BossMonsterGroggyState::Clone()
{
	return fq::game_module::ObjectPool::GetInstance()->Assign<BossMonsterGroggyState>(*this);
}

void fq::client::BossMonsterGroggyState::OnStateEnter(game_module::Animator& animator, game_module::AnimationStateNode& state)
{
	auto agent = animator.GetComponent<game_module::NavigationAgent>();
	agent->Stop();

	animator.GetScene()->GetEventManager()->FireEvent<fq::event::OnPlaySound>({ "MB_Groggy", false ,  fq::sound::EChannel::SE });

	animator.SetParameterBoolean("OnHit", false);
	animator.SetParameterBoolean("IsAnyState", false);

	auto collider = animator.GetComponent<game_module::CapsuleCollider>();

	if (collider != nullptr)
	{
		auto info = collider->GetCapsuleInfomation();

		mPrevCollisionRadius = info.raidus;
		mPrevCollisionOffset = collider->GetOffset();
		info.raidus = mCollisionRadius;
		
		collider->SetCapsuleInfomation(info);
		collider->SetOffset(mCollisionOffset);
	}
}

void fq::client::BossMonsterGroggyState::OnStateUpdate(game_module::Animator& animator, game_module::AnimationStateNode& state, float dt)
{
}

void fq::client::BossMonsterGroggyState::OnStateExit(game_module::Animator& animator, game_module::AnimationStateNode& state)
{
	auto collider = animator.GetComponent<game_module::CapsuleCollider>();

	if (collider != nullptr)
	{
		auto info = collider->GetCapsuleInfomation();
		info.raidus = mPrevCollisionRadius;
		
		collider->SetCapsuleInfomation(info);
		collider->SetOffset(mPrevCollisionOffset);
	}

	animator.SetParameterBoolean("OnHit", false);
	animator.SetParameterBoolean("IsAnyState", true);
}

