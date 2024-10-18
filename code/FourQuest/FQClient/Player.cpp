#define NOMINMAX 
#include "Player.h"

#include "../FQGameModule/CharacterController.h"
#include "../FQGameModule/Transform.h"
#include "../FQGameModule/StaticMeshRenderer.h"
#include "../FQGameModule/Decal.h"
#include "../FQGameModule/CharacterController.h"
#include "../FQGameModule/SkinnedMeshRenderer.h"
#include "../FQGameModule/RigidBody.h"

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
#include "SoulVariable.h"
#include "PlayerVariable.h"
#include "DebuffPoisonZone.h"
#include "DeadArmour.h"
#include "BGaugeUI.h"
#include "EffectColorTransmitter.h"
#include "PlayerDummy.h"

fq::client::Player::Player()
	:mAttackPower(1.f)
	, mController(nullptr)
	, mHp(0.f)
	, mMaxHp(0.f)
	, mMaxSoulGauge(100.f)
	, mSoulGauge(0.f)
	, mInvincibleTime(1.f)
	, mInvincibleElapsedTime(1.f)
	, mAnimator(nullptr)
	, mFeverTime(10.f)
	, mSoulType(ESoulType::Sword)
	, mArmourType(EArmourType::Knight)
	, mbOnShieldBlock(false)
	, mTransform(nullptr)
	, mBGaugeUI(nullptr)
	, mEquipWeapone(ESoulType::Sword)
	, mWeaponeMeshes{ nullptr }
	, mFeverElapsedTime(0.f)
	, mbIsActiveOnHit(true)
	, mbIsFeverTime(false)
	, mSoulBuffNumber()
	, mUnequipArmourDurationTime()
	, mbIsUnequipArmourButton(false)
	, mSwordHaed{}
	, mStaffHaed{}
	, mAxeHaed{}
	, mBowHaed{}
	, mRSkillCoolTimeRatio(0.f)
	, mASkillCoolTimeRatio(0.f)
	, mXSkillCoolTimeRatio(0.f)
	, mbIsEmitEnhanceEffect(false)
	, mGBDecreaseDurability(false)
	, mGBIncreaseAttackPower(0)
	, mGBDecreaseCooltime(1)
	, mGBIncreaseSpeed(0)
	, mbCanCreateDummy(false)
	, mPreRSkillCoolTimeRatio(0.f)
	, mPreASkillCoolTimeRatio(0.f)
	, mPreXSkillCoolTimeRatio(0.f)
{
}

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

	processInput(dt);
	processFeverTime(dt);
	processCoolTime(dt);
	processDebuff(dt);

	checkPoisonDuration(dt);
	checkCoolTime();
}

void fq::client::Player::OnLateUpdate(float dt)
{
	mAttackPower = mBaseAttackPower;

	processBuff();
	mSoulBuffNumber = 0;
}

void fq::client::Player::OnStart()
{
	mAnimator = GetComponent<fq::game_module::Animator>();
	mController = GetComponent<fq::game_module::CharacterController>();
	mTransform = GetComponent<fq::game_module::Transform>();
	mBaseSpeed = GetComponent<fq::game_module::CharacterController>()->GetMovementInfo().maxSpeed;
	mBaseAcceleration = GetComponent<fq::game_module::CharacterController>()->GetMovementInfo().acceleration;
	for (const auto& child : GetGameObject()->GetChildren())
	{
		if (child->HasComponent<BGaugeUI>())
			mBGaugeUI = child->GetComponent<BGaugeUI>();
	}
	mBaseAttackPower = mAttackPower;

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

	// Decal 색상 적용
	setDecalColor();

	// 머리 설정
	linkSoulTypeHead();

	for (auto& child : GetGameObject()->GetChildren())
	{
		if (child->HasComponent<game_module::SkinnedMeshRenderer>())
		{
			mSkinnedMesh = child->GetComponent<game_module::SkinnedMeshRenderer>();
		}
	}

	GetComponent<HpBar>()->DecreaseHp((mMaxHp - mHp) / mMaxHp);

	mbCanCreateDummy = false;
}

