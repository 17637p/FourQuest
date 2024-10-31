#define NOMINMAX

#include "BossMonster.h"

#include "../FQGameModule/NavigationAgent.h"
#include "../FQGameModule/Transform.h"
#include "../FQGameModule/Animator.h"
#include "../FQGameModule/SkinnedMeshRenderer.h"
#include "../FQGameModule/BoxCollider.h"
#include "../FQGameModule/Socket.h"

#include "Attack.h"
#include "GameManager.h"
#include "DamageCalculation.h"
#include "KnockBack.h"
#include "BossHP.h"
#include "ClientHelper.h"
#include "MonsterDefine.h"
#include "DeadArmour.h"
#include "LevelHepler.h"
#include "Player.h"
#include "PlayerInfoVariable.h"
#include "PlayerDummy.h"
#include "ClientHelper.h"
#include "UIShaker.h"
#include "MeleeMonster.h"
#include "PlantMonster.h"

#include <spdlog/spdlog.h>

fq::client::BossMonster::BossMonster()
	: mGameManager(nullptr)
	, mKnockBack(nullptr)
	, mTransform(nullptr)
	, mAnimator(nullptr)
	, mSkinnedMesh(nullptr)
	, mTarget(nullptr)
	, mHpBar(nullptr)

	, mMaxHp(0.f)
	, mHp(2000.f)
	, mAttackPower(50.f)
	, mMoveSpeed(1.f)
	, mAcceleration(2.f)
	, mRotationSpeed(0.1f)

	, mAttackRange(10.f)
	, mDetectRange(5.f)

	, mRushKnockBackPower(5.f)
	, mSmashKnockBackPower(3.f)
	, mComboAttackKnockBackPower(3.f)
	, mContinousKnockBackPower(3.f)
	, mJumpAttackKnockBack(3.f)

	, mAttackElapsedTime(0.f)
	, mAttackCoolTime(3.f)
	, mMinWaitAttackTime(0.5f)
	, mMaxWaitAttackTime(2.f)

	, mSmashDownOffset(0.f)
	, mComboAttackOffset(0.f)
	, mSecondComboAttackRatio(0.5f)
	, mComboAttackReboundPower(5.f)

	, mStartGroggyGauge(100.f)
	, mGroggyGauge(0.f)
	, mGroggyIncreaseRatio(0.1f)
	, mGroggyDecreasePerSecond(1.f)
	, mGroggyDecreaseHpRatio(2.f)
	, mGroggyDuration(5.f)
	, mGroggyElapsed(0.f)

	, mRushProbability(0.1f)
	, mSmashProbability(0.2f)
	, mRoarProbability(0.1f)
	, mContinousProbability(0.1f)
	, mEatProbability(0.1f)

	, mSmashDownAttack()
	, mComboAttack()
	, mRushAttack()
	, mJumpDownAttack()

	, mSmashDownEffect()
	, mComboEffect()
	, mRushEffect()
	, mJumpDecalEffect()

	, mHpBarPrefab()

	, mDummyTraceDurationTime(0.f)
	, mbUseDummyTraceRandomRange(false)
	, mDummyDurationRandomRangeMin(0.f)
	, mDummyDurationRandomRangeMax(0.f)
	, mCurrentDummyTraceDurationTime(0.f)
	, mDummyTraceElapsedTime(0.f)
	, mIsDummyTarget(false)

	, mJumpDistance(10.f)
	, mJumpAttackPower(50.f)
	, mJumpPosition()
	, mDefaultJumpPosition()

	, mArrowImotalTime()
	, mArrowHitDuration()

	, mbEnterAngryState(false)
	, mAngryRatio(0.5f)
	, mAngrySpeedRatio(2.f)
	, mAngryCoolTimeRatio(0.5f)
	, mAngryPreAttackSpeedRatio(2.f)
	, mAngryAttackSpeedRatio(2.f)
	, mAngryAttackPowerRatio(2.f)
	, mAngryEnteringVigilantCount(0)
	, mAngryEnteringVigilantMinCount(1)
	, mAngryEnteringVigilantMaxCount(3)
	, mVigilantMinCount(1)
	, mVigilantMaxCount(3)
	, mAngryGroggyDecreasePerSecond(2.f)

	//Roar 패턴 관련
	, mMinMonsterCount(10)
	, mRoarCoolTime(30.f)
	, mRoarElapsed(0.f)
{}


std::shared_ptr<fq::game_module::Component> fq::client::BossMonster::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	std::shared_ptr<BossMonster> cloneMonster = std::dynamic_pointer_cast<BossMonster>(clone);

	if (cloneMonster == nullptr) // 새로 생성해서 복사본을 준다
	{
		cloneMonster = game_module::ObjectPool::GetInstance()->Assign<BossMonster>(*this);
	}
	else // clone에 데이터를 복사한다.
	{
		// 기본 대입 연산자 호출한다.
		*cloneMonster = *this;
	}

	return cloneMonster;
}

