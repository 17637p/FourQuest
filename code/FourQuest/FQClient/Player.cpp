#define NOMINMAX
#include "Player.h"

#include "Attack.h"
#include "CameraMoving.h"
#include "HpBar.h"

fq::client::Player::Player()
	:mAttackPower(1.f)
	, mAttackPrafab{}
	, mController(nullptr)
	, mHp(0.f)
	, mMaxHp(0.f)
	, mDashCoolTime(1.f)
	, mDashElapsedTime(0.f)
	, mInvincibleTime(1.f)
	, mInvincibleElapsedTime(0.f)
	, mAnimator(nullptr)
	, mSoulStack(0.f)
{}

fq::client::Player::~Player()
{

}

std::shared_ptr<fq::game_module::Component> fq::client::Player::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	auto cloneController = std::dynamic_pointer_cast<Player>(clone);

	if (cloneController == nullptr) // 새로 생성해서 복사본을 준다
	{
		cloneController = game_module::ObjectPool::GetInstance()->Assign<Player>(*this);
	}
	else // clone에 데이터를 복사한다.
	{
		// 기본 대입 연산자 호출한다.
		*cloneController = *this;
	}

	return cloneController;
}

void fq::client::Player::OnUpdate(float dt)
{ 
	mAnimator->SetParameterBoolean("OnMove", mController->OnMove());

	processInput();
	processCoolTime(dt);
}

void fq::client::Player::OnStart()
{
	mMaxHp = mHp;

	mAnimator = GetComponent<fq::game_module::Animator>();
	assert(mAnimator);

	mController = GetComponent<fq::game_module::CharacterController>();

	// 카메라에 플레이어 등록 
	GetScene()->ViewComponents<CameraMoving>([this](game_module::GameObject& object, CameraMoving& camera)
		{
			camera.AddPlayerTransform(GetComponent<game_module::Transform>());
		});

}

void fq::client::Player::processInput()
{
	auto input = GetScene()->GetInputManager();

	bool isDashAble = mDashElapsedTime == 0.f;
	if (isDashAble && input->IsPadKeyState(mController->GetControllerID(), EPadKey::A, EKeyState::Tap))
	{
		mAnimator->SetParameterTrigger("OnDash");
		mDashElapsedTime = mDashCoolTime;
	}

	if (input->IsPadKeyState(mController->GetControllerID(), EPadKey::B, EKeyState::Tap))
	{
		SummonSoul();
	}
}


void fq::client::Player::Attack()
{
	auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(mAttackPrafab);
	auto& attackObj = *(instance.begin());

	auto attackT = attackObj->GetComponent<game_module::Transform>();
	auto transform = GetComponent<game_module::Transform>();

	// 공격 설정
	auto attackComponent = attackObj->GetComponent<client::Attack>();
	attackComponent->SetAttacker(GetGameObject());
	attackComponent->SetAttackPower(100.f);

	auto forward = transform->GetWorldMatrix().Forward();
	forward.Normalize();

	attackObj->SetTag(game_module::ETag::PlayerAttack);

	attackT->SetLocalRotation(transform->GetWorldRotation());
	attackT->SetLocalPosition(transform->GetWorldPosition() + forward);

	GetScene()->AddGameObject(attackObj);
}

void fq::client::Player::OnDestroy()
{
	// 카메라에 플레이어 해제 
	GetScene()->ViewComponents<CameraMoving>([this](game_module::GameObject& object, CameraMoving& camera)
		{
			camera.DeletePlayerTransform(GetComponent<game_module::Transform>());
		});
}

void fq::client::Player::OnTriggerEnter(const game_module::Collision& collision)
{
	bool isHitAble = mInvincibleElapsedTime == 0.f;

	// 플레이어 피격
	if (isHitAble && collision.other->GetTag() == game_module::ETag::MonsterAttack)
	{
		mAnimator->SetParameterTrigger("OnHit");
		auto monsterAtk = collision.other->GetComponent<client::Attack>();
		float attackPower = monsterAtk->GetAttackPower();
		mHp -= attackPower;

		mInvincibleElapsedTime = mInvincibleTime;

		// UI 설정
		GetComponent<HpBar>()->DecreaseHp(attackPower / mMaxHp);

		// 플레이어 사망처리 
		if (mHp <= 0.f)
		{
			SummonSoul();
		}
	}
}

void fq::client::Player::SummonSoul()
{
	auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(mSoulPrefab);
	auto& soul = *(instance.begin());

	// 컨트롤러 연결
	soul->GetComponent<game_module::CharacterController>()
		->SetControllerID(mController->GetControllerID());

	// 위치 설정
	auto localMat = GetComponent<game_module::Transform>()->GetLocalMatrix();
	localMat._42 += 1.f;
	soul->GetComponent<game_module::Transform>()->SetLocalMatrix(localMat);

	GetScene()->AddGameObject(soul);
	GetScene()->DestroyGameObject(GetGameObject());
}

void fq::client::Player::processCoolTime(float dt)
{
	mDashElapsedTime = std::max(mDashElapsedTime - dt, 0.f);
	mInvincibleElapsedTime = std::max(mInvincibleElapsedTime - dt, 0.f);
}


