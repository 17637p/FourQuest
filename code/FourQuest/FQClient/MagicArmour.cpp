#define NOMINMAX
#include "MagicArmour.h"

#include "../FQGameModule/Animator.h"
#include "../FQGameModule/CharacterController.h"
#include "../FQGameModule/Transform.h"

#include "LinearAttack.h"
#include "Attack.h"
#include "Player.h"
#include "PlayerVariable.h"
#include "DamageCalculation.h"
#include "AimAssist.h"
#include "SpeechBubbleUI.h"
#include "PlayerInfoVariable.h"
#include "EffectColorTransmitter.h"
#include "ClientEvent.h"

fq::client::MagicArmour::MagicArmour()
	:mPlayer(nullptr)
	, mAnimator(nullptr)
	, mTransform(nullptr)
	, mController(nullptr)
	, mMagicBall{}
	, mAOE{}
	, mLaserHeadEffectPrefab{}
	, mAttackWarningUI{}
	, mMagicBallSpeed(10.f)
	, mAOEMoveRange(10.f)
	, mAOECoolTime(7.f)
	, mAOECoolTimeReduction(0.f)
	, mAOEElapsedTime(0.f)
	, mLaserCoolTime(5.f)
	, mLaserCoolTimeReduction(0.f)
	, mLaserElapsedTime(0.f)
	, mRStickNoInputTime(0.f)
	, mLaserDistance(30.f)
	, mLaserHiTick(0.25f)
	, mLaserHitElapsedTime(0.f)
	, mMagicBallPenetrationCount(1)
	, mLaserHitVibrationDuration(0.1f)
	, mLaserHitVibrationIntensity(1000.f)
	, mLaserHitVibrationMode(EVibrationMode::Left)
	, mAimAisst(nullptr)
{}

fq::client::MagicArmour::~MagicArmour()
{

}

std::shared_ptr<fq::game_module::Component> fq::client::MagicArmour::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	auto cloneArmour = std::dynamic_pointer_cast<MagicArmour>(clone);

	if (cloneArmour == nullptr) // ���� �����ؼ� ���纻�� �ش�
	{
		cloneArmour = game_module::ObjectPool::GetInstance()->Assign<MagicArmour>(*this);
	}
	else // clone�� �����͸� �����Ѵ�.
	{
		// �⺻ ���� ������ ȣ���Ѵ�.
		*cloneArmour = *this;
	}

	cloneArmour->mPlayer = nullptr;

	return cloneArmour;
}

void fq::client::MagicArmour::EmitMagicBall()
{
	auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(mMagicBall);
	auto& attackObj = *(instance.begin());

	// ���� ����
	AttackInfo attackInfo;
	auto attackComponent = attackObj->GetComponent<client::Attack>();
	auto attackT = attackObj->GetComponent<game_module::Transform>();

	attackInfo.attacker = GetGameObject();
	float attackPower = mPlayer->GetAttackPower();
	attackInfo.damage = dc::GetMagicBallDamage(attackPower);
	attackInfo.bIsInfinite = false;
	attackInfo.remainingAttackCount = mMagicBallPenetrationCount;
	attackInfo.hitSound = "M_MagicBoll_Attack";
	attackInfo.mHitCallback = [this, isIncrease = false]() mutable
		{
			if (!isIncrease)
			{
				this->mPlayer->AddSoulGauge(PlayerSoulVariable::SoulGaugeCharging);
				isIncrease = true;
			}
		};
	attackComponent->Set(attackInfo);

	// ���� ��ġ ����
	DirectX::SimpleMath::Vector3 pos = mTransform->GetWorldPosition();
	pos.y += 1.f;
	attackT->SetLocalPosition(pos);

	// ���� ���� ����
	auto linearAttack = attackObj->GetComponent<LinearAttack>();

	auto direction = DirectX::SimpleMath::Matrix::CreateFromQuaternion(mTransform->GetWorldRotation()).Forward();
	direction.Normalize();
	linearAttack->SetMoveSpeed(mMagicBallSpeed);
	linearAttack->SetMoveDirection(direction);

	// MagicBall Attack ����  
	GetScene()->GetEventManager()->FireEvent<fq::event::OnPlaySound>({ "M_MagicBoll_Start", false , fq::sound::EChannel::SE });
	GetScene()->AddGameObject(attackObj);

	// ���ݽ� ü�� ���� 
	mPlayer->DecreaseHp(PlayerVariable::HpReductionOnAttack, true, true);

	// ����Ʈ ���� ����
	auto effectColorTransmitter = attackObj->GetComponent<EffectColorTransmitter>();
	if (effectColorTransmitter != nullptr && mPlayer != nullptr)
	{
		effectColorTransmitter->SetSoulType(mPlayer->GetSoulType());
	}

	int id = mController->GetControllerID();
	GetScene()->GetEventManager()
		->FireEvent<fq::client::event::PushButtonEvent>({ id, ESkillType::X });
}

