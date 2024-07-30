#include "RazerAttackState.h"
#include "../FQGameModule/GameModule.h"
#include "MagicArmour.h"

fq::client::RazerAttackState::RazerAttackState()
{

}

fq::client::RazerAttackState::~RazerAttackState()
{

}

std::shared_ptr<fq::game_module::IStateBehaviour> fq::client::RazerAttackState::Clone()
{
	return fq::game_module::ObjectPool::GetInstance()->Assign<RazerAttackState>(*this);
}

void fq::client::RazerAttackState::OnStateEnter(game_module::Animator& animator, game_module::AnimationStateNode& state)
{
	// Razer ¼Ò¸® 
	animator.GetScene()->GetEventManager()->FireEvent<fq::event::OnPlaySound>({ "LaserAttack", false , 0 });
}

void fq::client::RazerAttackState::OnStateUpdate(game_module::Animator& animator, game_module::AnimationStateNode& state, float dt)
{
	auto magic = animator.GetComponent<MagicArmour>();
	magic->SetLookAtRStickInput();
	magic->EmitLazer();
}

void fq::client::RazerAttackState::OnStateExit(game_module::Animator& animator, game_module::AnimationStateNode& state)
{

}
