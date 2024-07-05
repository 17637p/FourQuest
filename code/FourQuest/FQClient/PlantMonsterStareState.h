#pragma once
#include "../FQGameModule/IStateBehaviour.h"

namespace fq::client
{
	class PlantMonsterStareState :public fq::game_module::IStateBehaviour
	{
	public:
		PlantMonsterStareState();
		~PlantMonsterStareState();

	private:
		std::shared_ptr<IStateBehaviour> Clone() override;
		entt::meta_handle GetHandle() override { return *this; }
		void OnStateUpdate(game_module::Animator& animator, game_module::AnimationStateNode& state, float dt) override;
	};

}