void fq::client::MagicArmour::EmitAOE(DirectX::SimpleMath::Vector3 attackPoint)
{
	auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(mAOE);
	auto& attackObj = *(instance.begin());

	// ���� ����
	AttackInfo attackInfo{};

	attackInfo.attacker = GetGameObject();
	auto attackComponent = attackObj->GetComponent<client::Attack>();
	auto attackT = attackObj->GetComponent<game_module::Transform>();
	float attackPower = mPlayer->GetAttackPower();
	attackInfo.damage = dc::GetAOEDamage(attackPower);
	attackInfo.mHitCallback = [this, isIncrease = false]() mutable
		{
			if (!isIncrease)
			{
				this->mPlayer->AddSoulGauge(PlayerSoulVariable::SoulGaugeCharging);
				isIncrease = true;
			}
		};
	attackComponent->Set(attackInfo);

	// ���� ��ġ ����
	attackT->SetWorldPosition(attackPoint);

	// AOE Sound
	GetScene()->GetEventManager()->FireEvent<fq::event::OnPlaySound>({ "M_Explosion_Attack", false , fq::sound::EChannel::SE });
	GetScene()->AddGameObject(attackObj);

	// CoolTime
	if (mPlayer->IsFeverTime())
	{
		mAOEElapsedTime = (mAOECoolTime - mAOECoolTimeReduction) * mPlayer->GetGBDecreaseCooltime();
	}
	else
	{
		mAOEElapsedTime = mAOECoolTime * mPlayer->GetGBDecreaseCooltime();
	}

	// ���ݽ� ü�� ���� 
	mPlayer->DecreaseHp(PlayerVariable::HpReductionOnAttack, true, true);

	// ����Ʈ ���� ����
	auto effectColorTransmitter = attackObj->GetComponent<EffectColorTransmitter>();
	if (effectColorTransmitter != nullptr && mPlayer != nullptr)
	{
		effectColorTransmitter->SetSoulType(mPlayer->GetSoulType());
	}

	int id = mController->GetControllerID();
	GetScene()->GetEventManager()
		->FireEvent<fq::client::event::PushButtonEvent>({ id, ESkillType::A });
}

void fq::client::MagicArmour::EmitLaser()
{
	// RayCastTest
	fq::event::RayCast::ResultData data;

	auto origin = mTransform->GetWorldPosition();
	origin.y += 1.f;
	auto direction = mTransform->GetLookAtVector();
	auto distance = mLaserDistance;
	bool bUseDebugDraw = false;
	auto tag = GetGameObject()->GetTag();

	GetScene()->GetEventManager()->FireEvent<fq::event::RayCast>(
		fq::event::RayCast {origin, direction, distance, tag, & data, bUseDebugDraw}
	);

	// ���̿��� ���� ����� ������Ʈ ��ġ ã��
	DirectX::SimpleMath::Vector3 closestPoint{};
	float minDistance = distance;

	if (data.hitCount > 0)
	{
		for (int i = 0; i < data.hitCount; ++i)
		{
			if (data.hitLayerNumber[i] == static_cast<unsigned int>(fq::game_module::ETag::DeadMonster))
				continue;

			float hitDistance = (origin - data.hitContactPoints[i]).Length();

			if (minDistance >= hitDistance)
			{
				minDistance = hitDistance;
				closestPoint = data.hitContactPoints[i];
			}
		}
	}

	if (data.hitCount > 0 && mLaserHitElapsedTime == 0.f)
	{
		// RazerAttckBox ��ȯ
		auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(mLaserAttackBox);
		auto& attackObj = *(instance.begin());

		// ���� ����
		AttackInfo attackInfo{};
		auto attackComponent = attackObj->GetComponent<client::Attack>();
		auto attackT = attackObj->GetComponent<game_module::Transform>();

		float attackPower = mPlayer->GetAttackPower();
		attackInfo.damage = dc::GetLaserDamage(attackPower);
		attackInfo.attacker = GetGameObject();
		attackInfo.remainingAttackCount = 1;
		attackInfo.bIsInfinite = false;

		// ���� �ݹ� ���� 
		attackInfo.mHitCallback = [this, isIncrease = false]() mutable
			{
				if (!isIncrease)
				{
					this->mPlayer->AddSoulGauge(PlayerSoulVariable::SoulGaugeCharging);
					isIncrease = true;
				}

				// ���� ���� 
				auto input = this->GetScene()->GetInputManager();
				input->SetVibration(mController->GetControllerID(),
					mLaserHitVibrationMode,
					mLaserHitVibrationIntensity,
					mLaserHitVibrationDuration);
			};
		attackInfo.HitEffectName = "M_Laser_Hit";
		attackComponent->Set(attackInfo);

		// ���� ��ġ ����
		attackT->SetWorldPosition(closestPoint);

		GetScene()->AddGameObject(attackObj);

		// Razer HitSound 
		GetScene()->GetEventManager()->FireEvent<fq::event::OnPlaySound>({ "M_Lazer_Attack", false , fq::sound::EChannel::SE });

		mLaserHitElapsedTime = mLaserHiTick;

		// ����Ʈ ���� ����
		auto effectColorTransmitter = attackObj->GetComponent<EffectColorTransmitter>();
		if (effectColorTransmitter != nullptr && mPlayer != nullptr)
		{
			effectColorTransmitter->SetSoulType(mPlayer->GetSoulType());
		}
	}

	// ������ ���� ����Ʈ ����
	if (mLaserLineEffect)
	{
		auto laserT = mLaserLineEffect->GetTransform();
		auto position = mLaserHeadEffect->GetTransform()->GetWorldPosition();
		auto rotation = mTransform->GetWorldRotation();
		auto scale = laserT->GetWorldScale();

		// �������� ���̴� scale.z �� �����մϴ� 
		if (data.hitCount > 0)
		{
			scale.z = (position - closestPoint).Length();
		}
		else
			scale.z = mLaserDistance;

		laserT->GenerateWorld(position, rotation, scale);
	}

}

