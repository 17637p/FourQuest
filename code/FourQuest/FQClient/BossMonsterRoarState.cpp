#include "BossMonsterRoarState.h"

#include "../FQGameModule/GameModule.h"
#include "../FQGameModule/NavigationAgent.h"
#include "BossMonster.h"
#include "SpawnStruct.h"

fq::client::BossMonsterRoarState::BossMonsterRoarState()
	:mPosition{ 0.f,0.f,0.f }
	, mMeleeNumber(1)
	, mExplosionNumber(0)
{}

fq::client::BossMonsterRoarState::~BossMonsterRoarState()
{

}

void fq::client::BossMonsterRoarState::OnStateEnter(game_module::Animator& animator, game_module::AnimationStateNode& state)
{
	auto agent = animator.GetComponent<game_module::NavigationAgent>();
	agent->Stop();

	// 몬스터 소환 요청 이벤트 
	auto event = animator.GetScene()->GetEventManager();
	event->FireEvent<RequestSpawnMonster>({ EMonsterType::Melee , mMeleeNumber, mPosition });
	event->FireEvent<RequestSpawnMonster>({ EMonsterType::Explosion, mExplosionNumber, mPosition });
}

void fq::client::BossMonsterRoarState::OnStateExit(game_module::Animator& animator, game_module::AnimationStateNode& state)
{
	auto boss = animator.GetComponent<BossMonster>();
	boss->EndPattern();
}

std::shared_ptr<fq::game_module::IStateBehaviour> fq::client::BossMonsterRoarState::Clone()
{
	return fq::game_module::ObjectPool::GetInstance()->Assign<BossMonsterRoarState>(*this);
}