void fq::client::BossMonster::OnStart()
{
	mTransform = GetComponent<game_module::Transform>();
	mAnimator = GetComponent<game_module::Animator>();
	mKnockBack = GetComponent<KnockBack>();

	// 난이도에 따른 공격력 Hp 설정
	mAttackPower = GetAttackPower() * LevelHepler::GetDamageRatio();
	mHp = mHp * LevelHepler::GetHpRatio();
	mMaxHp = mHp;

	auto agent = GetComponent<game_module::NavigationAgent>();
	agent->SetSpeed(mMoveSpeed);
	agent->SetAcceleration(mAcceleration);
	agent->SetRadius(1.f);

	// GameManager 연결
	mGameManager = GetScene()->GetObjectByName("GameManager")->GetComponent<GameManager>();

	// SkinnedMesh 연결
	mSkinnedMesh = mTransform->GetChildren()[0]->GetComponent<game_module::SkinnedMeshRenderer>();

	mbEnterAngryState = false;

	GenerateAngryEnteringVigilantCount();
}

void fq::client::BossMonster::OnUpdate(float dt)
{
	mArrowHitDuration += dt;

	// 공격 쿨타임 계산 
	mAttackElapsedTime = std::min(mAttackCoolTime, mAttackElapsedTime + dt);

	bool canAttack = mAttackElapsedTime >= mAttackCoolTime;
	mAnimator->SetParameterBoolean("CanAttack", canAttack);

	updateGroggy(dt);
	updateJump(dt);

	// 현재 타겟이 더미 타겟인지 체크
	if (mIsDummyTarget)
	{
		mDummyTraceElapsedTime += dt;

		if (mCurrentDummyTraceDurationTime < mDummyTraceElapsedTime)
		{
			SetTarget(nullptr);
		}
	}

	if (IsAngry())
	{
		// 첫 진입 시 속도와 플래그 지정
		if (!mbEnterAngryState)
		{
			auto agent = GetComponent<game_module::NavigationAgent>();
			agent->SetSpeed(mMoveSpeed * mAngrySpeedRatio);
			agent->SetAcceleration(mAcceleration * mAngrySpeedRatio);
			mAnimator->SetParameterBoolean("IsAngry", true);
			mAnimator->SetParameterBoolean("IsEnterAngry", true);

			mbEnterAngryState = true;
		}
	}

	mRoarElapsed += dt;
}


void fq::client::BossMonster::SetJumpPosition(const DirectX::SimpleMath::Vector3& jumpPosition)
{
	mJumpPosition = jumpPosition;
}

const DirectX::SimpleMath::Vector3& fq::client::BossMonster::GetJumpPosition() const
{
	return mJumpPosition;
}

const DirectX::SimpleMath::Vector3& fq::client::BossMonster::GetDefaultJumpPosition() const
{
	return mDefaultJumpPosition;
}

std::shared_ptr<fq::game_module::GameObject> fq::client::BossMonster::EmitSmashDecalEffect()
{
	if (mSmashDownDecalEffect.GetPrefabPath().empty())
	{
		return nullptr;
	}

	auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(mSmashDownDecalEffect);
	auto& effectObj = *(instance.begin());

	SetSmashDecalEffect(effectObj);

	GetScene()->AddGameObject(effectObj);

	return effectObj;
}

void fq::client::BossMonster::SetSmashDecalEffect(std::shared_ptr<game_module::GameObject> gameObject)
{
	auto effectT = gameObject->GetComponent<game_module::Transform>();

	// 공격 트랜스폼 설정
	auto attackPos = mTransform->GetWorldPosition();
	auto scale = effectT->GetWorldScale();
	auto rotation = mTransform->GetWorldRotation();

	auto rotationMat = DirectX::SimpleMath::Matrix::CreateFromQuaternion(rotation);
	auto foward = rotationMat.Forward();
	attackPos += foward * mSmashDownOffset;
	effectT->GenerateWorld(attackPos, rotation, scale);
}

std::shared_ptr<fq::game_module::GameObject> fq::client::BossMonster::EmitComboEffect(float xAxisOffset)
{
	if (mComboEffect.GetPrefabPath().empty())
	{
		return nullptr;
	}

	auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(mComboEffect);
	auto effectObj = *(instance.begin());

	auto effectT = effectObj->GetComponent<game_module::Transform>();
	{
		auto worldPos = mTransform->GetWorldPosition();
		auto scale = effectT->GetWorldScale();
		auto rotation = mTransform->GetWorldRotation();

		auto rotationMat = DirectX::SimpleMath::Matrix::CreateFromQuaternion(rotation);
		auto foward = rotationMat.Forward();
		auto right = rotationMat.Right();
		worldPos += foward * mComboAttackOffset;
		worldPos += right * xAxisOffset;
		worldPos.y += mEffectYOffset;
		effectT->GenerateWorld(worldPos, rotation, scale);
	}

	GetScene()->AddGameObject(effectObj);

	return effectObj;
}

std::shared_ptr<fq::game_module::GameObject> fq::client::BossMonster::EmitComboDecalEffect(float xAxisOffset)
{
	if (mComboDecalEffect.GetPrefabPath().empty())
	{
		return nullptr;
	}

	auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(mComboDecalEffect);
	auto effectObj = *(instance.begin());

	SetComboDecalEffect(effectObj, xAxisOffset);

	GetScene()->AddGameObject(effectObj);

	return effectObj;
}

