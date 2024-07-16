#include "UVAnimator.h"

using namespace fq::game_module;

fq::game_module::UVAnimator::UVAnimator()
	: mUVAnimationPath{}
	, mIUVAnimation{ nullptr }
	, mIUVAnimationInstance{ nullptr }
{
}

// .animation
// .uvAnimation
// .nodeHierarchy
// .nodeHierarchy

std::shared_ptr<Component> fq::game_module::UVAnimator::Clone(std::shared_ptr<Component> clone) const
{
	auto cloneAnimator = std::dynamic_pointer_cast<UVAnimator>(clone);

	if (cloneAnimator == nullptr) // ���� �����ؼ� ���纻�� �ش�
	{
		cloneAnimator = ObjectPool::GetInstance()->Assign<UVAnimator>(*this);
	}
	else // clone�� �����͸� �����Ѵ�.
	{
		// �⺻ ���� ������ ȣ���Ѵ�.
		*cloneAnimator = *this;
	}

	return cloneAnimator;
}

void fq::game_module::UVAnimator::UpdateTimePos(float dt)
{
	float duration = mIUVAnimation->GetUVAnimationClip().Duration;
	float timePos = mIUVAnimationInstance->GetTimePos();

	mIUVAnimationInstance->SetTimePos(std::fmod(timePos + dt, duration));
}

void fq::game_module::UVAnimator::SetTimePos(float timePos)
{
	mIUVAnimationInstance->SetTimePos(timePos);
}
