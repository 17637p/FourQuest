#define NOMINMAX
#include "ArcherArmour.h"

#include "../FQGameModule/Transform.h"
#include "../FQGameModule/Animator.h"
#include "../FQGameModule/CharacterController.h"
#include "../FQGameModule/RigidBody.h"

#include "Player.h"
#include "PlayerVariable.h"
#include "AimAssist.h"
#include "ArrowAttack.h"
#include "DamageCalculation.h"
#include "LinearAttack.h"
#include "SpeechBubbleUI.h"
#include "PlayerInfoVariable.h"
#include "EffectColorTransmitter.h"

namespace fq::client
{
	ArcherArmour::ArcherArmour()
		: mAnimator(nullptr)
		, mController(nullptr)
		, mTransform()
		, mPlayer(nullptr)
		, mAimAssist(nullptr)
		, mWeakAttack()
		, mStrongAttack()
		, mDashCoolTime()
		, mDashElapsedTime()
		, mDashCoolTimeReduction(0.f)
		, mStrongAttackCoolTime(0.f)
		, mStrongAttackCoolTimeReduction(0.f)
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
		DirectX::SimpleMath::Vector3 position{};

		for (const auto& child : mTransform->GetChildren())
		{
			auto name = child->GetGameObject()->GetName();

			if (name.find("WeaponeSocket") != std::string::npos)
			{
				auto transform = child->GetComponent<game_module::Transform>();
				position = transform->GetWorldPosition();

				break;
			}
		}

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

		// 약공격 소리
		//GetScene()->GetEventManager()->FireEvent<fq::event::OnPlaySound>({ "A_WeakAttack", false , 0 });

		GetScene()->AddGameObject(attackObj);

		// 공격 체력 감소
		mPlayer->DecreaseHp(PlayerVariable::HpReductionOnAttack, true, true);

