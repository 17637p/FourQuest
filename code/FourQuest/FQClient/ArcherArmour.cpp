#define NOMINMAX
#include "ArcherArmour.h"

#include "../FQGameModule/Transform.h"
#include "../FQGameModule/Animator.h"
#include "../FQGameModule/CharacterController.h"
#include "../FQGameModule/RigidBody.h"
#include "../FQGameModule/UVAnimator.h"

#include "Player.h"
#include "PlayerVariable.h"
#include "AimAssist.h"
#include "ArrowAttack.h"
#include "DamageCalculation.h"
#include "LinearAttack.h"
#include "SpeechBubbleUI.h"
#include "PlayerInfoVariable.h"
#include "EffectColorTransmitter.h"
#include "ClientEvent.h"

namespace fq::client
{
	ArcherArmour::ArcherArmour()
		: mAnimator(nullptr)
		, mController(nullptr)
		, mTransform()
		, mPlayer(nullptr)
		, mAimAssist(nullptr)
		, mWeaponeSocketT(nullptr)
		, mWeakAttack()
		, mStrongAttack()
		, mDashCoolTime()
		, mDashElapsedTime()
		, mDashCoolTimeReduction(0.f)
		, mStrongAttackCoolTime(0.f)
		, mStrongAttackCoolTimeReduction(0.f)
		, mStrongArrowOffset(15.f)
		, mStrongArrowRange(20.f)
		, mArrowPower(0.f)
		, mRStickNoInputTime{ 0.f }
	{
	}

	ArcherArmour::~ArcherArmour()
	{
	}

	void ArcherArmour::EmitmWeakAttack()
	{
		auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(mWeakAttack);
		auto& attackObj = *(instance.begin());

		auto attackComponent = attackObj->GetComponent<client::ArrowAttack>();
		auto rigidBodyComponent = attackObj->GetComponent<fq::game_module::RigidBody>();
		auto attackT = attackObj->GetComponent<game_module::Transform>();
		auto foward = mTransform->GetLookAtVector();
		DirectX::SimpleMath::Vector3 position = mWeaponeSocketT->GetWorldPosition();

		// 공격 트랜스폼 설정
		DirectX::SimpleMath::Quaternion rotation = mTransform->GetWorldRotation();
		const auto& localOffset = DirectX::SimpleMath::Vector3::TransformNormal(attackT->GetWorldPosition(), mTransform->GetWorldMatrix());
		attackT->GenerateWorld(position + localOffset, rotation, attackT->GetWorldScale());

		// 공격 설정
		ArrowAttackInfo attackInfo{};
		attackInfo.weakDamage = dc::GetArcherWADamage(mPlayer->GetAttackPower());
		attackInfo.weakProjectileVelocity = mWeakProjectileVelocity;
		attackInfo.strongProjectileVelocity = mStrongProjectileVelocity;
		attackInfo.attacker = GetGameObject();
		attackInfo.remainingAttackCount = 1;
		attackInfo.attackDirection = foward;
		attackInfo.attackTransform = attackT->GetWorldMatrix();
		attackInfo.bIsStrongAttack = false;
		attackInfo.hitSound = "A_Fastshoot_Hit";
		attackInfo.HitEffectName = "A_Shoot_Hit_blood";
		attackComponent->Set(attackInfo);

		GetScene()->AddGameObject(attackObj);

		// 이펙트 색상 설정
		auto effectColorTransmitter = attackObj->GetComponent<EffectColorTransmitter>();
		if (effectColorTransmitter != nullptr && mPlayer != nullptr)
		{
			effectColorTransmitter->SetSoulType(mPlayer->GetSoulType());
		}

		int id = static_cast<int>(mController->GetControllerID());
		GetScene()->GetEventManager()
			->FireEvent<fq::client::event::PushButtonEvent>({ id, ESkillType::R });
	}

