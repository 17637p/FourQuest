#define NOMINMAX
#include "KnightArmour.h"

#include "../FQGameModule/Transform.h"
#include "../FQGameModule/Animator.h"
#include "../FQGameModule/CharacterController.h"
#include "Player.h"
#include "Attack.h"
#include "DamageCalculation.h"
#include "PlayerSoulVariable.h"
#include "GaugeBar.h"

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
{}

fq::client::KnightArmour::~KnightArmour()
{

}

std::shared_ptr<fq::game_module::Component> fq::client::KnightArmour::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	auto cloneArmour = std::dynamic_pointer_cast<KnightArmour>(clone);

	if (cloneArmour == nullptr) // 새로 생성해서 복사본을 준다
	{
		cloneArmour = game_module::ObjectPool::GetInstance()->Assign<KnightArmour>(*this);
	}
	else // clone에 데이터를 복사한다.
	{
		// 기본 대입 연산자 호출한다.
		*cloneArmour = *this;
	}

	return cloneArmour;
}

void fq::client::KnightArmour::EmitSwordAttack()
{
	// 공격 생성
	auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(mSwordAttack);
	auto& attackObj = *(instance.begin());

	auto attackComponent = attackObj->GetComponent<client::Attack>();
	auto attackT = attackObj->GetComponent<game_module::Transform>();
	auto foward = mTransform->GetLookAtVector();

	// 공격 설정
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

	// 공격 트랜스폼 설정
	DirectX::SimpleMath::Vector3 pos = mTransform->GetWorldPosition();
	DirectX::SimpleMath::Quaternion rotation = mTransform->GetWorldRotation();
	pos.y += 1.f;
	pos += foward * mAttackOffset;
	attackT->GenerateWorld(pos, rotation, attackT->GetWorldScale());

	// Sword 소리
	GetScene()->GetEventManager()->FireEvent<fq::event::OnPlaySound>({ isSwing1 ? "K_Swing1" : "K_Swing2", false , fq::sound::EChannel::SE });

	GetScene()->AddGameObject(attackObj);
}

void fq::client::KnightArmour::EmitShieldAttack()
{
	auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(mShieldAttack);
	auto& attackObj = *(instance.begin());

	auto attackComponent = attackObj->GetComponent<client::Attack>();
	auto attackT = attackObj->GetComponent<game_module::Transform>();
	auto foward = mTransform->GetLookAtVector();

	// 공격 트랜스폼 설정
	DirectX::SimpleMath::Vector3 pos = mTransform->GetWorldPosition();
	DirectX::SimpleMath::Quaternion rotation = mTransform->GetWorldRotation();
	pos.y += 1.f;
	pos += foward * mAttackOffset;
	attackT->GenerateWorld(pos, rotation, attackT->GetWorldScale());

	// 공격 설정
	AttackInfo attackInfo{};
	attackInfo.damage = dc::GetShieldDamage(mPlayer->GetAttackPower());
	attackInfo.attacker = GetGameObject();
	attackInfo.type = EKnockBackType::TargetPosition;
	attackInfo.attackDirection = foward;
	attackInfo.knocBackPower = mDashKnockBackPower;
	attackInfo.attackPosition = mTransform->GetWorldPosition();
	attackInfo.hitSound = "K_Swing3_Hit";
	attackInfo.HitEffectName = "W_Hit_blunt";
	attackInfo.mHitCallback = [this, isIncrease = false]() mutable
		{
			if (!isIncrease)
			{
				this->mPlayer->AddSoulGauge(PlayerSoulVariable::SoulGaugeCharging);
				isIncrease = true;
			}
		};
	attackComponent->Set(attackInfo);

	// ShieldAttack 소리
	GetScene()->GetEventManager()->FireEvent<fq::event::OnPlaySound>({ "K_Swing3", false , fq::sound::EChannel::SE });

	GetScene()->AddGameObject(attackObj);
}

