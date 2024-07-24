#include "ShiedlDashState.h"

#include "../FQGameModule/GameModule.h"
#include "KnightArmour.h"

fq::client::ShiedlDashState::~ShiedlDashState()
{

}

fq::client::ShiedlDashState::ShiedlDashState()
{}

std::shared_ptr<fq::game_module::IStateBehaviour> fq::client::ShiedlDashState::Clone()
{
	return fq::game_module::ObjectPool::GetInstance()->Assign<ShiedlDashState>(*this);
}

void fq::client::ShiedlDashState::OnStateEnter(fq::game_module::Animator& animator, fq::game_module::AnimationStateNode& state)
{
	auto knightArmour = animator.GetComponent<KnightArmour>();

	knightArmour->EmitShieldDashAttack();

	// ��Ʈ�ѷ� �Է¹����� �ٶ󺾴ϴ�
	animator.GetComponent<game_module::CharacterController>()->SetPadInputRotation();

	auto transform = animator.GetComponent<game_module::Transform>();
	auto rigid = animator.GetComponent<game_module::RigidBody>();

	auto foward = transform->GetWorldMatrix().Forward();

	foward.Normalize();
	float dashPower = knightArmour->GetShieldDashPower();
	foward.x *= dashPower;
	foward.z *= dashPower;

	rigid->SetLinearVelocity(foward);

	// �÷��̾� �±׸� ����
	animator.GetGameObject()->SetTag(game_module::ETag::Dash);
}

void fq::client::ShiedlDashState::OnStateUpdate(game_module::Animator& animator, game_module::AnimationStateNode& state, float dt)
{

}

void fq::client::ShiedlDashState::OnStateExit(fq::game_module::Animator& animator, fq::game_module::AnimationStateNode& state)
{
	animator.GetGameObject()->SetTag(game_module::ETag::Player);

}
