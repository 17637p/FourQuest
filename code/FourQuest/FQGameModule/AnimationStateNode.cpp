#include "AnimationStateNode.h"

fq::game_module::AnimationStateNode::AnimationStateNode()
	:mController(nullptr)
	,mType(Type::State)
	,mPlayBackSpeed(1.f)
{}

fq::game_module::AnimationStateNode::~AnimationStateNode()
{

}

