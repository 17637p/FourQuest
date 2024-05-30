#include "AnimationStateNode.h"

fq::game_module::AnimationStateNode::AnimationStateNode(AnimatorController* controller)
	:mController(controller)
	, mType(Type::State)
	, mPlayBackSpeed(1.f)
	, mModelPath{}
	, mAnimationName{}
	, mAnimationKey{}
	, mbIsLoof(true)
	, mDuration(0.f)
{}

fq::game_module::AnimationStateNode::~AnimationStateNode()
{

}