fq::game_module::GameObject* fq::client::Player::CreateDummyOrNull()
{
	if (!mbCanCreateDummy)
	{
		return nullptr;
	}
	if (mDummyPrefab.GetPrefabPath().empty())
	{
		return nullptr;
	}

	auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(mDummyPrefab);
	auto& dummyObject = *(instance.begin());
	const auto& worldMatrix = GetTransform()->GetWorldMatrix();
	dummyObject->GetTransform()->SetWorldMatrix(worldMatrix);

	GetScene()->AddGameObject(dummyObject);

	return dummyObject.get();
}

void fq::client::Player::processInput(float dt)
{
	auto input = GetScene()->GetInputManager();

	if (input->IsPadKeyState(mController->GetControllerID(), EPadKey::B, EKeyState::Tap))
	{
		mUnequipArmourDurationTime = 0.f;
		mbIsUnequipArmourButton = true;
	}
	else if (input->IsPadKeyState(mController->GetControllerID(), EPadKey::B, EKeyState::Hold) && mbIsUnequipArmourButton)
	{
		mUnequipArmourDurationTime += dt;

		if (mBGaugeUI)
		{
			mBGaugeUI->SetVisible(true);
			mBGaugeUI->SetRatio(mUnequipArmourDurationTime / SoulVariable::ButtonTime * 360.f);
		}

		if (mUnequipArmourDurationTime < SoulVariable::ButtonTime)
			return;


		if ((mHp / mMaxHp) * 100.f >= SoulVariable::HpPercent)
		{
			RemainDeadArmour();
		}

		SummonSoul(false);
		mbIsUnequipArmourButton = false;
	}
	else if (input->IsPadKeyState(mController->GetControllerID(), EPadKey::B, EKeyState::Away))
	{
		mbIsUnequipArmourButton = false;

		if (mBGaugeUI)
		{
			mBGaugeUI->SetVisible(false);
		}
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
	// 플레이어 피격
	if (collision.other->GetTag() == game_module::ETag::MonsterAttack
		&& collision.other->HasComponent<client::Attack>())
	{
		auto monsterAtk = collision.other->GetComponent<client::Attack>();

		bool isHitAble = mInvincibleElapsedTime == 0.f;
		if (monsterAtk->ProcessAttack())
		{
			// 플레이어 넉백 처리
			if (monsterAtk->HasKnockBack())
			{
				auto type = monsterAtk->GetKnockBackType();
				float power = monsterAtk->GetKnockBackPower();

				if (type == EKnockBackType::TargetPosition)
				{
					auto playerPos = mTransform->GetWorldPosition();
					playerPos.y = 0.f;
					auto monsterPos = monsterAtk->GetTransform()->GetWorldPosition();
					monsterPos.y = 0.f;

					auto knockBackDir = playerPos - monsterPos;
					knockBackDir.Normalize();
					auto rigid = GetComponent<game_module::RigidBody>();

					knockBackDir *= power;
					rigid->AddLinearVelocity(knockBackDir);
				}
			}

			if (isHitAble)
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

						fq::event::OnCreateStateEvent stateEvent;
						stateEvent.gameObject = GetGameObject();
						stateEvent.RegisterKeyName = "K_Shield_Block";
						if (!stateEvent.RegisterKeyName.empty())
						{
							GetGameObject()->GetScene()->GetEventManager()->FireEvent<fq::event::OnCreateStateEvent>(std::move(stateEvent));
						}

						return;
					}
				}
				// 체력 감소
				float attackPower = monsterAtk->GetAttackPower();
				DecreaseHp(attackPower);

				// Hit 애니메이션 
				if (mbIsActiveOnHit)
				{
					// 무적시간 
					mAnimator->SetParameterTrigger("OnHit");
					mInvincibleElapsedTime = mInvincibleTime;
				}

				// 피격 사운드 재생
				monsterAtk->PlayHitSound();
			}
		}
	}

	// Quest Event 
	if (mController != nullptr)
	{
		GetScene()->GetEventManager()->FireEvent<client::event::PlayerCollideTrigger>(
			{ (int)GetPlayerID(), collision.other->GetName() });
	}
}

