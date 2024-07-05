#include "Animation.h"

using namespace fq::graphics;

fq::graphics::Animation::Animation(const fq::common::AnimationClip& animationClip)
{
	Create(animationClip);
}

void fq::graphics::Animation::Create(const fq::common::AnimationClip& animationClip)
{
	mAnimationClip = animationClip;
}
