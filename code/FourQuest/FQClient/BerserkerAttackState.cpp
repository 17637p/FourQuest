#include <cmath>

#include "BerserkerAttackState.h"
#include "Attack.h"
#include "BerserkerArmour.h"
#include "Player.h"
#include "../FQGameModule/Animator.h"
#include "../FQGameModule/CharacterController.h"
#include "../FQGameModule/RigidBody.h"
#include "../FQGameModule/Transform.h"
#include "../FQGameModule/Scene.h"
#include "../FQGameModule/PrefabManager.h"

namespace fq::client
{
	BerserkerAttackState::BerserkerAttackState()
		: mAttackType(EBerserkerAttackType::None)
		, mAttackMovement(0.f)
		, mElapsedTime(0.f)
		, mAttackTiming(1.f)
		, mColliderOffset{}
		, mColliderScale{ 1.f, 1.f, 1.f }
		, mKnocBackPower(1.f)
		, mDestroyTime(0.2f)
	{
	}

	std::shared_ptr<fq::game_module::IStateBehaviour> BerserkerAttackState::Clone()
	{
		return fq::game_module::ObjectPool::GetInstance()->Assign<BerserkerAttackState>(*this);
	}

	void BerserkerAttackState::OnStateEnter(game_module::Animator& animator, game_module::AnimationStateNode& state)
	{
		// 컨트롤러 입력방향을 바라봅니다
		auto controller = animator.GetComponent<game_module::CharacterController>();
		controller->SetPadInputRotation();
		controller->SetDashInput(true);


		// 몬스터 충돌에 의한 밀림 무시
		// 회전 공격 시 몬스터 위로 올라가는 버그로 인해 임시로 수정
		if (mAttackType != EBerserkerAttackType::SwingAround) 
		{
			animator.GetGameObject()->SetTag(game_module::ETag::PlayerMonsterIgnore);
		}

		mElapsedTime = 0.f;

		// 슈퍼아머 기능 On
		auto playerOrNull = animator.GetComponent<Player>();
		if (playerOrNull != nullptr)
		{
			playerOrNull->SetIsActiveOnHit(false);
		}
	}

	void BerserkerAttackState::OnStateUpdate(game_module::Animator& animator, game_module::AnimationStateNode& state, float dt)
	{
		if (mElapsedTime == mAttackTiming)
		{
			return;
		}

		mElapsedTime = std::min<float>(mElapsedTime + dt * state.GetPlayBackSpeed(), mAttackTiming);

		if (mElapsedTime == mAttackTiming)
		{
			auto berserkerArmour = animator.GetComponent<BerserkerArmour>();
			if (berserkerArmour != nullptr)
			{
				berserkerArmour->EmitAttackIntend(mAttackType, mColliderOffset, mColliderScale, mKnocBackPower, mDestroyTime);

				// 공격 시 발생하는 전진량
				auto transform = animator.GetComponent<game_module::Transform>();
				auto rigid = animator.GetComponent<game_module::RigidBody>();
				auto foward = transform->GetWorldMatrix().Forward();
				foward.Normalize();
				foward.x *= mAttackMovement;
				foward.z *= mAttackMovement;
				rigid->SetLinearVelocity(foward);
			}
		}
	}

	void BerserkerAttackState::OnStateExit(game_module::Animator& animator, game_module::AnimationStateNode& state)
	{
		auto controller = animator.GetComponent<game_module::CharacterController>();
		controller->SetDashInput(false);

		// 몬스터 충돌에 의한 밀림 무시
		animator.GetGameObject()->SetTag(game_module::ETag::Player);

		// 슈퍼아머 기능 off
		auto playerOrNull = animator.GetComponent<Player>();
		if (playerOrNull != nullptr)
		{
			playerOrNull->SetIsActiveOnHit(true);
		}
	}
}