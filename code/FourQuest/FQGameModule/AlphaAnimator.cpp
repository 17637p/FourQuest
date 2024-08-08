#include "AlphaAnimator.h"
#include "AlphaAnimator.h"

using namespace fq::game_module;

std::shared_ptr<Component> fq::game_module::AlphaAnimator::Clone(std::shared_ptr<Component> clone) const
{
	auto cloneAnimator = std::dynamic_pointer_cast<AlphaAnimator>(clone);

	if (cloneAnimator == nullptr) // ���� �����ؼ� ���纻�� �ش�
	{
		cloneAnimator = ObjectPool::GetInstance()->Assign<AlphaAnimator>(*this);
	}
	else // clone�� �����͸� �����Ѵ�.
	{
		// �⺻ ���� ������ ȣ���Ѵ�.
		*cloneAnimator = *this;
	}

	return cloneAnimator;
}

void fq::game_module::AlphaAnimator::UpdateTimePos(float dt)
{
	if (!mInfo.bIsUpdate)
	{
		return;
	}

	if (mInfo.bIsRecursive)
	{
		mInfo.TimePos = std::fmod(mInfo.TimePos + dt, mInfo.Duration);
	}
	else
	{
		mInfo.TimePos = std::min<float>(mInfo.TimePos + dt, mInfo.Duration);
	}

	float timePos = std::max<float>(mInfo.TimePos - mInfo.DelayTime, 0.f);

	switch (mInfo.AlphaAnimationMode)
	{
	case EAlphaAnimationMode::Increase:
		mInfo.Alpha = mInfo.InitAlpha + timePos * mInfo.Speed;
		break;
	case EAlphaAnimationMode::Decrease:
		mInfo.Alpha = mInfo.InitAlpha - timePos * mInfo.Speed;
		break;
	default:
		break;
	}

	mInfo.Alpha = std::min<float>(mInfo.Alpha, mInfo.MaxAlpha);
	mInfo.Alpha = std::max<float>(mInfo.Alpha, mInfo.MinAlpha);
}
