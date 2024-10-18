#include "DecalUVAnimator.h"
#include "Decal.h"

using namespace fq::game_module;

fq::game_module::DecalUVAnimator::DecalUVAnimator()
	: mbIsLooping(false)
	, mbIsUpdate(true)
	, mPlaySpeed(1.f)
	, mDuration(1.f)
	, mAccumulationTime(0.f)
	, mDecalKeyframe{ DecalGeoKeyframe() }
	, mDecalUVKeyframe{ DecalUVKeyframe() }
{
}

std::shared_ptr<Component> fq::game_module::DecalUVAnimator::Clone(std::shared_ptr<Component> clone) const
{
	auto cloneAnimator = std::dynamic_pointer_cast<DecalUVAnimator>(clone);

	if (cloneAnimator == nullptr) // 새로 생성해서 복사본을 준다
	{
		cloneAnimator = ObjectPool::GetInstance()->Assign<DecalUVAnimator>(*this);
	}
	else // clone에 데이터를 복사한다.
	{
		// 기본 대입 연산자 호출한다.
		*cloneAnimator = *this;
	}

	return cloneAnimator;
}

void fq::game_module::DecalUVAnimator::OnStart()
{
	mAccumulationTime = 0.f;

	std::sort(mDecalKeyframes.begin(), mDecalKeyframes.end(), [](const DecalGeoKeyframe& lhs, const DecalGeoKeyframe& rhs)
		{
			return lhs.TimePos < rhs.TimePos;
		});

	std::sort(mDecalUVKeyframes.begin(), mDecalUVKeyframes.end(), [](const DecalUVKeyframe& lhs, const DecalUVKeyframe& rhs)
		{
			return lhs.TimePos < rhs.TimePos;
		});


	for (auto& keyframe : mDecalKeyframes)
	{
		keyframe.Rotation *= (3.141592f / 180.f);
	}
	for (auto& uvKeyframe : mDecalUVKeyframes)
	{
		uvKeyframe.Rotation *= (3.141592f / 180.f);
	}
}

void fq::game_module::DecalUVAnimator::OnUpdate(float dt)
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

void fq::game_module::DecalUVAnimator::updateKeyframe()
{
	if (mDecalKeyframes.empty())
	{
		return;
	}

	int index = 0u;
	for (const auto& keyframe : mDecalKeyframes)
	{
		if (keyframe.TimePos > mAccumulationTime)
		{
			break;
		}

		++index;
	}

	size_t prevIndex = std::min<size_t>(std::max<int>(index - 1, 0), mDecalKeyframes.size() - 1);
	size_t currIndex = std::min<size_t>(index, mDecalKeyframes.size() - 1);

	if (prevIndex == currIndex)
	{
		const auto& currFrame = mDecalKeyframes[currIndex];

		mDecalKeyframe.Scale = currFrame.Scale;
		mDecalKeyframe.Rotation = currFrame.Rotation;
		mDecalKeyframe.Translation = currFrame.Translation;
	}
	else
	{
		const auto& prevFrame = mDecalKeyframes[prevIndex];
		const auto& currFrame = mDecalKeyframes[currIndex];

		float ratio = (mAccumulationTime - prevFrame.TimePos) / (currFrame.TimePos - prevFrame.TimePos);

		mDecalKeyframe.Scale = DirectX::SimpleMath::Vector3::Lerp(prevFrame.Scale, currFrame.Scale, ratio);
		mDecalKeyframe.Rotation = std::lerp(prevFrame.Rotation, currFrame.Rotation, ratio);
		mDecalKeyframe.Translation = DirectX::SimpleMath::Vector3::Lerp(prevFrame.Translation, currFrame.Translation, ratio);
	}

	mDecalKeyframe.TimePos = mAccumulationTime;
}

void fq::game_module::DecalUVAnimator::updateUVKeyframe()
{
	if (mDecalUVKeyframes.empty())
	{
		return;
	}

	int index = 0u;
	for (const auto& uvkeyframe : mDecalUVKeyframes)
	{
		if (uvkeyframe.TimePos > mAccumulationTime)
		{
			break;
		}

		++index;
	}

	size_t prevIndex = std::min<size_t>(std::max<int>(index - 1, 0), mDecalUVKeyframes.size() - 1);
	size_t currIndex = std::min<size_t>(index, mDecalUVKeyframes.size() - 1);

	if (prevIndex == currIndex)
	{
		const auto& currFrame = mDecalUVKeyframes[currIndex];

		mDecalUVKeyframe.Scale = currFrame.Scale;
		mDecalUVKeyframe.Rotation = currFrame.Rotation;
		mDecalUVKeyframe.Translation = currFrame.Translation;
	}
	else
	{
		const auto& prevFrame = mDecalUVKeyframes[prevIndex];
		const auto& currFrame = mDecalUVKeyframes[currIndex];

		float ratio = (mAccumulationTime - prevFrame.TimePos) / (currFrame.TimePos - prevFrame.TimePos);

		mDecalUVKeyframe.Scale = DirectX::SimpleMath::Vector2::Lerp(prevFrame.Scale, currFrame.Scale, ratio);
		mDecalUVKeyframe.Rotation = std::lerp(prevFrame.Rotation, currFrame.Rotation, ratio);
		mDecalUVKeyframe.Translation = DirectX::SimpleMath::Vector2::Lerp(prevFrame.Translation, currFrame.Translation, ratio);
	}

	mDecalKeyframe.TimePos = mAccumulationTime;
}
