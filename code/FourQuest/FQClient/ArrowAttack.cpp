#include "ArrowAttack.h"

#include "../FQGameModule/Transform.h"
#include "../FQGameModule/RigidBody.h"
#include "../FQGameModule/BoxCollider.h"
#include "../FQGameModule/CapsuleCollider.h"
#include "../FQGameModule/Scene.h"
#include "../FQGameModule/EventManager.h"
#include "../FQGameModule/Event.h"

#include "Attack.h"
#include "Player.h"
#include "PlayerSoulVariable.h"

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
		// 화살이 박히고 난 뒤에 일정 시간 뒤 오브젝트 삭제
		if (mMaxBlockCount <= 0)
		{
			mLifeElapsedTime += dt;

			if (mLifeElapsedTime >= mLifeTime)
			{
				GetScene()->DestroyGameObject(GetGameObject());
			}
		}

		// 강공격 일 경우 일직선으로 날아가도록
		if (mbIsStrongAttack && mMaxBlockCount > 0)
		{
			auto transform = GetComponent<game_module::Transform>();
			DirectX::SimpleMath::Vector3 deltaPosition = mAttackDirection * dt * mStrongProjectileVelocity;
			transform->AddLocalPosition(deltaPosition);
		}
	}

	void ArrowAttack::OnTriggerEnter(const game_module::Collision& collision)
	{
		// 삭제된 공격은 처리하지 않습니다 
		if (GetGameObject()->IsDestroyed())
		{
			return;
		}

		// 몬스터 및 박스에 충돌 시 관통 카운트 감소
		if (collision.other->GetTag() == fq::game_module::ETag::Monster
			|| collision.other->GetTag() == fq::game_module::ETag::Spawner
			|| collision.other->GetTag() == fq::game_module::ETag::Box)
		{
			mMaxBlockCount--;
		}

		// 바닥 및 벽에 부딪히면 관통 카운트 즉시 0
		if (collision.other->GetTag() == fq::game_module::ETag::Floor
			|| collision.other->GetTag() == fq::game_module::ETag::Wall)
		{
			mMaxBlockCount = 0;
		}

		// 관통 카운트가 0이면, 콜라이더 삭제 및 부딪힌 해당 오브젝트를 부모 오브젝트로 지정
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
		// 속도, 공격력, 관통 카운트, 강공격 여부 등등
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

		// 강공격, 약공격에 따라 Attack 컴포넌트 데이터 세팅
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
				attackInfo.remainingAttackCount = 1;
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

		if (cloneArmour == nullptr) // 새로 생성해서 복사본을 준다
		{
			cloneArmour = game_module::ObjectPool::GetInstance()->Assign<ArrowAttack>(*this);
		}
		else // clone에 데이터를 복사한다.
		{
			// 기본 대입 연산자 호출한다.
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
