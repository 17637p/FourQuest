#pragma once
#include "../FQGameModule/IStateBehaviour.h"


namespace fq::client
{
	class BossMonsterGroggyState : public fq::game_module::IStateBehaviour
	{
	public:
		BossMonsterGroggyState();
		~BossMonsterGroggyState();

	private:
		void OnStateEnter(game_module::Animator& animator, game_module::AnimationStateNode& state) override;
		std::shared_ptr<IStateBehaviour> Clone() override;
		entt::meta_handle GetHandle() override { return *this; }

	private:
		friend void RegisterMetaData();
	};


}