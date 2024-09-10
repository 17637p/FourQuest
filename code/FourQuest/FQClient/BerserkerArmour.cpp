#include "BerserkerArmour.h"
#include "Attack.h"
#include "DamageCalculation.h"
#include "Player.h"
#include "../FQGameModule/Transform.h"
#include "../FQGameModule/CharacterController.h"
#include "../FQGameModule/Animator.h"

namespace fq::client
{
	void BerserkerArmour::EmitAttackIntend(EBerserkerAttackType attackType, const Vector3& offset, const Vector3& scale, float knocBackPower, float destroyTime)
	{
		auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(mAttackPrefab);
		auto& attackObj = *(instance.begin());

		auto attackComponent = attackObj->GetComponent<client::Attack>();
		auto attackTransform = attackObj->GetComponent<game_module::Transform>();
		attackComponent->SetDestroyTime(destroyTime);

		const auto addtiveTransform = DirectX::SimpleMath::Matrix::CreateScale(scale) * DirectX::SimpleMath::Matrix::CreateTranslation(offset);
		attackTransform->SetLocalMatrix(attackTransform->GetLocalMatrix() * addtiveTransform);
		mTransform->AddChild(attackTransform);

		DirectX::SimpleMath::Vector3 direction;
		EKnockBackType knockBackType;
		std::string hitSoundName;

		auto foward = mTransform->GetLookAtVector();
		auto right = DirectX::SimpleMath::Matrix::CreateFromQuaternion(mTransform->GetWorldRotation()).Right();

		switch (attackType)
		{
		case EBerserkerAttackType::Left:
			hitSoundName = mLeftAttackHitSound;
			knockBackType = EKnockBackType::TargetPositionAndDirectionByAngle;
			direction = -right;
			break;
		case EBerserkerAttackType::Right:
			hitSoundName = mRightAttackHitSound;
			knockBackType = EKnockBackType::TargetPositionAndDirectionByAngle;
			direction = right;
			break;
		case EBerserkerAttackType::StrikeDown:
			hitSoundName = mStrikeDownAttackHitSound;
			knockBackType = EKnockBackType::TargetPosition;
			direction = foward;
			break;
		case EBerserkerAttackType::SwingAround:
			hitSoundName = mSwingAroundHitSound;
			knockBackType = EKnockBackType::TargetPositionAndKnockDown;
			direction = foward;
			break;
		case EBerserkerAttackType::Rush:
			hitSoundName = mAttackRushHitSound;
			knockBackType = EKnockBackType::Fixed;
			direction = foward;
			break;
		default:
			assert(false);
			break;
		}

		AttackInfo attackInfo{};
		attackInfo.attacker = GetGameObject();
		attackInfo.damage = dc::GetSwordDamage(mPlayer->GetAttackPower());
		attackInfo.type = knockBackType;
		attackInfo.attackDirection = direction;
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

		attackComponent->Set(attackInfo);

		GetScene()->AddGameObject(attackObj);
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
			mSwingAroundElapsedTime = mSwingAroundCoolTime;
		}

		// RushCharging
		if (input->IsPadKeyState(mController->GetControllerID(), EPadKey::RightThumb, EKeyState::Tap)
			&& mRushElapsedTime == 0.f)
		{
			mAnimator->SetParameterTrigger("OnRushCharging");
			mRushElapsedTime = mRushCoolTime;
		}
	}
}