void fq::client::KnightArmour::EmitShieldDashAttack()
{
	auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(mDashAttack);
	auto& attackObj = *(instance.begin());

	// 공격 트랜스폼 설정
	auto attackComponent = attackObj->GetComponent<client::Attack>();
	auto attackT = attackObj->GetComponent<game_module::Transform>();
	auto foward = mTransform->GetLookAtVector();
	mTransform->AddChild(attackT);
	attackT->SetLocalPosition({ 0.f,1.f,0.f });

	// 공격 설정
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
		};
	attackInfo.HitEffectName = "W_Hit_blunt";
	attackComponent->Set(attackInfo);

	GetScene()->GetEventManager()->FireEvent<fq::event::OnPlaySound>({ "K_Bash", false , fq::sound::EChannel::SE });
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
		mDashElapsedTime = mDashCoolTime;
	}

	// Shield Input
	DirectX::SimpleMath::Vector3 rightInput{};
	rightInput.x = input->GetStickInfomation(mController->GetControllerID(), EPadStick::rightX);
	rightInput.z = input->GetStickInfomation(mController->GetControllerID(), EPadStick::rightY);

	// 컨트롤러 스틱을 조작하 땔때 반동으로 생기는 미세한 방향설정을 무시하는 값
	constexpr float rotationOffsetSq = 0.5f * 0.5f;

	// 쉴드 상태 진입 
	if (rightInput.LengthSquared() >= rotationOffsetSq && mShieldElapsedTime == 0.f)
	{
		EnterShieldState();
	}
}


void fq::client::KnightArmour::UpdateBlockState(float dt)
{
	// 방패 밀려나는 방향설정
	if (mShieldObject)
	{
		mShieldObject->GetComponent<Attack>()->SetAttackDirection(mTransform->GetLookAtVector());
	}

	auto input = GetScene()->GetInputManager();

	DirectX::SimpleMath::Vector3 rightInput{};
	rightInput.x = input->GetStickInfomation(mController->GetControllerID(), EPadStick::rightX);
	rightInput.z = input->GetStickInfomation(mController->GetControllerID(), EPadStick::rightY);

	// 방패 해제 조건 
	if (rightInput.LengthSquared() <= 0.001f
		|| mShieldCoolTime - mShieldElapsedTime >= mShieldDuration)
	{
		mAnimator->SetParameterBoolean("OnShield", false);
		mPlayer->SetOnShieldBlock(false);
	}

	float ratio = 1.f - ((mShieldCoolTime - mShieldElapsedTime) / mShieldDuration);
	GetComponent<GaugeBar>()->SetRatio(ratio);

	// 방패 이동속도 적용
	mController->AddFinalSpeedMultiplier(mShieldSpeedRatio - 1.f);
}


void fq::client::KnightArmour::OnStart()
{
	mAnimator = GetComponent<game_module::Animator>();
	mController = GetComponent<game_module::CharacterController>();
	mTransform = GetComponent<game_module::Transform>();
	mPlayer = GetComponent<Player>();
	mGaugeBar = GetComponent<GaugeBar>();
}

void fq::client::KnightArmour::checkSkillCoolTime(float dt)
{
	mDashElapsedTime = std::max(0.f, mDashElapsedTime - dt);
	mShieldElapsedTime = std::max(0.f, mShieldElapsedTime - dt);
}

std::shared_ptr<fq::game_module::GameObject> fq::client::KnightArmour::EmitSwordEffect()
{
	// 공격 이펙트 생성
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
}

void fq::client::KnightArmour::EnterShieldState()
{
	mAnimator->SetParameterBoolean("OnShield", true);
	mPlayer->SetOnShieldBlock(true);
	mShieldElapsedTime = mShieldCoolTime;
	mGaugeBar->SetVisible(true);
	mGaugeBar->SetRatio(1.f);

	// 쉴드 콜라이더 생성
	auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(mShieldCollider);
	auto& shieldObj = *(instance.begin());

	// 공격 트랜스폼 설정
	auto attackComponent = shieldObj->GetComponent<client::Attack>();
	auto shieldT = shieldObj->GetComponent<game_module::Transform>();
	auto foward = mTransform->GetLookAtVector();
	mTransform->AddChild(shieldT);
	shieldT->SetLocalPosition({ 0.f,1.f,-1.f });

	// 공격 설정
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
		};
	attackComponent->Set(attackInfo);

	GetScene()->AddGameObject(shieldObj);
	mShieldObject = shieldObj;
}
