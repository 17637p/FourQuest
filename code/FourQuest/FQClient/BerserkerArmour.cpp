#include "BerserkerArmour.h"
#include "Attack.h"
#include "DamageCalculation.h"
#include "Player.h"
#include "PlayerVariable.h"
#include "../FQGameModule/Transform.h"
#include "../FQGameModule/CharacterController.h"
#include "../FQGameModule/Animator.h"

namespace fq::client
{
	std::shared_ptr<fq::game_module::GameObject> BerserkerArmour::EmitAttackIntend(EBerserkerAttackType attackType, const Vector3& offset, const Vector3& scale, float knocBackPower, float destroyTime)
	{
		DirectX::SimpleMath::Vector3 direction;
		EKnockBackType knockBackType;
		std::string hitSoundName;
		float damage;
		fq::game_module::PrefabResource attackPrefabResource;

		auto foward = mTransform->GetLookAtVector();
		auto right = DirectX::SimpleMath::Matrix::CreateFromQuaternion(mTransform->GetWorldRotation()).Right();

		switch (attackType)
		{
			case EBerserkerAttackType::Left:
				hitSoundName = mLeftAttackHitSound;
				knockBackType = EKnockBackType::TargetPositionAndDirectionByAngle;
				direction = -right;
				damage = dc::GetBluntFirstConsecutiveAttackDamage(mPlayer->GetAttackPower());
				attackPrefabResource = mBoxAttackPrefab;
				break;
			case EBerserkerAttackType::Right:
				hitSoundName = mRightAttackHitSound;
				knockBackType = EKnockBackType::TargetPositionAndDirectionByAngle;
				direction = right;
				damage = dc::GetBluntSecondConsecutiveAttackDamage(mPlayer->GetAttackPower());
				attackPrefabResource = mBoxAttackPrefab;
				break;
			case EBerserkerAttackType::StrikeDown:
				hitSoundName = mStrikeDownAttackHitSound;
				knockBackType = EKnockBackType::TargetPosition;
				direction = foward;
				damage = dc::GetBluntThirdConsecutiveAttackDamage(mPlayer->GetAttackPower());
				attackPrefabResource = mBoxAttackPrefab;
				break;
			case EBerserkerAttackType::SwingAround:
				hitSoundName = mSwingAroundHitSound;
				knockBackType = EKnockBackType::TargetPositionAndKnockDown;
				direction = foward;
				damage = dc::GetBluntSwingAroundDamage(mPlayer->GetAttackPower());
				attackPrefabResource = mCircleAttackPrefab;
				break;
			case EBerserkerAttackType::Rush:
				hitSoundName = mAttackRushHitSound;
				knockBackType = EKnockBackType::TargetPosition;
				direction = foward;
				damage = dc::GetBluntRsuhDamage(mPlayer->GetAttackPower());
				attackPrefabResource = mCircleAttackPrefab;
				break;
			default:
				assert(false);
				break;
		}

		auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(attackPrefabResource);
		auto& attackObj = *(instance.begin());

		auto attackComponent = attackObj->GetComponent<client::Attack>();
		auto attackTransform = attackObj->GetComponent<game_module::Transform>();
		attackComponent->SetDestroyTime(destroyTime);

		const auto addtiveTransform = DirectX::SimpleMath::Matrix::CreateScale(scale) * DirectX::SimpleMath::Matrix::CreateTranslation(offset);
		attackTransform->SetLocalMatrix(attackTransform->GetLocalMatrix() * addtiveTransform);
		mTransform->AddChild(attackTransform);

		AttackInfo attackInfo{};
		attackInfo.attacker = GetGameObject();
		attackInfo.damage = damage;
		attackInfo.type = knockBackType;
		attackInfo.attackDirection = direction;
		attackInfo.attackPosition = mTransform->GetWorldPosition();
		attackInfo.knocBackPower = knocBackPower;
		attackInfo.mHitCallback = [this, isIncrease = false]() mutable
			{
				if (!isIncrease)
				{
					this->mPlayer->AddSoulGauge(PlayerSoulVariable::SoulGaugeCharging);
					isIncrease = true;
				}
			};
		attackInfo.hitSound = hitSoundName;
		attackInfo.targetPosRatio = mTargetPosRatio;
		attackInfo.directionRatio = mDirectionRatio;
		attackInfo.HitEffectName = "W_Hit_blunt";

		attackComponent->Set(attackInfo);

		GetScene()->AddGameObject(attackObj);

		// 공격시 체력 감소 
		mPlayer->DecreaseHp(PlayerVariable::HpReductionOnAttack, true, true);

		return attackObj;
	}

