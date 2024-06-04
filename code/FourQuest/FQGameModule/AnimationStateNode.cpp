#include "AnimationStateNode.h"
#include "AnimatorController.h"
#include "IStateBehaviour.h"

fq::game_module::AnimationStateNode::AnimationStateNode(AnimatorController* controller)
	:mController(controller)
	, mType(Type::State)
	, mPlayBackSpeed(1.f)
	, mModelPath{}
	, mAnimationName{}
	, mAnimationKey{}
	, mbIsLoof(true)
	, mDuration(0.f)
	, mBehaviours{}
{}

fq::game_module::AnimationStateNode::~AnimationStateNode()
{}

void fq::game_module::AnimationStateNode::Update(float dt)
{
	for (auto& [id, behaviour] : mBehaviours)
	{
		behaviour->OnStateUpdate(*mController->GetAnimator(), *this);
	}
}

