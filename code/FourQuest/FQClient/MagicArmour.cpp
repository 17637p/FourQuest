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
	AttackInfo attackInfo;
	auto attackComponent = attackObj->GetComponent<client::Attack>();
	auto attackT = attackObj->GetComponent<game_module::Transform>();

	attackInfo.attacker = attackObj.get();
	float attackPower = mPlayer->GetAttackPower();
	attackInfo.damage = dc::GetMagicBallDamage(attackPower);
	attackInfo.bIsInfinite = false;
	attackInfo.remainingAttackCount = 1;
	attackComponent->Set(attackInfo);

	// 공격 위치 설정
	DirectX::SimpleMath::Vector3 pos = mTransform->GetWorldPosition();
	pos.y += 1.f;
	attackT->SetLocalPosition(pos);

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
	AttackInfo attackInfo{};

	attackInfo.attacker = attackObj.get();
	auto attackComponent = attackObj->GetComponent<client::Attack>();
	auto attackT = attackObj->GetComponent<game_module::Transform>();
	float attackPower = mPlayer->GetAttackPower();
	attackInfo.damage = dc::GetAOEDamage(attackPower);
	attackComponent->Set(attackInfo);

	// 공격 위치 설정
	attackT->SetWorldPosition(attackPoint);

	// TODO:: AOE Sound 추가
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
			// RazerAttckBox 소환
			auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(mRazerAttackBox);
			auto& attackObj = *(instance.begin());

			// 공격 설정
			AttackInfo attackInfo{};
			auto attackComponent = attackObj->GetComponent<client::Attack>();
			auto attackT = attackObj->GetComponent<game_module::Transform>();
			
			float attackPower = mPlayer->GetAttackPower();
			attackInfo.damage = dc::GetRazerDamage(attackPower);
			attackInfo.attacker = attackObj.get();
			attackInfo.remainingAttackCount = 1;
			attackInfo.bIsInfinite = false;
			attackComponent->Set(attackInfo);

			// 공격 위치 설정
			attackT->SetWorldPosition(data.blockPosition);

			// TODO :: Razer HitSound 추가
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

	// Razer Hit Tick
	mRazerHitElapsedTime = std::max(0.f, mRazerHitElapsedTime - dt);
}

void fq::client::MagicArmour::SetLookAtRStickInput()
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
