#define NOMINMAX
#include "Player.h"

#include "Attack.h"
#include "CameraMoving.h"
#include "HpBar.h"
#include "Soul.h"
#include "ClientEvent.h"

fq::client::Player::Player()
	:mAttackPower(1.f)
	, mAttackPrafab{}
	, mController(nullptr)
	, mHp(0.f)
	, mMaxHp(0.f)
	, mInvincibleTime(1.f)
	, mInvincibleElapsedTime(0.f)
	, mAnimator(nullptr)
	, mSoulStack(0.f)
	, mFeverTime(0.f)
	, mSoulType(ESoulType::Sword)
	, mAttackPositionOffset{}
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
	processFeverTime(dt);
	processCoolTime(dt);
}

void fq::client::Player::OnStart()
{
	mMaxHp = mHp;

	mAnimator = GetComponent<fq::game_module::Animator>();
	assert(mAnimator);

	mController = GetComponent<fq::game_module::CharacterController>();

	// Player등록
	GetScene()->GetEventManager()->FireEvent<client::event::RegisterPlayer>(
		{ GetGameObject(), EPlayerType::LivingArmour });

	// 카메라에 플레이어 등록 
	GetScene()->ViewComponents<CameraMoving>([this](game_module::GameObject& object, CameraMoving& camera)
		{
			camera.AddPlayerTransform(GetComponent<game_module::Transform>());
		});

	// 영혼 타입 적용
	mAnimator->SetParameterInt("SoulType", static_cast<int>(mSoulType));

	// TODO : 갑옷 버프 적용
}

void fq::client::Player::processInput()
{
	auto input = GetScene()->GetInputManager();

	
	if (input->IsPadKeyState(mController->GetControllerID(), EPadKey::B, EKeyState::Tap))
	{
		SummonSoul();
	}
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
		auto monsterAtk = collision.other->GetComponent<client::Attack>();
		if (monsterAtk->ProcessAttack())
		{
			mAnimator->SetParameterTrigger("OnHit");
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
}

void fq::client::Player::SummonSoul()
{
	auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(mSoulPrefab);
	auto& soul = *(instance.begin());

	// 컨트롤러 연결
	soul->GetComponent<game_module::CharacterController>()
		->SetControllerID(mController->GetControllerID());

	// 소울 타입 설정
	soul->GetComponent<Soul>()->SetSoulType(mSoulType);

	// 위치 설정
	auto localMat = GetComponent<game_module::Transform>()->GetLocalMatrix();
	localMat._42 += 1.f;
	soul->GetComponent<game_module::Transform>()->SetLocalMatrix(localMat);

	GetScene()->AddGameObject(soul);
	GetScene()->DestroyGameObject(GetGameObject());
}

void fq::client::Player::processCoolTime(float dt)
{
	mInvincibleElapsedTime = std::max(mInvincibleElapsedTime - dt, 0.f);
}

void fq::client::Player::processFeverTime(float dt)
{
	if (mFeverTime == 0.f)
		return;

	mFeverTime = std::max(mFeverTime - dt, 0.f);

	// 갑옷 버프 종료
	if (mFeverTime == 0.f)
	{
		// TODO : 갑옷 해제에 대한 버프를 진행
		mAttackPower = mAttackPower * 0.5f;
	}
}

