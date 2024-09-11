#define NOMINMAX
#include "Player.h"

#include "../FQGameModule/CharacterController.h"
#include "../FQGameModule/Transform.h"
#include "../FQGameModule/StaticMeshRenderer.h"
#include "Attack.h"
#include "CameraMoving.h"
#include "HpBar.h"
#include "Soul.h"
#include "ClientEvent.h"
#include "PlayerUIManager.h"
#include "DamageCalculation.h"
#include "StaffSoulAttack.h"
#include "LinearAttack.h"
#include "ClientHelper.h"
#include "PlayerSoulVariable.h"

fq::client::Player::Player()
	:mAttackPower(1.f)
	, mController(nullptr)
	, mHp(0.f)
	, mMaxHp(0.f)
	, mMaxSoulGauge(100.f)
	, mSoulGauge(0.f)
	, mInvincibleTime(1.f)
	, mInvincibleElapsedTime(0.f)
	, mAnimator(nullptr)
	, mFeverTime(10.f)
	, mSoulType(ESoulType::Sword)
	, mArmourType(EArmourType::Knight)
	, mbOnShieldBlock(false)
	, mTransform(nullptr)
	, mAttackSpeed(1.f)
	, mEquipWeapone(ESoulType::Sword)
	, mWeaponeMeshes{ nullptr }
	, mFeverElapsedTime(0.f)
	, mbIsActiveOnHit(true)
{}

fq::client::Player::~Player()
{}

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
	mController = GetComponent<fq::game_module::CharacterController>();
	mTransform = GetComponent<fq::game_module::Transform>();

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

	// 무기 연결
	linkWeaponeMeshes();

	// 무기 착용
	EquipArmourWeapone();

	// 피버 버프 적용
	setFeverBuff(true);
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
					GetScene()->GetEventManager()->FireEvent<fq::event::OnPlaySound>({ "K_Shield_Block", false , fq::sound::EChannel::SE });
					return;
				}
			}

			if (mbIsActiveOnHit)
			{
				mAnimator->SetParameterTrigger("OnHit");
			}

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
	auto worldMat = GetComponent<game_module::Transform>()->GetWorldMatrix();
	worldMat._42 += 1.f;
	soul->GetComponent<game_module::Transform>()->SetWorldMatrix(worldMat);

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

int fq::client::Player::GetPlayerID() const
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

void fq::client::Player::EmitBowSoulAttack()
{

	auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(mBowSoulAttack);
	auto& attackObj = *(instance.begin());

	auto attackT = attackObj->GetComponent<game_module::Transform>();
	auto attackComponent = attackObj->GetComponent<Attack>();
	auto linearAttack = attackObj->GetComponent<LinearAttack>();

	// 화살 방향 설정
	auto rotation = mTransform->GetWorldRotation();
	float halfRadian = DirectX::XMConvertToRadians(PlayerSoulVariable::SoulBowAttackAngle) * 0.5f;
	float angle = helper::RandomGenerator::GetInstance().GetRandomNumber(-halfRadian, +halfRadian);
	auto angleOffset = rotation * DirectX::SimpleMath::Quaternion::CreateFromAxisAngle({ 0.f,1.f,0.f }, angle);
	auto fowardDirection = DirectX::SimpleMath::Matrix::CreateFromQuaternion(angleOffset).Forward();

	// 화살 시작 위치
	auto position = mTransform->GetWorldPosition();
	position.y += 1.f;
	position += fowardDirection * PlayerSoulVariable::SoulBowAttackOffset;

	attackT->GenerateWorld(position, angleOffset, attackT->GetWorldScale());
	linearAttack->SetMoveDirection(fowardDirection);
	linearAttack->SetMoveSpeed(PlayerSoulVariable::SoulBowAttackSpeed);

	// 공격 설정 
	AttackInfo attackInfo{};
	attackInfo.damage = dc::GetBowSoulDamage(mAttackPower);
	attackInfo.attackDirection = fowardDirection;
	attackInfo.attacker = GetGameObject();
	attackInfo.bIsInfinite = false;
	attackInfo.remainingAttackCount = 1;
	attackComponent->Set(attackInfo);

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
		mWeaponeMeshes[static_cast<int>(EWeaponeMesh::Shield)]->SetIsRender(isEquip);
		mWeaponeMeshes[static_cast<int>(EWeaponeMesh::Sword)]->SetIsRender(isEquip);
	}
	break;
	case fq::client::ESoulType::Staff:
	{
		mWeaponeMeshes[static_cast<int>(EWeaponeMesh::Staff)]->SetIsRender(isEquip);
	}
	break;
	case fq::client::ESoulType::Axe:
	{
		mWeaponeMeshes[static_cast<int>(EWeaponeMesh::Axe)]->SetIsRender(isEquip);
	}
	break;
	case fq::client::ESoulType::Bow:
	{
		mWeaponeMeshes[static_cast<int>(EWeaponeMesh::Bow)]->SetIsRender(isEquip);
	}
	break;
	}
}

