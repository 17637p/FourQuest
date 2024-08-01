#include "BossMonster.h"

#include "../FQGameModule/NavigationAgent.h"
#include "../FQGameModule/Transform.h"
#include "../FQGameModule/Animator.h"
#include "Attack.h"
#include "GameManager.h"
#include "KnockBack.h"


fq::client::BossMonster::BossMonster()
	:mMaxHp(0.f)
	,mHp(2000.f)
	,mAttackPower(50.f)
	,mMoveSpeed(1.f)
	,mAcceleration(2.f)
	,mAttackRange(10.f)
	,mAttackCoolTime(3.f)
	,mAttackElapsedTime(0.f)
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

}
