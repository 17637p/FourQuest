#include "PlantMonsterDeadState.h"

#include "../FQGameModule/GameModule.h"
#include "../FQGameModule/CapsuleCollider.h"

#include "ClientEvent.h"
#include "../FQGameModule/EventManager.h"
#include "../FQGameModule/Event.h"

fq::client::PlantMonsterDeadState::PlantMonsterDeadState()
{}

fq::client::PlantMonsterDeadState::~PlantMonsterDeadState()
{}

std::shared_ptr<fq::game_module::IStateBehaviour> fq::client::PlantMonsterDeadState::Clone()
{
	return fq::game_module::ObjectPool::GetInstance()->Assign<PlantMonsterDeadState>(*this);
}

void fq::client::PlantMonsterDeadState::OnStateExit(game_module::Animator& animator, game_module::AnimationStateNode& state)
{
	auto scene = animator.GetScene();
	scene->DestroyGameObject(animator.GetGameObject());

	// 몬스터 죽음 이벤트
	scene->GetEventManager()->FireEvent<client::event::KillMonster>(
		{ EMonsterType::Plant });
}

void fq::client::PlantMonsterDeadState::OnStateEnter(game_module::Animator& animator, game_module::AnimationStateNode& state)
{
	auto gameObject = animator.GetGameObject();
	gameObject->RemoveComponent<game_module::CapsuleCollider>();
}
