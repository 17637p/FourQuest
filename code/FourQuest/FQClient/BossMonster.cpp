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

#include <spdlog/spdlog.h>

fq::client::BossMonster::BossMonster()
	:mMaxHp(0.f)
	, mHp(2000.f)
	, mAttackPower(50.f)
	, mMoveSpeed(1.f)
	, mAcceleration(2.f)
	, mAttackRange(10.f)
	, mAttackCoolTime(3.f)
	, mAttackElapsedTime(0.f)
	, mDetectRange(5.f)
	, mRushPower(10.f)
	, mComboAttackReboundPower(5.f)
	, mRotationSpeed(0.1f)
	, mGameManager(nullptr)
	, mAnimator(nullptr)
	, mTarget(nullptr)
	, mKnockBack(nullptr)
	, mTransform(nullptr)
	, mGroggyGauge(0.f)
	, mStartGroggyGauge(100.f)
	, mGroggyIncreaseRatio(0.1f)
	, mGroggyDecreasePerSecond(1.f)
	, mSecondComboAttackRatio(0.5f)
	, mComboAttackOffset{}
	, mSmashDownOffset{}
	, mMinWaitAttackTime(0.5f)
	, mMaxWaitAttackTime(2.f)
	, mContinousProbability(0.1f)
	, mRoarProbability(0.1f)
	, mEatProbability(0.1f)
	, mRushProbability(0.1f)
	, mSmashProbability(0.2f)
	, mRushKnockBackPower(5.f)
	, mSmashKnockBackPower(3.f)
	, mComboAttackKnockBackPower(3.f)
	, mContinousKnockBackPower(3.f)
	, mDummyTraceDurationTime(0.f)
	, mbUseDummyTraceRandomRange(false)
	, mDummyDurationRandomRangeMin(0.f)
	, mDummyDurationRandomRangeMax(0.f)
	, mCurrentDummyTraceDurationTime(0.f)
	, mDummyTraceElapsedTime(0.f)
	, mIsDummyTarget(false)
	, mGroggyDuration(5.f)
	, mGroggyElapsed(0.f)
	, mShakeCount(10)
	, mRimPow(3.f)
{}

fq::client::BossMonster::~BossMonster()
{

}

std::shared_ptr<fq::game_module::Component> fq::client::BossMonster::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	std::shared_ptr<BossMonster> cloneMonster = std::dynamic_pointer_cast<BossMonster>(clone);

	if (cloneMonster == nullptr) // ���� �����ؼ� ���纻�� �ش�
	{
		cloneMonster = game_module::ObjectPool::GetInstance()->Assign<BossMonster>(*this);
	}
	else // clone�� �����͸� �����Ѵ�.
	{
		// �⺻ ���� ������ ȣ���Ѵ�.
		*cloneMonster = *this;
	}

	return cloneMonster;
}

void fq::client::BossMonster::OnStart()
{
	mTransform = GetComponent<game_module::Transform>();
	mAnimator = GetComponent<game_module::Animator>();
	mKnockBack = GetComponent<KnockBack>();

	// ���̵��� ���� ���ݷ� Hp ����
	mAttackPower = mAttackPower * LevelHepler::GetDamageRatio();
	mHp = mHp * LevelHepler::GetHpRatio();
	mMaxHp = mHp;

	auto agent = GetComponent<game_module::NavigationAgent>();
	agent->SetSpeed(mMoveSpeed);
	agent->SetAcceleration(mAcceleration);
	agent->SetRadius(1.f);

	// GameManager ����
	mGameManager = GetScene()->GetObjectByName("GameManager")->GetComponent<GameManager>();

	// SkinnedMesh ����
	mSkinnedMesh = mTransform->GetChildren()[0]->GetComponent<game_module::SkinnedMeshRenderer>();
}

