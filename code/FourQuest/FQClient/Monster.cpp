#include "Monster.h"

//temp: ���߿� �� �÷��̾�� �ٲ� ��
#include "Player.h"

fq::client::Monster::Monster()
	:mHP(),
	mAttackPower(),
	mMoveSpeed(),
	mIsDamaged(false),
	mTargetAttackRange(500),
	mChaseDistance(30)
{
	
}

fq::client::Monster::~Monster()
{
	
}

std::shared_ptr<fq::game_module::Component> fq::client::Monster::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	std::shared_ptr<Monster> cloneMonster = std::dynamic_pointer_cast<Monster>(clone);

	if (cloneMonster == nullptr) // ���� �����ؼ� ���纻�� �ش�
	{
		cloneMonster = game_module::ObjectPool::GetInstance()->Assign<Monster>(*this);
	}
	else // clone�� �����͸� �����Ѵ�.
	{
		// �⺻ ���� ������ ȣ���Ѵ�.
		*cloneMonster = *this;
	}

	return cloneMonster;
}

float fq::client::Monster::CalculateDistance(fq::game_module::GameObject gameObject)
{
	DirectX::SimpleMath::Vector3 playerPosition =
		gameObject.GetComponent<fq::game_module::Transform>()->GetWorldPosition();
	DirectX::SimpleMath::Vector3 myPosition =
		GetComponent<fq::game_module::Transform>()->GetWorldPosition();

	float dist = DirectX::SimpleMath::Vector3::Distance(playerPosition, myPosition);

	return dist;
}

float fq::client::Monster::GetChaseDistance() const
{
	return mChaseDistance;
}

float fq::client::Monster::GetHP() const
{
	return mHP;
}

void fq::client::Monster::SetHP(float hp)
{
	mHP = hp;
}

float fq::client::Monster::GetAttackWaitTime() const
{
	return mAttackWaitTime;
}

fq::game_module::GameObject* fq::client::Monster::GetTarget() const
{
	return mTarget;
}

void fq::client::Monster::SetTarget(fq::game_module::GameObject* target)
{
	mTarget = target;
}

bool fq::client::Monster::GetIsDamagaed() const
{
	return mIsDamaged;
}

float fq::client::Monster::GetTargetAttackRange() const
{
	return mTargetAttackRange;
}

void fq::client::Monster::SetTargetAttackRange(float targetAttackRange)
{
	mTargetAttackRange = targetAttackRange;
}

float fq::client::Monster::GetDamaged() const
{
	return mDamaged;
}

float fq::client::Monster::GetMoveSpeed() const
{
	return mMoveSpeed;
}

void fq::client::Monster::SetMoveSpeed(float moveSpeed)
{
	mMoveSpeed = moveSpeed;
}

void fq::client::Monster::SetChaseDistance(float chaseDistance)
{
	mChaseDistance = chaseDistance;
}

void fq::client::Monster::SetAttackWaitTime(float attackWaitTime)
{
	mAttackWaitTime = attackWaitTime;
}

float fq::client::Monster::GetAttackPower() const
{
	return mAttackPower;
}

void fq::client::Monster::SetAttackPower(float attackPower)
{
	mAttackPower = attackPower;
}

fq::game_module::GameObject* fq::client::Monster::GetLastAttacker() const
{
	return mLastAttacker;
}