	void ArcherArmour::makeStrongAttackArrow(float damage, DirectX::SimpleMath::Quaternion direction, DirectX::SimpleMath::Vector3 position)
	{
		using namespace DirectX::SimpleMath;

		// 프리팹 인스턴스화 
		auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(mStrongAttack);
		auto& attackObj = *(instance.begin());
		auto attackComponent = attackObj->GetComponent<client::ArrowAttack>();
		auto attackT = attackObj->GetComponent<game_module::Transform>();
		attackT->GenerateWorld(position, direction, attackT->GetWorldScale());
		Vector3 attackDirection = Matrix::CreateFromQuaternion(direction).Forward();

		// 공격 설정
		ArrowAttackInfo attackInfo{};
		attackInfo.attacker = GetGameObject();
		attackInfo.attackDirection = attackDirection;
		attackInfo.attackTransform = attackT->GetWorldMatrix();
		attackInfo.bIsStrongAttack = true;
		attackInfo.hitSound = "A_StrongAttack_Hit";
		attackInfo.HitEffectName = "A_Shoot_Hit_blood";
		attackInfo.remainingAttackCount = 0b11111111;
		attackInfo.strongDamage = damage;
		attackInfo.strongProjectileVelocity = mStrongProjectileVelocity;
		attackComponent->Set(attackInfo);

		// 화살 생존시간 설정
		attackComponent->SetLifeTime(mStrongArrowRange / mStrongProjectileVelocity);
		GetScene()->AddGameObject(attackObj);

		// 이펙트 색상 설정
		auto effectColorTransmitter = attackObj->GetComponent<EffectColorTransmitter>();
		if (effectColorTransmitter != nullptr && mPlayer != nullptr)
		{
			effectColorTransmitter->SetSoulType(mPlayer->GetSoulType());
		}
	}

	void ArcherArmour::MakeLineOfSight()
	{
		auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(mLineOfSightEffect);
		auto& lineObj = *(instance.begin());
		mLineOfSightObject = lineObj.get();

		if (!mLineOfSightObject)
			return;

		auto uvAnimationComponent = mLineOfSightObject->GetComponent<fq::game_module::UVAnimator>();
		auto LOSTrnasform = mLineOfSightObject->GetComponent<fq::game_module::Transform>();
		uvAnimationComponent->SetIsRecursive(false);
		LOSTrnasform->SetParent(mTransform);

		GetScene()->AddGameObject(lineObj);
	}

	void ArcherArmour::EmitStrongAttack(int chargeLevel)
	{
		using namespace DirectX::SimpleMath;
		Vector3 position = mWeaponeSocketT->GetWorldPosition();

		float attackPower = mPlayer->GetAttackPower();

		switch (chargeLevel)
		{
			case 0:
				attackPower = dc::GetArcherCA_1_Damage(attackPower);
				break;
			case 1:
				attackPower = dc::GetArcherCA_2_Damage(attackPower);
				break;
			case 2:
				attackPower = dc::GetArcherCA_3_Damage(attackPower);
				break;
			case 4:
				attackPower = dc::GetArcherCA_4_Damage(attackPower);
				break;
		}

		// 차지레벨에 따라서 화살을 생성 
		auto directions = GetStrongArrowDirections(chargeLevel);
		for (int i = 0; i < directions.size(); ++i)
		{
			makeStrongAttackArrow(attackPower, directions[i], position);
		}

		// 키입력 이벤트 
		int id = static_cast<int>(mController->GetControllerID());
		GetScene()->GetEventManager()
			->FireEvent<fq::client::event::PushButtonEvent>({ id, ESkillType::X });
	}

	void ArcherArmour::EmitSound(EArcherSound archerSound)
	{
		std::string soundName;

		switch (archerSound)
		{
			case fq::client::EArcherSound::ShootStart:
				soundName = "A_Shoot_start";
				break;
			case fq::client::EArcherSound::Charge1:
				soundName = "A_Charge_1";
				break;
			case fq::client::EArcherSound::Charge2:
				soundName = "A_Charge_2";
				break;
			case fq::client::EArcherSound::Shoot:
				soundName = "A_Fastshoot_end";
				break;
			case fq::client::EArcherSound::Fastshoot1:
				soundName = "A_Fastshoot_1";
				break;
			case fq::client::EArcherSound::Fastshoot2:
				soundName = "A_Fastshoot_2";
				break;
			case fq::client::EArcherSound::Fastshoot3:
				soundName = "A_Fastshoot_3";
				break;
			case fq::client::EArcherSound::Rolling:
				soundName = "A_Rolling";
				break;
			default:
				break;
		}

		GetScene()->GetEventManager()->FireEvent<fq::event::OnPlaySound>({ soundName, false , fq::sound::EChannel::SE });
	}

