#include "MeleeMonsterExplosionState.h"

#include "../FQGameModule/GameModule.h"
#include "../FQGameModule/NavigationAgent.h"
#include "MeleeMonsterExplosion.h"
#include "KnockBack.h"

fq::client::MeleeMonsterExplosionState::MeleeMonsterExplosionState()
{

}

fq::client::MeleeMonsterExplosionState::~MeleeMonsterExplosionState()
{

}

void fq::client::MeleeMonsterExplosionState::OnStateEnter(game_module::Animator& animator, game_module::AnimationStateNode& state)
{
	auto agent = animator.GetComponent<game_module::NavigationAgent>();
	agent->Stop();

	mWarningUI = animator.GetComponent<MeleeMonsterExplosion>()->EmitExplosionWaringUI();
}

void fq::client::MeleeMonsterExplosionState::OnStateExit(game_module::Animator& animator, game_module::AnimationStateNode& state)
{
	animator.GetScene()->DestroyGameObject(mWarningUI.get());
	mWarningUI = nullptr;

	animator.GetComponent<MeleeMonsterExplosion>()->Explode();
}

std::shared_ptr<fq::game_module::IStateBehaviour> fq::client::MeleeMonsterExplosionState::Clone()
{
	return fq::game_module::ObjectPool::GetInstance()->Assign<MeleeMonsterExplosionState>(*this);
}
