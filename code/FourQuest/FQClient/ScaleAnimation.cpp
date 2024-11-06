#include "ScaleAnimation.h"
#include "../FQGameModule/Transform.h"

fq::client::ScaleAnimation::ScaleAnimation()
	: mDuration(1.f)
	, mStartScale(1, 1, 1)
	, mEndScale(1, 1, 1)
	, mElapsed(0.f)
	, mPlaySpeed(1.f)
{
}

std::shared_ptr<fq::game_module::Component> fq::client::ScaleAnimation::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	std::shared_ptr<ScaleAnimation> cloneMonster = std::dynamic_pointer_cast<ScaleAnimation>(clone);

	if (cloneMonster == nullptr) // ���� �����ؼ� ���纻�� �ش�
	{
		cloneMonster = game_module::ObjectPool::GetInstance()->Assign<ScaleAnimation>(*this);
	}
	else // clone�� �����͸� �����Ѵ�.
	{
		// �⺻ ���� ������ ȣ���Ѵ�.
		*cloneMonster = *this;
	}

	return cloneMonster;
}

void fq::client::ScaleAnimation::OnStart()
{
	mElapsed = 0.f;
}

void fq::client::ScaleAnimation::OnUpdate(float dt)
{
	mElapsed += dt * mPlaySpeed;
	mElapsed = std::min<float>(mElapsed, mDuration);

	auto transform = GetComponent<fq::game_module::Transform>();

	if (transform != nullptr)
	{
		float ratio = mElapsed / mDuration;
		const auto curScale = DirectX::SimpleMath::Vector3::Lerp(mStartScale, mEndScale, ratio);
		transform->SetLocalScale(curScale);
	}
}