void fq::client::MagicArmour::OnStart()
{
	mController = GetComponent<game_module::CharacterController>();
	mAnimator = GetComponent<game_module::Animator>();
	mTransform = GetComponent<game_module::Transform>();
	mPlayer = GetComponent<Player>();

	//
	for (auto child : GetGameObject()->GetChildren())
	{
		if (child->GetName().find("AimAssist") != std::string::npos)
		{
			mAimAisst = child->GetComponent<AimAssist>();
		}
	}

	setName();
}

void fq::client::MagicArmour::OnUpdate(float dt)
{
	checkInput(dt);
	checkCoolTime(dt);
}

void fq::client::MagicArmour::checkInput(float dt)
{
	auto input = GetScene()->GetInputManager();
	auto padID = mController->GetControllerID();

	// AOE
	if (input->IsPadKeyState(padID, EPadKey::A, EKeyState::Tap))
	{
		mAnimator->SetParameterBoolean("PushA", true);
	}
	else if (input->IsPadKeyState(padID, EPadKey::A, EKeyState::Away))
	{
		mAnimator->SetParameterBoolean("PushA", false);
	}

	// Razer R Stick ����
	DirectX::SimpleMath::Vector2 r;
	r.x = input->GetStickInfomation(padID, EPadStick::rightX);
	r.y = input->GetStickInfomation(padID, EPadStick::rightY);

	if (r.Length() > 0.f)
	{
		mAnimator->SetParameterBoolean("OnRazer", true);

		// R Stick�� ���۽����� ������ȯ���� �Է��� 0�� �Ǵ�
		// ������ ��ų�� ĵ���Ǵ� ���� �����ϴ� ���Դϴ�
		constexpr float RStickInputCorrectTime = 0.1f;
		mRStickNoInputTime = RStickInputCorrectTime;
	}
	else
	{
		mRStickNoInputTime = std::max(0.f, mRStickNoInputTime - dt);

		if (mRStickNoInputTime == 0.f)
			mAnimator->SetParameterBoolean("OnRazer", false);
	}
}

void fq::client::MagicArmour::checkCoolTime(float dt)
{
	// AOE
	mAOEElapsedTime = std::max(0.f, mAOEElapsedTime - dt);
	mAnimator->SetParameterFloat("AOECoolTime", mAOEElapsedTime);

	// Laser
	mLaserElapsedTime = std::max(0.f, mLaserElapsedTime - dt);
	mAnimator->SetParameterFloat("RazerCoolTime", mLaserElapsedTime);

	// Laser Hit Tick
	mLaserHitElapsedTime = std::max(0.f, mLaserHitElapsedTime - dt);

	if (mPlayer->IsFeverTime())
	{
		float AOECoolTime = (mAOECoolTime - mAOECoolTimeReduction) * mPlayer->GetGBDecreaseCooltime();
		float LaserCoolTime = (mLaserCoolTime - mLaserCoolTimeReduction) * mPlayer->GetGBDecreaseCooltime();
		mPlayer->SetASkillCoolTimeRatio(mAOEElapsedTime / AOECoolTime);
		mPlayer->SetRSkillCoolTimeRatio(mLaserElapsedTime / LaserCoolTime);
	}
	else
	{
		float AOECoolTime = mAOECoolTime * mPlayer->GetGBDecreaseCooltime();
		float LaserCoolTime = mLaserCoolTime * mPlayer->GetGBDecreaseCooltime();
		mPlayer->SetASkillCoolTimeRatio(mAOEElapsedTime / AOECoolTime);
		mPlayer->SetRSkillCoolTimeRatio(mLaserElapsedTime / LaserCoolTime);
	}
}

