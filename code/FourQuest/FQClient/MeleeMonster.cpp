#define NOMINMAX
#include "MeleeMonster.h"

#include <random>
#include <algorithm>

#include "Attack.h"
#include "GameManager.h"
#include "HpBar.h"
#include "MonsterGroup.h"

fq::client::MeleeMonster::MeleeMonster()
	:mMaxHp(0.f)
	, mHp(100.f)
	, mAttackPower(10.f)
	, mMoveSpeed(1.f)
	, mAcceleration(1.f)
	, mAttackRange(10.f)
	, mPatrolRange(1.f)
	, mDetectRange(3.f)
	, mAttackCoolTime(3.f)
	, mAttackElapsedTime(0.f)
	, mGameManager(nullptr)
	, mAnimator(nullptr)
	, mTarget(nullptr)
	, mStartPosition{}
	, mPatrolDestination{}
	, mTransform(nullptr)
	, mAttackOffset(1.f)
{}

fq::client::MeleeMonster::~MeleeMonster()
{}

std::shared_ptr<fq::game_module::Component> fq::client::MeleeMonster::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	std::shared_ptr<MeleeMonster> cloneMonster = std::dynamic_pointer_cast<MeleeMonster>(clone);

	if (cloneMonster == nullptr) // ���� �����ؼ� ���纻�� �ش�
	{
		cloneMonster = game_module::ObjectPool::GetInstance()->Assign<MeleeMonster>(*this);
	}
	else // clone�� �����͸� �����Ѵ�.
	{
		// �⺻ ���� ������ ȣ���Ѵ�.
		*cloneMonster = *this;
	}

	return cloneMonster;
}

void fq::client::MeleeMonster::SetTarget(game_module::GameObject* target)
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

void fq::client::MeleeMonster::OnStart()
{
	mTransform = GetComponent<game_module::Transform>();
	mStartPosition = mTransform->GetWorldPosition();
	mAnimator = GetComponent<game_module::Animator>();

	mMaxHp = mHp;

	// Agent ����
	auto agent = GetComponent<game_module::NavigationAgent>();
	agent->SetSpeed(mMoveSpeed);
	agent->SetAcceleration(mAcceleration);
	agent->SetRadius(0.3f);

	// GameManager ����
	mGameManager = GetScene()->GetObjectByName("GameManager")->GetComponent<GameManager>();
}

void fq::client::MeleeMonster::EmitAttack()
{
	using namespace game_module;

	auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(mAttackPrefab);
	auto& attackObj = *(instance.begin());

	auto attackT = attackObj->GetComponent<Transform>();

	// ���� ����
	auto attackComponent = attackObj->GetComponent<client::Attack>();
	attackComponent->SetAttacker(GetGameObject());
	attackComponent->SetAttackPower(mAttackPower);

	auto attackPos = mTransform->GetWorldPosition();
	auto scale = attackT->GetWorldScale();
	auto rotation = mTransform->GetWorldRotation();

	auto rotationMat = DirectX::SimpleMath::Matrix::CreateFromQuaternion(rotation);
	auto foward = rotationMat.Forward();
	attackPos += foward * mAttackOffset;

	attackT->GenerateWorld(attackPos, rotation, scale);

	GetScene()->AddGameObject(attackObj);

	// ���� ��Ÿ�� ����ó��
	mAttackElapsedTime = mAttackCoolTime;

	// TODO : ���� ���� ���ݻ��� �߰� 
}


void fq::client::MeleeMonster::Move(DirectX::SimpleMath::Vector3 destination)
{
	fq::game_module::NavigationAgent* agent = GetComponent<fq::game_module::NavigationAgent>();
	agent->MoveTo(destination);
}

void fq::client::MeleeMonster::Patrol()
{
	std::random_device rd;
	std::mt19937 gen(rd());

	// ���� ��ġ ���ϱ� 
	std::uniform_real_distribution<float> patrolDis(0, mPatrolRange);
	float distance = patrolDis(gen);

	std::uniform_real_distribution<float> radianDis(0, DirectX::XM_2PI);
	float radian = radianDis(gen);

	mPatrolDestination = mStartPosition;
	mPatrolDestination.x += std::cos(radian) * distance;
	mPatrolDestination.z += std::sin(radian) * distance;

	fq::game_module::NavigationAgent* agent = GetComponent<fq::game_module::NavigationAgent>();

	Move(mPatrolDestination);
}

void fq::client::MeleeMonster::OnTriggerEnter(const game_module::Collision& collision)
{
	// �÷��̾� ���� �ǰ� ó��
	if (collision.other->GetTag() == game_module::ETag::PlayerAttack)
	{
		mAnimator->SetParameterTrigger("OnHit");
		auto playerAttack = collision.other->GetComponent<Attack>();
		float attackPower = playerAttack->GetAttackPower();

		mHp -= attackPower;

		GetComponent<HpBar>()->DecreaseHp(attackPower / mMaxHp);

		// Ÿ���� �ڽ��� ���� ������� �ٲߴϴ� 
		SetTarget(playerAttack->GetAttacker());

		// ���ó�� 
		if (mHp <= 0.f)
		{
			mAnimator->SetParameterBoolean("IsDead", true);
		}
	}
}

void fq::client::MeleeMonster::ChaseTarget()
{
	auto targetT = mTarget->GetComponent<game_module::Transform>();
	auto targetPos = targetT->GetWorldPosition();
	Move(targetPos);
}

void fq::client::MeleeMonster::DetectTarget()
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

void fq::client::MeleeMonster::CheckTargetInAttackRange()
{
	if (mTarget == nullptr || mTarget->IsDestroyed())
	{
		SetTarget(nullptr);
		return;
	}

	auto targetT = mTarget->GetComponent<game_module::Transform>();

	auto targetPos = targetT->GetWorldPosition();
	auto monsterPos = mTransform->GetWorldPosition();

	float distance = (targetPos - monsterPos).Length();
	bool isInAttackRange = mAttackRange >= distance;

	mAnimator->SetParameterBoolean("InAttackRange", isInAttackRange);
}

void fq::client::MeleeMonster::CheckAttackAble() const
{
	bool attackAble = mAttackElapsedTime == 0.f;

	if (attackAble)
	{
		mAnimator->SetParameterTrigger("OnAttack");
	}
}

void fq::client::MeleeMonster::OnUpdate(float dt)
{
	mAttackElapsedTime = std::max(0.f, mAttackElapsedTime - dt);
}

void fq::client::MeleeMonster::AnnounceFindedTarget()
{
	auto group = MonsterGroup::GetMonsterGroup(GetGameObject());

	if (group)
	{
		group->AnnounceFindedTarget(mTarget.get());
	}
}

