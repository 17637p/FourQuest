#include "MaterialAnimator.h"

using namespace fq::game_module;

namespace fq::game_module
{
	std::shared_ptr<Component> fq::game_module::MaterialAnimator::Clone(std::shared_ptr<Component> clone) const
	{
		auto cloneAnimator = std::dynamic_pointer_cast<MaterialAnimator>(clone);

		if (cloneAnimator == nullptr) // 새로 생성해서 복사본을 준다
		{
			cloneAnimator = ObjectPool::GetInstance()->Assign<MaterialAnimator>(*this);
		}
		else // clone에 데이터를 복사한다.
		{
			// 기본 대입 연산자 호출한다.
			*cloneAnimator = *this;
		}

		return cloneAnimator;
	}

	void fq::game_module::MaterialAnimator::OnUpdate(float dt)
	{
		updateAlpha(dt);
		updateDissolve(dt);
	}

	void MaterialAnimator::updateDissolve(float dt)
	{
		auto& info = mDissovleAnimatorInfo;

		if (!info.bIsUpdate)
		{
			return;
		}

		if (info.bIsLooping)
		{
			info.TimePos = std::fmod(info.TimePos + dt, info.Duration);
		}
		else
		{
			info.TimePos = std::min<float>(info.TimePos + dt, info.Duration);
		}

		float timePos = std::max<float>(info.TimePos - info.DelayTime, 0.f);

		switch (info.AnimationMode)
		{
		case EAnimationMode::Increase:
			info.DissolveCutoff = info.InitDissolveCutoff + timePos * info.Speed;
			break;
		case EAnimationMode::Decrease:
			info.DissolveCutoff = info.InitDissolveCutoff - timePos * info.Speed;
			break;
		default:
			break;
		}

		info.DissolveCutoff = std::min<float>(info.DissolveCutoff, info.MaxDissolveCutoff);
		info.DissolveCutoff = std::max<float>(info.DissolveCutoff, info.MinDissolveCutoff);
	}

	void MaterialAnimator::updateAlpha(float dt)
	{
		auto& info = mAlphaAnimatorInfo;

		if (!info.bIsUpdate)
		{
			return;
		}

		if (info.bIsLooping)
		{
			info.TimePos = std::fmod(info.TimePos + dt, info.Duration);
		}
		else
		{
			info.TimePos = std::min<float>(info.TimePos + dt, info.Duration);
		}

		float timePos = std::max<float>(info.TimePos - info.DelayTime, 0.f);

		switch (info.AnimationMode)
		{
		case EAnimationMode::Increase:
			info.Alpha = info.InitAlpha + timePos * info.Speed;
			break;
		case EAnimationMode::Decrease:
			info.Alpha = info.InitAlpha - timePos * info.Speed;
			break;
		default:
			break;
		}

		info.Alpha = std::min<float>(info.Alpha, info.MaxAlpha);
		info.Alpha = std::max<float>(info.Alpha, info.MinAlpha);
	}
}