void fq::client::BossMonster::OnUpdate(float dt)
{
	// �׷α� ������ 
	mGroggyGauge -= mGroggyDecreasePerSecond * dt;
	mGroggyGauge = std::max<float>(mGroggyGauge, 0);

	// ���� ��Ÿ�� ��� 
	mAttackElapsedTime = std::min(mAttackCoolTime, mAttackElapsedTime + dt);

	bool canAttack = mAttackElapsedTime >= mAttackCoolTime;
	mAnimator->SetParameterBoolean("CanAttack", canAttack);

	updateGroggy(dt);
}


void fq::client::BossMonster::waitAttack()
{
	mAttackCoolTime = helper::RandomGenerator::GetInstance().GetRandomNumber(mMinWaitAttackTime, mMaxWaitAttackTime);
	mAttackElapsedTime = 0.f;
	mAnimator->SetParameterBoolean("CanAttack", false);
}

void fq::client::BossMonster::OnTriggerEnter(const game_module::Collision& collision)
{
	// �ǰ�ó��
	if (collision.other->GetTag() == game_module::ETag::PlayerAttack)
	{
		auto playerAttack = collision.other->GetComponent<Attack>();

		if (playerAttack->ProcessAttack())
		{
			float attackPower = playerAttack->GetAttackPower();

			// HP ����
			mHp -= attackPower;

			// �ǰ� ���� ���
			playerAttack->PlayHitSound();

			// �׷α� ���� ó��
			if (!isGroggyState())
			{
				mGroggyGauge = std::min(mGroggyGauge + mGroggyIncreaseRatio * attackPower, mStartGroggyGauge);
			}
			else
			{
				// �ٴ��� ������ { 0, 1, 0}���� ������
				DirectX::SimpleMath::Vector3 groundNormal = { 0, 1, 0 };
				auto currLook = GetTransform()->GetLookAtVector();
				currLook.y = 0;
				currLook.Normalize();
				auto objectLook = collision.other->GetTransform()->GetWorldPosition() - GetTransform()->GetWorldPosition();
				objectLook.y = 0;
				objectLook.Normalize();
				auto crossVec = currLook.Cross(objectLook);
				bool isLeft = crossVec.Dot(groundNormal) >= 0;

				// �յ� ����
				float dotProduct = currLook.Dot(objectLook);
				float angle = acosf(dotProduct); // �� ���� ������ ��

				if (angle < DirectX::XM_PIDIV4)
				{
					mAnimator->SetParameterBoolean("OnHitFront", true);
				}
				else if (angle < DirectX::XM_PIDIV4 + DirectX::XM_PIDIV2)
				{
					if (isLeft)
					{
						mAnimator->SetParameterBoolean("OnHitLeft", true);
					}
					else
					{
						mAnimator->SetParameterBoolean("OnHitRight", true);
					}
				}
				else
				{
					mAnimator->SetParameterBoolean("OnHitBack", true);
				}

				// // HP ����
				// GetScene()->ViewComponents<UIShaker>(
				// 	[this](fq::game_module::GameObject& object, UIShaker& uiShaker)
				// 	{
				// 		uiShaker.AddCount(mShakeCount);
				// 	});
			}

			if (mGroggyGauge == mStartGroggyGauge)
			{
				mGroggyGauge = 0.f;
				mAnimator->SetParameterBoolean("OnGroggy", true);
				mGroggyElapsed = 0.f;
			}

			// ���ó��
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

			// ����Ʈ ����
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
		auto playerT = player->GetComponent<game_module::Transform>();
		auto playerPos = playerT->GetWorldPosition();

		float distance = (monsterPos - playerPos).Length();

		if (distance <= mDetectRange)
		{
			SetTarget(player.get());
			mAnimator->SetParameterBoolean("FindTarget", true);
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

	// ���� Ÿ������ üũ
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
	// ��� �� �÷��̾��� ���� ������ �������� üũ�Ѵ�.
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

std::shared_ptr<fq::game_module::GameObject> fq::client::BossMonster::Rush()
{
	using namespace game_module;

	// ���� ��Ʈ �ڽ� ����	
	using namespace game_module;
	auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(mRushAttack);
	auto& attackObj = *(instance.begin());
	auto attackT = attackObj->GetComponent<Transform>();
	attackT->SetParent(mTransform);

	// ���� ���� ����
	AttackInfo attackInfo{};
	auto attackComponent = attackObj->GetComponent<Attack>();
	attackInfo.attacker = GetGameObject();
	attackInfo.damage = dc::GetMonsterRushDamage(mAttackPower);
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

	// ���� Ʈ������ ����
	auto attackPos = mTransform->GetWorldPosition();
	auto scale = attackT->GetWorldScale();
	auto rotation = mTransform->GetWorldRotation();

	auto rotationMat = DirectX::SimpleMath::Matrix::CreateFromQuaternion(rotation);
	auto foward = rotationMat.Forward();
	attackPos += foward * mSmashDownOffset;
	attackT->GenerateWorld(attackPos, rotation, scale);

	// ���� ���� ����
	AttackInfo attackInfo{};
	auto attackComponent = attackObj->GetComponent<Attack>();

	attackInfo.attacker = GetGameObject();
	attackInfo.damage = dc::GetMonsterSmashDownDamage(mAttackPower);
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

	// ���� Ʈ������ ����
	auto attackPos = mTransform->GetWorldPosition();
	auto scale = effectT->GetWorldScale();
	auto rotation = mTransform->GetWorldRotation();

	auto rotationMat = DirectX::SimpleMath::Matrix::CreateFromQuaternion(rotation);
	auto foward = rotationMat.Forward();
	attackPos += foward * mSmashDownOffset;
	effectT->GenerateWorld(attackPos, rotation, scale);

	GetScene()->AddGameObject(effectObj);

	return effectObj;
}

void fq::client::BossMonster::HomingTarget()
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

	// UpVector�� �������� ��쿡 ���� ���� ó�� 
	if (directV.z >= 1.f)
		directionQuaternion = DirectX::SimpleMath::Quaternion::LookRotation({ 0.f,0.f,1.f }, { 0.f, -1.f, 0.f });
	else if (directV != DirectX::SimpleMath::Vector3::Zero)
		directionQuaternion = DirectX::SimpleMath::Quaternion::LookRotation(directV, { 0.f,1.f,0.f });
	directionQuaternion.Normalize();

	DirectX::SimpleMath::Quaternion result =
		DirectX::SimpleMath::Quaternion::Slerp(currentRotation, directionQuaternion, mRotationSpeed);

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

	// ���� Ʈ������ ����
	auto attackPos = mTransform->GetWorldPosition();
	auto scale = attackT->GetWorldScale();
	auto rotation = mTransform->GetWorldRotation();

	auto rotationMat = DirectX::SimpleMath::Matrix::CreateFromQuaternion(rotation);
	auto foward = rotationMat.Forward();
	auto right = rotationMat.Right();
	attackPos += foward * mComboAttackOffset;
	attackPos += right * xAxisOffset;
	attackT->GenerateWorld(attackPos, rotation, scale);

	// ���� ���� ����
	AttackInfo attackInfo{};
	auto attackComponent = attackObj->GetComponent<Attack>();

	attackInfo.attacker = GetGameObject();
	attackInfo.damage = dc::GetMonsterComboAttackDamage(mAttackPower);
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

	// ���� �Ա� ������ ��쿡�� ���� �� �ִ� ������ ������ �����մϴ�
	if (parttern == EBossMonsterAttackType::Eat)
	{
		bool isFind = FindAndSetTargetDeadArmour();
		if (!isFind)
		{
			parttern = getNextPattern(false);
		}
	}

	// �޺� ������ 2, 3Ÿ�� �����մϴ�. 
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

	// ���� ����� ���� ã��
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

	// ������ Ÿ������ ����
	SetTarget(armour);

	return true;
}

void fq::client::BossMonster::EndPattern()
{
	SetRandomTarget();
	setNextPattern();
	waitAttack();
}


void fq::client::BossMonster::SetRimLightColor(bool bUseRimLight, DirectX::SimpleMath::Color color) const
{
	auto info = mSkinnedMesh->GetMaterialInstanceInfo();

	info.bUseRimLight = bUseRimLight;
	info.RimLightColor = color;
	info.RimPow = mRimPow; // �ӽ÷� ����

	mSkinnedMesh->SetMaterialInstanceInfo(info);
}

fq::client::EBossMonsterAttackType fq::client::BossMonster::getNextPattern(bool bIsInludeEat) const
{
	float val = helper::RandomGenerator::GetInstance().GetRandomNumber(0.f, 1.f);
	float sum = 0.f;

	// ���� ����
	sum += mRushProbability;
	if (val <= sum)
	{
		return EBossMonsterAttackType::Rush;
	}

	sum += mSmashProbability;
	// ������� 
	if (val <= sum)
	{
		return EBossMonsterAttackType::SmashDown;
	}

	sum += mRoarProbability;
	// �ξ� 
	if (val <= sum)
	{
		return EBossMonsterAttackType::Roar;
	}

	// ���Ӱ��� ���� 
	sum += mContinousProbability;
	if (val <= sum)
	{
		return EBossMonsterAttackType::Continous;
	}

	// �Ա� ����
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

bool fq::client::BossMonster::isGroggyState() const
{
	return mAnimator->GetController().GetParameter("OnGroggy").cast<bool>();
}

void fq::client::BossMonster::HitArrow(fq::game_module::GameObject* object)
{
	auto playerAttack = object->GetComponent<Attack>();

	// �̹� �������� ���� ������Ʈ��� �Լ� ����
	auto attackObject = mArrowAttackObject.find(object->GetID());
	if (attackObject == mArrowAttackObject.end())
	{
		mArrowAttackObject.insert(object->GetID());
	}
	else
	{
		return;
	}
	if (playerAttack->ProcessAttack())
	{
		float attackPower = playerAttack->GetAttackPower();

		// HP ����
		mHp -= attackPower;

		// �ǰ� ���� ���
		playerAttack->PlayHitSound();

		// �׷α� 
		if (!isGroggyState())
		{
			mGroggyGauge = std::min(mGroggyGauge + mGroggyIncreaseRatio * attackPower, mStartGroggyGauge);
		}

		if (mGroggyGauge == mStartGroggyGauge)
		{
			mGroggyGauge = 0.f;
			mAnimator->SetParameterBoolean("OnGroggy", true);
		}

		// ���ó��
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

		// ����Ʈ ����
		fq::event::OnCreateStateEvent stateEvent;
		stateEvent.gameObject = GetGameObject();
		stateEvent.RegisterKeyName = playerAttack->GetAttackEffectEvent();
		if (!stateEvent.RegisterKeyName.empty())
		{
			GetGameObject()->GetScene()->GetEventManager()->FireEvent<fq::event::OnCreateStateEvent>(std::move(stateEvent));
		}
	}
}

void fq::client::BossMonster::destroySocketCollider()
{
	// ������ ������ �ִ� �ڽ� ������Ʈ�� �ڽ� �ݶ��̴� ����
	for (auto childObject : GetGameObject()->GetChildren())
	{
		auto socket = childObject->GetComponent<fq::game_module::Socket>();

		if (socket)
			childObject->RemoveComponent<fq::game_module::BoxCollider>();
	}
}

void fq::client::BossMonster::updateGroggy(float dt)
{
	if (!isGroggyState())
	mGroggyElapsed += dt;

	// todo : groggy ���� ���� �ɹ��� �����ϱ�
	if (mGroggyElapsed > mGroggyDuration)
	{
		mAnimator->SetParameterBoolean("OnGroggy", false);
	}
}