	std::shared_ptr<fq::game_module::Component> BerserkerArmour::Clone(std::shared_ptr<Component> clone) const
	{
		auto cloneArmour = std::dynamic_pointer_cast<BerserkerArmour>(clone);

		if (cloneArmour == nullptr) // 새로 생성해서 복사본을 준다
		{
			cloneArmour = game_module::ObjectPool::GetInstance()->Assign<BerserkerArmour>(*this);
		}
		else // clone에 데이터를 복사한다.
		{
			// 기본 대입 연산자 호출한다.
			*cloneArmour = *this;
		}

		return cloneArmour;
	}

	void BerserkerArmour::OnUpdate(float dt)
	{
		checkSkillCoolTime(dt);
		checkInput();
	}

	void BerserkerArmour::OnStart()
	{
		mAnimator = GetComponent<game_module::Animator>();
		mController = GetComponent<game_module::CharacterController>();
		mTransform = GetComponent<game_module::Transform>();
		mPlayer = GetComponent<Player>();
	}

	void BerserkerArmour::checkSkillCoolTime(float dt)
	{
		mSwingAroundElapsedTime = std::max<float>(0.f, mSwingAroundElapsedTime - dt);
		mRushElapsedTime = std::max<float>(0.f, mRushElapsedTime - dt);

		if (mPlayer->IsFeverTime())
		{
			mPlayer->SetASkillCoolTimeRatio(mSwingAroundElapsedTime / (mSwingAroundCoolTime - mSwingAroundCoolTimeReduction));
			mPlayer->SetRSkillCoolTimeRatio(mRushElapsedTime / (mRushCoolTime - mRushCoolTimeReduction));
		}
		else
		{
			mPlayer->SetASkillCoolTimeRatio(mSwingAroundElapsedTime / mSwingAroundCoolTime);
			mPlayer->SetRSkillCoolTimeRatio(mRushElapsedTime / mRushCoolTime);
		}
	}

	void BerserkerArmour::checkInput()
	{
		using namespace DirectX::SimpleMath;

		auto input = GetScene()->GetInputManager();

		// SwingAround
		if (input->IsPadKeyState(mController->GetControllerID(), EPadKey::A, EKeyState::Tap)
			&& mSwingAroundElapsedTime == 0.f)
		{
			mAnimator->SetParameterTrigger("OnSwingAround");
			mSwingAroundElapsedTime = mPlayer->IsFeverTime() ? mSwingAroundCoolTime - mSwingAroundCoolTimeReduction : mSwingAroundCoolTime;
		}

		DirectX::SimpleMath::Vector3 rightInput{};
		rightInput.x = input->GetStickInfomation(mController->GetControllerID(), EPadStick::rightX);
		rightInput.z = input->GetStickInfomation(mController->GetControllerID(), EPadStick::rightY);
		constexpr float rotationOffsetSq = 0.5f * 0.5f;

		// RushCharging
		if (rightInput.LengthSquared() >= rotationOffsetSq && mRushElapsedTime == 0.f)
		{
			mAnimator->SetParameterTrigger("OnRushCharging");
			mRushElapsedTime = mPlayer->IsFeverTime() ? mRushCoolTime - mRushCoolTimeReduction : mRushCoolTime;
		}
		else
		{
			mAnimator->SetParameterOffTrigger("OnRushCharging");
		}
	}
}