void fq::client::BossMonster::SetComboDecalEffect(std::shared_ptr<game_module::GameObject> gameObject, float xAxisOffset)
{
	auto effectT = gameObject->GetComponent<game_module::Transform>();
	{
		auto worldPos = mTransform->GetWorldPosition();
		auto scale = effectT->GetWorldScale();
		auto rotation = mTransform->GetWorldRotation();

		auto rotationMat = DirectX::SimpleMath::Matrix::CreateFromQuaternion(rotation);
		auto foward = rotationMat.Forward();
		auto right = rotationMat.Right();
		worldPos += foward * mComboAttackOffset;
		worldPos += right * xAxisOffset;
		effectT->GenerateWorld(worldPos, rotation, scale);
	}
}

std::shared_ptr<fq::game_module::GameObject> fq::client::BossMonster::EmitRushEffect()
{
	if (mRushEffect.GetPrefabPath().empty())
	{
		return nullptr;
	}

	auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(mRushEffect);
	auto effectObj = *(instance.begin());

	auto effectT = effectObj->GetComponent<game_module::Transform>();
	effectT->SetParent(mTransform);

	GetScene()->AddGameObject(effectObj);

	return effectObj;
}

std::shared_ptr<fq::game_module::GameObject> fq::client::BossMonster::EmitRushDecalEffect()
{
	if (mRushDecalEffect.GetPrefabPath().empty())
	{
		return nullptr;
	}

	auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(mRushDecalEffect);
	auto effectObj = *(instance.begin());

	auto effectT = effectObj->GetComponent<game_module::Transform>();
	auto worldPosition = mTransform->GetWorldPosition();
	effectT->SetParent(mTransform);
	effectT->SetLocalPosition({ 0, mEffectYOffset, 0 });

	GetScene()->AddGameObject(effectObj);

	return effectObj;
}

std::shared_ptr<fq::game_module::GameObject> fq::client::BossMonster::EmitJumpEffect()
{
	if (mJumpEffect.GetPrefabPath().empty())
	{
		return nullptr;
	}

	auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(mJumpEffect);
	auto effectObj = *(instance.begin());

	auto effectT = effectObj->GetComponent<game_module::Transform>();
	auto effectPosition = mJumpPosition;
	mJumpPosition.y += mEffectYOffset;
	effectT->SetWorldPosition(effectPosition);

	GetScene()->AddGameObject(effectObj);

	return effectObj;
}

std::shared_ptr<fq::game_module::GameObject> fq::client::BossMonster::EmitJumpDecalEffect()
{
	if (mJumpDecalEffect.GetPrefabPath().empty())
	{
		return nullptr;
	}

	auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(mJumpDecalEffect);
	auto effectObj = *(instance.begin());

	auto effectT = effectObj->GetComponent<game_module::Transform>();
	effectT->SetWorldPosition(mJumpPosition);

	GetScene()->AddGameObject(effectObj);

	return effectObj;
}

std::shared_ptr<fq::game_module::GameObject> fq::client::BossMonster::EmitJumpDecalEndEffect()
{
	if (mJumpDecalEndEffect.GetPrefabPath().empty())
	{
		return nullptr;
	}

	auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(mJumpDecalEndEffect);
	auto effectObj = *(instance.begin());

	auto effectT = effectObj->GetComponent<game_module::Transform>();
	effectT->SetWorldPosition(mJumpPosition);

	GetScene()->AddGameObject(effectObj);

	return effectObj;
}

void fq::client::BossMonster::EmitJumpAttack()
{
	if (mJumpDownAttack.GetPrefabPath().empty())
	{
		return;
	}

	auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(mJumpDownAttack);
	auto& object = *(instance.begin());

	auto transform = object->GetComponent<game_module::Transform>();
	auto worldPos = mJumpPosition;
	transform->SetWorldPosition(worldPos);

	// 공격 정보 설정
	AttackInfo attackInfo{};
	auto attackComponent = object->GetComponent<Attack>();

	if (attackComponent != nullptr)
	{
		auto rotation = mTransform->GetWorldRotation();
		auto rotationMat = DirectX::SimpleMath::Matrix::CreateFromQuaternion(rotation);
		auto foward = rotationMat.Forward();

		attackInfo.attacker = GetGameObject();
		attackInfo.damage = mJumpAttackPower;
		attackInfo.attackDirection = foward;
		attackInfo.type = EKnockBackType::TargetPosition;
		attackInfo.knocBackPower = mJumpAttackKnockBack;
		attackComponent->Set(attackInfo);
	}

	GetScene()->AddGameObject(object);
}

void fq::client::BossMonster::waitAttack()
{
	mAttackCoolTime = helper::RandomGenerator::GetInstance().GetRandomNumber(mMinWaitAttackTime, mMaxWaitAttackTime);

	if (IsAngry())
	{
		mAttackCoolTime *= mAngryCoolTimeRatio;
	}

	mAttackElapsedTime = 0.f;
	mAnimator->SetParameterBoolean("CanAttack", false);
}

