#pragma once
#include "../FQGameModule/IStateBehaviour.h"
#include <directxtk/SimpleMath.h>

namespace fq::client
{
	class MonsterChase : public fq::game_module::IStateBehaviour
	{
	public:
		MonsterChase();
		~MonsterChase();

		std::shared_ptr<IStateBehaviour> Clone() override;
		void OnStateEnter(fq::game_module::Animator& animator, fq::game_module::AnimationStateNode& state) override;
		void OnStateUpdate(fq::game_module::Animator& animator, fq::game_module::AnimationStateNode& state) override;
		void OnStateExit(fq::game_module::Animator& animator, fq::game_module::AnimationStateNode& state) override;

	private:
		entt::meta_handle GetHandle() override { return *this; }

		void MoveToTarget(fq::game_module::Animator& animator, float dist);
		void rotateTowards(fq::game_module::Animator& animator);
		DirectX::SimpleMath::Vector3 getDirectionVectorFromQuaternion(const DirectX::SimpleMath::Quaternion& quaternion);

	private:
		float mMoveDistance;
	};
}
