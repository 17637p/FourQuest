#include "ShieldDashState.h"

#include "../FQGameModule/Transform.h"
#include "../FQGameModule/CharacterController.h"
#include "../FQGameModule/RigidBody.h"
#include "KnightArmour.h"

fq::client::ShieldDashState::~ShieldDashState()
{

}

fq::client::ShieldDashState::ShieldDashState()
{}

std::shared_ptr<fq::game_module::IStateBehaviour> fq::client::ShieldDashState::Clone()
{
	return fq::game_module::ObjectPool::GetInstance()->Assign<ShieldDashState>(*this);
}

void fq::client::ShieldDashState::OnStateEnter(fq::game_module::Animator& animator, fq::game_module::AnimationStateNode& state)
{
	auto knightArmour = animator.GetComponent<KnightArmour>();
	auto controller = animator.GetComponent<game_module::CharacterController>();

	knightArmour->EmitShieldDashAttack();
	// 컨트롤러 입력방향을 바라봅니다
	controller->SetPadInputRotation();
	controller->SetDashInput(true);

	auto transform = animator.GetComponent<game_module::Transform>();
	auto rigid = animator.GetComponent<game_module::RigidBody>();
	auto foward = transform->GetWorldMatrix().Forward();

	foward.Normalize();
	float dashPower = knightArmour->GetShieldDashPower();
	foward.x *= dashPower;
	foward.z *= dashPower;

	rigid->SetLinearVelocity(foward);

	// 플레이어 태그를 변경
	animator.GetGameObject()->SetTag(game_module::ETag::Dash);
}

void fq::client::ShieldDashState::OnStateUpdate(game_module::Animator& animator, game_module::AnimationStateNode& state, float dt)
{

}

void fq::client::ShieldDashState::OnStateExit(fq::game_module::Animator& animator, fq::game_module::AnimationStateNode& state)
{
	animator.GetGameObject()->SetTag(game_module::ETag::Player);

	auto controller = animator.GetComponent<game_module::CharacterController>();
	controller->SetDashInput(false);
}
