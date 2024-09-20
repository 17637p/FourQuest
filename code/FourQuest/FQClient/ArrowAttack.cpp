#include "ArrowAttack.h"

#include "../FQGameModule/Transform.h"
#include "../FQGameModule/RigidBody.h"
#include "../FQGameModule/BoxCollider.h"
#include "../FQGameModule/CapsuleCollider.h"
#include "../FQGameModule/Scene.h"
#include "../FQGameModule/EventManager.h"
#include "../FQGameModule/Event.h"

#include "Attack.h"

namespace fq::client
{
	ArrowAttack::ArrowAttack()
		: mbIsStrongAttack(false)
		, mLifeTime()
		, mLifeElapsedTime()
		, mWeakAttackPower()
		, mStrongAttackPower()
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
		
		// ������ �� ��� ���������� ���ư�����
		if (mbIsStrongAttack && mMaxBlockCount > 0)
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

		if (collision.other->GetTag() == fq::game_module::ETag::Monster
			|| collision.other->GetTag() == fq::game_module::ETag::Spawner
			|| collision.other->GetTag() == fq::game_module::ETag::Box)
		{
			mMaxBlockCount--;

		}

		if (collision.other->GetTag() == fq::game_module::ETag::Floor
			|| collision.other->GetTag() == fq::game_module::ETag::Wall)
		{
			mMaxBlockCount = 0;
		}

		if (mMaxBlockCount > 0)
		{
			return;
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
		mMaxBlockCount = info.remainingAttackCount;
		mHitSound = info.hitSound;
		mAttackTransform = info.attackTransform;
		mWeakProjectileVelocity = info.weakProjectileVelocity;
		mStrongProjectileVelocity = info.strongProjectileVelocity;
		mAttackDirection = info.attackDirection;

		if (GetGameObject()->HasComponent<fq::game_module::RigidBody>())
		{
			auto transform = GetComponent<fq::game_module::Transform>();
			auto rigidBody = GetComponent<fq::game_module::RigidBody>();

			transform->SetLocalMatrix(mAttackTransform);
			rigidBody->SetLinearVelocity(mAttackDirection * info.weakProjectileVelocity);
		}

		if (GetGameObject()->HasComponent<Attack>())
		{
			auto attack = GetComponent<Attack>();
			AttackInfo info;
			info.attacker = mAttacker;
			info.hitSound = mHitSound;
			info.attackDirection = mAttackDirection;
			info.attackPosition = mAttackTransform.Translation();

			if (mbIsStrongAttack)
			{
				info.remainingAttackCount = mMaxBlockCount;
				info.damage = mStrongAttackPower;
			}
			else
			{
				info.remainingAttackCount = 1;
				info.damage = mWeakAttackPower;
			}

			attack->Set(info);
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
