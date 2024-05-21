#include "AnimationStateNode.h"

fq::game_module::AnimationStateNode::AnimationStateNode(AnimatorController* controller)
	:mController(controller)
	, mType(Type::State)
	, mPlayBackSpeed(1.f)
	, mModelPath{}
	, mAnimationName{}
	, mAnimationKey{}
{}

fq::game_module::AnimationStateNode::~AnimationStateNode()
{

}

