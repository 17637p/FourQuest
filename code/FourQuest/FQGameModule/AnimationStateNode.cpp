#include "AnimationStateNode.h"
#include "AnimatorController.h"
#include "IStateBehaviour.h"
#include "LogStateBehaviour.h"


fq::game_module::AnimationStateNode::AnimationStateNode(AnimatorController* controller)
	:mController(controller)
	, mType(Type::State)
	, mPlayBackSpeed(1.f)
	, mAnimationPath{}
	, mAnimationKey{}
	, mbIsLoof(true)
	, mDuration(0.f)
	, mStartTimePos(0.f)
	, mBehaviours{}
{
}

fq::game_module::AnimationStateNode::~AnimationStateNode()
{}

void fq::game_module::AnimationStateNode::OnStateUpdate(float dt)
{
	for (auto& [id, behaviour] : mBehaviours)
	{
		behaviour->OnStateUpdate(*mController->GetAnimator(), *this, dt);
	}
}

void fq::game_module::AnimationStateNode::OnStateExit()
{
	for (auto& [id, behaviour] : mBehaviours)
	{
		behaviour->OnStateExit(*mController->GetAnimator(), *this);
	}
}

void fq::game_module::AnimationStateNode::OnStateEnter()
{
	for (auto& [id, behaviour] : mBehaviours)
	{
		behaviour->OnStateEnter(*mController->GetAnimator(), *this);
	}
}

