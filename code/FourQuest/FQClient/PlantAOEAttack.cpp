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

		// �Ÿ� �� ����ؼ� ���� �ð� ���
		mYPosition = mStartPosition.y;
		mTargetPosition.y = 0.f;
		mStartPosition.y = 0.f;
		DirectX::SimpleMath::Vector3 Vector = mTargetPosition - mStartPosition;
		float targetLength = Vector.Length();
		targetLength = targetLength / mMaxRange;

		mArrivalTime = std::lerp(mMinArrivalTime, mMaxArrivalTime, targetLength);

		determineYVelocity();
	}

	void PlantAOEAttack::determineYVelocity()
	{
		// �ʱ� Y�ӵ� ���
		mInitialYVelocity = (mTargetPosition.y - mStartPosition.y + 0.5f * 9.81f * mArrivalTime * mArrivalTime) / mArrivalTime;
		mYVelocity = mInitialYVelocity;
	}

	void PlantAOEAttack::OnUpdate(float dt)
	{
		mDurationTime += dt;

		if (mDurationTime <= mArrivalTime)
		{
			auto transform = GetComponent<fq::game_module::Transform>();
		
			// x, y ��ġ ����
			float lerpDuration = mDurationTime / mArrivalTime;
			DirectX::SimpleMath::Vector3 XZPosition = DirectX::SimpleMath::Vector3::Lerp(mStartPosition, mTargetPosition, lerpDuration);

			// y�ӵ��� ��ġ ���
			mYVelocity = mYVelocity + -9.81 * dt;
			mYPosition = mYPosition + mYVelocity * dt;

			// ���� ��ġ ����
			transform->SetWorldPosition(DirectX::SimpleMath::Vector3(XZPosition.x, mYPosition, XZPosition.z));
		}
		else if (!mbIsCreateEffect)
		{
			mbIsCreateEffect = true;

			auto transform = GetComponent<fq::game_module::Transform>();

			transform->SetWorldPosition(mTargetPosition);

			// �����ϴ� ���� ����Ʈ ������Ʈ ����
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

			// ������Ʈ �±� ����
			GetGameObject()->SetTag(fq::game_module::ETag::MonsterAttack);
		}
		else
		{
			mDestroyDurationTime += dt;

			// �� Ÿ�� ������ ���� �±� ����
			if (mDestroyDurationTime >= 0.1f)
			{
				GetGameObject()->SetTag(fq::game_module::ETag::Floor);
			}

			if (mDestroyDurationTime >= mDestroyTime)
			{
				GetScene()->DestroyGameObject(GetGameObject());
			}
		}
	}

	void PlantAOEAttack::OnTriggerEnter(const fq::game_module::Collision& collision)
	{
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
		if (collision.other->HasComponent<DebuffPoisonZone>())
		{
			collision.other->RemoveComponent<DebuffPoisonZone>();
		}
	}

	std::shared_ptr<fq::game_module::Component> fq::client::PlantAOEAttack::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
	{
		std::shared_ptr<PlantAOEAttack> cloneAttack = std::dynamic_pointer_cast<PlantAOEAttack>(clone);

		if (cloneAttack == nullptr) // ���� �����ؼ� ���纻�� �ش�
		{
			cloneAttack = game_module::ObjectPool::GetInstance()->Assign<PlantAOEAttack>(*this);
		}
		else // clone�� �����͸� �����Ѵ�.
		{
			// �⺻ ���� ������ ȣ���Ѵ�.
			*cloneAttack = *this;
		}

		return cloneAttack;
	}
}
