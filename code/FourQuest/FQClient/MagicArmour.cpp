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
{}

fq::client::MagicArmour::~MagicArmour()
{

}

std::shared_ptr<fq::game_module::Component> fq::client::MagicArmour::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	auto cloneArmour = std::dynamic_pointer_cast<MagicArmour>(clone);

	if (cloneArmour == nullptr) // 새로 생성해서 복사본을 준다
	{
		cloneArmour = game_module::ObjectPool::GetInstance()->Assign<MagicArmour>(*this);
	}
	else // clone에 데이터를 복사한다.
	{
		// 기본 대입 연산자 호출한다.
		*cloneArmour = *this;
	}

	cloneArmour->mPlayer = nullptr;

	return cloneArmour;
}

void fq::client::MagicArmour::EmitMagicBall()
{
	auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(mMagicBall);
	auto& attackObj = *(instance.begin());

	// 공격 설정
	auto attackComponent = attackObj->GetComponent<client::Attack>();
	attackComponent->SetAttacker(GetGameObject());
	auto attackT = attackObj->GetComponent<game_module::Transform>();

	// 공격 위치 설정
	DirectX::SimpleMath::Vector3 pos = mTransform->GetWorldPosition();
	pos.y += 1.f;
	attackT->SetLocalPosition(pos);

	// 매직볼 공격력 계산 
	float attackPower = mPlayer->GetAttackPower();
	attackComponent->SetAttackPower(dc::GetMagicBallDamage(attackPower));

	// 공격 방향 설정
	auto linearAttack = attackObj->GetComponent<LinearAttack>();

	auto direction = DirectX::SimpleMath::Matrix::CreateFromQuaternion(mTransform->GetWorldRotation()).Forward();
	direction.Normalize();
	linearAttack->SetMoveSpeed(mMagicBallSpeed);
	linearAttack->SetMoveDirection(direction);

	// TODO:: MagicBall 사운드 추가 
	GetScene()->AddGameObject(attackObj);
}

void fq::client::MagicArmour::EmitAOE(DirectX::SimpleMath::Vector3 attackPoint)
{
	auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(mAOE);
	auto& attackObj = *(instance.begin());

	// 공격 설정
	auto attackComponent = attackObj->GetComponent<client::Attack>();
	attackComponent->SetAttacker(GetGameObject());
	auto attackT = attackObj->GetComponent<game_module::Transform>();

	// 공격 위치 설정
	attackT->SetLocalPosition(attackPoint);

	// AOE 공격력 계산 
	float attackPower = mPlayer->GetAttackPower();
	attackComponent->SetAttackPower(dc::GetAOEDamage(attackPower));

	// TODO:: AOE Sound 추가

	GetScene()->AddGameObject(attackObj);

	// CoolTime
	mAOEElapsedTime = mAOECoolTime;
}

void fq::client::MagicArmour::EmitRazer()
{

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

	// Razer R Stick 조작
	DirectX::SimpleMath::Vector2 r;
	r.x = input->GetStickInfomation(padID, EPadStick::rightX);
	r.y = input->GetStickInfomation(padID, EPadStick::rightY);

	if (r.Length() > 0.f)
	{
		mAnimator->SetParameterBoolean("OnRazer", true);
	
		// R Stick의 갑작스러운 방향전환으로 입력이 0이 되는
		// 순간에 스킬이 캔슬되는 것을 방지하는 값입니다
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
}
