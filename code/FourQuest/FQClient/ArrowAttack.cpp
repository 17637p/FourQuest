#include "ArrowAttack.h"

#include "../FQGameModule/Transform.h"
#include "../FQGameModule/RigidBody.h"
#include "../FQGameModule/BoxCollider.h"
#include "../FQGameModule/CapsuleCollider.h"
#include "../FQGameModule/Scene.h"
#include "../FQGameModule/EventManager.h"
#include "../FQGameModule/Event.h"
#include "../FQGameModule/UVAnimator.h"
#include "../FQGameModule/StateEventEmitter.h"

#include "Attack.h"
#include "Player.h"
#include "PlayerSoulVariable.h"
#include "MeleeMonster.h"
#include "PlantMonster.h"
#include "BossMonster.h"
#include "MonsterSpawner.h"

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
		, mHitMonsterID{}
	{
	}

	ArrowAttack::~ArrowAttack()
	{
	}

	void ArrowAttack::OnUpdate(float dt)
	{
		// ȭ���� ������ �� �ڿ� ���� �ð� �� ������Ʈ ����
		if (mMaxBlockCount <= 0 || mbIsStrongAttack)
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

		// ���� ī��Ʈ�� 0�̸�, �ݶ��̴� ���� �� �ε��� �ش� ������Ʈ�� �θ� ������Ʈ�� ����
		if (mMaxBlockCount == 1)
		{
			GetGameObject()->SetTag(fq::game_module::ETag::Arrow);
		}
	}

	void ArrowAttack::OnStart()
	{
		if (mMaxBlockCount == 1)
		{
			GetGameObject()->SetTag(fq::game_module::ETag::Arrow);
		}
	}

	void ArrowAttack::OnTriggerEnter(const game_module::Collision& collision)
	{
		// ������ ������ ó������ �ʽ��ϴ� 
		if (GetGameObject()->IsDestroyed())
		{
			return;
		}

		// ���� �� �ڽ��� �浹 �� ���� ī��Ʈ ����
		if (collision.other->GetTag() == fq::game_module::ETag::RangeHitBox)
		{
			// �̹� ������ ������ ��Ʈ �ڽ��� �ٽ� �浹 ���� ��, üũ
			auto monsterIter = mHitMonsterID.find(collision.other->GetParent()->GetID());
			if (monsterIter == mHitMonsterID.end())
			{
				mHitMonsterID.insert(collision.other->GetParent()->GetID());
			}
			else
			{
				return;
			}

			mMaxBlockCount--;
		}

		if (collision.other->GetTag() == fq::game_module::ETag::Spawner
			|| collision.other->GetTag() == fq::game_module::ETag::Box)
		{
			mMaxBlockCount--;
		}

		// �ٴ� �� ���� �ε����� ���� ī��Ʈ ��� 0
		if (collision.other->GetTag() == fq::game_module::ETag::Floor
			|| collision.other->GetTag() == fq::game_module::ETag::Wall
			|| collision.other->GetTag() == fq::game_module::ETag::Untagged)
		{
			mMaxBlockCount = 0;
		}

		auto parent = collision.other->GetParent();
		if (parent)
		{
			// ������ ȭ���� �θ� ������Ʈ�� �������� ���� �Լ��� �����մϴ�.
			if (parent->HasComponent<MeleeMonster>())
			{
				auto melee = parent->GetComponent<MeleeMonster>();
				melee->HitArrow(GetGameObject());
			}
			if (parent->HasComponent<PlantMonster>())
			{
				auto plant = parent->GetComponent<PlantMonster>();
				plant->HitArrow(GetGameObject());
			}
			if (parent->HasComponent<BossMonster>())
			{
				auto boss = parent->GetComponent<BossMonster>();
				boss->HitArrow(GetGameObject());
			}
		}

		if (collision.other->HasComponent<MonsterSpawner>())
		{
			auto monster = collision.other->GetComponent<MonsterSpawner>();
			monster->HitArrow(GetGameObject());
		}

		if (mMaxBlockCount <= 0)
		{
			auto transform = GetComponent<fq::game_module::Transform>();
			auto otherTransform = collision.other->GetComponent<fq::game_module::Transform>();

			transform->SetLocalMatrix(transform->GetWorldMatrix() * otherTransform->GetWorldMatrix().Invert());
			transform->SetParent(otherTransform);

			GetGameObject()->RemoveComponent<fq::game_module::RigidBody>();
			GetGameObject()->RemoveComponent<fq::game_module::BoxCollider>();
			GetGameObject()->RemoveComponent<fq::game_module::CapsuleCollider>();
			GetGameObject()->RemoveComponent<fq::game_module::StateEventEmitter>();

			// ����Ʈ ����
			for (auto child : GetGameObject()->GetChildren())
			{
				auto uvAnimatorOrNull = child->GetComponent<fq::game_module::UVAnimator>();

				if (uvAnimatorOrNull != nullptr)
				{
					GetScene()->DestroyGameObject(child);
				}
			}
		}
	}

	void ArrowAttack::Set(const ArrowAttackInfo& info)
	{
		// �ӵ�, ���ݷ�, ���� ī��Ʈ, ������ ���� ���
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

		// ������, ����ݿ� ���� Attack ������Ʈ ������ ����
		if (GetGameObject()->HasComponent<Attack>())
		{
			auto attack = GetComponent<Attack>();
			AttackInfo attackInfo;
			attackInfo.attacker = mAttacker;
			attackInfo.hitSound = mHitSound;
			attackInfo.attackDirection = mAttackDirection;
			attackInfo.attackPosition = mAttackTransform.Translation();
			attackInfo.mHitCallback = [this, isIncrease = false]() mutable
				{
					if (!isIncrease)
					{
						mAttacker->GetComponent<Player>()->AddSoulGauge(PlayerSoulVariable::SoulGaugeCharging);
						isIncrease = true;
					}
				};

			if (mbIsStrongAttack)
			{
				attackInfo.remainingAttackCount = mMaxBlockCount;
				attackInfo.damage = mStrongAttackPower;
			}
			else
			{
				attackInfo.remainingAttackCount = 5;
				attackInfo.damage = mWeakAttackPower;
			}


			attackInfo.HitEffectName = info.HitEffectName;
			attack->Set(attackInfo);
		}
		else
		{
			spdlog::error("ERROR : GameObject(this) have not Attack!!!");
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
