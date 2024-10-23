#include "RotationAnimation.h"
#include "../FQGameModule/Transform.h"

std::shared_ptr<fq::game_module::Component> fq::client::RotationAnimation::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	std::shared_ptr<RotationAnimation> cloneMonster = std::dynamic_pointer_cast<RotationAnimation>(clone);

	if (cloneMonster == nullptr) // ���� �����ؼ� ���纻�� �ش�
	{
		cloneMonster = game_module::ObjectPool::GetInstance()->Assign<RotationAnimation>(*this);
	}
	else // clone�� �����͸� �����Ѵ�.
	{
		// �⺻ ���� ������ ȣ���Ѵ�.
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
