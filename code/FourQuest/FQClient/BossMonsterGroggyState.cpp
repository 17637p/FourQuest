#include "BossMonsterGroggyState.h"

#include "../FQGameModule/GameModule.h"
#include "../FQGameModule/NavigationAgent.h"

fq::client::BossMonsterGroggyState::BossMonsterGroggyState()
	:mGroggyTime(5.f)
	,mGroggyElaspsedTime(0.f)
{}

fq::client::BossMonsterGroggyState::~BossMonsterGroggyState()
{

}

std::shared_ptr<fq::game_module::IStateBehaviour> fq::client::BossMonsterGroggyState::Clone()
{
	return fq::game_module::ObjectPool::GetInstance()->Assign<BossMonsterGroggyState>(*this);
}

void fq::client::BossMonsterGroggyState::OnStateEnter(game_module::Animator& animator, game_module::AnimationStateNode& state)
{
	mGroggyElaspsedTime = 0.f;
	auto agent = animator.GetComponent<game_module::NavigationAgent>();
	agent->Stop();

	animator.GetScene()->GetEventManager()->FireEvent<fq::event::OnPlaySound>({ "MB_Groggy", false ,  fq::sound::EChannel::SE });
}

void fq::client::BossMonsterGroggyState::OnStateExit(game_module::Animator& animator, game_module::AnimationStateNode& state)
{}

void fq::client::BossMonsterGroggyState::OnStateUpdate(game_module::Animator& animator, game_module::AnimationStateNode& state, float dt)
{
	mGroggyElaspsedTime += dt;

	if (mGroggyElaspsedTime >= mGroggyTime)
	{
		animator.SetParameterBoolean("OnGroggy", false);
	}
}