	std::shared_ptr<game_module::GameObject> ArcherArmour::EmitChargingEffect()
	{
		//auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(mStrongAttackChargingEffect);
		//auto& chargingObj = *(instance.begin());

		//auto attackT = chargingObj->GetComponent<game_module::Transform>();

		//// 활 트랜스폼 가져오기
		//attackT->SetParent(mTransform->GetChildren()[3]);

		//GetScene()->AddGameObject(chargingObj);

		// 차징 사운드 재생
		//GetScene()->GetEventManager()->FireEvent<fq::event::OnPlaySound>({ "A_Charging", true , 0 });

		//return chargingObj;

		return nullptr;
	}

	std::shared_ptr<game_module::GameObject> ArcherArmour::EmitStrongAttackEffect()
	{
		return nullptr;
	}

	std::shared_ptr<game_module::GameObject> ArcherArmour::EmitDash()
	{
		// 대쉬 이펙트 생성
		auto name = mAnimator->GetController().GetCurrentStateName();

		auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(mDashEffect);
		auto& effectObj = *(instance.begin());
		auto effectT = effectObj->GetComponent<game_module::Transform>();
		effectT->SetParent(mTransform);

		GetScene()->AddGameObject(effectObj);

		// 공격 체력 감소
		mPlayer->DecreaseHp(PlayerVariable::HpReductionOnAttack, true, true);

		// 키 입력 이벤트 호출 
		int id = static_cast<int>(mController->GetControllerID());
		GetScene()->GetEventManager()
			->FireEvent<fq::client::event::PushButtonEvent>({ id, ESkillType::A });

		return effectObj;
	}

	void ArcherArmour::OnStart()
	{
		mController = GetComponent<game_module::CharacterController>();
		mAnimator = GetComponent<game_module::Animator>();
		mTransform = GetComponent<game_module::Transform>();
		mPlayer = GetComponent<Player>();
		for (auto child : GetGameObject()->GetChildren())
		{
			if (child->HasComponent<AimAssist>())
			{
				mAimAssist = child->GetComponent<AimAssist>();
				break;
			}
		}

		// 웨폰 소켓 위치 가져오기  
		for (const auto& child : mTransform->GetChildren())
		{
			auto name = child->GetGameObject()->GetName();

			if (name.find("WeaponeSocket") != std::string::npos)
			{
				mWeaponeSocketT = child->GetComponent<game_module::Transform>();
				break;
			}
		}

		setName();
	}

	void ArcherArmour::OnUpdate(float dt)
	{
		checkSkillCoolTime(dt);
		checkInput(dt);
	}

	void ArcherArmour::checkSkillCoolTime(float dt)
	{
		mDashElapsedTime = std::max(0.f, mDashElapsedTime - dt);
		mStrongAttackElapsedTime = std::max(0.f, mStrongAttackElapsedTime - dt);

		if (mPlayer->IsFeverTime())
		{
			float dashCoolTime = (mDashCoolTime - mDashCoolTimeReduction) * mPlayer->GetGBDecreaseCooltime();
			float strongAttackCoolTime = (mStrongAttackCoolTime - mStrongAttackCoolTimeReduction) * mPlayer->GetGBDecreaseCooltime();
			mPlayer->SetASkillCoolTimeRatio(mDashElapsedTime / dashCoolTime);
			mPlayer->SetXSkillCoolTimeRatio(mStrongAttackElapsedTime / strongAttackCoolTime);
		}
		else
		{
			float dashCoolTime = mDashCoolTime * mPlayer->GetGBDecreaseCooltime();
			float strongAttackCoolTime = mStrongAttackCoolTime * mPlayer->GetGBDecreaseCooltime();
			mPlayer->SetASkillCoolTimeRatio(mDashElapsedTime / dashCoolTime);
			mPlayer->SetXSkillCoolTimeRatio(mStrongAttackElapsedTime / strongAttackCoolTime);
		}
	}