void fq::client::Player::SummonSoul(bool isDestroyArmour)
{
	if (isDestroyArmour)
	{
		spdlog::trace("DestroyArmour");

		// 이펙트 방출
		fq::event::OnCreateStateEvent stateEvent;
		stateEvent.gameObject = GetGameObject();
		stateEvent.RegisterKeyName = "P_Die_Armor";
		if (!stateEvent.RegisterKeyName.empty())
		{
			GetGameObject()->GetScene()->GetEventManager()->FireEvent<fq::event::OnCreateStateEvent>(std::move(stateEvent));
		}
	}

	// 위치 설정
	auto worldMat = GetComponent<game_module::Transform>()->GetWorldMatrix();
	worldMat._42 += 1.f;

	GetScene()->GetEventManager()->FireEvent<client::event::SummonSoul>(
		{ (int)GetPlayerID(), mSoulType, worldMat, mSoulPrefab, isDestroyArmour, mSoulGauge, mMaxSoulGauge });

	GetScene()->DestroyGameObject(GetGameObject());

	mbCanCreateDummy = !isDestroyArmour;
}

void fq::client::Player::processCoolTime(float dt)
{
	mInvincibleElapsedTime = std::max(mInvincibleElapsedTime - dt, 0.f);
}

void fq::client::Player::processFeverTime(float dt)
{
	if (!mbIsFeverTime)
		return;

	mFeverElapsedTime += dt;

	// 갑옷 버프 종료
	if (mFeverElapsedTime >= mFeverTime)
	{
		setFeverBuff(false);
	}

}

