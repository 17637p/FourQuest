#include "AnimationStateNode.h"

fq::game_module::AnimationStateNode::AnimationStateNode(AnimatorController* controller)
	:mController(controller)
	, mType(Type::State)
	, mPlayBackSpeed(1.f)
{}

fq::game_module::AnimationStateNode::~AnimationStateNode()
{

}

