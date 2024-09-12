#include "ArcAttack.h"

#include "../FQGameModule/GameModule.h"
#include "../FQGameModule/Transform.h"

namespace fq::client
{
	ArcAttack::ArcAttack()
		: mAOEAttackPrefeb()
		, mStartPosition{}
		, mTargetPosition{}
		, mMaxRange()
		, mMinArrivalTime()
		, mMaxArrivalTime()
		, mArrivalTime()
		, mInitialYVelocity()
		, mYVelocity()
		, mDurationTime()
	{
	}

	ArcAttack::~ArcAttack()
	{
	}

	void ArcAttack::DetermineArrivalTime()
	{
		// �Ÿ� �� ����ؼ� ���� �ð� ���
		float targetLength = mTargetPosition.Length() - mStartPosition.Length();
		targetLength = targetLength / mMaxRange;

		mArrivalTime = std::lerp(mMinArrivalTime, mMaxArrivalTime, targetLength);

		determineYVelocity();
	}

	void ArcAttack::determineYVelocity()
	{
		// �ʱ� Y�ӵ� ���
		mInitialYVelocity = 0.5f * 9.81f * mArrivalTime;
		mYVelocity = mInitialYVelocity;
	}

	void ArcAttack::OnUpdate(float dt)
	{
		mDurationTime += dt;

		if (mDurationTime <= mArrivalTime)
		{
			auto transform = GetComponent<fq::game_module::Transform>();
		
			// x, y ��ġ ����
			float lerpDuration = mDurationTime / mArrivalTime;
			DirectX::SimpleMath::Vector3 XZPosition = DirectX::SimpleMath::Vector3::Lerp(mStartPosition, mTargetPosition, lerpDuration);

			// y�ӵ��� ��ġ ���
			float yPosition = transform->GetWorldPosition().y;
			mYVelocity = mYVelocity + -9.81 * dt;
			yPosition = yPosition + mYVelocity * dt;

			// ���� ��ġ ����
			transform->SetWorldPosition(DirectX::SimpleMath::Vector3(XZPosition.x, yPosition, XZPosition.z));
		}
		else
		{
			auto transform = GetComponent<fq::game_module::Transform>();

			transform->SetWorldPosition(mTargetPosition);

			// �����ϴ� ���� ������Ʈ ����


			GetScene()->DestroyGameObject(GetGameObject());
		}
	}





	std::shared_ptr<fq::game_module::Component> fq::client::ArcAttack::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
	{
		std::shared_ptr<ArcAttack> cloneAttack = std::dynamic_pointer_cast<ArcAttack>(clone);

		if (cloneAttack == nullptr) // ���� �����ؼ� ���纻�� �ش�
		{
			cloneAttack = game_module::ObjectPool::GetInstance()->Assign<ArcAttack>(*this);
		}
		else // clone�� �����͸� �����Ѵ�.
		{
			// �⺻ ���� ������ ȣ���Ѵ�.
			*cloneAttack = *this;
		}

		return cloneAttack;
	}
}
