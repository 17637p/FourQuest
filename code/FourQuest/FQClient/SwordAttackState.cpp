#define  NOMINMAX
#include "SwordAttackState.h"

#include "../FQGameModule/GameModule.h"
#include "KnightArmour.h"
#include "Player.h"

fq::client::SwordAttackState::SwordAttackState()
	:mAttackTiming(1.f)
	, mElapsedTime(0.f)
{}

fq::client::SwordAttackState::~SwordAttackState()
{}

void fq::client::SwordAttackState::OnStateEnter(game_module::Animator& animator, game_module::AnimationStateNode& state)
{
	animator.GetComponent<game_module::CharacterController>()->SetPadInputRotation();
	float dashPower = animator.GetComponent<KnightArmour>()->GetXAttackDashPower();

	// 컨트롤러 입력방향을 바라봅니다
	auto controller = animator.GetComponent<game_module::CharacterController>();
	controller->SetPadInputRotation();
	controller->SetDashInput(true);

	// 공격 전진 
	auto transform = animator.GetComponent<game_module::Transform>();
	auto rigid = animator.GetComponent<game_module::RigidBody>();
	auto foward = transform->GetWorldMatrix().Forward();
	foward.Normalize();
	foward.x *= dashPower;
	foward.z *= dashPower;
	rigid->SetLinearVelocity(foward);
	

	mElapsedTime = 0.f;

	// 검 공격 이펙트 생성
	auto knightArmour = animator.GetComponent<KnightArmour>();
	mEffect = knightArmour->EmitSwordEffect();
	auto uvAnimator = mEffect->GetComponent<game_module::UVAnimator>();

	// UV 재생속도 
	float statePlayebackSpeed = state.GetPlayBackSpeed();
	float defaultSpeed = animator.GetPlaySpeed();
  	uvAnimator->SetPlaySpeed(statePlayebackSpeed * defaultSpeed);
}

void fq::client::SwordAttackState::OnStateUpdate(game_module::Animator& animator, game_module::AnimationStateNode& state, float dt)
{
	if (mElapsedTime == mAttackTiming)
	{
		return;
	}

	mElapsedTime = std::min(mElapsedTime + dt * state.GetPlayBackSpeed(), mAttackTiming);

	if (mElapsedTime == mAttackTiming)
	{
		auto knightArmour = animator.GetComponent<KnightArmour>();
		knightArmour->EmitSwordAttack();
	}
}

void fq::client::SwordAttackState::OnStateExit(game_module::Animator& animator, game_module::AnimationStateNode& state)
{
	mEffect->GetScene()->DestroyGameObject(mEffect.get());
	mEffect = nullptr;

	auto controller = animator.GetComponent<game_module::CharacterController>();
	controller->SetDashInput(false);
}

std::shared_ptr<fq::game_module::IStateBehaviour> fq::client::SwordAttackState::Clone()
{
	return fq::game_module::ObjectPool::GetInstance()->Assign<SwordAttackState>(*this);
}
