#include "MeleeMonsterChaseState.h"

#include "../FQGameModule/GameModule.h"
#include "MeleeMonster.h"
#include "ClientHelper.h"

fq::client::MeleeMonsterChaseState::~MeleeMonsterChaseState()
{}

fq::client::MeleeMonsterChaseState::MeleeMonsterChaseState()
{}

std::shared_ptr<fq::game_module::IStateBehaviour> fq::client::MeleeMonsterChaseState::Clone()
{
	return fq::game_module::ObjectPool::GetInstance()->Assign<MeleeMonsterChaseState>(*this);
}

void fq::client::MeleeMonsterChaseState::OnStateUpdate(game_module::Animator& animator, game_module::AnimationStateNode& state, float dt)
{
	auto monster = animator.GetComponent<MeleeMonster>();
	monster->ChaseTarget();
	monster->CheckTargetInAttackRange();
}

void fq::client::MeleeMonsterChaseState::OnStateEnter(game_module::Animator& animator, game_module::AnimationStateNode& state)
{
	// ���Ͱ� ��ô�ϴ� �ִϸ��̼��� ���� �ٸ��� ����
	float duration = state.GetDuration();
	float startTimePos = state.GetStartTimePos();
	float randomStartTimePos = helper::RandomGenerator::GetInstance().GetRandomNumber(startTimePos, duration);
	state.SetStartTimePos(randomStartTimePos);
}
