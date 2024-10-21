#define NOMINMAX
#include "KnightArmour.h"

#include "../FQGameModule/Transform.h"
#include "../FQGameModule/Animator.h"
#include "../FQGameModule/CharacterController.h"
#include "Player.h"
#include "PlayerVariable.h"
#include "Attack.h"
#include "DamageCalculation.h"
#include "PlayerSoulVariable.h"
#include "GaugeBar.h"
#include "SpeechBubbleUI.h"
#include "PlayerInfoVariable.h"

fq::client::KnightArmour::KnightArmour()
	:mDashCoolTime(1.f)
	, mDashElapsedTime(0.f)
	, mShieldDashPower(0.5f)
	, mXAttackDashPower(0.1f)
	, mDashKnockBackPower(1.f)
	, mSwordKnockBackPower(1.f)
	, mShieldKnockBackPower(5.f)
	, mController(nullptr)
	, mAnimator(nullptr)
	, mTransform(nullptr)
	, mPlayer(nullptr)
	, mGaugeBar(nullptr)
	, mAttackOffset(2.f)
	, mShieldElapsedTime(0.f)
	, mShieldCoolTime(5.f)
	, mShieldDuration(3.f)
	, mShieldSpeedRatio(0.5f)
	, mShieldCoolTimeReduction(0.f)
	, mDashCoolTimeReduction(0.f)
	, mbOnShield(false)
	, mDashHitVibrationDuration(0.1f)
	, mDashHitVibrationIntensity(1000.f)
	, mDashHitVibrationMode(EVibrationMode::Left)
	, mShieldHitVibrationDuration(0.1f)
	, mShieldHitVibrationIntensity(1000.f)
	, mShieldHitVibrationMode(EVibrationMode::Right)
{}

fq::client::KnightArmour::~KnightArmour()
{}

std::shared_ptr<fq::game_module::Component> fq::client::KnightArmour::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	auto cloneArmour = std::dynamic_pointer_cast<KnightArmour>(clone);

	if (cloneArmour == nullptr) // ���� �����ؼ� ���纻�� �ش�
	{
		cloneArmour = game_module::ObjectPool::GetInstance()->Assign<KnightArmour>(*this);
	}
	else // clone�� �����͸� �����Ѵ�.
	{
		// �⺻ ���� ������ ȣ���Ѵ�.
		*cloneArmour = *this;
	}

	return cloneArmour;
}

void fq::client::KnightArmour::EmitSound(EKnightSound soundType)
{
	std::string soundName;

	switch (soundType)
	{
	case fq::client::EKnightSound::Swing1:
		soundName = "K_Swing1";
		break;
	case fq::client::EKnightSound::Swing2:
		soundName = "K_Swing2";
		break;
	case fq::client::EKnightSound::Swing3:
		soundName = "K_Swing3";
		break;
	case fq::client::EKnightSound::ShieldStart:
		soundName = "K_Shield_Start";
		break;
	case fq::client::EKnightSound::ShieldLoop:
		soundName = "K_Shield_Loop";
		break;
	case fq::client::EKnightSound::Bash:
		soundName = "K_Bash";
		break;
	default:
		assert(false);
		break;
	}

	GetScene()->GetEventManager()->FireEvent<fq::event::OnPlaySound>({ soundName, false , fq::sound::EChannel::SE });
}

void fq::client::KnightArmour::EmitSwordAttack()
{
	// ���� ����
	auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(mSwordAttack);
	auto& attackObj = *(instance.begin());

	auto attackComponent = attackObj->GetComponent<client::Attack>();
	auto attackT = attackObj->GetComponent<game_module::Transform>();
	auto foward = mTransform->GetLookAtVector();

	// ���� ����
	AttackInfo attackInfo{};
	attackInfo.attacker = GetGameObject();
	attackInfo.damage = dc::GetSwordDamage(mPlayer->GetAttackPower());
	attackInfo.type = EKnockBackType::Fixed;
	attackInfo.attackDirection = foward;
	attackInfo.knocBackPower = mSwordKnockBackPower;
	attackInfo.mHitCallback = [this, isIncrease = false]() mutable
		{
			if (!isIncrease)
			{
				this->mPlayer->AddSoulGauge(PlayerSoulVariable::SoulGaugeCharging);
				isIncrease = true;
			}
		};

	auto name = mAnimator->GetController().GetCurrentStateName();

	bool isSwing1 = name == "Swing1";

	attackInfo.hitSound = isSwing1 ? "K_Swing1_Hit" : "K_Swing2_Hit";
	attackInfo.HitEffectName = "K_Swing_Hit_blood";
	attackComponent->Set(attackInfo);

	// ���� Ʈ������ ����
	DirectX::SimpleMath::Vector3 pos = mTransform->GetWorldPosition();
	DirectX::SimpleMath::Quaternion rotation = mTransform->GetWorldRotation();
	pos.y += 1.f;
	pos += foward * mAttackOffset;
	attackT->GenerateWorld(pos, rotation, attackT->GetWorldScale());

	// Sword �Ҹ�
	if (isSwing1)
	{
		EmitSound(EKnightSound::Swing1);
	}
	else
	{
		EmitSound(EKnightSound::Swing2);
	}

	// ���ݽ� ü�� ���� 
	mPlayer->DecreaseHp(PlayerVariable::HpReductionOnAttack, true, true);

	GetScene()->AddGameObject(attackObj);
}

