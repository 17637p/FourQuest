#include "Animation.h"
#include "AnimationHelper.h"

using namespace fq::graphics;

fq::graphics::Animation::Animation(const fq::common::AnimationClip& animationClip)
{
	Create(animationClip);
}

void fq::graphics::Animation::Create(const fq::common::AnimationClip& animationClip)
{
	mAnimationClip = animationClip;
}

fq::graphics::UVAnimation::UVAnimation(const fq::common::UVAnimationClip& uvAnimationClip)
	: mUVAnimationClip(uvAnimationClip)
{
}

std::shared_ptr<IUVAnimationInstance> fq::graphics::UVAnimation::CreateUVAnimationInstance()
{
	return std::make_shared<UVAnimationInstance>(shared_from_this());
}

fq::graphics::UVAnimationInstance::UVAnimationInstance(std::shared_ptr<UVAnimation> uvAnimation)
	: mUVAnimation(uvAnimation)
{
	const auto& animData = mUVAnimation->GetUVAnimationClip();

	for (const auto& [nodeName, clipData] : animData.NodeClips)
	{
		mTexTransform[nodeName]; // 단위행렬 초기화
	}
}

void fq::graphics::UVAnimationInstance::SetTimePos(float timePos)
{
	mTimePos = timePos;
	const auto& animData = mUVAnimation->GetUVAnimationClip();

	for (const auto& [nodeName, clipData] : animData.NodeClips)
	{
		size_t uvIndex = std::min<size_t>(mTimePos / animData.FramePerSecond, clipData.UVData.size() - 1);
		mTexTransform[nodeName] = AnimationHelper::CreateMatrix(clipData.UVData[uvIndex]);
	}
}

const DirectX::SimpleMath::Matrix& fq::graphics::UVAnimationInstance::GetTexTransform(const std::string& nodeName) const
{
	auto find = mTexTransform.find(nodeName);
	assert(find != mTexTransform.end());
	return find->second;
}
