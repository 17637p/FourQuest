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

	if (cloneMonster == nullptr) // 새로 생성해서 복사본을 준다
	{
		cloneMonster = game_module::ObjectPool::GetInstance()->Assign<ScaleAnimation>(*this);
	}
	else // clone에 데이터를 복사한다.
	{
		// 기본 대입 연산자 호출한다.
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
