#include "ArcAttack.h"

#include "../FQGameModule/GameModule.h"
#include "../FQGameModule/Transform.h"
#include "../FQGameModule/GameObject.h"
#include "../FQGameModule/SphereCollider.h"

#include "Attack.h"

namespace fq::client
{
	ArcAttack::ArcAttack()
		: mAOEAttackPoolEffectPrefeb()
		, mAOEAttackSplashEffectPrefeb()
		, mStartPosition{}
		, mTargetPosition{}
		, mMaxRange()
		, mMinArrivalTime()
		, mMaxArrivalTime()
		, mArrivalTime()
		, mInitialYVelocity()
		, mYVelocity()
		, mDurationTime()
		, mbIsCreateEffect(false)
		, mDestroyTime()
		, mDestroyDurationTime()
	{
	}

	ArcAttack::~ArcAttack()
	{
	}

	void ArcAttack::DetermineArrivalTime()
	{
		GetGameObject()->SetTag(fq::game_module::ETag::Untagged);

		// 거리 값 계산해서 도착 시간 계산
		mYPosition = mStartPosition.y;
		mTargetPosition.y = 0.f;
		mStartPosition.y = 0.f;
		DirectX::SimpleMath::Vector3 Vector = mTargetPosition - mStartPosition;
		float targetLength = Vector.Length();
		targetLength = targetLength / mMaxRange;

		mArrivalTime = std::lerp(mMinArrivalTime, mMaxArrivalTime, targetLength);

		determineYVelocity();
	}

	void ArcAttack::determineYVelocity()
	{
		// 초기 Y속도 계산
		mInitialYVelocity = (mTargetPosition.y - mStartPosition.y + 0.5f * 9.81f * mArrivalTime * mArrivalTime) / mArrivalTime;
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
			mYVelocity = mYVelocity + -9.81 * dt;
			mYPosition = mYPosition + mYVelocity * dt;

			// 최종 위치 결정
			transform->SetWorldPosition(DirectX::SimpleMath::Vector3(XZPosition.x, mYPosition, XZPosition.z));
		}
		else if (!mbIsCreateEffect)
		{
			mbIsCreateEffect = true;

			auto transform = GetComponent<fq::game_module::Transform>();

			transform->SetWorldPosition(mTargetPosition);

			// 폭발하는 공격 이펙트 오브젝트 생성
			auto poolInstance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(mAOEAttackPoolEffectPrefeb);
			auto splashInstance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(mAOEAttackSplashEffectPrefeb);
			auto& poolAttackObj = *(poolInstance.begin());
			auto& splashAttackObj = *(splashInstance.begin());

			auto poolTransform = poolAttackObj->GetComponent<fq::game_module::Transform>();
			poolTransform->SetParent(transform);
			auto splashTransform = splashAttackObj->GetComponent<fq::game_module::Transform>();
			splashTransform->SetParent(transform);

			GetScene()->AddGameObject(poolAttackObj);
			GetScene()->AddGameObject(splashAttackObj);

			GetComponent<fq::game_module::SphereCollider>()->SetOffset(DirectX::SimpleMath::Vector3());

			// 오브젝트 태그 변경
			GetGameObject()->SetTag(fq::game_module::ETag::MonsterAttack);
		}
		else
		{
			mDestroyDurationTime += dt;

			if (mDestroyDurationTime >= mDestroyTime)
			{
				GetScene()->DestroyGameObject(GetGameObject());
			}
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