void fq::client::BossMonster::OnTriggerEnter(const game_module::Collision& collision)
{
	// 피격처리
	if (collision.other->GetTag() == game_module::ETag::PlayerAttack)
	{
		auto playerAttack = collision.other->GetComponent<Attack>();

		if (playerAttack->ProcessAttack())
		{
			float attackPower = playerAttack->GetAttackPower();

			bool isGroggy = isGroggyState();

			// 그로기 상태에서는 hp가 더 많이 감소 
			if (isGroggy)
			{
				attackPower *= mGroggyDecreaseHpRatio;
			}
			mHp -= attackPower;

			// 피격 사운드 재생
			playerAttack->PlayHitSound();

			// 그로기 상태 처리
			if (!isGroggy)
			{
				mGroggyGauge = std::min(mGroggyGauge + mGroggyIncreaseRatio * attackPower, mStartGroggyGauge);
			}
			else
			{
				mAnimator->SetParameterBoolean("OnHit", true);

				// // HP 흔들기
				// GetScene()->ViewComponents<UIShaker>(
				// 	[this](fq::game_module::GameObject& object, UIShaker& uiShaker)
				// 	{
				// 		uiShaker.AddCount(mShakeCount);
				// 	});
			}

			// 그로기보다 화남을 우선함
			bool bIsEnteringAngry = mAnimator->GetController().GetParameter("IsEnterAngry").cast<bool>();

			if (mGroggyGauge == mStartGroggyGauge && !bIsEnteringAngry)
			{
				mAnimator->SetParameterBoolean("OnGroggy", true);
				mGroggyElapsed = 0.f;
			}

			// 사망처리
			if (mHp <= 0.f)
			{
				if (playerAttack->GetAttacker() != nullptr)
				{
					auto attackerID = playerAttack->GetAttacker()->GetComponent<Player>()->GetPlayerID();
					if (attackerID == 0)
					{
						PlayerInfoVariable::Player1Monster += 1;
						spdlog::trace("Player1Monster: {}", PlayerInfoVariable::Player1Monster);
					}
					if (attackerID == 1)
					{
						PlayerInfoVariable::Player2Monster += 1;
						spdlog::trace("Player1Monster: {}", PlayerInfoVariable::Player2Monster);
					}
					if (attackerID == 2)
					{
						PlayerInfoVariable::Player3Monster += 1;
						spdlog::trace("Player1Monster: {}", PlayerInfoVariable::Player3Monster);
					}
					if (attackerID == 3)
					{
						PlayerInfoVariable::Player4Monster += 1;
						spdlog::trace("Player1Monster: {}", PlayerInfoVariable::Player4Monster);
					}
				}

				mAnimator->SetParameterBoolean("IsDead", true);
				destroySocketCollider();
			}

			// 이펙트 방출
			fq::event::OnCreateStateEvent stateEvent;
			stateEvent.gameObject = GetGameObject();
			stateEvent.RegisterKeyName = playerAttack->GetAttackEffectEvent();
			if (!stateEvent.RegisterKeyName.empty())
			{
				GetGameObject()->GetScene()->GetEventManager()->FireEvent<fq::event::OnCreateStateEvent>(std::move(stateEvent));
			}
		}
	}
}

void fq::client::BossMonster::DetectTarget()
{
	auto monsterPos = mTransform->GetWorldPosition();

	for (const auto& player : mGameManager->GetPlayers())
	{
		if (player->GetComponent<Player>() == nullptr)
		{
			continue;
		}

		auto playerT = player->GetComponent<game_module::Transform>();
		auto playerPos = playerT->GetWorldPosition();

		float distance = (monsterPos - playerPos).Length();

		if (distance <= mDetectRange)
		{
			SetTarget(player.get());
		}
	}
}

void fq::client::BossMonster::SetRandomTarget()
{
	auto players = mGameManager->GetPlayers();

	players.erase(std::remove_if(players.begin(), players.end(), [](std::shared_ptr<game_module::GameObject>& player)
		{
			return player->GetTag() == game_module::ETag::Soul;
		}), players.end());

	if (!players.empty())
	{
		int size = players.size();
		int index = helper::RandomGenerator::GetInstance().GetRandomNumber(0, size - 1);
		SetTarget(players[index].get());
	}
	else
	{
		SetTarget(nullptr);
	}
}

void fq::client::BossMonster::SetTarget(game_module::GameObject* target)
{
	if (mAnimator == nullptr)
	{
		mAnimator = GetComponent<game_module::Animator>();
		assert(mAnimator);
	}

	if (target == nullptr)
	{
		mTarget = nullptr;
		mAnimator->SetParameterBoolean("HasTarget", false);
		mIsDummyTarget = false;
		return;
	}

	// 더미 타겟인지 체크
	mIsDummyTarget = target->GetComponent<PlayerDummy>() != nullptr;
	if (mIsDummyTarget)
	{
		mDummyTraceElapsedTime = 0.f;
		float random = helper::RandomGenerator::GetInstance().GetRandomNumber(mDummyDurationRandomRangeMin, mDummyDurationRandomRangeMax);
		mCurrentDummyTraceDurationTime = mDummyTraceDurationTime + random;
	}

	mTarget = target->shared_from_this();
	mAnimator->SetParameterBoolean("HasTarget", true);
}

void fq::client::BossMonster::Move(DirectX::SimpleMath::Vector3 destination)
{
	fq::game_module::NavigationAgent* agent = GetComponent<fq::game_module::NavigationAgent>();

	if (agent)
		agent->MoveTo(destination);
}

