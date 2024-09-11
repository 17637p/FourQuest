#include "BossMonsterDeadState.h"

#include "../FQGameModule/GameModule.h"
#include "../FQGameModule/NavigationAgent.h"
#include "BossMonster.h"

#include "ClientEvent.h"
#include "../FQGameModule/EventManager.h"
#include "../FQGameModule/Event.h"

#include "ArmourSpawner.h"

fq::client::BossMonsterDeadState::BossMonsterDeadState()
{

}

fq::client::BossMonsterDeadState::~BossMonsterDeadState()
{

}

void fq::client::BossMonsterDeadState::OnStateEnter(game_module::Animator& animator, game_module::AnimationStateNode& state)
{
	auto agent = animator.GetComponent<game_module::NavigationAgent>();
	agent->Stop();
}

void fq::client::BossMonsterDeadState::OnStateExit(game_module::Animator& animator, game_module::AnimationStateNode& state)
{
	auto scene = animator.GetScene();
	scene->DestroyGameObject(animator.GetGameObject());
	animator.GetComponent<BossMonster>()->DestroryHpBar();

	// 몬스터 처리 이벤트
	scene->GetEventManager()->FireEvent<client::event::KillMonster>(
		{ EMonsterType::Boss });

	// 죽었는데 ArmourSpawner 컴포넌트가 있을 경우 갑옷 소환
	auto armourSpawner = animator.GetComponent<ArmourSpawner>();
	if (armourSpawner)
	{
		armourSpawner->SpawnArmour();
	}
}

std::shared_ptr<fq::game_module::IStateBehaviour> fq::client::BossMonsterDeadState::Clone()
{
	return fq::game_module::ObjectPool::GetInstance()->Assign<BossMonsterDeadState>(*this);
}
