#pragma once
#include "../FQGameModule/IStateBehaviour.h"
#include <directxtk/SimpleMath.h>

namespace fq::client
{
	class MonsterAttack : public fq::game_module::IStateBehaviour
	{
	public:
		MonsterAttack();
		~MonsterAttack();

		std::shared_ptr<IStateBehaviour> Clone() override;
		void OnStateEnter(fq::game_module::Animator& animator, fq::game_module::AnimationStateNode& state) override;
		void OnStateUpdate(fq::game_module::Animator& animator, fq::game_module::AnimationStateNode& state) override;
		void OnStateExit(fq::game_module::Animator& animator, fq::game_module::AnimationStateNode& state) override;

	private:
		entt::meta_handle GetHandle() override { return *this; }

		void makeAttackRangeCollider();
		void rotateTowards(fq::game_module::Animator& animator);
		DirectX::SimpleMath::Vector3 getDirectionVectorFromQuaternion(const DirectX::SimpleMath::Quaternion& quaternion);

	private:
		float mWaitTime;
		bool mIsStartAttack;
	};
}
