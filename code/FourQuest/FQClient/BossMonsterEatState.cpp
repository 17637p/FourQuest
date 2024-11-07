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

	// 데드 아머 컴포넌트가 없거나 빙의 진행 중인 갑옷이라면 먹지 못함
	if (boss == nullptr
		|| target == nullptr
		|| deadArmour == nullptr
		|| deadArmour->IsOnSummon())
	{
		mArmourTargetOrNull = nullptr;
		boss->SetTarget(nullptr);
		return;
	}
	
	// 초기값 설정
	mArmourTargetOrNull = target;
	mEatElapsedTime = 0.f;
	mbIsDestroyArmour = false;

	// 림라이트 설정
	boss->SetRimLightColor(true, mRimLightColor);

	// 데드아머 빙의 불가능 상태 설정
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

	// 데드아머 파괴 
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

	// 타겟 확인
	if (mArmourTargetOrNull->IsDestroyed())
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

