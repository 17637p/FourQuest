#pragma once


#include "../FQGameModule/IStateBehaviour.h"

namespace fq::game_module
{
	class GameObject;
}

namespace fq::client
{
	class MeleeMonsterExplosionState : public game_module::IStateBehaviour
	{
	public:
		MeleeMonsterExplosionState();
		~MeleeMonsterExplosionState();

	private:
		std::shared_ptr<IStateBehaviour> Clone() override;
		entt::meta_handle GetHandle() override { return *this; }

		void OnStateEnter(game_module::Animator& animator, game_module::AnimationStateNode& state) override;
		void OnStateExit(game_module::Animator& animator, game_module::AnimationStateNode& state) override;

	private:
		std::shared_ptr<fq::game_module::GameObject> mWarningUI;
	};


}