#include "AnimationStateNode.h"

fq::game_module::AnimationStateNode::AnimationStateNode()
	:mController(nullptr)
	,mType(Type::State)
	,mPlayBackSpeed(1.f)
	,mTransitions{}
{}

fq::game_module::AnimationStateNode::~AnimationStateNode()
{

}

bool fq::game_module::AnimationStateNode::CheckTransition()
{


	return false;
}
