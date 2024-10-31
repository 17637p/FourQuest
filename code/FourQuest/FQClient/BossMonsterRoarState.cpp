#include "BossMonsterRoarState.h"

#include "../FQGameModule/GameModule.h"
#include "../FQGameModule/NavigationAgent.h"
#include "BossMonster.h"
#include "SpawnStruct.h"
#include "ClientEvent.h"

fq::client::BossMonsterRoarState::BossMonsterRoarState()
	:mPosition{ 0.f,0.f,0.f }
	, mMeleeNumber(1)
	, mExplosionNumber(0)
	, mbIsEnterAngry(false)
	, mChangeColorDuration(2.f)
	, mChangeColorElapsed(0.f)
	, mStartColor(0, 0, 0, 1)
	, mEndColor(1, 0, 0, 1)
	, mStartInvRimPow(1.f)
	, mEndInvRimPow(10.f)
{
}

fq::client::BossMonsterRoarState::~BossMonsterRoarState()
{
}

void fq::client::BossMonsterRoarState::OnStateEnter(game_module::Animator& animator, game_module::AnimationStateNode& state)
{
	auto agent = animator.GetComponent<game_module::NavigationAgent>();
	agent->Stop();

	// 몬스터 소환 요청 이벤트 
	auto event = animator.GetScene()->GetEventManager();

	auto boss = animator.GetComponent<BossMonster>();
	event->FireEvent<event::BossSpawnMonster>({ boss->GetTarget() });
	// event->FireEvent<RequestSpawnMonster>({ EMonsterType::Melee , mMeleeNumber, mPosition });
	// event->FireEvent<RequestSpawnMonster>({ EMonsterType::Explosion, mExplosionNumber, mPosition });

	mbIsEnterAngry = animator.GetController().GetParameter("IsEnterAngry").cast<bool>();

	mChangeColorElapsed = 0.f;
}

void fq::client::BossMonsterRoarState::OnStateUpdate(game_module::Animator& animator, game_module::AnimationStateNode& state, float dt)
{
	if (mbIsEnterAngry)
	{
		mChangeColorElapsed += dt;

		float ratio = std::min<float>(mChangeColorElapsed / mChangeColorDuration, 1.f);
		auto bossMonster = animator.GetComponent<BossMonster>();
		auto currentColor = DirectX::SimpleMath::Color::Lerp(mStartColor, mEndColor, ratio);
		auto currentInvRimPow = std::lerp<float>(mStartInvRimPow, mEndInvRimPow, ratio);

		if (bossMonster != nullptr)
		{
			bossMonster->SetInvRimLightColor(true, currentColor, currentInvRimPow);
		}
	}
}

void fq::client::BossMonsterRoarState::OnStateExit(game_module::Animator& animator, game_module::AnimationStateNode& state)
{
	auto boss = animator.GetComponent<BossMonster>();

	if (boss != nullptr)
	{
		boss->EndPattern();
		boss->ResetRourCoolTime();
	}

	if (mbIsEnterAngry)
	{
		animator.SetParameterBoolean("IsEnterAngry", false);
		animator.SetParameterBoolean("OnVigilant", false);
		animator.SetParameterInt("VigilantCount", 0);
	}
}

std::shared_ptr<fq::game_module::IStateBehaviour> fq::client::BossMonsterRoarState::Clone()
{
	return fq::game_module::ObjectPool::GetInstance()->Assign<BossMonsterRoarState>(*this);
}
