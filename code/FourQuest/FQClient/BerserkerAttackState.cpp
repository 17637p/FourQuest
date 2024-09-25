#include <cmath>

#include "BerserkerAttackState.h"
#include "Attack.h"
#include "BerserkerArmour.h"
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
		// ��Ʈ�ѷ� �Է¹����� �ٶ󺾴ϴ�
		auto controller = animator.GetComponent<game_module::CharacterController>();
		controller->SetPadInputRotation();
		controller->SetDashInput(true);

		// ���� �� �߻��ϴ� ������
		auto transform = animator.GetComponent<game_module::Transform>();
		auto rigid = animator.GetComponent<game_module::RigidBody>();
		auto foward = transform->GetWorldMatrix().Forward();
		foward.Normalize();
		foward.x *= mAttackMovement;
		foward.z *= mAttackMovement;
		rigid->SetLinearVelocity(foward);

		// ���� �浹�� ���� �и� ����
		animator.GetGameObject()->SetTag(game_module::ETag::PlayerMonsterIgnore);

		mElapsedTime = 0.f;
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
			berserkerArmour->EmitAttackIntend(mAttackType, mColliderOffset, mColliderScale, mKnocBackPower, mDestroyTime);
		}
	}

	void BerserkerAttackState::OnStateExit(game_module::Animator& animator, game_module::AnimationStateNode& state)
	{
		auto controller = animator.GetComponent<game_module::CharacterController>();
		controller->SetDashInput(false);
		
		// ���� �浹�� ���� �и� ����
		animator.GetGameObject()->SetTag(game_module::ETag::Player);
	}
}