void fq::client::KnightArmour::EmitShieldAttack()
{
	auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(mShieldAttack);
	auto& attackObj = *(instance.begin());

	auto attackComponent = attackObj->GetComponent<client::Attack>();
	auto attackT = attackObj->GetComponent<game_module::Transform>();
	auto foward = mTransform->GetLookAtVector();

	// ���� Ʈ������ ����
	DirectX::SimpleMath::Vector3 pos = mTransform->GetWorldPosition();
	DirectX::SimpleMath::Quaternion rotation = mTransform->GetWorldRotation();
	pos.y += 1.f;
	pos += foward * mAttackOffset;
	attackT->GenerateWorld(pos, rotation, attackT->GetWorldScale());

	// ���� ����
	AttackInfo attackInfo{};
	attackInfo.damage = dc::GetShieldDamage(mPlayer->GetAttackPower());
	attackInfo.attacker = GetGameObject();
	attackInfo.type = EKnockBackType::TargetPosition;
	attackInfo.attackDirection = foward;
	attackInfo.knocBackPower = mDashKnockBackPower;
	attackInfo.attackPosition = mTransform->GetWorldPosition();
	attackInfo.hitSound = "K_Swing3_Hit";
	attackInfo.HitEffectName = "W_Hit_blunt";
	attackInfo.mHitCallback = [this, isIncrease = false, isEmitEffect = false]() mutable
		{
			if (!isIncrease)
			{
				this->mPlayer->AddSoulGauge(PlayerSoulVariable::SoulGaugeCharging);
				isIncrease = true;
			}

			if (!isEmitEffect)
			{
				fq::event::OnCreateStateEvent stateEvent;
				stateEvent.gameObject = GetGameObject();
				stateEvent.RegisterKeyName = "K_Swing_3_Hit";
				if (!stateEvent.RegisterKeyName.empty())
				{
					GetGameObject()->GetScene()->GetEventManager()->FireEvent<fq::event::OnCreateStateEvent>(std::move(stateEvent));
				}

				isEmitEffect = true;
			}
		};
	attackComponent->Set(attackInfo);

	// ���ݽ� ü�� ���� 
	mPlayer->DecreaseHp(PlayerVariable::HpReductionOnAttack, true, true);

	// ShieldAttack �Ҹ�
	EmitSound(EKnightSound::Swing3);

	GetScene()->AddGameObject(attackObj);
}

void fq::client::KnightArmour::EmitShieldDashAttack()
{
	auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(mDashAttack);
	auto& attackObj = *(instance.begin());

	// ���� Ʈ������ ����
	auto attackComponent = attackObj->GetComponent<client::Attack>();
	auto attackT = attackObj->GetComponent<game_module::Transform>();
	auto foward = mTransform->GetLookAtVector();
	mTransform->AddChild(attackT);
	attackT->SetLocalPosition({ 0.f,1.f,0.f });

	// ���� ����
	AttackInfo attackInfo{};
	attackInfo.damage = dc::GetShieldDashDamage(mPlayer->GetAttackPower());
	attackInfo.attacker = GetGameObject();
	attackInfo.type = EKnockBackType::Fixed;
	attackInfo.attackDirection = foward;
	attackInfo.knocBackPower = mDashKnockBackPower;
	attackInfo.mHitCallback = [this, isIncrease = false]() mutable
		{
			if (!isIncrease)
			{
				this->mPlayer->AddSoulGauge(PlayerSoulVariable::SoulGaugeCharging);
				isIncrease = true;
			}

			auto input = GetScene()->GetInputManager();
			input->SetVibration(mController->GetControllerID(),
				mDashHitVibrationMode,
				mDashHitVibrationIntensity,
				mDashHitVibrationDuration);
		};

	attackInfo.hitSound = "K_Bash_Hit";
	attackInfo.HitEffectName = "W_Hit_blunt";
	attackComponent->Set(attackInfo);

	// ���ݽ� ü�� ���� 
	mPlayer->DecreaseHp(PlayerVariable::HpReductionOnAttack, true, true);

	EmitSound(EKnightSound::Bash);
	GetScene()->AddGameObject(attackObj);
}

