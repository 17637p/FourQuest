#pragma once
#include "../FQGameModule/IStateBehaviour.h"

namespace fq::client
{
	class BossMonsterVigilantCountingState : public fq::game_module::IStateBehaviour
	{
	public:
		BossMonsterVigilantCountingState();
		~BossMonsterVigilantCountingState();

	private:
		void OnStateEnter(game_module::Animator& animator, game_module::AnimationStateNode& state) override;

		std::shared_ptr<IStateBehaviour> Clone() override;
		entt::meta_handle GetHandle() override { return *this; }

	private:
		friend void RegisterMetaData();
	};
}