		// 이펙트 색상 설정
		auto effectColorTransmitter = attackObj->GetComponent<EffectColorTransmitter>();
		if (effectColorTransmitter != nullptr && mPlayer != nullptr)
		{
			effectColorTransmitter->SetSoulType(mPlayer->GetSoulType());
		}
	}

	void ArcherArmour::EmitStrongAttack(int chargeLevel)
	{
		auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(mStrongAttack);
		auto& attackObj = *(instance.begin());

		auto attackComponent = attackObj->GetComponent<client::ArrowAttack>();
		auto linearComponent = attackObj->GetComponent<LinearAttack>();
		auto rigidBodyComponent = attackObj->GetComponent<fq::game_module::RigidBody>();
		auto attackT = attackObj->GetComponent<game_module::Transform>();
		auto foward = mTransform->GetLookAtVector();
		DirectX::SimpleMath::Vector3 position{};

		for (const auto& child : mTransform->GetChildren())
		{
			auto name = child->GetGameObject()->GetName();

			if (name.find("WeaponeSocket") != std::string::npos)
			{
				auto transform = child->GetComponent<game_module::Transform>();
				position = transform->GetWorldPosition();

				break;
			}
		}

		// 공격 트랜스폼 설정
		DirectX::SimpleMath::Quaternion rotation = mTransform->GetWorldRotation();
		const auto& localOffset = DirectX::SimpleMath::Vector3::TransformNormal(attackT->GetWorldPosition(), mTransform->GetWorldMatrix());
		attackT->GenerateWorld(position + localOffset, rotation, attackT->GetWorldScale());

		// 공격 설정
		ArrowAttackInfo attackInfo{};
		attackInfo.weakDamage = dc::GetArcherWADamage(mPlayer->GetAttackPower());
		attackInfo.weakProjectileVelocity = mWeakProjectileVelocity;
		attackInfo.attacker = GetGameObject();
		attackInfo.attackDirection = foward;
		attackInfo.attackTransform = attackT->GetWorldMatrix();
		attackInfo.bIsStrongAttack = true;
		attackInfo.hitSound = "A_StrongAttack_Hit";
		attackInfo.HitEffectName = "A_Shoot_Hit_blood";

		switch (chargeLevel)
		{
		case 1:
		{
			attackInfo.strongProjectileVelocity = mWeakProjectileVelocity * 1.f;
			attackInfo.strongDamage = dc::GetArcherCA_1_Damage(mPlayer->GetAttackPower());
			attackInfo.remainingAttackCount = 1;
		}
		break;
		case 2:
		{
			attackInfo.strongProjectileVelocity = mWeakProjectileVelocity * 2.f;
			attackInfo.strongDamage = dc::GetArcherCA_2_Damage(mPlayer->GetAttackPower());
			attackInfo.remainingAttackCount = 3;
		}
		break;
		case 3:
		{
			attackInfo.strongProjectileVelocity = mWeakProjectileVelocity * 3.f;
			attackInfo.strongDamage = dc::GetArcherCA_3_Damage(mPlayer->GetAttackPower());
			attackInfo.remainingAttackCount = 5;
		}
		break;
		case 4:
		{
			attackInfo.strongProjectileVelocity = mWeakProjectileVelocity * 5.f;
			attackInfo.strongDamage = dc::GetArcherCA_4_Damage(mPlayer->GetAttackPower());
			attackInfo.remainingAttackCount = 0b11111111;
		}
		break;
		default:
			break;
		}

		attackComponent->Set(attackInfo);

		// MagicBall Attack 사운드  
		GetScene()->AddGameObject(attackObj);

		// 공격 체력 감소
		mPlayer->DecreaseHp(PlayerVariable::HpReductionOnAttack, true, true);

		// 이펙트 색상 설정
		auto effectColorTransmitter = attackObj->GetComponent<EffectColorTransmitter>();
		if (effectColorTransmitter != nullptr && mPlayer != nullptr)
		{
			effectColorTransmitter->SetSoulType(mPlayer->GetSoulType());
		}
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

	void ArcherArmour::SetLookAtRStickInput()
	{
		using namespace DirectX::SimpleMath;

		auto inputMgr = GetScene()->GetInputManager();
		Vector3 input = Vector3::Zero;

		// 컨트롤러 입력
		input.x = inputMgr->GetStickInfomation(mController->GetControllerID(), EPadStick::rightX);
		input.z = inputMgr->GetStickInfomation(mController->GetControllerID(), EPadStick::rightY);

		float lengthSq = input.LengthSquared();

		// 컨트롤러 스틱을 조작하 땔때 반동으로 생기는 미세한 방향설정을 무시하는 값
		constexpr float rotationOffsetSq = 0.5f * 0.5f;

		// 캐릭터 컨트롤러 회전 처리
		if (lengthSq >= rotationOffsetSq)
		{
			// 바라보는 방향 설정 
			input.Normalize();

			if (input == Vector3::Backward)
			{
				mTransform->SetWorldRotation(Quaternion::LookRotation(input, { 0.f,-1.f,0.f }));
			}
			else if (input != Vector3::Zero)
			{
				mTransform->SetWorldRotation(Quaternion::LookRotation(input, { 0.f,1.f,0.f }));
			}
		}
	}

	void ArcherArmour::SetLookAtLStickInput(float dt, float rotationSpeed)
	{
		using namespace DirectX::SimpleMath;

		auto inputMgr = GetScene()->GetInputManager();
		Vector3 input = Vector3::Zero;

		// 컨트롤러 입력
		input.x = inputMgr->GetStickInfomation(mController->GetControllerID(), EPadStick::leftX);
		input.z = inputMgr->GetStickInfomation(mController->GetControllerID(), EPadStick::leftY);

		float lengthSq = input.LengthSquared();

		// 컨트롤러 스틱을 조작하 땔때 반동으로 생기는 미세한 방향설정을 무시하는 값
		constexpr float rotationOffsetSq = 0.5f * 0.5f;

		// 캐릭터 컨트롤러 회전 처리
		if (lengthSq >= rotationOffsetSq)
		{
			// 바라보는 방향 설정 
			input.Normalize();

			// 입력 값으로부터 Y축 회전 각도를 계산
			float targetYaw = atan2f(-input.x, -input.z); // atan2 사용으로 XZ 평면 기준 각도 계산

			// 현재 회전 값을 가져와 Y축 회전만 남긴다
			Quaternion startRotation = mTransform->GetWorldRotation();
			Vector3 eulerStartRotation = startRotation.ToEuler();
			eulerStartRotation.x = eulerStartRotation.z = 0.0f; // Y축 회전만 남기고 다른 축은 0으로 설정

			// 목표 회전 값 (Y축 회전만 적용)
			Quaternion targetRotation = Quaternion::CreateFromYawPitchRoll(targetYaw, 0.0f, 0.0f);

			// 회전 방향을 결정
			Quaternion newRotation = Quaternion::Slerp(startRotation, targetRotation, rotationSpeed * dt);

			// 새 회전 값 설정
			mTransform->SetWorldRotation(newRotation);
		}
	}

	void ArcherArmour::SetLookAtLStickInput()
	{
		using namespace DirectX::SimpleMath;

		auto inputMgr = GetScene()->GetInputManager();
		Vector3 input = Vector3::Zero;

		// 컨트롤러 입력
		input.x = inputMgr->GetStickInfomation(mController->GetControllerID(), EPadStick::leftX);
		input.z = inputMgr->GetStickInfomation(mController->GetControllerID(), EPadStick::leftY);

		float lengthSq = input.LengthSquared();

		// 컨트롤러 스틱을 조작하 땔때 반동으로 생기는 미세한 방향설정을 무시하는 값
		constexpr float rotationOffsetSq = 0.5f * 0.5f;

		// 캐릭터 컨트롤러 회전 처리
		if (lengthSq >= rotationOffsetSq)
		{
			// 바라보는 방향 설정 
			input.Normalize();

			if (input == Vector3::Backward)
			{
				mTransform->SetWorldRotation(Quaternion::LookRotation(input, { 0.f,-1.f,0.f }));
			}
			else if (input != Vector3::Zero)
			{
				mTransform->SetWorldRotation(Quaternion::LookRotation(input, { 0.f,1.f,0.f }));
			}
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

}
