#include "RotationAnimation.h"
#include "../FQGameModule/Transform.h"

std::shared_ptr<fq::game_module::Component> fq::client::RotationAnimation::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	std::shared_ptr<RotationAnimation> cloneMonster = std::dynamic_pointer_cast<RotationAnimation>(clone);

	if (cloneMonster == nullptr) // 새로 생성해서 복사본을 준다
	{
		cloneMonster = game_module::ObjectPool::GetInstance()->Assign<RotationAnimation>(*this);
	}
	else // clone에 데이터를 복사한다.
	{
		// 기본 대입 연산자 호출한다.
		*cloneMonster = *this;
	}

	return cloneMonster;
}

void fq::client::RotationAnimation::OnStart()
{
	mElapsed = 0.f;
}

void fq::client::RotationAnimation::OnUpdate(float dt)
{
	mElapsed += dt;

	if (mbUseLooping)
	{
		mElapsed = std::fmod(mElapsed, mDuration);
	}
	else
	{
		mElapsed = std::min<float>(mElapsed, mDuration);
	}

	auto transform = GetComponent<fq::game_module::Transform>();

	if (transform == nullptr)
	{
		return;
	}

	float ratio = mElapsed / mDuration;

	if (mbUseEuler)
	{
		const auto rotationInEuler = DirectX::SimpleMath::Vector3::Lerp(mStartRotationInEuler, mEndRotationInEuler, ratio);
		transform->SetLocalRotation(DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(rotationInEuler));
	}
	if (mbUseQuaternion)
	{
		const auto rotationInQuaternion = DirectX::SimpleMath::Quaternion::Slerp(mStartRotationInQuaternion, mEndRotationInQuaternion, ratio);
		transform->SetLocalRotation(rotationInQuaternion);
	}
}