void fq::client::Player::linkWeaponeMeshes()
{
	for (const auto& child : mTransform->GetChildren())
	{
		auto name = child->GetGameObject()->GetName();

		if (name.find("ShieldSocket") != std::string::npos)
		{
			for (const auto& weapone : child->GetChildren())
			{
				auto name = weapone->GetGameObject()->GetName();
				auto mesh = weapone->GetComponent<game_module::StaticMeshRenderer>();

				if (name.find("Shield") != std::string::npos)
				{
					mWeaponeMeshes[static_cast<int>(EWeaponeMesh::Shield)] = mesh;
				}
				else if (name.find("Bow") != std::string::npos)
				{
					mWeaponeMeshes[static_cast<int>(EWeaponeMesh::Bow)] = mesh;
				}
			}
		}
		else if (name.find("WeaponeSocket") != std::string::npos)
		{
			for (const auto& weapone : child->GetChildren())
			{
				auto name = weapone->GetGameObject()->GetName();
				auto mesh = weapone->GetComponent<game_module::StaticMeshRenderer>();

				if (name.find("Sword") != std::string::npos)
				{
					mWeaponeMeshes[static_cast<int>(EWeaponeMesh::Sword)] = mesh;
				}
				else if (name.find("Staff") != std::string::npos)
				{
					mWeaponeMeshes[static_cast<int>(EWeaponeMesh::Staff)] = mesh;
				}
				else if (name.find("Axe") != std::string::npos)
				{
					mWeaponeMeshes[static_cast<int>(EWeaponeMesh::Axe)] = mesh;
				}
				else if (name.find("Bow") != std::string::npos)
				{
					mWeaponeMeshes[static_cast<int>(EWeaponeMesh::Bow)] = mesh;
				}
			}
		}
	}
}

void fq::client::Player::EmitAxeSoulAttack()
{
	auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(mAxeSoulAttack);
	auto& attackObj = *(instance.begin());

	auto attackT = attackObj->GetComponent<game_module::Transform>();
	auto attackComponent = attackObj->GetComponent<Attack>();
	auto foward = mTransform->GetLookAtVector();

	// 공격 트랜스폼 설정
	DirectX::SimpleMath::Vector3 pos = mTransform->GetWorldPosition();
	attackT->SetWorldPosition(pos);

	// 공격설정
	AttackInfo attackInfo{};
	attackInfo.damage = dc::GetAxeSoulDamage(mAttackPower);
	attackInfo.attacker = GetGameObject();
	attackComponent->Set(attackInfo);

	GetScene()->AddGameObject(attackObj);
}

void fq::client::Player::AddSoulGauge(float soul)
{
	mSoulGauge = std::clamp(mSoulGauge + soul, 0.f, mMaxSoulGauge);
}

void fq::client::Player::SetHp(float hp)
{
	mHp = hp;
	mMaxHp = hp;
}

bool fq::client::Player::CanUseSoulAttack() const
{
	float cost = 0.f;

	switch (mSoulType)
	{
	case fq::client::ESoulType::Sword:
		cost = PlayerSoulVariable::SoulSwordAttackCost;
		break;
	case fq::client::ESoulType::Staff:
		cost = PlayerSoulVariable::SoulStaffAttackCost;
		break;
	case fq::client::ESoulType::Axe:
		cost = PlayerSoulVariable::SoulAxeAttackCost;
		break;
	case fq::client::ESoulType::Bow:
		cost = PlayerSoulVariable::SoulBowAttackCost;
		break;
	}

	return mSoulGauge >= cost;
}

void fq::client::Player::setFeverBuff(bool isFever)
{
	if (isFever)
	{
	}
	else
	{

	}

}
