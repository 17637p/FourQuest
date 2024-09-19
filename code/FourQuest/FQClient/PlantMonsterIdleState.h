#pragma once

#include "../FQGameModule/IStateBehaviour.h"

namespace fq::client
{
	class PlantMonsterIdleState : public fq::game_module::IStateBehaviour
	{
	public:
		PlantMonsterIdleState();
		~PlantMonsterIdleState();

		void OnStateUpdate(game_module::Animator& animator, game_module::AnimationStateNode& state, float dt) override;

	private:
		std::shared_ptr<IStateBehaviour> Clone() override;
		entt::meta_handle GetHandle() override { return *this; }
	};
}