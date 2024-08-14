#include "BossMonster.h"

#include "../FQGameModule/NavigationAgent.h"
#include "../FQGameModule/Transform.h"
#include "../FQGameModule/Animator.h"
#include "Attack.h"
#include "GameManager.h"
#include "DamageCalculation.h"
#include "KnockBack.h"
#include "BossHP.h"
#include "ClientHelper.h"
#include "MonsterDefine.h"

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
{

}

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

	mMaxHp = mHp;

	auto agent = GetComponent<game_module::NavigationAgent>();
	agent->SetSpeed(mMoveSpeed);
	agent->SetAcceleration(mAcceleration);
	agent->SetRadius(1.f);

	// GameManager ����
	mGameManager = GetScene()->GetObjectByName("GameManager")->GetComponent<GameManager>();
}

void fq::client::BossMonster::OnUpdate(float dt)
{

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

			// ���ó��
			if (mHp <= 0.f)
			{
				mAnimator->SetParameterBoolean("IsDead", true);
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
	const auto& players = mGameManager->GetPlayers();

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
	if (target == nullptr)
	{
		mTarget = nullptr;
		mAnimator->SetParameterBoolean("HasTarget", false);
		return;
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
	if (mTarget == nullptr || mTarget->IsDestroyed())
	{
		SetTarget(nullptr);
		return;
	}

	auto targetPos = mTarget->GetTransform()->GetWorldPosition();
	Move(targetPos);
}

void fq::client::BossMonster::CheckTargetInAttackRange()
{
	if (mTarget == nullptr || mTarget->IsDestroyed())
	{
		SetTarget(nullptr);
		mAnimator->SetParameterBoolean("InAttackRange", false);
		return;
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

	// �ٶ󺸴� �������� �����մϴ� 
	auto look = mTransform->GetLookAtVector();
	mKnockBack->Set(mRushPower, look);

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
	attackComponent->Set(attackInfo);

	GetScene()->AddGameObject(attackObj);
}

std::shared_ptr<fq::game_module::GameObject> fq::client::BossMonster::EmitSmashDownEffect()
{
	auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(mSmashDownEffect);
	auto& effectObj = *(instance.begin());

	auto effectT = effectObj->GetComponent<game_module::Transform>();
	effectT->SetParent(mTransform);

	GetScene()->AddGameObject(effectObj);

	return effectObj;
}

void fq::client::BossMonster::HomingTarget()
{
	if (mTarget == nullptr || mTarget->IsDestroyed())
	{
		SetTarget(nullptr);
		return;
	}

	auto targetT = mTarget->GetComponent<game_module::Transform>();

	auto targetPos = targetT->GetWorldPosition();
	auto myPos = mTransform->GetWorldPosition();

	auto directV = targetPos - myPos;
	directV.y = 0.f;
	directV.Normalize();

	auto currentRotation = mTransform->GetWorldRotation();
	DirectX::SimpleMath::Quaternion directionQuaternion = DirectX::SimpleMath::Quaternion::LookRotation(directV, { 0, 1, 0 });
	directionQuaternion.Normalize();
	DirectX::SimpleMath::Quaternion result =
		DirectX::SimpleMath::Quaternion::Slerp(currentRotation, directionQuaternion, mRotationSpeed);

	DirectX::SimpleMath::Matrix rotationMatrix = DirectX::SimpleMath::Matrix::CreateFromQuaternion(result);

	// UpVector�� ������ ���
	if (rotationMatrix._22 <= -0.9f)
	{
		rotationMatrix._22 = 1.f;
	}
	mTransform->SetLocalRotationToMatrix(rotationMatrix);
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

void fq::client::BossMonster::EmitComboAttack()
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
	attackPos += foward * mComboAttackOffset;
	attackT->GenerateWorld(attackPos, rotation, scale);

	// ���� ���� ����
	AttackInfo attackInfo{};
	auto attackComponent = attackObj->GetComponent<Attack>();

	attackInfo.attacker = GetGameObject();
	attackInfo.damage = dc::GetMonsterComboAttackDamage(mAttackPower);
	attackInfo.attackDirection = foward;
	attackComponent->Set(attackInfo);

	GetScene()->AddGameObject(attackObj);
}

void fq::client::BossMonster::ReboundComboAttack()
{
	auto look = mTransform->GetLookAtVector();
	mKnockBack->Set(mComboAttackReboundPower, look);
}

void fq::client::BossMonster::SetNextAttack()
{
	int index = helper::RandomGenerator::GetInstance()
		.GetRandomNumber(0, static_cast<int>(EBossMonsterAttackType::Combo));

	mAnimator->SetParameterInt("AttackType", index);
}