void fq::client::Player::processDebuff(float dt)
{
	auto poisonZone = GetComponent<DebuffPoisonZone>();

	// 포이즌 존에 밟았을 경우에 생긴 포이즌 존 컴포넌트에 의해 HP 깎이는 코드
	if (poisonZone != nullptr)
	{
		float durationTime = poisonZone->GetDurationTime();
		float poisonTurm = poisonZone->GetPoisonTurm();
		float poisonDamage = poisonZone->GetPosionDamage();

		durationTime += dt;

		if (durationTime >= poisonTurm)
		{
			durationTime -= poisonTurm;

			mHp -= poisonDamage;

			// UI 설정
			GetComponent<HpBar>()->DecreaseHp(poisonDamage / mMaxHp);

			// 플레이어 사망처리 
			if (mHp <= 0.f)
			{
				SummonSoul(true);
			}
		}

		poisonZone->SetDurationTime(durationTime);
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

	// 이펙트 색상 설정
	auto effectColorTransmitter = attackObj->GetComponent<EffectColorTransmitter>();
	if (effectColorTransmitter != nullptr)
	{
		effectColorTransmitter->SetSoulType(GetSoulType());
	}

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

	// 사운드 실행
	playBowSoulSound();

	// 이펙트 색상 설정
	auto effectColorTransmitter = attackObj->GetComponent<EffectColorTransmitter>();
	if (effectColorTransmitter != nullptr)
	{
		effectColorTransmitter->SetSoulType(GetSoulType());
	}

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
	attackInfo.HitEffectName = "K_Swing_Hit_blood";
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
	attackInfo.HitEffectName = "W_Hit_blunt";
	attackComponent->Set(attackInfo);

	GetScene()->AddGameObject(attackObj);
}

void fq::client::Player::AddSoulGauge(float soul)
{
	mSoulGauge = std::clamp(mSoulGauge + soul, 0.f, mMaxSoulGauge);
}

void fq::client::Player::SetHp(float hp)
{
	mHp = std::min<float>(hp, mMaxHp);
}

void fq::client::Player::SetMaxHp(float maxHp)
{
	mMaxHp = maxHp;
}

float fq::client::Player::GetHp() const
{
	return mHp;
}

float fq::client::Player::GetMaxHp() const
{
	return mMaxHp;
}

void fq::client::Player::SetEditorHp(float hp)
{
	mMaxHp = hp;
	mHp = hp;
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
	assert(isFever != mbIsFeverTime);

	mFeverElapsedTime = 0.f;
	mbIsFeverTime = isFever;

	// 피버 타임 이펙트
	if (isFever)
	{
		fq::event::OnCreateStateEvent stateEvent;
		stateEvent.gameObject = GetGameObject();
		stateEvent.RegisterKeyName = "P_Fever";
		if (!stateEvent.RegisterKeyName.empty())
		{
			GetGameObject()->GetScene()->GetEventManager()->FireEvent<fq::event::OnCreateStateEvent>(std::move(stateEvent));
		}
	}
	else
	{
		fq::event::OnDeleteStateEvent stateEvent;
		stateEvent.gameObject = GetGameObject();
		stateEvent.RegisterKeyName = "P_Fever";
		if (!stateEvent.RegisterKeyName.empty())
		{
			GetGameObject()->GetScene()->GetEventManager()->FireEvent<fq::event::OnDeleteStateEvent>(std::move(stateEvent));
		}
	}
}

void fq::client::Player::processBuff()
{
	// 피버 버프
	if (mbIsFeverTime)
	{
		mAttackPower = mBaseAttackPower * PlayerVariable::FeverAttackIncreaseRatio;
		mController->AddFinalSpeedMultiplier(PlayerVariable::FeverSpeedIncreaseRatio - 1.f);
	}

	// 소울 버프 
	if (mSoulBuffNumber != 0)
	{
		mAttackPower += (mBaseAttackPower * ((SoulVariable::DamageUpRatio - 1.f) * mSoulBuffNumber));
		mController->AddFinalSpeedMultiplier((SoulVariable::SpeedUpRatio - 1.f) * mSoulBuffNumber);
	}

	handleEmitEnhanceEffect();

	// 여신상 버프 
	mAttackPower *= (1.f + mGBIncreaseAttackPower);
	mController->AddFinalSpeedMultiplier(mGBIncreaseSpeed);
}

void fq::client::Player::setDecalColor()
{
	auto gameObject = GetGameObject();

	for (auto child : gameObject->GetChildren())
	{
		auto decal = child->GetComponent<game_module::Decal>();
		if (decal)
		{
			auto info = decal->GetDecalMaterialInfo();
			info.BaseColor = { 0.f,0.f,0.f,1.f };
			switch (mSoulType)
			{
			case fq::client::ESoulType::Sword:
				info.BaseColor = PlayerSoulVariable::SwordSoulColor;
				break;
			case fq::client::ESoulType::Staff:
				info.BaseColor = PlayerSoulVariable::StaffSoulColor;
				break;
			case fq::client::ESoulType::Axe:
				info.BaseColor = PlayerSoulVariable::AxeSoulColor;
				break;
			case fq::client::ESoulType::Bow:
				info.BaseColor = PlayerSoulVariable::BowSoulColor;
				break;
			}

			decal->SetDecalMaterialInfo(info);
		}
	}

}

void fq::client::Player::linkSoulTypeHead()
{
	// 머리 소켓 
	for (const auto& child : mTransform->GetChildren())
	{
		auto name = child->GetGameObject()->GetName();
		if (name.find("HeadSocket") != std::string::npos)
		{
			game_module::PrefabResource res{};

			switch (mSoulType)
			{
			case fq::client::ESoulType::Sword:
				res = mSwordHaed;
				break;
			case fq::client::ESoulType::Staff:
				res = mStaffHaed;
				break;
			case fq::client::ESoulType::Axe:
				res = mAxeHaed;
				break;
			case fq::client::ESoulType::Bow:
				res = mBowHaed;
				break;
			}

			auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(res);
			auto& head = *(instance.begin());

			child->AddChild(head->GetComponent<game_module::Transform>());
			GetScene()->AddGameObject(head);
		}
	}

}

bool fq::client::Player::IsFeverTime() const
{
	return mbIsFeverTime;
}

void fq::client::Player::OnTriggerStay(const game_module::Collision& collision)
{
	// Quest Event 
	if (mController != nullptr)
	{
		GetScene()->GetEventManager()->FireEvent<client::event::PlayerCollideStayTrigger>(
			{ (int)GetPlayerID(), collision.other->GetName() });
	}
}

void fq::client::Player::playBowSoulSound()
{
	int random = std::rand() % 4;

	switch (random)
	{
	case 0:
		GetScene()->GetEventManager()->FireEvent<fq::event::OnPlaySound>({ "P_RapidFire_1", false , fq::sound::EChannel::SE });
		break;
	case 1:
		GetScene()->GetEventManager()->FireEvent<fq::event::OnPlaySound>({ "P_RapidFire_2", false , fq::sound::EChannel::SE });
		break;
	case 2:
		GetScene()->GetEventManager()->FireEvent<fq::event::OnPlaySound>({ "P_RapidFire_3", false , fq::sound::EChannel::SE });
		break;
	case 3:
		GetScene()->GetEventManager()->FireEvent<fq::event::OnPlaySound>({ "P_RapidFire_4", false , fq::sound::EChannel::SE });
		break;
	default:
		break;
	}
}

void fq::client::Player::handleEmitEnhanceEffect()
{
	if (mSoulBuffNumber != 0)
	{
		if (!mbIsEmitEnhanceEffect)
		{
			// 이펙트 생성
			fq::event::OnCreateStateEvent stateEvent;
			stateEvent.gameObject = GetGameObject();
			stateEvent.RegisterKeyName = "P_Enhance";
			GetGameObject()->GetScene()->GetEventManager()->FireEvent<fq::event::OnCreateStateEvent>(std::move(stateEvent));

			mbIsEmitEnhanceEffect = true;
		}
	}
	else
	{
		// 이펙트 삭제
		fq::event::OnDeleteStateEvent stateEvent;
		stateEvent.gameObject = GetGameObject();
		stateEvent.RegisterKeyName = "P_Enhance";
		GetGameObject()->GetScene()->GetEventManager()->FireEvent<fq::event::OnDeleteStateEvent>(std::move(stateEvent));

		mbIsEmitEnhanceEffect = false;
	}
}

void fq::client::Player::SetLowerBodyAnimation()
{
	auto input = GetScene()->GetInputManager();
	auto id = mController->GetControllerID();

	// 이동 방향
	auto x = input->GetStickInfomation(id, EPadStick::leftX);
	auto y = input->GetStickInfomation(id, EPadStick::leftY);
	DirectX::SimpleMath::Vector3 moveDir{ x, 0.f, y };
	LowerDirection direction = {};

	if (x == 0.f && y == 0.f)
	{
		direction = LowerDirection::Stop;
	}
	else
	{
		moveDir.Normalize();

		// 시선 방향
		auto look = mTransform->GetLookAtVector();
		look.y = 0.f;
		look.Normalize();

		// 내적으로 각도 판단
		float angle = std::acos(look.Dot(moveDir));

		// 외적으로 방향 판단 
		bool isClock = look.Cross(moveDir).y > 0.f;

		constexpr float Angle135 = DirectX::XM_PIDIV2 + DirectX::XM_PIDIV4;

		if (angle <= DirectX::XM_PIDIV4)
			direction = LowerDirection::Foward;
		else if (angle <= Angle135 && isClock)
			direction = LowerDirection::Right;
		else if (angle <= Angle135 && !isClock)
			direction = LowerDirection::Left;
		else
			direction = LowerDirection::Back;
	}

	mAnimator->SetParameterInt("LowerDir", static_cast<int>(direction));
}

void fq::client::Player::DecreaseHp(float hp, bool bUseMinHp /*= false*/, bool bIgnoreInvincible /*= fasle*/)
{
	if (GetGameObject()->IsDestroyed())
	{
		return;
	}

	bool isHitAble = mInvincibleElapsedTime == 0.f;

	if (!isHitAble && !bIgnoreInvincible) return;

	// 피버타임에는 공격 
	if (bUseMinHp && bIgnoreInvincible && (mbIsFeverTime || !mGBDecreaseDurability))
		return;

	if (bUseMinHp)
	{
		float prevHp = mHp;
		mHp = std::max(PlayerVariable::HpReductionOnAttackMinHp, mHp - hp);
		GetComponent<HpBar>()->DecreaseHp(hp / mMaxHp);
	}
	else
	{
		mHp -= hp;
		GetComponent<HpBar>()->DecreaseHp(hp / mMaxHp);
	}

	// 플레이어 사망처리 
	if (mHp <= 0.f)
	{
		SummonSoul(true);
	}
}

float fq::client::Player::GetASkillCoolTimeRatio() const
{
	return mASkillCoolTimeRatio;
}

float fq::client::Player::GetRSkillCoolTimeRatio() const
{
	return mRSkillCoolTimeRatio;
}

void fq::client::Player::SetASkillCoolTimeRatio(float ratio)
{
	mASkillCoolTimeRatio = std::clamp(ratio, 0.f, 1.f);
}

void fq::client::Player::SetRSkillCoolTimeRatio(float ratio)
{
	mRSkillCoolTimeRatio = std::clamp(ratio, 0.f, 1.f);
}

float fq::client::Player::GetXSkillCoolTimeRatio() const
{
	return mXSkillCoolTimeRatio;
}

void fq::client::Player::SetXSkillCoolTimeRatio(float ratio)
{
	mXSkillCoolTimeRatio = std::clamp(ratio, 0.f, 1.f);
}

void fq::client::Player::SetPoisonRimLight(float duration)
{
	mDuration = duration;
	mCurTime = 0;
	auto matInfo = mSkinnedMesh->GetMaterialInstanceInfo();
	matInfo.bUseRimLight = true;
	matInfo.RimPow = 0.4f;
	matInfo.RimLightColor = { 6 / (float)255, 97 / (float)255, 0, 1 };
	mSkinnedMesh->SetMaterialInstanceInfo(matInfo);
}

void fq::client::Player::checkPoisonDuration(float dt)
{
	mCurTime += dt;
	if (mCurTime > mDuration)
	{
		auto matInfo = mSkinnedMesh->GetMaterialInstanceInfo();
		matInfo.bUseRimLight = false;
		mSkinnedMesh->SetMaterialInstanceInfo(matInfo);
	}
}

void fq::client::Player::RemainDeadArmour()
{
	std::vector<std::shared_ptr<fq::game_module::GameObject>> instance;

	if (mArmourType == EArmourType::Knight)
		instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(mDeadKnightArmour);
	if (mArmourType == EArmourType::Magic)
		instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(mDeadMagicArmour);
	if (mArmourType == EArmourType::Archer)
		instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(mDeadArcherArmour);
	if (mArmourType == EArmourType::Warrior)
		instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(mDeadWarriorArmour);

	auto& soul = *(instance.begin());

	// 벗은 갑옷 위치 세팅
	auto thisTransform = GetComponent<fq::game_module::Transform>();
	auto deadArmourTransform = soul->GetComponent<fq::game_module::Transform>();

	deadArmourTransform->SetWorldMatrix(thisTransform->GetWorldMatrix()
		* DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(0.f, 1.372f, 0.f)));

	// 해당 갑옷에 벗은 플레이어 ID 저장 및 체력 설정
	auto deadArmour = soul->GetComponent<DeadArmour>();
	assert(deadArmour != nullptr);
	if (deadArmour != nullptr)
	{
		deadArmour->SetUnequippedPlayerId(GetPlayerID());
		deadArmour->SetHp(mHp);
	}

	GetScene()->AddGameObject(soul);
}

void fq::client::Player::checkCoolTime()
{
	if (mPreRSkillCoolTimeRatio > 0)
	{
		if (mRSkillCoolTimeRatio <= 0)
		{
			GetScene()->GetEventManager()->FireEvent<client::event::InitCoolTime>(
				{ GetPlayerID(), mArmourType, 'R' });
		}
	}
	if (mPreASkillCoolTimeRatio > 0)
	{
		if (mASkillCoolTimeRatio <= 0)
		{
			GetScene()->GetEventManager()->FireEvent<client::event::InitCoolTime>(
				{ GetPlayerID(), mArmourType, 'A' });
		}
	}
	if (mPreXSkillCoolTimeRatio > 0)
	{
		if (mXSkillCoolTimeRatio <= 0)
		{
			GetScene()->GetEventManager()->FireEvent<client::event::InitCoolTime>(
				{ GetPlayerID(), mArmourType, 'X' });
		}
	}

	mPreRSkillCoolTimeRatio = mRSkillCoolTimeRatio;
	mPreASkillCoolTimeRatio = mASkillCoolTimeRatio;
	mPreXSkillCoolTimeRatio = mXSkillCoolTimeRatio;
}

