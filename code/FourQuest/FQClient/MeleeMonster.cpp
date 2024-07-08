#include "MeleeMonster.h"

#include <random>

#include "Attack.h"
#include "GameManager.h"
#include "HpBar.h"

fq::client::MeleeMonster::MeleeMonster()
	:mMaxHp(0.f)
	, mHp(100.f)
	, mAttackPower(10.f)
	, mMoveSpeed(1.f)
	, mAcceleration(1.f)
	, mAttackRange(10.f)
	, mGameManager(nullptr)
	, mAnimator(nullptr)
	, mTarget(nullptr)
	, mStartPosition{}
	, mPatrolDestination{}
	, mTransform(nullptr)
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
}

void fq::client::MeleeMonster::EmitAttack()
{
	using namespace game_module;

	auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(mAttackPrefab);
	auto& attackObj = *(instance.begin());

	auto attackT = attackObj->GetComponent<Transform>();
	auto transform = GetComponent<Transform>();

	// ���� ����
	auto attackComponent = attackObj->GetComponent<client::Attack>();
	attackComponent->SetAttacker(GetGameObject());
	attackComponent->SetAttackPower(mAttackPower);

	DirectX::SimpleMath::Vector3 offset = { 0.f,1.f,0.f };
	attackT->SetLocalPosition(transform->GetWorldPosition() + offset);

	GetScene()->AddGameObject(attackObj);

	// ���� ��Ÿ�� ����ó��
	//mAttackElapsedTime = mAttackCoolTime;

	// TODO : ���� ���� ���ݻ��� �߰� 
}

void fq::client::MeleeMonster::LookAtTarget()
{

}


void fq::client::MeleeMonster::Move(DirectX::SimpleMath::Vector3 destination)
{
	fq::game_module::NavigationAgent* agent = GetComponent<fq::game_module::NavigationAgent>();
	agent->MoveTo(destination);
}

void fq::client::MeleeMonster::Patrol()
{
	auto worldPos = mTransform->GetWorldPosition();

	float distance = (worldPos - mPatrolDestination).Length();

	constexpr float EndPatrolDistance = 1.5f;

	// ��ǥ ���� ���� 
	if (distance <= EndPatrolDistance)
	{
		ChangeToRandomIdleOrPatrol();
	}
	else //  ���� �̵� 
	{
		Move(mPatrolDestination);
	}
}

void fq::client::MeleeMonster::ChangeToRandomIdleOrPatrol()
{
	static std::random_device rd;
	static std::mt19937 gen(rd());

	std::uniform_real_distribution<float> dis(0, 1.f);
	float val = dis(gen);

	// Idle
	if (val < 0.5f)  
	{
		mAnimator->SetParameterBoolean("OnPatrol", false);
	}
	else // Patrol
	{
		// ���� ��ġ ���ϱ� 
		std::uniform_real_distribution<float> patrolDis(0, mPatrolRange);
		float distance = patrolDis(gen);

		std::uniform_real_distribution<float> radianDis(0, DirectX::XM_2PI);
		float radian = radianDis(gen);

		mPatrolDestination = mStartPosition;
		mPatrolDestination.x += std::cos(radian) * distance;
		mPatrolDestination.z += std::sin(radian) * distance;

		mAnimator->SetParameterBoolean("OnPatrol", true);
	}

}

