#pragma once

#include "../FQCommon/IFQRenderResource.h"

namespace fq::graphics
{
	class Animation : public IAnimation
	{
	public:
		Animation() = default;
		Animation(const fq::common::AnimationClip& animationClip);
		virtual ~Animation() = default;

		virtual void Create(const fq::common::AnimationClip& animationClip) override;

		virtual const fq::common::AnimationClip& GetAnimationClip() const override { return mAnimationClip; }

	private:
		fq::common::AnimationClip mAnimationClip;
	};
}