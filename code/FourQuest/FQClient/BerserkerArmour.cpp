#include "BerserkerArmour.h"
#include "Attack.h"
#include "DamageCalculation.h"
#include "Player.h"
#include "PlayerVariable.h"
#include "../FQGameModule/Transform.h"
#include "../FQGameModule/CharacterController.h"
#include "../FQGameModule/Animator.h"
#include "SpeechBubbleUI.h"
#include "PlayerInfoVariable.h"

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
				EmitSound(EBerserkerSoundType::Left);
				hitSoundName = mLeftAttackHitSound;
				knockBackType = EKnockBackType::TargetPositionAndDirectionByAngle;
				direction = -right;
				damage = dc::GetBluntFirstConsecutiveAttackDamage(mPlayer->GetAttackPower());
				attackPrefabResource = mBoxAttackPrefab;
				break;
			case EBerserkerAttackType::Right:
				EmitSound(EBerserkerSoundType::Right);
				hitSoundName = mRightAttackHitSound;
				knockBackType = EKnockBackType::TargetPositionAndDirectionByAngle;
				direction = right;
				damage = dc::GetBluntSecondConsecutiveAttackDamage(mPlayer->GetAttackPower());
				attackPrefabResource = mBoxAttackPrefab;
				break;
			case EBerserkerAttackType::StrikeDown:
				EmitSound(EBerserkerSoundType::StrikeDown);
				hitSoundName = mStrikeDownAttackHitSound;
				knockBackType = EKnockBackType::TargetPosition;
				direction = foward;
				damage = dc::GetBluntThirdConsecutiveAttackDamage(mPlayer->GetAttackPower());
				attackPrefabResource = mBoxAttackPrefab;
				break;
			case EBerserkerAttackType::SwingAround:
				EmitSound(EBerserkerSoundType::SwingAround);
				hitSoundName = mSwingAroundHitSound;
				knockBackType = EKnockBackType::TargetPositionAndKnockDown;
				direction = foward;
				damage = dc::GetBluntSwingAroundDamage(mPlayer->GetAttackPower());
				attackPrefabResource = mCircleAttackPrefab;
				break;
			case EBerserkerAttackType::Rush:
				EmitSound(EBerserkerSoundType::Rush);
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
		attackInfo.mHitCallback = [this, isIncrease = false, attackType]() mutable
			{
				if (!isIncrease)
				{
					this->mPlayer->AddSoulGauge(PlayerSoulVariable::SoulGaugeCharging);
					isIncrease = true;
				}

				if (attackType == EBerserkerAttackType::Rush)
				{
					auto input = GetScene()->GetInputManager();
					input->SetVibration(mController->GetControllerID()
						, mDashHitVibrationMode
						, mDashHitVibrationIntensity
						, mDashHitVibrationDuration);
				}
			};

		attackInfo.hitSound = hitSoundName;
		attackInfo.targetPosRatio = mTargetPosRatio;
		attackInfo.directionRatio = mDirectionRatio;
		attackInfo.HitEffectName = "W_Hit_blunt";

		attackComponent->Set(attackInfo);

		GetScene()->AddGameObject(attackObj);

		// ���ݽ� ü�� ���� 
		mPlayer->DecreaseHp(PlayerVariable::HpReductionOnAttack, true, true);

		return attackObj;
	}

	void BerserkerArmour::EmitSound(EBerserkerSoundType soundType)
	{
		std::string soundName;

		switch (soundType)
		{
			case fq::client::EBerserkerSoundType::Left:
				soundName = mLeftAttackSound;
				break;
			case fq::client::EBerserkerSoundType::Right:
				soundName = mRightAttackSound;
				break;
			case fq::client::EBerserkerSoundType::StrikeDown:
				soundName = mStrikeDownAttackSound;
				break;
			case fq::client::EBerserkerSoundType::SwingAround:
				soundName = mSwingAroundSound;
				break;
			case fq::client::EBerserkerSoundType::Rush:
				soundName = mAttackRushSound;
				break;
			case fq::client::EBerserkerSoundType::RushReady:
				soundName = mAttackRushReadySound;
				break;
			default:
				assert(false);
				break;
		}

		GetScene()->GetEventManager()->FireEvent<fq::event::OnPlaySound>({ soundName, false , fq::sound::EChannel::SE });
	}

	std::shared_ptr<fq::game_module::Component> BerserkerArmour::Clone(std::shared_ptr<Component> clone) const
	{
		auto cloneArmour = std::dynamic_pointer_cast<BerserkerArmour>(clone);

		if (cloneArmour == nullptr) // ���� �����ؼ� ���纻�� �ش�
		{
			cloneArmour = game_module::ObjectPool::GetInstance()->Assign<BerserkerArmour>(*this);
		}
		else // clone�� �����͸� �����Ѵ�.
		{
			// �⺻ ���� ������ ȣ���Ѵ�.
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

		setName();
	}

	void BerserkerArmour::checkSkillCoolTime(float dt)
	{
		mSwingAroundElapsedTime = std::max<float>(0.f, mSwingAroundElapsedTime - dt);
		mRushElapsedTime = std::max<float>(0.f, mRushElapsedTime - dt);
		float decreaseCooltime = mPlayer->GetGBDecreaseCooltime();

		if (mPlayer->IsFeverTime())
		{
			float swingCoolTime = (mSwingAroundCoolTime - mSwingAroundCoolTimeReduction) * decreaseCooltime;
			float rushCoolTime = (mRushCoolTime - mRushCoolTimeReduction) * decreaseCooltime;
			mPlayer->SetASkillCoolTimeRatio(mSwingAroundElapsedTime / swingCoolTime);
			mPlayer->SetRSkillCoolTimeRatio(mRushElapsedTime / rushCoolTime);
		}
		else
		{
			float swingCoolTime = mSwingAroundCoolTime * decreaseCooltime;
			float rushCoolTime = mRushCoolTime * decreaseCooltime;
			mPlayer->SetASkillCoolTimeRatio(mSwingAroundElapsedTime / swingCoolTime);
			mPlayer->SetRSkillCoolTimeRatio(mRushElapsedTime / rushCoolTime);
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
			mSwingAroundElapsedTime *= mPlayer->GetGBDecreaseCooltime();
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
			mRushElapsedTime *= mPlayer->GetGBDecreaseCooltime();
		}
	}

	void BerserkerArmour::setName()
	{
		int soulType = static_cast<int>(mPlayer->GetSoulType());
		SpeechBubbleUI* speechBubble = nullptr;
		for (auto& child : GetGameObject()->GetChildren())
		{
			if (child->HasComponent<SpeechBubbleUI>())
			{
				speechBubble = child->GetComponent<SpeechBubbleUI>();
			}
		}

		if (speechBubble != nullptr)
		{
			switch (soulType)
			{
				case 0:
					speechBubble->SetName(PlayerInfoVariable::KnightName);
					break;
				case 1:
					speechBubble->SetName(PlayerInfoVariable::MagicName);
					break;
				case 2:
					speechBubble->SetName(PlayerInfoVariable::BerserkerName);
					break;
				case 3:
					speechBubble->SetName(PlayerInfoVariable::ArcherName);
					break;
				default:
					break;
			}
		}
	}

}