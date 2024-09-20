#pragma once

#include "../FQGameModule/IStateBehaviour.h"

namespace fq::client
{
	class BossMonsterEatState : public fq::game_module::IStateBehaviour
	{
	public:
		BossMonsterEatState();
		~BossMonsterEatState();

	private:
		void OnStateEnter(game_module::Animator& animator, game_module::AnimationStateNode& state) override;
		void OnStateUpdate(game_module::Animator& animator, game_module::AnimationStateNode& state, float dt) override;
		void OnStateExit(game_module::Animator& animator, game_module::AnimationStateNode& state) override;
		std::shared_ptr<IStateBehaviour> Clone() override;
		entt::meta_handle GetHandle() override { return *this; }

	private:
		DirectX::SimpleMath::Color mRimLightColor;
		float mEatTime;
		float mEatElapsedTime;
		float mRecoverHp;
		bool mbIsDestroyArmour;

		std::shared_ptr<game_module::GameObject> mArmour;

		friend void RegisterMetaData();
	};

}
