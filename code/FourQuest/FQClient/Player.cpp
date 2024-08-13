#define NOMINMAX
#include "Player.h"

#include "../FQGameModule/CharacterController.h"
#include "../FQGameModule/Transform.h"
#include "../FQGameModule/SoundClip.h"
#include "Attack.h"
#include "CameraMoving.h"
#include "HpBar.h"
#include "Soul.h"
#include "ClientEvent.h"
#include "PlayerUIManager.h"
#include "DamageCalculation.h"
#include "StaffSoulAttack.h"

fq::client::Player::Player()
	:mAttackPower(1.f)
	, mController(nullptr)
	, mHp(0.f)
	, mMaxHp(0.f)
	, mMaxSoulStack(0.f)
	, mSoulStack(0.f)
	, mInvincibleTime(1.f)
	, mInvincibleElapsedTime(0.f)
	, mAnimator(nullptr)
	, mFeverTime(0.f)
	, mSoulType(ESoulType::Sword)
	, mArmourType(EArmourType::Knight)
	, mAttackPositionOffset{}
	, mbOnShieldBlock(false)
	, mTransform(nullptr)
	, mAttackSpeed(1.f)
	, mEquipWeapone(ESoulType::Sword)
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
	mMaxSoulStack = mSoulStack;

	mAnimator = GetComponent<fq::game_module::Animator>();
	mController = GetComponent<fq::game_module::CharacterController>();
	mTransform = GetComponent<fq::game_module::Transform>();

	// 사운드 가져오기  
	mSoundClip = GetScene()->GetObjectByName("SoundManager")->GetComponent<game_module::SoundClip>();

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

	// 무기 착용
	EquipArmourWeapone();

	// TODO : 갑옷 버프 적용

	// Player HUD 등록
	GetScene()->GetObjectByName("PlayerUIManager")->GetComponent<PlayerUIManager>()->AddPlayer(GetPlayerID());
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
			// 플레이어 방패 막기 처리 
			if (mbOnShieldBlock)
			{
				auto attackDir = monsterAtk->GetAttackDirection();
				auto lookAtDir = mTransform->GetLookAtVector();

				attackDir.Normalize();
				lookAtDir.Normalize();

				float dotProduct = lookAtDir.Dot(attackDir);
				float radian = std::acos(dotProduct);

				if (radian >= DirectX::XM_PIDIV2)
				{
					// TODO :: Shield Block 소리 , 이펙트  추가
					mSoundClip->Play("ShieldBlock", false, 0);

					return;
				}
			}

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

	// Quest Event 처리
	GetScene()->GetEventManager()->FireEvent<client::event::PlayerCollideTrigger>(
		{ (int)GetPlayerID(), collision.other->GetName() });
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

float fq::client::Player::GetPlayerID() const
{
	return mController->GetControllerID();
}

void fq::client::Player::EmitStaffSoulAttack()
{
	auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(mStaffSoulAttack);
	auto& attackObj = *(instance.begin());

	auto attackT = attackObj->GetComponent<game_module::Transform>();
	auto attackComponent = attackObj->GetComponent<StaffSoulAttack>();
	attackT->SetParent(mTransform);
	attackComponent->SetPlayer(this);

	GetScene()->AddGameObject(attackObj);
}

void fq::client::Player::EmitSwordSoulAttack()
{
	auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(mSwordSoulAttack);
	auto& attackObj = *(instance.begin());

	auto attackT = attackObj->GetComponent<game_module::Transform>();
	auto attackComponent = attackObj->GetComponent<Attack>();
	auto foward = mTransform->GetLookAtVector();

	// 공격 트랜스폼 설정
	DirectX::SimpleMath::Vector3 pos = mTransform->GetWorldPosition();
	DirectX::SimpleMath::Quaternion rotation = mTransform->GetWorldRotation();
	pos.y += 1.f;
	constexpr float AttackOffset = 1.5f;
	pos += foward * AttackOffset;
	attackT->GenerateWorld(pos, rotation, attackT->GetWorldScale());

	// 공격설정
	AttackInfo attackInfo{};
	attackInfo.damage = dc::GetSwordSoulDamage(mAttackPower);
	attackInfo.attacker = GetGameObject();
	attackInfo.type = EKnockBackType::TargetPosition;
	attackInfo.attackDirection = foward;
	attackInfo.knocBackPower = 20.f;
	attackInfo.attackPosition = mTransform->GetWorldPosition();
	attackComponent->Set(attackInfo);

	GetScene()->AddGameObject(attackObj);
}

void fq::client::Player::EquipSoulWeapone()
{
	equipWeapone(mEquipWeapone, false);
	mEquipWeapone = mSoulType;
	equipWeapone(mEquipWeapone, true);
}

void fq::client::Player::EquipArmourWeapone()
{
	equipWeapone(mEquipWeapone, false);
	mEquipWeapone = static_cast<ESoulType>(mArmourType);
	equipWeapone(mEquipWeapone, true);
}

void fq::client::Player::equipWeapone(ESoulType equipType, bool isEquip)
{
	switch (equipType)
	{
		case fq::client::ESoulType::Sword:
		{
		}
		break;
		case fq::client::ESoulType::Staff:
		{

		}
		break;
		case fq::client::ESoulType::Axe:
		{

		}
		break;
		case fq::client::ESoulType::Bow:
		{

		}
		break;
	}
}

