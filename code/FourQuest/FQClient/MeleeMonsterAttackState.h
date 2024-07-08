#pragma once

#include "../FQGameModule/IStateBehaviour.h"

namespace fq::client
{
	/// <summary>
	/// ���� ���� ���� ����
	/// </summary>
	class MeleeMonsterAttackState :public fq::game_module::IStateBehaviour
	{
	public:
		MeleeMonsterAttackState();
		~MeleeMonsterAttackState();

		void OnStateEnter(game_module::Animator& animator, game_module::AnimationStateNode& state) override;
		void OnStateUpdate(game_module::Animator& animator, game_module::AnimationStateNode& state, float dt) override;

	private:
		std::shared_ptr<IStateBehaviour> Clone() override;
		entt::meta_handle GetHandle() override { return *this; }

	private:
		float mAttackTiming; // ���ݹߵ� �ð�
		float mElapsedTime;

		friend void RegisterMetaData();
	};

}
