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

	// 림라이트 설정
	boss->SetRimLightColor(true, mRimLightColor);

	// 데드아머 빙의 불가능 상태 설정
	mArmour->GetComponent<DeadArmour>()->SetSummonAble(false);
}

void fq::client::BossMonsterEatState::OnStateExit(game_module::Animator& animator, game_module::AnimationStateNode& state)
{
	auto boss = animator.GetComponent<BossMonster>();
	
	// 데드아머 파괴 
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
	
	// 타겟 확인
	if (mArmour->IsDestroyed())
	{
		boss->SetTarget(nullptr);
		return;
	}

	// 시간 계산
	mEatElapsedTime += dt;
	if (mEatTime <= mEatElapsedTime)
	{
		boss->SetTarget(nullptr);
		mbIsDestroyArmour = true;
		return;
	}

	// 체력 회복 
	float hp = mRecoverHp * dt;
	boss->AddHp(hp);
 }

