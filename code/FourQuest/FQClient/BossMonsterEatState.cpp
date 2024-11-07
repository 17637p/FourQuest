#include "BossMonsterEatState.h"

#include "../FQGameModule/GameModule.h"
#include "BossMonster.h"
#include "DeadArmour.h"

std::shared_ptr<fq::game_module::IStateBehaviour> fq::client::BossMonsterEatState::Clone()
{
	return fq::game_module::ObjectPool::GetInstance()->Assign<BossMonsterEatState>(*this);
}

fq::client::BossMonsterEatState::BossMonsterEatState()
	:mEatElapsedTime(0.f)
	, mEatTime(3.f)
	, mRecoverHp(200.f)
	, mArmourTargetOrNull(nullptr)
	, mbIsDestroyArmour(false)
	, mRimLightColor{ 0.f,1.f,0.f,1.f }
{}

fq::client::BossMonsterEatState::~BossMonsterEatState()
{

}

void fq::client::BossMonsterEatState::OnStateEnter(game_module::Animator& animator, game_module::AnimationStateNode& state)
{
	auto boss = animator.GetComponent<BossMonster>();
	auto target = boss->GetTarget();
	auto deadArmour = target->GetComponent<DeadArmour>();

	// ���� �Ƹ� ������Ʈ�� ���ų� ���� ���� ���� �����̶�� ���� ����
	if (boss == nullptr
		|| target == nullptr
		|| deadArmour == nullptr
		|| deadArmour->IsOnSummon())
	{
		mArmourTargetOrNull = nullptr;
		boss->SetTarget(nullptr);
		return;
	}
	
	// �ʱⰪ ����
	mArmourTargetOrNull = target;
	mEatElapsedTime = 0.f;
	mbIsDestroyArmour = false;

	// ������Ʈ ����
	boss->SetRimLightColor(true, mRimLightColor);

	// ����Ƹ� ���� �Ұ��� ���� ����
	deadArmour->SetSummonAble(false);
}

void fq::client::BossMonsterEatState::OnStateExit(game_module::Animator& animator, game_module::AnimationStateNode& state)
{
	auto boss = animator.GetComponent<BossMonster>();
	boss->EndPattern();

	if (mArmourTargetOrNull == nullptr)
	{
		return;
	}

	// ����Ƹ� �ı� 
	if (mbIsDestroyArmour)
	{
		animator.GetScene()->DestroyGameObject(mArmourTargetOrNull.get());
	}
	else
	{
		mArmourTargetOrNull->GetComponent<DeadArmour>()->SetSummonAble(true);
	}

	boss->SetRimLightColor(false, { 0.f,0.f,0.f,0.f });
	mArmourTargetOrNull = nullptr;
}

void fq::client::BossMonsterEatState::OnStateUpdate(game_module::Animator& animator, game_module::AnimationStateNode& state, float dt)
{
	if (mArmourTargetOrNull == nullptr)
	{
		return;
	}

	auto boss = animator.GetComponent<BossMonster>();

	// Ÿ�� Ȯ��
	if (mArmourTargetOrNull->IsDestroyed())
	{
		boss->SetTarget(nullptr);
		return;
	}

	// �ð� ���
	mEatElapsedTime += dt;
	if (mEatTime <= mEatElapsedTime)
	{
		boss->SetTarget(nullptr);
		mbIsDestroyArmour = true;
		return;
	}

	// ü�� ȸ�� 
	float hp = mRecoverHp * dt;
	boss->AddHp(hp);
}