void fq::client::BossMonster::ChaseTarget()
{
	if (mTarget == nullptr)
	{
		SetTarget(nullptr);
		return;
	}
	if (mTarget->IsDestroyed())
	{
		auto playerOrNull = mTarget->GetComponent<Player>();

		if (playerOrNull == nullptr)
		{
			SetTarget(nullptr);
			return;
		}

		auto playerDummyOrNull = playerOrNull->CreateDummyOrNull();

		if (playerDummyOrNull == nullptr)
		{
			SetTarget(nullptr);
			return;
		}

		SetTarget(playerDummyOrNull);
	}

	auto targetPos = mTarget->GetTransform()->GetWorldPosition();
	Move(targetPos);
}

void fq::client::BossMonster::CheckTargetInAttackRange()
{
	if (mTarget == nullptr)
	{
		SetTarget(nullptr);
		mAnimator->SetParameterBoolean("InAttackRange", false);
		return;
	}
	// 사망 시 플레이어라면 더미 생성이 가능한지 체크한다.
	if (mTarget->IsDestroyed())
	{
		auto playerOrNull = mTarget->GetComponent<Player>();

		if (playerOrNull == nullptr)
		{
			SetTarget(nullptr);
			mAnimator->SetParameterBoolean("InAttackRange", false);
			return;
		}

		auto dummyOrNull = playerOrNull->CreateDummyOrNull();

		if (dummyOrNull == nullptr)
		{
			SetTarget(nullptr);
			mAnimator->SetParameterBoolean("InAttackRange", false);
			return;
		}

		SetTarget(dummyOrNull);
	}

	auto targetT = mTarget->GetComponent<game_module::Transform>();

	auto targetPos = targetT->GetWorldPosition();
	auto monsterPos = mTransform->GetWorldPosition();

	float distance = (targetPos - monsterPos).Length();
	bool isInAttackRange = mAttackRange >= distance;

	mAnimator->SetParameterBoolean("InAttackRange", isInAttackRange);
}

std::shared_ptr<fq::game_module::GameObject> fq::client::BossMonster::EmitRushAttack()
{
	using namespace game_module;

	if (mRushAttack.GetPrefabPath().empty())
	{
		return nullptr;
	}

	// 러쉬 히트 박스 생성	
	using namespace game_module;
	auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(mRushAttack);
	auto& attackObj = *(instance.begin());
	auto attackT = attackObj->GetComponent<Transform>();
	attackT->SetParent(mTransform);

	// 공격 정보 설정
	AttackInfo attackInfo{};
	auto attackComponent = attackObj->GetComponent<Attack>();
	attackInfo.attacker = GetGameObject();
	attackInfo.damage = dc::GetMonsterRushDamage(GetAttackPower());
	attackInfo.type = EKnockBackType::TargetPosition;
	attackInfo.knocBackPower = mRushKnockBackPower;
	attackComponent->Set(attackInfo);

	GetScene()->AddGameObject(attackObj);

	return attackObj;
}

void fq::client::BossMonster::EmitSmashDown()
{
	using namespace game_module;
	auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(mSmashDownAttack);
	auto& attackObj = *(instance.begin());
	auto attackT = attackObj->GetComponent<Transform>();

	// 공격 트랜스폼 설정
	auto attackPos = mTransform->GetWorldPosition();
	auto scale = attackT->GetWorldScale();
	auto rotation = mTransform->GetWorldRotation();

	auto rotationMat = DirectX::SimpleMath::Matrix::CreateFromQuaternion(rotation);
	auto foward = rotationMat.Forward();
	attackPos += foward * mSmashDownOffset;
	attackT->GenerateWorld(attackPos, rotation, scale);

	// 공격 정보 설정
	AttackInfo attackInfo{};
	auto attackComponent = attackObj->GetComponent<Attack>();

	attackInfo.attacker = GetGameObject();
	attackInfo.damage = dc::GetMonsterSmashDownDamage(GetAttackPower());
	attackInfo.attackDirection = foward;
	attackInfo.type = EKnockBackType::TargetPosition;
	attackInfo.knocBackPower = mSmashKnockBackPower;
	attackComponent->Set(attackInfo);

	GetScene()->AddGameObject(attackObj);
}

std::shared_ptr<fq::game_module::GameObject> fq::client::BossMonster::EmitSmashDownEffect()
{
	auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(mSmashDownEffect);
	auto& effectObj = *(instance.begin());

	auto effectT = effectObj->GetComponent<game_module::Transform>();

	// 공격 트랜스폼 설정
	auto attackPos = mTransform->GetWorldPosition();
	auto scale = effectT->GetWorldScale();
	auto rotation = mTransform->GetWorldRotation();

	auto rotationMat = DirectX::SimpleMath::Matrix::CreateFromQuaternion(rotation);
	auto foward = rotationMat.Forward();
	attackPos += foward * mSmashDownOffset;
	attackPos.y += mEffectYOffset;
	effectT->GenerateWorld(attackPos, rotation, scale);

	GetScene()->AddGameObject(effectObj);

	return effectObj;
}

