#include "PlantAOEAttack.h"

#include "../FQGameModule/GameModule.h"
#include "../FQGameModule/Transform.h"
#include "../FQGameModule/GameObject.h"
#include "../FQGameModule/SphereCollider.h"

#include "Attack.h"
#include "DebuffPoisonZone.h"

namespace fq::client
{
	PlantAOEAttack::PlantAOEAttack()
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
		, mPoisonTurm()
		, mPoisonDamage()
	{
	}

	PlantAOEAttack::~PlantAOEAttack()
	{
	}

	void PlantAOEAttack::DetermineArrivalTime()
	{
		GetGameObject()->SetTag(fq::game_module::ETag::Untagged);
		GetComponent<Attack>()->SetDestroyTime(mDestroyTime + mMaxArrivalTime);

		// 거리 값 계산해서 도착 시간 계산
		mYPosition = mStartPosition.y + 0.1f;
		DirectX::SimpleMath::Vector3 Vector = mTargetPosition - mStartPosition;
		float targetLength = Vector.Length();
		targetLength = targetLength / mMaxRange;

		mArrivalTime = std::lerp(mMinArrivalTime, mMaxArrivalTime, targetLength);

		determineYVelocity();
	}

	void PlantAOEAttack::determineYVelocity()
	{
		// 초기 Y속도 계산
		mInitialYVelocity = (mTargetPosition.y - mStartPosition.y + 0.5f * 9.81f * mArrivalTime * mArrivalTime) / mArrivalTime;
		mYVelocity = mInitialYVelocity;
	}

	void PlantAOEAttack::OnUpdate(float dt)
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
			// 자식 오브젝트 제거
			for (auto* child : GetGameObject()->GetChildren())
			{
				GetScene()->DestroyGameObject(child);
			}

			mbIsCreateEffect = true;

			auto transform = GetComponent<fq::game_module::Transform>();

			mTargetPosition.y = mYPosition;
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

			// 독 타일 판정을 위한 태그 변경 ( 플레이어가 Hit 애니메이션이 작동 안되면서 Trigger 충돌은 되도록 )
			if (mDestroyDurationTime >= 0.1f)
			{
				GetGameObject()->SetTag(fq::game_module::ETag::Monster);
			}

			if (mDestroyDurationTime >= mDestroyTime)
			{
				GetScene()->DestroyGameObject(GetGameObject());
			}
		}
	}

	void PlantAOEAttack::OnTriggerEnter(const fq::game_module::Collision& collision)
	{
		// 플레이어와 충돌했을 경우에 플레이어가 독 디버프를 가지고 있지 않으면 독 디버프를 부여
		if (collision.other->GetTag() == fq::game_module::ETag::Player)
		{
			if (collision.other->HasComponent<DebuffPoisonZone>())
				return;

			collision.other->AddComponent<DebuffPoisonZone>();
			auto poisonZone = collision.other->GetComponent<DebuffPoisonZone>();

			poisonZone->SetPoisonTurm(mPoisonTurm);
			poisonZone->SetPosionDamage(mPoisonDamage);
		}
	}

	void PlantAOEAttack::OnTriggerExit(const fq::game_module::Collision& collision)
	{
		// 벗어났을 경우에 플레이어의 독 디버프 해제
		if (collision.other->HasComponent<DebuffPoisonZone>())
		{
			collision.other->RemoveComponent<DebuffPoisonZone>();
		}
	}

	std::shared_ptr<fq::game_module::Component> fq::client::PlantAOEAttack::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
	{
		std::shared_ptr<PlantAOEAttack> cloneAttack = std::dynamic_pointer_cast<PlantAOEAttack>(clone);

		if (cloneAttack == nullptr) // 새로 생성해서 복사본을 준다
		{
			cloneAttack = game_module::ObjectPool::GetInstance()->Assign<PlantAOEAttack>(*this);
		}
		else // clone에 데이터를 복사한다.
		{
			// 기본 대입 연산자 호출한다.
			*cloneAttack = *this;
		}

		return cloneAttack;
	}
}
