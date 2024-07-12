#include "AOEAttackState.h"

#include "../FQGameModule/GameModule.h"
#include "MagicArmour.h"

fq::client::AOEAttackState::AOEAttackState()
	:mAttackWarningUI(nullptr)
{}

fq::client::AOEAttackState::~AOEAttackState()
{
}

std::shared_ptr<fq::game_module::IStateBehaviour> fq::client::AOEAttackState::Clone()
{
	return fq::game_module::ObjectPool::GetInstance()->Assign<AOEAttackState>(*this);
}

void fq::client::AOEAttackState::OnStateEnter(game_module::Animator& animator, game_module::AnimationStateNode& state)
{
	auto magicArmour = animator.GetComponent<MagicArmour>();
	auto scene = animator.GetScene();
	auto instance = scene->GetPrefabManager()
		->InstantiatePrefabResoure(magicArmour->GetAttackWarningUI());

	auto& attackUI = *(instance.begin());
	mAttackWarningUI = attackUI.get();

	// UI 위치설정
	auto transform = mAttackWarningUI->GetComponent<game_module::Transform>();


	scene->AddGameObject(attackUI);
}

void fq::client::AOEAttackState::OnStateExit(game_module::Animator& animator, game_module::AnimationStateNode& state)
{

}