void fq::client::KnightArmour::OnUpdate(float dt)
{
	checkSkillCoolTime(dt);
	checkInput();
}

void fq::client::KnightArmour::checkInput()
{
	using namespace DirectX::SimpleMath;

	auto input = GetScene()->GetInputManager();

	// Dash Input
	if (input->IsPadKeyState(mController->GetControllerID(), EPadKey::A, EKeyState::Tap)
		&& mDashElapsedTime == 0.f)
	{
		mAnimator->SetParameterTrigger("OnDash");

		if (mPlayer->IsFeverTime())
		{
			mDashElapsedTime = (mDashCoolTime - mDashCoolTimeReduction) * mPlayer->GetGBDecreaseCooltime();
		}
		else
		{
			mDashElapsedTime = mDashCoolTime * mPlayer->GetGBDecreaseCooltime();
		}
	}

	// Shield Input
	DirectX::SimpleMath::Vector3 rightInput{};
	rightInput.x = input->GetStickInfomation(mController->GetControllerID(), EPadStick::rightX);
	rightInput.z = input->GetStickInfomation(mController->GetControllerID(), EPadStick::rightY);

	// ��Ʈ�ѷ� ��ƽ�� ������ ���� �ݵ����� ����� �̼��� ���⼳���� �����ϴ� ��
	constexpr float rotationOffsetSq = 0.5f * 0.5f;

	// ���� ���� ���� 
	if (rightInput.LengthSquared() >= rotationOffsetSq && mShieldElapsedTime == 0.f)
	{
		mAnimator->SetParameterBoolean("OnShield", true);
	}
}


void fq::client::KnightArmour::UpdateBlockState(float dt)
{
	mOnShieldElapsedTime += dt;

	// ���� �з����� ���⼳��
	if (mShieldObject)
	{
		mShieldObject->GetComponent<Attack>()->SetAttackDirection(mTransform->GetLookAtVector());
	}

	auto input = GetScene()->GetInputManager();

	DirectX::SimpleMath::Vector3 rightInput{};
	rightInput.x = input->GetStickInfomation(mController->GetControllerID(), EPadStick::rightX);
	rightInput.z = input->GetStickInfomation(mController->GetControllerID(), EPadStick::rightY);

	// ���� ���� ���� 
	if (rightInput.LengthSquared() <= 0.001f
		|| mOnShieldElapsedTime >= mShieldDuration)
	{
		mAnimator->SetParameterBoolean("OnShield", false);
		mPlayer->SetOnShieldBlock(false);
	}

	float ratio = 1.f - (mOnShieldElapsedTime / mShieldDuration);

	GetComponent<GaugeBar>()->SetRatio(ratio);

	// ���� �̵��ӵ� ����
	mController->AddFinalSpeedMultiplier(mShieldSpeedRatio - 1.f);

	// ���� ü�� ����
	mPlayer->DecreaseHp(PlayerVariable::HpReductionOnAttack * dt, true, true);
}

void fq::client::KnightArmour::OnStart()
{
	mAnimator = GetComponent<game_module::Animator>();
	mController = GetComponent<game_module::CharacterController>();
	mTransform = GetComponent<game_module::Transform>();
	mPlayer = GetComponent<Player>();
	mGaugeBar = GetComponent<GaugeBar>();

	setName();
}

