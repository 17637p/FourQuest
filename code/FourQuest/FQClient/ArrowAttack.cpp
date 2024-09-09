#include "ArrowAttack.h"

#include "../FQGameModule/Transform.h"
#include "../FQGameModule/RigidBody.h"
#include "../FQGameModule/BoxCollider.h"
#include "../FQGameModule/CapsuleCollider.h"
#include "../FQGameModule/Scene.h"
#include "../FQGameModule/EventManager.h"
#include "../FQGameModule/Event.h"

namespace fq::client
{
	ArrowAttack::ArrowAttack()
		: mbIsStrongAttack(false)
		, mLifeTime()
		, mLifeElapsedTime()
		, mWeakAttackPower()
		, mStrongAttackPower()
		, mWeakProjectileVelocity()
		, mStrongProjectileVelocity()
		, mMaxBlockCount()
		, mAttacker(nullptr)
		, mHitSound{}
		, mAttackDirection{}
		, mAttackTransform{}
	{
	}

	ArrowAttack::~ArrowAttack()
	{
	}

	void ArrowAttack::OnUpdate(float dt)
	{
		if (mMaxBlockCount <= 0)
		{
			mLifeElapsedTime += dt;

			if (mLifeElapsedTime >= mLifeTime)
			{
				GetScene()->DestroyGameObject(GetGameObject());
			}
		}

		if (mbIsStrongAttack)
		{
			auto transform = GetComponent<game_module::Transform>();
			DirectX::SimpleMath::Vector3 deltaPosition = mAttackDirection * dt * mStrongProjectileVelocity;
			transform->AddLocalPosition(deltaPosition);
		}
	}

	void ArrowAttack::OnTriggerEnter(const game_module::Collision& collision)
	{
		// ������ ������ ó������ �ʽ��ϴ� 
		if (GetGameObject()->IsDestroyed())
		{
			return;
		}

		if (collision.other->GetTag() == fq::game_module::ETag::Monster)
		{
			mMaxBlockCount--;

			if (mMaxBlockCount > 0)
			{
				return;
			}
		}

		auto transform = GetComponent<fq::game_module::Transform>();
		auto otherTransform = collision.other->GetComponent<fq::game_module::Transform>();

		transform->SetLocalMatrix(transform->GetWorldMatrix() * otherTransform->GetWorldMatrix().Invert());
		transform->SetParent(otherTransform);

		GetGameObject()->RemoveComponent<fq::game_module::RigidBody>();
		GetGameObject()->RemoveComponent<fq::game_module::BoxCollider>();
		GetGameObject()->RemoveComponent<fq::game_module::CapsuleCollider>();


	}

	void ArrowAttack::Set(const ArrowAttackInfo& info)
	{
		mAttacker = info.attacker;
		mbIsStrongAttack = info.bIsStrongAttack;
		mWeakAttackPower = info.weakDamage;
		mStrongAttackPower = info.strongDamage;
		mWeakProjectileVelocity = info.weakProjectileVelocity;
		mStrongProjectileVelocity = info.strongProjectileVelocity;
		mMaxBlockCount = info.remainingAttackCount;
		mLifeTime = info.lifeTime;
		mHitSound = info.hitSound;
		mAttackDirection = info.attackDirection;
		mAttackTransform = info.attackTransform;

		if (GetGameObject()->HasComponent<fq::game_module::RigidBody>())
		{
			auto transform = GetComponent<fq::game_module::Transform>();
			auto rigidBody = GetComponent<fq::game_module::RigidBody>();

			transform->SetLocalMatrix(mAttackTransform);
			rigidBody->SetLinearVelocity(mAttackDirection * mWeakProjectileVelocity);
		}
	}

	std::shared_ptr<fq::game_module::Component> ArrowAttack::Clone(std::shared_ptr<Component> clone) const
	{
		auto cloneArmour = std::dynamic_pointer_cast<ArrowAttack>(clone);

		if (cloneArmour == nullptr) // ���� �����ؼ� ���纻�� �ش�
		{
			cloneArmour = game_module::ObjectPool::GetInstance()->Assign<ArrowAttack>(*this);
		}
		else // clone�� �����͸� �����Ѵ�.
		{
			// �⺻ ���� ������ ȣ���Ѵ�.
			*cloneArmour = *this;
		}

		return cloneArmour;
	}

	void fq::client::ArrowAttack::PlayHitSound()
	{
		if (!mHitSound.empty())
		{
			GetScene()->GetEventManager()->FireEvent<fq::event::OnPlaySound>({ mHitSound, false , 3 });
		}
	}
}
