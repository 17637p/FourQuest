#define  NOMINMAX
#include "SwordAttackState.h"

#include "../FQGameModule/GameModule.h"
#include "KnightArmour.h"

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
	animator.GetComponent<game_module::CharacterController>()->SetPadInputRotation();

	auto transform = animator.GetComponent<game_module::Transform>();
	auto rigid = animator.GetComponent<game_module::RigidBody>();

	auto foward = transform->GetWorldMatrix().Forward();

	foward.Normalize();
	foward.x *= dashPower;
	foward.z *= dashPower;

	rigid->SetLinearVelocity(foward);

	mElapsedTime = 0.f;
}

void fq::client::SwordAttackState::OnStateUpdate(game_module::Animator& animator, game_module::AnimationStateNode& state, float dt)
{
	if (mElapsedTime == mAttackTiming)
	{
		return;
	}

	mElapsedTime = std::max(mElapsedTime + dt * state.GetPlayBackSpeed(), mAttackTiming);

	if (mElapsedTime == mAttackTiming)
	{
		auto magicArmour = animator.GetComponent<KnightArmour>();
		magicArmour->EmitSwordAttack();
	}
}

void fq::client::SwordAttackState::OnStateExit(game_module::Animator& animator, game_module::AnimationStateNode& state)
{

}

std::shared_ptr<fq::game_module::IStateBehaviour> fq::client::SwordAttackState::Clone()
{
	return fq::game_module::ObjectPool::GetInstance()->Assign<SwordAttackState>(*this);
}