void fq::client::BossMonster::HomingTarget(bool bUseSpeed)
{
	if (mTarget == nullptr)
	{
		SetTarget(nullptr);
		return;
	}
	if (mTarget->IsDestroyed())
	{
		auto playerOrNull = mTarget->GetComponent<Player>();

		if (playerOrNull == nullptr)
		{
			SetTarget(nullptr);
			return;
		}

		auto dummyOrNull = playerOrNull->CreateDummyOrNull();

		if (dummyOrNull == nullptr)
		{
			SetTarget(nullptr);
			return;
		}

		SetTarget(dummyOrNull);
	}

	auto targetT = mTarget->GetComponent<game_module::Transform>();

	auto targetPos = targetT->GetWorldPosition();
	auto myPos = mTransform->GetWorldPosition();
	auto currentRotation = mTransform->GetWorldRotation();

	auto directV = targetPos - myPos;
	directV.y = 0.f;
	directV.Normalize();

	DirectX::SimpleMath::Quaternion directionQuaternion = currentRotation;

	// UpVector가 뒤집히는 경우에 대한 예외 처리 
	if (directV.z >= 1.f)
		directionQuaternion = DirectX::SimpleMath::Quaternion::LookRotation({ 0.f,0.f,1.f }, { 0.f, -1.f, 0.f });
	else if (directV != DirectX::SimpleMath::Vector3::Zero)
		directionQuaternion = DirectX::SimpleMath::Quaternion::LookRotation(directV, { 0.f,1.f,0.f });
	directionQuaternion.Normalize();

	DirectX::SimpleMath::Quaternion result =
		DirectX::SimpleMath::Quaternion::Slerp(currentRotation, directionQuaternion, bUseSpeed ? mRotationSpeed : 1.f);

	mTransform->SetWorldRotation(result);
}

float fq::client::BossMonster::GetHPRatio() const
{
	return mHp / mMaxHp;
}

void fq::client::BossMonster::CreateHpBar()
{
	auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(mHpBarPrefab);
	mHpBar = *(instance.begin());
	mHpBar->GetComponent<BossHP>()->SetBoss(this);
	GetScene()->AddGameObject(mHpBar);
}

void fq::client::BossMonster::DestroryHpBar()
{
	if (mHpBar)
	{
		GetScene()->DestroyGameObject(mHpBar.get());
		mHpBar = nullptr;
	}
}

void fq::client::BossMonster::EmitComboAttack(float xAxisOffset)
{
	using namespace game_module;
	auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(mComboAttack);
	auto& attackObj = *(instance.begin());
	auto attackT = attackObj->GetComponent<Transform>();

	// 공격 트랜스폼 설정
	auto attackPos = mTransform->GetWorldPosition();
	auto scale = attackT->GetWorldScale();
	auto rotation = mTransform->GetWorldRotation();

	auto rotationMat = DirectX::SimpleMath::Matrix::CreateFromQuaternion(rotation);
	auto foward = rotationMat.Forward();
	auto right = rotationMat.Right();
	attackPos += foward * mComboAttackOffset;
	attackPos += right * xAxisOffset;
	attackT->GenerateWorld(attackPos, rotation, scale);

	// 공격 정보 설정
	AttackInfo attackInfo{};
	auto attackComponent = attackObj->GetComponent<Attack>();

	attackInfo.attacker = GetGameObject();
	attackInfo.damage = dc::GetMonsterComboAttackDamage(GetAttackPower());
	attackInfo.attackDirection = foward;
	attackInfo.type = EKnockBackType::TargetPosition;
	attackInfo.knocBackPower = mComboAttackKnockBackPower;
	attackComponent->Set(attackInfo);

	GetScene()->AddGameObject(attackObj);
}

void fq::client::BossMonster::ReboundComboAttack()
{
	auto look = mTransform->GetLookAtVector();
	mKnockBack->SetKnockBack(mComboAttackReboundPower, look);
}

void fq::client::BossMonster::setNextPattern()
{
	EBossMonsterAttackType parttern = getNextPattern(true);

	// 갑옷 먹기 패턴의 경우에는 먹을 수 있는 갑옷이 있으면 설정합니다
	if (parttern == EBossMonsterAttackType::Eat)
	{
		bool isFind = FindAndSetTargetDeadArmour();
		if (!isFind)
		{
			parttern = getNextPattern(false);
		}
	}

	// 콤보 공격의 2, 3타를 결정합니다. 
	if (parttern == EBossMonsterAttackType::Combo)
	{
		float ratio = helper::RandomGenerator::GetInstance().GetRandomNumber(0.f, 1.f);
		mAnimator->SetParameterBoolean("OnCombo3", ratio > mSecondComboAttackRatio);
	}

	mAnimator->SetParameterInt("AttackType", static_cast<int>(parttern));
}

void fq::client::BossMonster::AddHp(float hp)
{
	mHp = std::min(mMaxHp, mHp + hp);
}

bool fq::client::BossMonster::FindAndSetTargetDeadArmour()
{
	auto view = GetScene()->GetComponentView<DeadArmour>();

	if (view.begin() == view.end())
	{
		return false;
	}

	// 가장 가까운 갑옷 찾기
	auto bossPosition = mTransform->GetWorldPosition();
	float minDistance = FLT_MAX;
	game_module::GameObject* armour = nullptr;

	for (auto& iter : view)
	{
		if (iter.IsDestroyed())
		{
			continue;
		}

		auto deadArmour = iter.GetComponent<DeadArmour>();
		auto armourT = iter.GetComponent<game_module::Transform>();
		auto armourPos = armourT->GetWorldPosition();

		float distance = (armourPos - bossPosition).Length();

		if (minDistance > distance)
		{
			armour = &iter;
			minDistance = distance;
		}
	}

	// 갑옷을 타겟으로 설정
	SetTarget(armour);

	return true;
}

