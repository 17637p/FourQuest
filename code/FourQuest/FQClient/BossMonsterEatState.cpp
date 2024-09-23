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
	,mEatTime(3.f)
	,mRecoverHp(200.f)
	,mArmour(nullptr)
	,mbIsDestroyArmour(false)
	,mRimLightColor{0.f,1.f,0.f,1.f}
{}

fq::client::BossMonsterEatState::~BossMonsterEatState()
{

}

void fq::client::BossMonsterEatState::OnStateEnter(game_module::Animator& animator, game_module::AnimationStateNode& state)
{
	mEatElapsedTime = 0.f;
	mbIsDestroyArmour = false;
	auto boss = animator.GetComponent<BossMonster>();

	mArmour = boss->GetTarget();
	if (!mArmour->HasComponent<DeadArmour>())
	{
		spdlog::warn("Boss Eat Target Error");
		boss->SetTarget(nullptr);
		return;
	}

	// ������Ʈ ����
	boss->SetRimLightColor(true, mRimLightColor);

	// ����Ƹ� ���� �Ұ��� ���� ����
	mArmour->GetComponent<DeadArmour>()->SetSummonAble(false);
}

void fq::client::BossMonsterEatState::OnStateExit(game_module::Animator& animator, game_module::AnimationStateNode& state)
{
	auto boss = animator.GetComponent<BossMonster>();
	
	// ����Ƹ� �ı� 
	if (mbIsDestroyArmour)
	{
		animator.GetScene()->DestroyGameObject(mArmour.get());
	}
	else
	{
		mArmour->GetComponent<DeadArmour>()->SetSummonAble(true);
	}

	boss->SetRimLightColor(false, {0.f,0.f,0.f,0.f});

	mArmour = nullptr;
	boss->EndPattern();
}

void fq::client::BossMonsterEatState::OnStateUpdate(game_module::Animator& animator, game_module::AnimationStateNode& state, float dt)
{
	auto boss = animator.GetComponent<BossMonster>();
	
	// Ÿ�� Ȯ��
	if (mArmour->IsDestroyed())
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

