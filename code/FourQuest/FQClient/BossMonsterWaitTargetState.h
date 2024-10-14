#pragma once

#include "../FQGameModule/IStateBehaviour.h"

namespace fq::client
{
	/// <summary>
	/// �÷��̾ ��� ��ȥȭ���¿��� ��ٸ��� ����
	/// </summary>
	class BossMonsterWaitTargetState : public fq::game_module::IStateBehaviour
	{
	public:
		BossMonsterWaitTargetState();
		~BossMonsterWaitTargetState();

	private:
		void OnStateEnter(game_module::Animator& animator, game_module::AnimationStateNode& state) override;
		void OnStateUpdate(game_module::Animator& animator, game_module::AnimationStateNode& state , float dt) override;
		//void OnStateExit(game_module::Animator& animator, game_module::AnimationStateNode& state) override;
		std::shared_ptr<IStateBehaviour> Clone() override;
		entt::meta_handle GetHandle() override { return *this; }
	};
}