void fq::client::BossMonster::EndPattern()
{
	SetRandomTarget();
	setNextPattern();
	waitAttack();
}

void fq::client::BossMonster::SetRimLightColor(bool bUseRimLight, DirectX::SimpleMath::Color color, float rimPow) const
{
	auto info = mSkinnedMesh->GetMaterialInstanceInfo();

	info.bUseRimLight = bUseRimLight;
	info.RimLightColor = color;
	info.RimPow = rimPow; // 임시로 넣음

	mSkinnedMesh->SetMaterialInstanceInfo(info);
}

void fq::client::BossMonster::SetInvRimLightColor(bool bUseInvRimLight, DirectX::SimpleMath::Color color, float invRimPow) const
{
	auto info = mSkinnedMesh->GetMaterialInstanceInfo();

	info.bUseInvRimLight = bUseInvRimLight;
	info.InvRimLightColor = color;
	info.InvRimPow = invRimPow; // 임시로 넣음

	mSkinnedMesh->SetMaterialInstanceInfo(info);
}

fq::client::EBossMonsterAttackType fq::client::BossMonster::getNextPattern(bool bIsInludeEat) const
{
	float val = helper::RandomGenerator::GetInstance().GetRandomNumber(0.f, 1.f);
	float sum = 0.f;

	// // 러쉬 패턴
	// sum += mRushProbability;
	// if (val <= sum)
	// {
	// 	return EBossMonsterAttackType::Rush;
	// }

	sum += mSmashProbability;
	// 내려찍기 
	if (val <= sum)
	{
		return EBossMonsterAttackType::SmashDown;
	}

	sum += mRoarProbability;
	// 로어 
	if (val <= sum && IsAngry() && canRoar())
	{
		return EBossMonsterAttackType::Roar;
	}

	// 연속공격 패턴 
	sum += mContinousProbability;
	if (val <= sum)
	{
		return EBossMonsterAttackType::Continous;
	}

	// 먹기 패턴
	sum += mEatProbability;
	if (val <= sum && bIsInludeEat)
	{
		return EBossMonsterAttackType::Eat;
	}

	return EBossMonsterAttackType::Combo;
}

void fq::client::BossMonster::SetOutLineColor(DirectX::SimpleMath::Color color)
{
	auto info = mSkinnedMesh->GetMaterialInstanceInfo();

	info.RimLightColor = color;

	mSkinnedMesh->SetMaterialInstanceInfo(info);
}

void fq::client::BossMonster::StepBack()
{
	auto back = -mTransform->GetLookAtVector();
	mKnockBack->SetKnockBack(mComboAttackReboundPower * 0.5f, back);
}

float fq::client::BossMonster::GetGroggyGaugeRatio() const
{
	return mGroggyGauge / mStartGroggyGauge;
}

float fq::client::BossMonster::GetAttackPower() const
{
	if (IsAngry())
	{
		return mAttackPower * mAngryAttackPowerRatio;
	}

	return mAttackPower;
}

bool fq::client::BossMonster::isGroggyState() const
{
	return mAnimator->GetController().GetParameter("OnGroggy").cast<bool>();
}

void fq::client::BossMonster::HitArrow(fq::game_module::GameObject* object)
{
	if (mArrowHitDuration < mArrowImotalTime)
		return;

	auto playerAttack = object->GetComponent<Attack>();

	if (playerAttack->ProcessAttack())
	{
		mArrowHitDuration = 0.f;
		float attackPower = playerAttack->GetAttackPower();

		// HP 감소
		mHp -= attackPower;

		// 피격 사운드 재생
		playerAttack->PlayHitSound();

		// 그로기 
		if (!isGroggyState())
		{
			mGroggyGauge = std::min(mGroggyGauge + mGroggyIncreaseRatio * attackPower, mStartGroggyGauge);
		}

		if (mGroggyGauge == mStartGroggyGauge)
		{
			mGroggyGauge = 0.f;
			mAnimator->SetParameterBoolean("OnGroggy", true);
		}

		// 사망처리
		if (mHp <= 0.f)
		{
			if (playerAttack->GetAttacker() != nullptr)
			{
				auto attackerID = playerAttack->GetAttacker()->GetComponent<Player>()->GetPlayerID();
				if (attackerID == 0)
				{
					PlayerInfoVariable::Player1Monster += 1;
					spdlog::trace("Player1Monster: {}", PlayerInfoVariable::Player1Monster);
				}
				if (attackerID == 1)
				{
					PlayerInfoVariable::Player2Monster += 1;
					spdlog::trace("Player1Monster: {}", PlayerInfoVariable::Player2Monster);
				}
				if (attackerID == 2)
				{
					PlayerInfoVariable::Player3Monster += 1;
					spdlog::trace("Player1Monster: {}", PlayerInfoVariable::Player3Monster);
				}
				if (attackerID == 3)
				{
					PlayerInfoVariable::Player4Monster += 1;
					spdlog::trace("Player1Monster: {}", PlayerInfoVariable::Player4Monster);
				}
			}

			mAnimator->SetParameterBoolean("IsDead", true);
			destroySocketCollider();
		}

		// 이펙트 방출
		fq::event::OnCreateStateEvent stateEvent;
		stateEvent.gameObject = GetGameObject();
		stateEvent.RegisterKeyName = playerAttack->GetAttackEffectEvent();
		if (!stateEvent.RegisterKeyName.empty())
		{
			GetGameObject()->GetScene()->GetEventManager()->FireEvent<fq::event::OnCreateStateEvent>(std::move(stateEvent));
		}
	}
}

