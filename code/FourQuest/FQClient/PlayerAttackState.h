#pragma once

#include "../FQGameModule/IStateBehaviour.h"
#include "PlayerDefine.h"

namespace fq::client
{
	/// <summary>
	/// �÷��̾� ���ݻ��¿� ���� ó���� ����մϴ�
	/// </summary>
	class PlayerAttackState : public game_module::IStateBehaviour
	{
	public:
		PlayerAttackState();
		~PlayerAttackState();

		std::shared_ptr<IStateBehaviour> Clone() override;
		void OnStateEnter(game_module::Animator& animator, game_module::AnimationStateNode& state) override;
		void OnStateUpdate(game_module::Animator& animator, game_module::AnimationStateNode& state, float dt) override;
		void OnStateExit(game_module::Animator& animator, game_module::AnimationStateNode& state) override;

	private:
		entt::meta_handle GetHandle() override { return *this; }

	private:
		float mAttackRebound; // ���� �ݵ�
		float mAttackTiming; // ���� �ߵ� �ð�
		float mElapsedTime;

		friend void RegisterMetaData();
	};
}

