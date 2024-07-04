#pragma once

#include "../FQGameModule/IStateBehaviour.h"

namespace fq::client
{
	/// <summary>
	/// ���� ���¿��� ĳ������ ������, ȸ�� ���θ� �����մϴ�
	/// </summary>
	class PlayerMovementState : public game_module::IStateBehaviour
	{
	public:
		PlayerMovementState();
		~PlayerMovementState();
		std::shared_ptr<IStateBehaviour> Clone() override;
		void OnStateEnter(fq::game_module::Animator& animator, fq::game_module::AnimationStateNode& state) override;
		
	private:
		entt::meta_handle GetHandle() override { return *this; }

	private:
		bool mbCanMovePlayer;
		bool mbOnRotation;

		friend void RegisterMetaData();
	};
}