void fq::client::KnightArmour::checkSkillCoolTime(float dt)
{
	mDashElapsedTime = std::max(0.f, mDashElapsedTime - dt);
	mShieldElapsedTime = std::max(0.f, mShieldElapsedTime - dt);

	if (mPlayer->IsFeverTime())
	{
		float dashSkillCoolTime = (mDashCoolTime - mDashCoolTimeReduction) * mPlayer->GetGBDecreaseCooltime();
		float shieldSkiilCoolTime = (mShieldCoolTime - mShieldCoolTimeReduction) * mPlayer->GetGBDecreaseCooltime();
		mPlayer->SetASkillCoolTimeRatio(mDashElapsedTime / dashSkillCoolTime);
		mPlayer->SetRSkillCoolTimeRatio(mShieldElapsedTime / shieldSkiilCoolTime);

	}
	else
	{
		float dashSkillCoolTime = mDashCoolTime * mPlayer->GetGBDecreaseCooltime();
		float shieldSkiilCoolTime = mShieldCoolTime * mPlayer->GetGBDecreaseCooltime();
		mPlayer->SetASkillCoolTimeRatio(mDashElapsedTime / dashSkillCoolTime);
		mPlayer->SetRSkillCoolTimeRatio(mShieldElapsedTime / shieldSkiilCoolTime);
	}
}

std::shared_ptr<fq::game_module::GameObject> fq::client::KnightArmour::EmitSwordEffect()
{
	// ���� ����Ʈ ����
	auto name = mAnimator->GetController().GetCurrentStateName();

	game_module::PrefabResource& res = (name != "Swing1") ? mSwordAttackEffect1
		: mSwordAttackEffect2;

	auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(res);
	auto& effectObj = *(instance.begin());
	auto effectT = effectObj->GetComponent<game_module::Transform>();
	effectT->SetParent(mTransform);

	GetScene()->AddGameObject(effectObj);

	return effectObj;
}


void fq::client::KnightArmour::ExitShieldState()
{
	mGaugeBar->SetVisible(false);

	if (mShieldObject)
	{
		GetScene()->DestroyGameObject(mShieldObject.get());
		mShieldObject = nullptr;
	}

	mbOnShield = false;
	mOnShieldElapsedTime = 0.f;

	// ����Ʈ ����
	GetGameObject()->GetScene()->GetEventManager()->FireEvent<fq::event::OnDeleteStateEvent>({ "K_Shield_Stay", GetGameObject() });
}

void fq::client::KnightArmour::EnterShieldState()
{
	if (mbOnShield)
	{
		return;
	}

	mbOnShield = true;
	mPlayer->SetOnShieldBlock(true);
	mGaugeBar->SetVisible(true);
	mGaugeBar->SetRatio(1.f);

	if (mPlayer->IsFeverTime())
		mShieldElapsedTime = (mShieldCoolTime - mShieldCoolTimeReduction) * mPlayer->GetGBDecreaseCooltime();
	else
		mShieldElapsedTime = mShieldCoolTime * mPlayer->GetGBDecreaseCooltime();

	mOnShieldElapsedTime = 0.f;

	// ���� �ݶ��̴� ����
	auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(mShieldCollider);
	auto& shieldObj = *(instance.begin());

	// ���� Ʈ������ ����
	auto attackComponent = shieldObj->GetComponent<client::Attack>();
	auto shieldT = shieldObj->GetComponent<game_module::Transform>();
	auto foward = mTransform->GetLookAtVector();
	mTransform->AddChild(shieldT);
	shieldT->SetLocalPosition({ 0.f,1.f,-1.f });

	// ���� ����
	AttackInfo attackInfo{};
	attackInfo.damage = 0.f;
	attackInfo.attacker = GetGameObject();
	attackInfo.type = EKnockBackType::Fixed;
	attackInfo.attackDirection = foward;
	attackInfo.knocBackPower = mShieldKnockBackPower;
	attackInfo.mHitCallback = [this, isIncrease = false]() mutable
		{
			if (!isIncrease)
			{
				this->mPlayer->AddSoulGauge(PlayerSoulVariable::SoulGaugeCharging);
				isIncrease = true;
			}

			auto input = GetScene()->GetInputManager();
			input->SetVibration(mController->GetControllerID(),
				mShieldHitVibrationMode,
				mShieldHitVibrationIntensity,
				mShieldHitVibrationDuration);
		};
	attackComponent->Set(attackInfo);

	// ���� ����
	EmitSound(EKnightSound::ShieldStart);

	// ����Ʈ ����
	fq::event::OnCreateStateEvent stateEvent;
	stateEvent.gameObject = GetGameObject();
	stateEvent.RegisterKeyName = "K_Shield_Stay";
	if (!stateEvent.RegisterKeyName.empty())
	{
		GetGameObject()->GetScene()->GetEventManager()->FireEvent<fq::event::OnCreateStateEvent>(std::move(stateEvent));
	}

	GetScene()->AddGameObject(shieldObj);
	mShieldObject = shieldObj;
}

void fq::client::KnightArmour::setName()
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
