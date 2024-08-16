#include "LaserAttackState.h"
#include "../FQGameModule/GameModule.h"
#include "MagicArmour.h"

fq::client::LaserAttackState::LaserAttackState()
	:mLaserEmitTime(1.f)
	, mElapsedTime(0.f)
	, mGatherEffect(nullptr)
{}

fq::client::LaserAttackState::~LaserAttackState()
{

}

std::shared_ptr<fq::game_module::IStateBehaviour> fq::client::LaserAttackState::Clone()
{
	return fq::game_module::ObjectPool::GetInstance()->Assign<LaserAttackState>(*this);
}

void fq::client::LaserAttackState::OnStateEnter(game_module::Animator& animator, game_module::AnimationStateNode& state)
{
	mElapsedTime = 0.f;
	auto magic = animator.GetComponent<MagicArmour>();
	mGatherEffect = magic->EmitLaserGatherEffect();

	// Razer 소리 
	animator.GetScene()->GetEventManager()->FireEvent<fq::event::OnPlaySound>({ "LaserAttack", false , 0 });
}

void fq::client::LaserAttackState::OnStateUpdate(game_module::Animator& animator, game_module::AnimationStateNode& state, float dt)
{
	auto magic = animator.GetComponent<MagicArmour>();
	magic->SetLookAtRStickInput();

	mElapsedTime += dt;

	// Laser 발사 
	if (mElapsedTime >= mLaserEmitTime)
	{
		magic->EmitLaser();

		if (mLaserHeadEffect == nullptr)
		{
			mLaserHeadEffect = magic->EmitLaserHeadEffect();
		}
		if (mLaserTailEffect == nullptr)
		{
			mLaserTailEffect = magic->EmitLaserTailEffect();
		}
		if (mGatherEffect != nullptr)
		{
			animator.GetScene()->DestroyGameObject(mGatherEffect.get());
			mGatherEffect = nullptr;
		}
	}
}

void fq::client::LaserAttackState::OnStateExit(game_module::Animator& animator, game_module::AnimationStateNode& state)
{
	if (mGatherEffect != nullptr)
	{
		animator.GetScene()->DestroyGameObject(mGatherEffect.get());
		mGatherEffect = nullptr;
	}

	if (mLaserHeadEffect != nullptr)
	{
		animator.GetScene()->DestroyGameObject(mLaserHeadEffect.get());
		mLaserHeadEffect = nullptr;
	}

	if (mLaserTailEffect != nullptr)
	{
		animator.GetScene()->DestroyGameObject(mLaserTailEffect.get());
		mLaserTailEffect = nullptr;
	}


}