	void ArcherArmour::checkInput(float dt)
	{
		using namespace DirectX::SimpleMath;

		auto input = GetScene()->GetInputManager();
		auto padID = mController->GetControllerID();

		// Dash
		if (input->IsPadKeyState(mController->GetControllerID(), EPadKey::A, EKeyState::Tap)
			&& mDashElapsedTime == 0.f)
		{
			mAnimator->SetParameterTrigger("OnDash");
			mDashElapsedTime = mPlayer->IsFeverTime() ? mDashCoolTime - mDashCoolTimeReduction : mDashCoolTime;
			mDashElapsedTime *= mPlayer->GetGBDecreaseCooltime();
		}
		// StrongAttack
		if (input->IsPadKeyState(mController->GetControllerID(), EPadKey::X, EKeyState::Tap)
			&& mStrongAttackElapsedTime == 0.f)
		{
			mAnimator->SetParameterTrigger("OnStrongAttack");
			mStrongAttackElapsedTime = mPlayer->IsFeverTime() ? mStrongAttackCoolTime - mStrongAttackCoolTimeReduction : mStrongAttackCoolTime;
			mStrongAttackElapsedTime *= mPlayer->GetGBDecreaseCooltime();
		}
		else
		{
			mAnimator->SetParameterOffTrigger("OnStrongAttack");

		}

		// 조준선 삭제
		if (input->IsPadKeyState(mController->GetControllerID(), EPadKey::X, EKeyState::Away))
		{
			if (mLineOfSightObject)
			{
				GetScene()->DestroyGameObject(mLineOfSightObject);
				mLineOfSightObject = nullptr;
			}
		}

		// MultiShot R Stick 조작
		DirectX::SimpleMath::Vector2 r;
		r.x = input->GetStickInfomation(padID, EPadStick::rightX);
		r.y = input->GetStickInfomation(padID, EPadStick::rightY);

		if (r.Length() > 0.f)
		{
			mAnimator->SetParameterBoolean("OnFastShot", true);

			// R Stick의 갑작스러운 방향전환으로 입력이 0이 되는
			// 순간에 스킬이 캔슬되는 것을 방지하는 값입니다
			constexpr float RStickInputCorrectTime = 0.1f;
			mRStickNoInputTime = RStickInputCorrectTime;
		}
		else
		{
			mRStickNoInputTime = std::max(0.f, mRStickNoInputTime - dt);

			if (mRStickNoInputTime == 0.f)
				mAnimator->SetParameterBoolean("OnFastShot", false);
		}
	}

	void ArcherArmour::AimToNearMonster()
	{
		if (mAimAssist)
			mAimAssist->SetNearMonsterDirection();
	}

	std::shared_ptr<fq::game_module::Component> ArcherArmour::Clone(std::shared_ptr<Component> clone) const
	{
		auto cloneArmour = std::dynamic_pointer_cast<ArcherArmour>(clone);

		if (cloneArmour == nullptr) // 새로 생성해서 복사본을 준다
		{
			cloneArmour = game_module::ObjectPool::GetInstance()->Assign<ArcherArmour>(*this);
		}
		else // clone에 데이터를 복사한다.
		{
			// 기본 대입 연산자 호출한다.
			*cloneArmour = *this;
		}

		return cloneArmour;
	}

	void ArcherArmour::setName()
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

	std::vector<DirectX::SimpleMath::Quaternion> ArcherArmour::GetStrongArrowDirections(int chargeLevel)
	{
		using namespace DirectX::SimpleMath;
		std::vector<Quaternion> directions{};
		float offsetAngle = DirectX::XMConvertToRadians(mStrongArrowOffset);

		// 시선방향 
		auto look = mTransform->GetWorldRotation();
		directions.push_back(look);

		// 차징 레벨에 따라서 화살 방향 생성 1/3/5/7
		for (int i = 0; i < chargeLevel; ++i)
		{
			auto positive = look * Quaternion::CreateFromAxisAngle(Vector3::UnitY, offsetAngle * i);
			auto negative = look * Quaternion::CreateFromAxisAngle(Vector3::UnitY, -offsetAngle * i);
			directions.push_back(positive);
			directions.push_back(negative);
		}

		return directions;
	}
}
