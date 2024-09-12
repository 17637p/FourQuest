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
		// 거리 값 계산해서 도착 시간 계산
		float targetLength = mTargetPosition.Length() - mStartPosition.Length();
		targetLength = targetLength / mMaxRange;

		mArrivalTime = std::lerp(mMinArrivalTime, mMaxArrivalTime, targetLength);

		determineYVelocity();
	}

	void ArcAttack::determineYVelocity()
	{
		// 초기 Y속도 계산
		mInitialYVelocity = 0.5f * 9.81f * mArrivalTime;
		mYVelocity = mInitialYVelocity;
	}

	void ArcAttack::OnUpdate(float dt)
	{
		mDurationTime += dt;

		if (mDurationTime <= mArrivalTime)
		{
			auto transform = GetComponent<fq::game_module::Transform>();
		
			// x, y 위치 보간
			float lerpDuration = mDurationTime / mArrivalTime;
			DirectX::SimpleMath::Vector3 XZPosition = DirectX::SimpleMath::Vector3::Lerp(mStartPosition, mTargetPosition, lerpDuration);

			// y속도와 위치 계산
			float yPosition = transform->GetWorldPosition().y;
			mYVelocity = mYVelocity + -9.81 * dt;
			yPosition = yPosition + mYVelocity * dt;

			// 최종 위치 결정
			transform->SetWorldPosition(DirectX::SimpleMath::Vector3(XZPosition.x, yPosition, XZPosition.z));
		}
		else
		{
			auto transform = GetComponent<fq::game_module::Transform>();

			transform->SetWorldPosition(mTargetPosition);

			// 폭발하는 공격 오브젝트 생성


			GetScene()->DestroyGameObject(GetGameObject());
		}
	}





	std::shared_ptr<fq::game_module::Component> fq::client::ArcAttack::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
	{
		std::shared_ptr<ArcAttack> cloneAttack = std::dynamic_pointer_cast<ArcAttack>(clone);

		if (cloneAttack == nullptr) // 새로 생성해서 복사본을 준다
		{
			cloneAttack = game_module::ObjectPool::GetInstance()->Assign<ArcAttack>(*this);
		}
		else // clone에 데이터를 복사한다.
		{
			// 기본 대입 연산자 호출한다.
			*cloneAttack = *this;
		}

		return cloneAttack;
	}
}
