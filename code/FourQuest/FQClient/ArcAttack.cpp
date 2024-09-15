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

	void ArcAttack::determineYVelocity()
	{
		// �ʱ� Y�ӵ� ���
		mInitialYVelocity = (mTargetPosition.y - mStartPosition.y + 0.5f * 9.81f * mArrivalTime * mArrivalTime) / mArrivalTime;
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

			if (mDestroyDurationTime >= mDestroyTime)
			{
				GetScene()->DestroyGameObject(GetGameObject());
			}
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
