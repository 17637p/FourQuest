#define NOMINMAX
#include "KnightArmour.h"

#include "Player.h"
#include "Attack.h"
#include "DamageCalculation.h"

fq::client::KnightArmour::KnightArmour()
	:mDashCoolTime(1.f)
	, mDashElapsedTime(0.f)
	, mShieldDashPower(0.5f)
	, mXAttackDashPower(0.1f)
	, mShieldKnockPower(1.f)
	, mSwordKnockBackPower(1.f)
	, mController(nullptr)
	, mAnimator(nullptr)
	, mTransform(nullptr)
	, mPlayer(nullptr)
	, mAttackOffset(2.f)
{}

fq::client::KnightArmour::~KnightArmour()
{

}

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
	attackComponent->Set(attackInfo);

	// ���� Ʈ������ ����
	DirectX::SimpleMath::Vector3 pos = mTransform->GetWorldPosition();
	DirectX::SimpleMath::Quaternion rotation = mTransform->GetWorldRotation();
	pos.y += 1.f;
	pos += foward * mAttackOffset;
	attackT->GenerateWorld(pos, rotation, attackT->GetWorldScale());

	// Sword �Ҹ�
	GetScene()->GetEventManager()->FireEvent<fq::event::OnPlaySound>({ "SwordAttack", false , 0 });

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
	attackInfo.knocBackPower = mShieldKnockPower;
	attackInfo.attackPosition = mTransform->GetWorldPosition();
	attackComponent->Set(attackInfo);

	// ShieldAttack �Ҹ�
	GetScene()->GetEventManager()->FireEvent<fq::event::OnPlaySound>({ "ShieldAttack", false , 0 });

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
	attackInfo.knocBackPower = 20.f;
	attackComponent->Set(attackInfo);

	// TODO:: Dash �Ҹ� �߰�
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

	// Dash
	if (input->IsPadKeyState(mController->GetControllerID(), EPadKey::A, EKeyState::Tap)
		&& mDashElapsedTime == 0.f)
	{
		mAnimator->SetParameterTrigger("OnDash");
		mDashElapsedTime = mDashCoolTime;
	}

	// Shield 
	DirectX::SimpleMath::Vector3 rightInput{};
	rightInput.x = input->GetStickInfomation(mController->GetControllerID(), EPadStick::rightX);
	rightInput.z = input->GetStickInfomation(mController->GetControllerID(), EPadStick::rightY);

	// ��Ʈ�ѷ� ��ƽ�� ������ ���� �ݵ����� ����� �̼��� ���⼳���� �����ϴ� ��
	constexpr float rotationOffsetSq = 0.5f * 0.5f;

	if (rightInput.LengthSquared() >= rotationOffsetSq)
	{
		rightInput.Normalize();

		if (rightInput == Vector3::Backward)
		{
			mTransform->SetWorldRotation(Quaternion::LookRotation(rightInput, { 0.f,-1.f,0.f }));
		}
		else
		{
			mTransform->SetWorldRotation(Quaternion::LookRotation(rightInput, { 0.f,1.f,0.f }));
		}

		mAnimator->SetParameterBoolean("OnShield", true);
		mPlayer->SetOnShieldBlock(true);

		// ���� ������ �Ǵ� 

	}
	else
	{
		mAnimator->SetParameterBoolean("OnShield", false);
		mPlayer->SetOnShieldBlock(false);
	}
}

void fq::client::KnightArmour::OnStart()
{
	mAnimator = GetComponent<game_module::Animator>();
	mController = GetComponent<game_module::CharacterController>();
	mTransform = GetComponent<game_module::Transform>();
	mPlayer = GetComponent<Player>();
}

void fq::client::KnightArmour::checkSkillCoolTime(float dt)
{
	mDashElapsedTime = std::max(0.f, mDashElapsedTime - dt);
}

std::shared_ptr<fq::game_module::GameObject> fq::client::KnightArmour::EmitSwordEffect()
{
	// ���� ����Ʈ ����
	auto name = mAnimator->GetController().GetCurrentStateName();

	game_module::PrefabResource& res = (name == "Swing1") ? mSwordAttackEffect1
		: mSwordAttackEffect2; 

	auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(res);
	auto& effectObj = *(instance.begin());
	auto effectT = effectObj->GetComponent<game_module::Transform>();

	effectT->SetParent(mTransform);

	GetScene()->AddGameObject(effectObj);

	return effectObj;  
}