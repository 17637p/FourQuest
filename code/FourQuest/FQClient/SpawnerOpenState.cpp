#include "SpawnerOpenState.h"

#include "../FQGameModule/GameModule.h"
#include "MonsterSpawner.h"

fq::client::SpawnerOpenState::SpawnerOpenState()
	:mSpwanTiming(1.f)
	,mElapsedTime(0.f)
{

}

fq::client::SpawnerOpenState::~SpawnerOpenState()
{

}


void fq::client::SpawnerOpenState::OnStateEnter(game_module::Animator& animator, game_module::AnimationStateNode& state)
{
	mElapsedTime = 0.f;
}

void fq::client::SpawnerOpenState::OnStateUpdate(game_module::Animator& animator, game_module::AnimationStateNode& state, float dt)
{
	if (mElapsedTime == mSpwanTiming)
		return;

	mElapsedTime += dt;

	// 공격 발동 
	if (mElapsedTime >= mSpwanTiming)
	{
		animator.GetComponent<MonsterSpawner>()->Spawn();
		mElapsedTime = mSpwanTiming;
	}

}

std::shared_ptr<fq::game_module::IStateBehaviour> fq::client::SpawnerOpenState::Clone()
{
	return fq::game_module::ObjectPool::GetInstance()->Assign<SpawnerOpenState>(*this);
}
