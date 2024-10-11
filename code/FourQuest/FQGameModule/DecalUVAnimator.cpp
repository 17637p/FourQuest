#include "DecalUVAnimator.h"
#include "Decal.h"

using namespace fq::game_module;

fq::game_module::DecalUVAnimator::DecalUVAnimator()
	: mDecalOrNull(nullptr)
	, mbIsLooping(false)
	, mbIsUpdate(true)
	, mPlaySpeed(1.f)
	, mDuration(1.f)
	, mAccumulationTime(0.f)
	, mAdditiveTransform(DirectX::SimpleMath::Matrix::Identity)
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
	mDecalOrNull = GetComponent<Decal>();

	mAccumulationTime = 0.f;

	std::sort(mKeyframes.begin(), mKeyframes.end(), [](const DecalKeyframe& lhs, const DecalKeyframe& rhs)
		{
			return lhs.TimePos < rhs.TimePos;
		});

	std::sort(mUVKeyframes.begin(), mUVKeyframes.end(), [](const DecalUVKeyframe& lhs, const DecalUVKeyframe& rhs)
		{
			return lhs.TimePos < rhs.TimePos;
		});


	for (auto& keyframe : mKeyframes)
	{
		const DirectX::SimpleMath::Vector3 ratotationInRadian =
		{
			keyframe.Rotation.x * 3.141592f / 180.f,
			keyframe.Rotation.y * 3.141592f / 180.f,
			keyframe.Rotation.z * 3.141592f / 180.f,
		};

		keyframe.RotationQuaternion = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(keyframe.Rotation);
	}
}

void fq::game_module::DecalUVAnimator::OnUpdate(float dt)
{
	if (mDecalOrNull == nullptr)
	{
		return;
	}
	if (mKeyframes.empty())
	{
		return;
	}

	mAccumulationTime += dt * mPlaySpeed;

	if (mbIsLooping)
	{
		mAccumulationTime = fmod(mAccumulationTime, mDuration);
	}
	else
	{
		mAccumulationTime = fmin(mAccumulationTime, mDuration);
	}

	auto decalInfo = mDecalOrNull->GetDecalInfo();

	// uv
	if (!mUVKeyframes.empty())
	{
		int index = 0u;
		for (const auto& uvkeyframe : mUVKeyframes)
		{
			if (uvkeyframe.TimePos > mAccumulationTime)
			{
				break;
			}

			++index;
		}

		size_t prevIndex = std::min<size_t>(std::max<int>(index - 1, 0), mUVKeyframes.size() - 1);
		size_t currIndex = std::min<size_t>(index, mKeyframes.size() - 1);

		DirectX::SimpleMath::Vector2 scale;
		float rotation;
		DirectX::SimpleMath::Vector2 translation;

		if (prevIndex == currIndex)
		{
			const auto& currFrame = mUVKeyframes[currIndex];

			scale = currFrame.Scale;
			rotation = currFrame.Rotation;
			translation = currFrame.Translation;
		}
		else
		{
			const auto& prevFrame = mUVKeyframes[prevIndex];
			const auto& currFrame = mUVKeyframes[currIndex];

			float ratio = (mAccumulationTime - prevFrame.TimePos) / (currFrame.TimePos - prevFrame.TimePos);

			scale = DirectX::SimpleMath::Vector2::Lerp(prevFrame.Scale, currFrame.Scale, ratio);
			rotation = std::lerp(prevFrame.Rotation, currFrame.Rotation, ratio);
			translation = DirectX::SimpleMath::Vector2::Lerp(prevFrame.Translation, currFrame.Translation, ratio);
		}

		decalInfo.Offset = translation;
		decalInfo.Tiling = scale;
		decalInfo.Rotation = rotation;

		mDecalOrNull->SetDecalInfo(decalInfo);
	}

	// transform
	if (!mKeyframes.empty())
	{
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

		DirectX::SimpleMath::Vector3 scale;
		DirectX::SimpleMath::Quaternion rotation;
		DirectX::SimpleMath::Vector3 translation;

		if (prevIndex == currIndex)
		{
			const auto& currFrame = mKeyframes[currIndex];

			scale = currFrame.Scale;
			rotation = currFrame.RotationQuaternion;
			translation = currFrame.Translation;
		}
		else
		{
			const auto& prevFrame = mKeyframes[prevIndex];
			const auto& currFrame = mKeyframes[currIndex];

			float ratio = (mAccumulationTime - prevFrame.TimePos) / (currFrame.TimePos - prevFrame.TimePos);

			scale = DirectX::SimpleMath::Vector3::Lerp(prevFrame.Scale, currFrame.Scale, ratio);
			rotation = DirectX::SimpleMath::Quaternion::Slerp(prevFrame.RotationQuaternion, currFrame.RotationQuaternion, ratio);
			translation = DirectX::SimpleMath::Vector3::Lerp(prevFrame.Translation, currFrame.Translation, ratio);
		}

		mAdditiveTransform = DirectX::SimpleMath::Matrix::CreateScale(scale)
			* DirectX::SimpleMath::Matrix::CreateFromQuaternion(rotation)
			* DirectX::SimpleMath::Matrix::CreateTranslation(translation);
	}

}