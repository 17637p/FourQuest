#pragma once
#include "../FQGameModule/IStateBehaviour.h"


namespace fq::client
{
	class LaserAttackState : public fq::game_module::IStateBehaviour
	{
	public:
		LaserAttackState();
		~LaserAttackState();

	private:
		std::shared_ptr<IStateBehaviour> Clone() override;
		entt::meta_handle GetHandle() override { return *this; }
		void OnStateEnter(game_module::Animator& animator, game_module::AnimationStateNode& state) override;
		void OnStateUpdate(game_module::Animator& animator, game_module::AnimationStateNode& state, float dt) override;
		void OnStateExit(game_module::Animator& animator, game_module::AnimationStateNode& state) override;
	private:
		float mLaserEmitTime;
		float mElapsedTime;

		std::shared_ptr<game_module::GameObject> mGatherEffect;

		friend void RegisterMetaData();
	};


}