#define NOMINMAX
#include "MeleeMonster.h"

#include <random>
#include <algorithm>

#include "../FQGameModule/NavigationAgent.h"
#include "../FQGameModule/Transform.h"
#include "../FQGameModule/Animator.h"
#include "Attack.h"
#include "GameManager.h"
#include "HpBar.h"
#include "MonsterGroup.h"
#include "KnockBack.h"

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
	, mKnockBack(nullptr)
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
	if (mAnimator == nullptr)
	{
		mAnimator = GetComponent<game_module::Animator>();
		assert(mAnimator);
	}

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
	mKnockBack = GetComponent<KnockBack>();
	mMaxHp = mHp;

	// Agent ����
	auto agent = GetComponent<game_module::NavigationAgent>();
	agent->SetSpeed(mMoveSpeed);
	agent->SetAcceleration(mAcceleration);
	agent->SetRadius(0.3f);

	// GameManager ����
	mGameManager = GetScene()->GetObjectByName("GameManager")->GetComponent<GameManager>();

	if (mTarget)
	{
		SetTarget(mTarget.get());
	}
}

void fq::client::MeleeMonster::EmitAttack()
{
	using namespace game_module;

	auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(mAttackPrefab);
	auto& attackObj = *(instance.begin());

	auto attackT = attackObj->GetComponent<Transform>();

	// ���� Ʈ������ ����
	auto attackPos = mTransform->GetWorldPosition();
	auto scale = attackT->GetWorldScale();
	auto rotation = mTransform->GetWorldRotation();

	auto rotationMat = DirectX::SimpleMath::Matrix::CreateFromQuaternion(rotation);
	auto foward = rotationMat.Forward();
	attackPos += foward * mAttackOffset;
	attackT->GenerateWorld(attackPos, rotation, scale);

	// ���� ���� ����
	AttackInfo attackInfo{};
	auto attackComponent = attackObj->GetComponent<client::Attack>();

	attackInfo.attacker = GetGameObject();
	attackInfo.damage = mAttackPower;
	attackInfo.attackDirection = foward;
	attackComponent->Set(attackInfo);

	GetScene()->AddGameObject(attackObj);

	// ���� ��Ÿ�� ����ó��
	mAttackElapsedTime = mAttackCoolTime;

	// ���� ����
	GetScene()->GetEventManager()->FireEvent<fq::event::OnPlaySound>({ "MM_Attack", false , fq::sound::EChannel::SE });
}


std::shared_ptr<fq::game_module::GameObject> fq::client::MeleeMonster::EmitAttackEffect()
{
	auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(mAttackEffect);
	auto& effentObj = *(instance.begin());

	auto effectT = effentObj->GetComponent<game_module::Transform>();
	effectT->SetParent(mTransform);

	GetScene()->AddGameObject(effentObj);

	return effentObj;
}



void fq::client::MeleeMonster::Move(DirectX::SimpleMath::Vector3 destination)
{
	fq::game_module::NavigationAgent* agent = GetComponent<fq::game_module::NavigationAgent>();

	if (agent)
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
		auto playerAttack = collision.other->GetComponent<Attack>();

		if (playerAttack->ProcessAttack())
		{
			mAnimator->SetParameterTrigger("OnHit");
			float attackPower = playerAttack->GetAttackPower();

			// Ÿ���� �ڽ��� ���� ������� �ٲߴϴ� 
			SetTarget(playerAttack->GetAttacker());

			// �˹�ó�� 
			if (playerAttack->HasKnockBack())
			{
				auto type = playerAttack->GetKnockBackType();
				float power = playerAttack->GetKnockBackPower();

				if (type == EKnockBackType::Fixed)
				{
					DirectX::SimpleMath::Vector3 direction = playerAttack->GetAttackDirection();
					mKnockBack->Set(power, direction);
				}
				else if (type == EKnockBackType::TargetPosition)
				{
					auto monsterPos = mTransform->GetWorldPosition();
					monsterPos.y = 0.f;
					auto attackPos = playerAttack->GetAttackPosition();
					attackPos.y = 0.f;

					auto knockBackDir = monsterPos - attackPos;
					knockBackDir.Normalize();

					mKnockBack->Set(power, knockBackDir);
				}
			}

			// HP ����
			mHp -= attackPower;
			GetComponent<HpBar>()->DecreaseHp(attackPower / mMaxHp);

			// �ǰ� ���� ���
			playerAttack->PlayHitSound();

			// ���ó�� 
			if (mHp <= 0.f)
			{
				mAnimator->SetParameterBoolean("IsDead", true);
			}
		}
	}
}

void fq::client::MeleeMonster::ChaseTarget()
{
	if (mTarget == nullptr || mTarget->IsDestroyed())
	{
		SetTarget(nullptr);
		return;
	}

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

void fq::client::MeleeMonster::LookAtTarget()
{
	if (mTarget == nullptr || mTarget->IsDestroyed())
		return;

	constexpr float RotationSpeed = 0.1f;

	auto transform = GetComponent<game_module::Transform>();
	auto targetT = mTarget->GetComponent<game_module::Transform>();
	auto targetPos = targetT->GetWorldPosition();
	auto myPos = transform->GetWorldPosition();
	auto currentRotation = transform->GetWorldRotation();
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

	DirectX::SimpleMath::Matrix rotationMatrix = DirectX::SimpleMath::Matrix::CreateFromQuaternion(directionQuaternion);

	DirectX::SimpleMath::Quaternion result =
		DirectX::SimpleMath::Quaternion::Slerp(currentRotation, directionQuaternion, RotationSpeed);

	mTransform->SetWorldRotation(result);
}

