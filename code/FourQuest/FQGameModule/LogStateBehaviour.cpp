#include "LogStateBehaviour.h"

#include <spdlog/spdlog.h>
#include "AnimationStateNode.h"
#include "ObjectPool.h"
#include "Animator.h"

void fq::game_module::LogStateBehaviour::OnStateEnter(Animator& animator, AnimationStateNode& state)
{
	++mEnterCount;
	spdlog::info("{} enter {}", animator.GetGameObject()->GetName(),state.GetAnimationKey());
}

void fq::game_module::LogStateBehaviour::OnStateUpdate(Animator& animator, AnimationStateNode& state, float dt)
{
}

void fq::game_module::LogStateBehaviour::OnStateExit(Animator& animator, AnimationStateNode& state)
{
	++mExitCount;
	spdlog::info("{} exit {}", animator.GetGameObject()->GetName(), state.GetAnimationKey());
}

std::shared_ptr<fq::game_module::IStateBehaviour> fq::game_module::LogStateBehaviour::Clone()
{
	return  ObjectPool::GetInstance()->Assign<LogStateBehaviour>(*this);
}

fq::game_module::LogStateBehaviour::LogStateBehaviour()
{}

fq::game_module::LogStateBehaviour::~LogStateBehaviour()
{}
