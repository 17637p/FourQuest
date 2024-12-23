#include "LaserAttackState.h"
#include "../FQGameModule/GameModule.h"
#include "MagicArmour.h"
#include "Player.h"
#include "PlayerVariable.h"
#include "../FQGameModule/CharacterController.h"

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
	magic->EnterLaserState();
}

void fq::client::LaserAttackState::OnStateUpdate(game_module::Animator& animator, game_module::AnimationStateNode& state, float dt)
{
	mElapsedTime += dt;

	auto magic = animator.GetComponent<MagicArmour>();
	auto controller = animator.GetComponent<game_module::CharacterController>();
	controller->SetPadInputRotationBySpeed(game_module::EPadStickType::Right, PlayerVariable::PadRotationSpeed, dt);

	// 에임 보정 
	//magic->AimToNearMonster();

	// Laser 발사 
	if (mElapsedTime >= mLaserEmitTime)
	{
		// Laser Effect
		if (mLaserHeadEffect == nullptr)
		{
			mLaserHeadEffect = magic->EmitLaserHeadEffect();
			magic->EmitLaserLineEffect();
		}

		// Raycast 
		magic->EmitLaser();

		if (mGatherEffect != nullptr)
		{
			animator.GetScene()->DestroyGameObject(mGatherEffect.get());
			mGatherEffect = nullptr;
		}
	}

	// 공격시 체력감소 
	animator.GetComponent<Player>()->DecreaseHp(PlayerVariable::HpReductionOnAttack * dt, true, true);
}

void fq::client::LaserAttackState::OnStateExit(game_module::Animator& animator, game_module::AnimationStateNode& state)
{
	if (mGatherEffect != nullptr)
	{
		animator.GetScene()->DestroyGameObject(mGatherEffect.get());
		mGatherEffect = nullptr;
	}

	auto magic = animator.GetComponent<MagicArmour>();
	magic->DestroyLaserEffect();
	mLaserHeadEffect = nullptr;
}
