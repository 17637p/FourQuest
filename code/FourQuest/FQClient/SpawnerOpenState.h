#pragma once

#include "../FQGameModule/IStateBehaviour.h"

namespace fq::client
{
	class SpawnerOpenState : public fq::game_module::IStateBehaviour
	{
	public:
		SpawnerOpenState();
		~SpawnerOpenState();

	private:
		std::shared_ptr<IStateBehaviour> Clone() override;
		entt::meta_handle GetHandle() override { return *this; }
		void OnStateEnter(game_module::Animator& animator, game_module::AnimationStateNode& state) override;
		void OnStateUpdate(game_module::Animator& animator, game_module::AnimationStateNode& state, float dt) override;

	private:
		float mSpwanTiming; // ���� Ÿ�̹�
		float mElapsedTime;

		friend void RegisterMetaData();
	};


}