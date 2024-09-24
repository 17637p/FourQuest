#pragma once

#include <vector>
#include <directxtk/SimpleMath.h>
#include "../FQCommon/FQCommonLoader.h"

namespace fq::graphics
{
	class AnimationHelper
	{
	public:
		static void FindKeyframe(const std::vector<fq::common::Keyframe>& keyframes,
			float timePos,
			fq::common::Keyframe* outLhs,
			fq::common::Keyframe* outRhs,
			float* outWeight);
		static void FindKeyframe(const std::vector<fq::common::Keyframe>& keyframes,
			const fq::common::AnimationClip& animationClip,
			float timePos,
			fq::common::Keyframe* outLhs,
			fq::common::Keyframe* outRhs,
			float* outWeight);
		static void FindKeyframeIndex(const std::vector<fq::common::Keyframe>& keyframes,
			const fq::common::AnimationClip& animationClip,
			float timePos,
			unsigned int* outLhsIndex,
			unsigned int* outRhsIndex,
			float* outWeight);
		static DirectX::SimpleMath::Matrix CreateMatrix(const fq::common::Keyframe& keyframe);
		static DirectX::SimpleMath::Matrix CreateMatrix(const fq::common::UVKeyframe& keyframe);
		static fq::common::Keyframe Interpolate(const fq::common::Keyframe& lhs, const fq::common::Keyframe& rhs, float weight);
	};
}
