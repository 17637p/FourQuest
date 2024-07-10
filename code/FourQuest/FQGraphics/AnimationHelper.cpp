#include "AnimationHelper.h"

namespace fq::graphics
{
	void AnimationHelper::FindKeyframe(const std::vector<fq::common::Keyframe>& keyframes,
		float timePos,
		fq::common::Keyframe* outLhs,
		fq::common::Keyframe* outRhs,
		float* outWeight)
	{
		for (size_t i = 0; i < keyframes.size(); ++i)
		{
			if (keyframes[i].TimePos < timePos)
			{
				continue;
			}

			if (i == 0)
			{
				*outLhs = keyframes[0];
				*outRhs = keyframes.size() > 1 ? keyframes[1] : keyframes[0];
			}
		}

		// to do
		for (const fq::common::Keyframe& keyframe : keyframes)
		{
			if (keyframe.TimePos > timePos)
			{

			}
		}
	}

	void AnimationHelper::FindKeyframe(const std::vector<fq::common::Keyframe>& keyframes,
		const fq::common::AnimationClip& animationClip,
		float timePos,
		fq::common::Keyframe* outLhs,
		fq::common::Keyframe* outRhs,
		float* outWeight)
	{
		size_t prevIndex = (size_t)floor(timePos / animationClip.FramePerSecond);

		if (prevIndex < keyframes.size())
		{
			*outLhs = keyframes[prevIndex];
			*outRhs = prevIndex == keyframes.size() - 1 ? keyframes.back() : keyframes[prevIndex + 1];
			*outWeight = fmod(timePos, animationClip.FramePerSecond) / animationClip.FramePerSecond;
		}
		else
		{
			*outLhs = keyframes.back();
			*outRhs = keyframes.back();
			*outWeight = 1.f;
		}
	}

	DirectX::SimpleMath::Matrix AnimationHelper::CreateMatrix(const fq::common::Keyframe& keyframe)
	{
		return DirectX::SimpleMath::Matrix::CreateScale(keyframe.Scale)
			* DirectX::SimpleMath::Matrix::CreateFromQuaternion(keyframe.Rotation)
			* DirectX::SimpleMath::Matrix::CreateTranslation(keyframe.Translation);
	}

	DirectX::SimpleMath::Matrix AnimationHelper::CreateMatrix(const fq::common::UVKeyframe& keyframe)
	{
		return DirectX::SimpleMath::Matrix::CreateScale(keyframe.Scale.x, keyframe.Scale.y, 0)
			* DirectX::SimpleMath::Matrix::CreateRotationZ(keyframe.Rotation)
			* DirectX::SimpleMath::Matrix::CreateTranslation(keyframe.Translation.x, keyframe.Translation.y, 0);
	}

	fq::common::Keyframe AnimationHelper::Interpolate(const fq::common::Keyframe& lhs, const fq::common::Keyframe& rhs, float weight)
	{
		fq::common::Keyframe keyframe;

		keyframe.Translation = DirectX::SimpleMath::Vector3::Lerp(lhs.Translation, rhs.Translation, weight);
		keyframe.Rotation = DirectX::SimpleMath::Quaternion::Slerp(lhs.Rotation, rhs.Rotation, weight);
		keyframe.Scale = DirectX::SimpleMath::Vector3::Lerp(lhs.Scale, rhs.Scale, weight);

		return keyframe;
	}
}