#pragma once
#include "../FQGameModule/IStateBehaviour.h"


namespace fq::client
{
	/// <summary>
	/// ���Ÿ� ���� ���� ����  ó��
	/// </summary>
	class PlantMonsterDeadState : public fq::game_module::IStateBehaviour
	{
	public:
		PlantMonsterDeadState();
		~PlantMonsterDeadState();

		void OnStateEnter(game_module::Animator& animator, game_module::AnimationStateNode& state) override;

		void OnStateExit(game_module::Animator& animator, game_module::AnimationStateNode& state) override;

	private:
		std::shared_ptr<IStateBehaviour> Clone() override;
		entt::meta_handle GetHandle() override { return *this; }
	};


}