bool fq::client::BossMonster::IsAngry() const
{
	return GetHPRatio() < mAngryRatio || mbEnterAngryState;
}

void fq::client::BossMonster::ResetJumpCoolTime()
{
	mJumpElapsed = 0.f;
}

void fq::client::BossMonster::ResetRourCoolTime()
{
	mRoarElapsed = 0.f;
}

void fq::client::BossMonster::destroySocketCollider()
{
	// 소켓을 가지고 있는 자식 오브젝트의 박스 콜라이더 삭제
	for (auto childObject : GetGameObject()->GetChildren())
	{
		auto socket = childObject->GetComponent<fq::game_module::Socket>();

		if (socket)
			childObject->RemoveComponent<fq::game_module::BoxCollider>();
	}
}

void fq::client::BossMonster::updateGroggy(float dt)
{
	bool isGroggy = isGroggyState();


	if (isGroggy)
	{
		// 그로기 게이지 갱신
		float ratio = 1.f - (mGroggyElapsed / mGroggyDuration);
		mGroggyGauge = ratio * mStartGroggyGauge;

		// 그로기 누적 시간 갱신
		mGroggyElapsed += dt;

		if (mGroggyElapsed > mGroggyDuration)
		{
			mAnimator->SetParameterBoolean("OnGroggy", false);
		}
	}
	else
	{
		bool bIsEnteringAngry = mAnimator->GetController().GetParameter("IsEnterAngry").cast<bool>();
		mGroggyGauge -= (IsAngry() ? mAngryGroggyDecreasePerSecond : mGroggyDecreasePerSecond) * dt * (bIsEnteringAngry ? 10 : 1);
		mGroggyGauge = std::max<float>(mGroggyGauge, 0);
	}

	// hp바 누적 이펙트
	if (mHpBar)
	{
		mHpBar->GetComponent<BossHP>()->SetUseDecreaseEffet(isGroggy);
	}
}

void fq::client::BossMonster::updateJump(float dt)
{
	if (!IsAngry())
	{
		return;
	}

	mJumpElapsed += dt;

	if (mJumpElapsed < mJumpCoolTime)
	{
		return;
	}
	if (mTarget == nullptr)
	{
		return;
	}
	if (mTarget->GetComponent<Player>() == nullptr
		&& mTarget->GetComponent<PlayerDummy>() == nullptr)
	{
		return;
	}

	// 점프 중일 때 예외 처리
	if (canJump(mTarget->GetTransform()))
	{
		mAnimator->SetParameterBoolean("OnJumpWaiting", true);
		mAnimator->SetParameterInt("AttackType", static_cast<int>(EBossMonsterAttackType::Jump));
	}
}

bool fq::client::BossMonster::canJump(const fq::game_module::Transform* transform) const
{
	assert(transform != nullptr);
	if (transform == nullptr)
	{
		return false;
	}

	const auto& targetWorldPosition = transform->GetWorldPosition();
	const auto& bossWorldPosition = mTransform->GetWorldPosition();

	float distanceSquared = DirectX::SimpleMath::Vector3::DistanceSquared(targetWorldPosition, bossWorldPosition);
	float jumpDistanceSquared = mJumpDistance * mJumpDistance;

	return distanceSquared > jumpDistanceSquared;
}

bool fq::client::BossMonster::canRoar() const
{
	if (mRoarElapsed < mRoarCoolTime)
	{
		return false;
	}

	unsigned int monsterCount = 0;

	GetScene()->ViewComponents<MeleeMonster>([&monsterCount](fq::game_module::GameObject& object, MeleeMonster& monster) { ++monsterCount; });
	GetScene()->ViewComponents<PlantMonster>([&monsterCount](fq::game_module::GameObject& object, PlantMonster& monster) { ++monsterCount; });

	if (mMinMonsterCount < monsterCount)
	{
		return false;
	}

	return true;
}

float fq::client::BossMonster::GetAngryPreAttackSpeedRatio() const
{
	return mAngryPreAttackSpeedRatio;
}

float fq::client::BossMonster::GetAngryAttackSpeedRatio() const
{
	return mAngryAttackSpeedRatio;
}

float fq::client::BossMonster::GetAttackCoolTime() const
{
	return mAttackCoolTime;
}

void fq::client::BossMonster::GenerateAngryEnteringVigilantCount()
{
	mAngryEnteringVigilantCount = helper::RandomGenerator::GetInstance().GetRandomNumber((int)mAngryEnteringVigilantMinCount, (int)mAngryEnteringVigilantMaxCount);
}

void fq::client::BossMonster::GenerateEnteringVigilantCount()
{
	mVigilantCount = helper::RandomGenerator::GetInstance().GetRandomNumber((int)mVigilantMinCount, (int)mVigilantMaxCount);
}

unsigned int fq::client::BossMonster::GetAngryEnteringVigilantCount() const
{
	return mAngryEnteringVigilantCount;
}

unsigned int fq::client::BossMonster::GetEnteringVigilantCount() const
{
	return mVigilantCount;
}
