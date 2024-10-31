#include "BossMonsterJumpState.h"
#include "BossMonster.h"

#include "../FQGameModule/Scene.h"
#include "../FQGameModule/Transform.h"
#include "../FQGameModule/PrefabManager.h"

fq::client::BossMonsterJumpState::BossMonsterJumpState()
	: mElapsed(0.f)
	, mStartPosition()
	, mEndPosition()
	, mMaxY(5.f)
	, mJumpDuration(2.f)
	, mAttackGenateTime(1.f)
	, mEffectGenateTime(1.f)
{
}

std::shared_ptr<fq::game_module::IStateBehaviour> fq::client::BossMonsterJumpState::Clone()
{
	return fq::game_module::ObjectPool::GetInstance()->Assign<BossMonsterJumpState>(*this);
}

void fq::client::BossMonsterJumpState::OnStateEnter(game_module::Animator& animator, game_module::AnimationStateNode& state)
{
	animator.GetGameObject()->SetTag(game_module::ETag::Dash);

	mElapsed = 0.f;

	mStartPosition = animator.GetTransform()->GetWorldPosition();

	auto bossMonster = animator.GetComponent<BossMonster>();

	// 종료 위치 설정 및 예외 처리
	if (bossMonster != nullptr)
	{
		mEndPosition = bossMonster->GetJumpPosition();
		mDecalEndEffectObject = bossMonster->EmitJumpDecalEndEffect();
	}
	else
	{
		mEndPosition = mStartPosition;
	}

	mbIsEmitAttack = false;
	mbIsEmitEffect = false;
}

void fq::client::BossMonsterJumpState::OnStateUpdate(game_module::Animator& animator, game_module::AnimationStateNode& state, float dt)
{
	// 시간 누적
	mElapsed += dt;

	float ratio = mElapsed / mJumpDuration;
	ratio = std::min<float>(ratio, 1);
	auto currentPosition = DirectX::SimpleMath::Vector3::Lerp(mStartPosition, mEndPosition, ratio);
	float period = sin(ratio * 3.141592);
	auto yPos = std::lerp(currentPosition.y, mMaxY, period);
	currentPosition.y = yPos;
	animator.GetTransform()->SetWorldPosition(currentPosition);

	// 종료 체크
	if (mJumpDuration < mElapsed)
	{
		animator.SetParameterBoolean("OnJump", false);
	}

	auto bossMonster = animator.GetComponent<BossMonster>();
	
	if (!mbIsEmitAttack && mAttackGenateTime < mElapsed)
	{
		if (bossMonster != nullptr)
		{
			bossMonster->EmitJumpAttack();
		}
		mbIsEmitAttack = true;
	}

	if (!mbIsEmitEffect && mEffectGenateTime < mElapsed)
	{
		if (bossMonster != nullptr)
		{
			bossMonster->EmitJumpEffect();
		}
		mbIsEmitEffect = true;
	}
}

void fq::client::BossMonsterJumpState::OnStateExit(game_module::Animator& animator, game_module::AnimationStateNode& state)
{
	animator.GetGameObject()->SetTag(game_module::ETag::Monster);

	auto bossMonster = animator.GetComponent<BossMonster>();

	if (bossMonster != nullptr)
	{
		bossMonster->EndPattern();
		bossMonster->ResetJumpCoolTime();

		if (!mbIsEmitEffect)
		{
			bossMonster->EmitJumpEffect();
		}

		if (!mbIsEmitAttack)
		{
			bossMonster->EmitJumpAttack();
		}
	}

	if (mDecalEndEffectObject != nullptr)
	{
		animator.GetScene()->DestroyGameObject(mDecalEndEffectObject.get());
		mDecalEndEffectObject = nullptr;
	}
}
