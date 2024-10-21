#include "BlendUVAnimator.h"

using namespace fq::game_module;

fq::game_module::BlendUVAnimator::BlendUVAnimator()
	: mbIsLooping(false)
	, mbIsUpdate(true)
	, mPlaySpeed(1.f)
	, mDuration(1.f)
	, mAccumulationTime(0.f)
	, mKeyframe{ BlendUVKeyframe() }
{
}

std::shared_ptr<Component> fq::game_module::BlendUVAnimator::Clone(std::shared_ptr<Component> clone) const
{
	auto cloneAnimator = std::dynamic_pointer_cast<BlendUVAnimator>(clone);

	if (cloneAnimator == nullptr) // 새로 생성해서 복사본을 준다
	{
		cloneAnimator = ObjectPool::GetInstance()->Assign<BlendUVAnimator>(*this);
	}
	else // clone에 데이터를 복사한다.
	{
		// 기본 대입 연산자 호출한다.
		*cloneAnimator = *this;
	}

	return cloneAnimator;
}

void fq::game_module::BlendUVAnimator::OnStart()
{
	mAccumulationTime = 0.f;

	std::sort(mKeyframes.begin(), mKeyframes.end(), [](const BlendUVKeyframe& lhs, const BlendUVKeyframe& rhs)
		{
			return lhs.TimePos < rhs.TimePos;
		});
}

void fq::game_module::BlendUVAnimator::OnUpdate(float dt)
{
	mAccumulationTime += dt * mPlaySpeed;

	if (mbIsLooping)
	{
		mAccumulationTime = fmod(mAccumulationTime, mDuration);
	}
	else
	{
		mAccumulationTime = fmin(mAccumulationTime, mDuration);
	}
}

void fq::game_module::BlendUVAnimator::updateKeyframe()
{
	if (mKeyframes.empty())
	{
		return;
	}

	int index = 0u;
	for (const auto& keyframe : mKeyframes)
	{
		if (keyframe.TimePos > mAccumulationTime)
		{
			break;
		}

		++index;
	}

	size_t prevIndex = std::min<size_t>(std::max<int>(index - 1, 0), mKeyframes.size() - 1);
	size_t currIndex = std::min<size_t>(index, mKeyframes.size() - 1);

	if (prevIndex == currIndex)
	{
		const auto& currFrame = mKeyframes[currIndex];

		mKeyframe.Scale = currFrame.Scale;
		mKeyframe.Translation = currFrame.Translation;
	}
	else
	{
		const auto& prevFrame = mKeyframes[prevIndex];
		const auto& currFrame = mKeyframes[currIndex];

		float ratio = (mAccumulationTime - prevFrame.TimePos) / (currFrame.TimePos - prevFrame.TimePos);

		mKeyframe.Scale = DirectX::SimpleMath::Vector2::Lerp(prevFrame.Scale, currFrame.Scale, ratio);
		mKeyframe.Translation = DirectX::SimpleMath::Vector2::Lerp(prevFrame.Translation, currFrame.Translation, ratio);
	}

	mKeyframe.TimePos = mAccumulationTime;
}
