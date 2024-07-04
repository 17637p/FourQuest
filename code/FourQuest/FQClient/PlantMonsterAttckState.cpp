#include "PlantMonsterAttckState.h"

#include "../FQGameModule/GameModule.h"
#include "PlantMonster.h"

std::shared_ptr<fq::game_module::IStateBehaviour> fq::client::PlantMonsterAttckState::Clone()
{
	return fq::game_module::ObjectPool::GetInstance()->Assign<PlantMonsterAttckState>(*this);

}

fq::client::PlantMonsterAttckState::PlantMonsterAttckState()
	:mAttackTiming(0.f)
	,mElapsedTime(0.f)
{}

fq::client::PlantMonsterAttckState::~PlantMonsterAttckState()
{}

void fq::client::PlantMonsterAttckState::OnStateEnter(game_module::Animator& animator, game_module::AnimationStateNode& state)
{
	// 아웃라인 변경
	for (auto mesh : animator.GetSkinnedMeshs())
	{
		mesh->GetSkinnedMeshObject()->SetOutlineColor({ 1.f,0.f,0.f,1.f });
	}

	mElapsedTime = 0.f;

	// 공격 즉시 발동
	if (mAttackTiming == mElapsedTime)
	{
		animator.GetComponent<PlantMonster>()->EmitAttack();
		mElapsedTime = mAttackTiming;

		// 아웃라인 변경
		for (auto mesh : animator.GetSkinnedMeshs())
		{
			mesh->GetSkinnedMeshObject()->SetOutlineColor({ 0.f,0.f,0.f,1.f });
		}
	}
}

void fq::client::PlantMonsterAttckState::OnStateUpdate(game_module::Animator& animator, game_module::AnimationStateNode& state, float dt)
{
	if (mElapsedTime == mAttackTiming)
		return;

	mElapsedTime += dt;

	// 공격 발동 
	if (mElapsedTime >= mAttackTiming)
	{
		animator.GetComponent<PlantMonster>()->EmitAttack();
		mElapsedTime = mAttackTiming;

		// 아웃라인 변경
		for (auto mesh : animator.GetSkinnedMeshs())
		{
			mesh->GetSkinnedMeshObject()->SetOutlineColor({ 0.f,0.f,0.f,1.f });
		}
	}
}

void fq::client::PlantMonsterAttckState::OnStateExit(game_module::Animator& animator, game_module::AnimationStateNode& state)
{
	// 아웃라인 변경
	for (auto mesh : animator.GetSkinnedMeshs())
	{
		mesh->GetSkinnedMeshObject()->SetOutlineColor({ 0.f,0.f,0.f,1.f });
	}
}
