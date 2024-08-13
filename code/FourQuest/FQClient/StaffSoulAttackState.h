#pragma once

#include "../FQGameModule/IStateBehaviour.h"

namespace fq::client
{
	class StaffSoulAttackState : public fq::game_module::IStateBehaviour
	{
	public:
		StaffSoulAttackState();
		~StaffSoulAttackState();

	private:
		std::shared_ptr<IStateBehaviour> Clone() override;
		// void OnStateEnter(game_module::Animator & animator, game_module::AnimationStateNode & state) override;
		// void OnStateUpdate(game_module::Animator& animator, game_module::AnimationStateNode& state, float dt) override;
		void OnStateExit(game_module::Animator& animator, game_module::AnimationStateNode& state) override;
		entt::meta_handle GetHandle() override { return *this; }

	private:
		std::shared_ptr<fq::game_module::GameObject> mEffect;

		friend void RegisterMetaData();
	};


}