void fq::client::MagicArmour::EnterLaserState()
{
	// ��Ÿ�� ���� 
	if (mPlayer->IsFeverTime())
		mLaserElapsedTime = (mLaserCoolTime - mLaserCoolTimeReduction) * mPlayer->GetGBDecreaseCooltime();
	else
		mLaserElapsedTime = mLaserCoolTime * mPlayer->GetGBDecreaseCooltime();

	// Ű�Է� �̺�Ʈ ȣ�� 
	int id = mController->GetControllerID();
	GetScene()->GetEventManager()
		->FireEvent<fq::client::event::PushButtonEvent>({ id, ESkillType::R });
}


void fq::client::MagicArmour::SetLookAtRStickInput()
{
	using namespace DirectX::SimpleMath;

	auto inputMgr = GetScene()->GetInputManager();
	Vector3 input = Vector3::Zero;

	// ��Ʈ�ѷ� �Է�
	input.x = inputMgr->GetStickInfomation(mController->GetControllerID(), EPadStick::rightX);
	input.z = inputMgr->GetStickInfomation(mController->GetControllerID(), EPadStick::rightY);

	float lengthSq = input.LengthSquared();

	// ��Ʈ�ѷ� ��ƽ�� ������ ���� �ݵ����� ����� �̼��� ���⼳���� �����ϴ� ��
	constexpr float rotationOffsetSq = 0.5f * 0.5f;

	// ĳ���� ��Ʈ�ѷ� ȸ�� ó��
	if (lengthSq >= rotationOffsetSq)
	{
		// �ٶ󺸴� ���� ���� 
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

std::shared_ptr<fq::game_module::GameObject> fq::client::MagicArmour::EmitLaserGatherEffect()
{
	auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(mLaserGatherEffect);
	auto& attackObj = *(instance.begin());

	auto attackT = attackObj->GetComponent<game_module::Transform>();

	// ������ Ʈ������ ��������
	attackT->SetParent(mTransform->GetChildren()[2]);

	GetScene()->AddGameObject(attackObj);

	// ������ ���� ���� ���
	GetScene()->GetEventManager()->FireEvent<fq::event::OnPlaySound>({ "M_Lazer_Start", false , fq::sound::EChannel::SE });

	// ����Ʈ ���� ����
	auto effectColorTransmitter = attackObj->GetComponent<EffectColorTransmitter>();
	if (effectColorTransmitter != nullptr && mPlayer != nullptr)
	{
		effectColorTransmitter->SetSoulType(mPlayer->GetSoulType());
	}

	return attackObj;
}

std::shared_ptr<fq::game_module::GameObject> fq::client::MagicArmour::EmitLaserHeadEffect()
{
	auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(mLaserHeadEffectPrefab);
	auto& effectObj = *(instance.begin());
	auto attackT = effectObj->GetComponent<game_module::Transform>();

	// ������ Ʈ������ ��������
	attackT->SetParent(mTransform->GetChildren()[2]);
	GetScene()->AddGameObject(effectObj);

	mLaserHeadEffect = effectObj;

	// ����Ʈ ���� ����
	auto effectColorTransmitter = effectObj->GetComponent<EffectColorTransmitter>();
	if (effectColorTransmitter != nullptr && mPlayer != nullptr)
	{
		effectColorTransmitter->SetSoulType(mPlayer->GetSoulType());
	}

	return effectObj;
}

void fq::client::MagicArmour::DestroyLaserEffect()
{
	if (mLaserLineEffect)
	{
		GetScene()->DestroyGameObject(mLaserLineEffect.get());
		mLaserLineEffect = nullptr;
	}
	if (mLaserHeadEffect)
	{
		GetScene()->DestroyGameObject(mLaserHeadEffect.get());
		mLaserHeadEffect = nullptr;
	}
}

void fq::client::MagicArmour::EmitLaserLineEffect()
{
	auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(mLaserLineEffectPrefab);
	auto& effect = *(instance.begin());
	mLaserLineEffect = effect;
	GetScene()->AddGameObject(effect);

	// ����Ʈ ���� ����
	auto effectColorTransmitter = effect->GetComponent<EffectColorTransmitter>();
	if (effectColorTransmitter != nullptr && mPlayer != nullptr)
	{
		effectColorTransmitter->SetSoulType(mPlayer->GetSoulType());
	}
}

void fq::client::MagicArmour::OnDestroy()
{
	DestroyLaserEffect();
}

void fq::client::MagicArmour::AimToNearMonster()
{
	if (mAimAisst)
		mAimAisst->SetNearMonsterDirection();
}

void fq::client::MagicArmour::setName()
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

