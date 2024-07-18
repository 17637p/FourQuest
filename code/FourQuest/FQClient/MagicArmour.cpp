#define NOMINMAX
#include "MagicArmour.h"

#include "LinearAttack.h"
#include "Attack.h"
#include "Player.h"
#include "DamageCalculation.h"

fq::client::MagicArmour::MagicArmour()
	:mPlayer(nullptr)
	, mAnimator(nullptr)
	, mTransform(nullptr)
	, mController(nullptr)
	, mMagicBall{}
	, mAOE{}
	, mRazer{}
	, mAttackWarningUI{}
	, mMagicBallSpeed(10.f)
	, mAOEMoveRange(10.f)
	, mAOECoolTime(7.f)
	, mAOEElapsedTime(0.f)
	, mRazerCoolTime(5.f)
	, mRazerElapsedTime(0.f)
	, mRStickNoInputTime(0.f)
	, mRazerDistance(30.f)
	, mRazerHiTick(0.25f)
	, mRazerHitElapsedTime(0.f)
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

	attackInfo.attacker = attackObj.get();
	float attackPower = mPlayer->GetAttackPower();
	attackInfo.damage = dc::GetMagicBallDamage(attackPower);
	attackInfo.bIsInfinite = false;
	attackInfo.remainingAttackCount = 1;
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

	// TODO:: MagicBall ���� �߰� 
	GetScene()->AddGameObject(attackObj);
}

void fq::client::MagicArmour::EmitAOE(DirectX::SimpleMath::Vector3 attackPoint)
{
	auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(mAOE);
	auto& attackObj = *(instance.begin());

	// ���� ����
	AttackInfo attackInfo{};

	attackInfo.attacker = attackObj.get();
	auto attackComponent = attackObj->GetComponent<client::Attack>();
	auto attackT = attackObj->GetComponent<game_module::Transform>();
	float attackPower = mPlayer->GetAttackPower();
	attackInfo.damage = dc::GetAOEDamage(attackPower);
	attackComponent->Set(attackInfo);

	// ���� ��ġ ����
	attackT->SetWorldPosition(attackPoint);

	// TODO:: AOE Sound �߰�
	GetScene()->AddGameObject(attackObj);

	// CoolTime
	mAOEElapsedTime = mAOECoolTime;
}

void fq::client::MagicArmour::EmitRazer()
{
	// RayCastTest
	fq::event::RayCast::ResultData data;

	auto tf = GetComponent<game_module::Transform>();
	auto origin = tf->GetWorldPosition();
	origin.y += 1.f;
	auto direction = tf->GetLookAtVector();
	auto distance = mRazerDistance;
	bool bUseDebugDraw = true;
	auto tag = GetGameObject()->GetTag();

	GetScene()->GetEventManager()->FireEvent<fq::event::RayCast>(
		fq::event::RayCast {origin, direction, distance, tag, & data, bUseDebugDraw}
	);

	if (data.hasBlock)
	{
		if (mRazerHitElapsedTime == 0.f)
		{
			// RazerAttckBox ��ȯ
			auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(mRazerAttackBox);
			auto& attackObj = *(instance.begin());

			// ���� ����
			AttackInfo attackInfo{};
			auto attackComponent = attackObj->GetComponent<client::Attack>();
			auto attackT = attackObj->GetComponent<game_module::Transform>();
			
			float attackPower = mPlayer->GetAttackPower();
			attackInfo.damage = dc::GetRazerDamage(attackPower);
			attackInfo.attacker = attackObj.get();
			attackInfo.remainingAttackCount = 1;
			attackInfo.bIsInfinite = false;
			attackComponent->Set(attackInfo);

			// ���� ��ġ ����
			attackT->SetWorldPosition(data.blockPosition);

			// TODO :: Razer HitSound �߰�
			GetScene()->AddGameObject(attackObj);

			mRazerHitElapsedTime = mRazerHiTick;
		}

	}
}

void fq::client::MagicArmour::OnStart()
{
	mController = GetComponent<game_module::CharacterController>();
	mAnimator = GetComponent<game_module::Animator>();
	mTransform = GetComponent<game_module::Transform>();
	mPlayer = GetComponent<Player>();
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

	// Razer
	mRazerElapsedTime = std::max(0.f, mRazerElapsedTime - dt);
	mAnimator->SetParameterFloat("RazerCoolTime", mRazerElapsedTime);

	// Razer Hit Tick
	mRazerHitElapsedTime = std::max(0.f, mRazerHitElapsedTime - dt);
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
