#pragma once

#include <memory>
#include "../FQCommon/IFQRenderResource.h"

namespace fq::graphics
{
	class Animation : public IAnimation
	{
	public:
		Animation() = default;
		Animation(const fq::common::AnimationClip& animationClip);
		virtual ~Animation() = default;

		void Create(const fq::common::AnimationClip& animationClip);

		virtual const fq::common::AnimationClip& GetAnimationClip() const override { return mAnimationClip; }

	private:
		fq::common::AnimationClip mAnimationClip;
	};

	class UVAnimation : public IUVAnimation, public std::enable_shared_from_this<UVAnimation>
	{
	public:
		UVAnimation(const fq::common::UVAnimationClip& uvAnimationClip);
		virtual ~UVAnimation() = default;

		virtual std::shared_ptr<IUVAnimationInstance> CreateUVAnimationInstance() override;
		virtual const fq::common::UVAnimationClip& GetUVAnimationClip() const override { return mUVAnimationClip; }
		virtual void SetUVAnimationClip(const fq::common::UVAnimationClip& clip) override { mUVAnimationClip = clip; }

	private:
		fq::common::UVAnimationClip mUVAnimationClip;
	};


	class UVAnimationInstance : public IUVAnimationInstance
	{
	public:
		UVAnimationInstance(std::shared_ptr<UVAnimation> uvAnimation);
		virtual ~UVAnimationInstance() = default;

		virtual void SetTimePos(float timePos) override;
		virtual float GetTimePos() const override { return mTimePos; }

		virtual const DirectX::SimpleMath::Matrix& GetTexTransform(const std::string& nodeName) const override;

	private:
		std::shared_ptr<UVAnimation> mUVAnimation;
		std::map<std::string, DirectX::SimpleMath::Matrix> mTexTransform;
		float mTimePos;
	};
}