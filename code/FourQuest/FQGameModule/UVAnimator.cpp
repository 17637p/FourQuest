#include "UVAnimator.h"

using namespace fq::game_module;

fq::game_module::UVAnimator::UVAnimator()
	: mUVAnimationPath{}
	, mIUVAnimation{ nullptr }
	, mIUVAnimationInstance{ nullptr }
	, mbIsUpdate(true)
	, mbIsRecursive(true)
	, mPlaySpeed(1.f)
{
}

// .animation
// .uvAnimation
// .nodeHierarchy
// .nodeHierarchy

std::shared_ptr<Component> fq::game_module::UVAnimator::Clone(std::shared_ptr<Component> clone) const
{
	auto cloneAnimator = std::dynamic_pointer_cast<UVAnimator>(clone);

	if (cloneAnimator == nullptr) // 새로 생성해서 복사본을 준다
	{
		cloneAnimator = ObjectPool::GetInstance()->Assign<UVAnimator>(*this);
	}
	else // clone에 데이터를 복사한다.
	{
		// 기본 대입 연산자 호출한다.
		*cloneAnimator = *this;
	}

	return cloneAnimator;
}

void fq::game_module::UVAnimator::UpdateTimePos(float dt)
{
	if (!mbIsUpdate)
	{
		return;
	}

	float duration = mIUVAnimation->GetUVAnimationClip().Duration;
	float timePos = mIUVAnimationInstance->GetTimePos();

	if (mbIsRecursive)
	{
		mIUVAnimationInstance->SetTimePos(std::fmod(timePos + dt, duration));
	}
	else
	{
		mIUVAnimationInstance->SetTimePos(std::min<float>(timePos + dt, duration));
	}
}

void fq::game_module::UVAnimator::SetTimePos(float timePos)
{
	mIUVAnimationInstance->SetTimePos(timePos);
}
