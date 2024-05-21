#pragma once

#include "../FQGameModule/GameModule.h"

namespace fq::client
{
	class PlayerMovement :	public fq::game_module::Component
	{
	public:
		PlayerMovement();
		~PlayerMovement();
		
		void OnStart() override;

		void OnUpdate(float dt) override;

		/// <summary>
		/// ���纻�� ��ȯ�մϴ� 
		/// </summary>
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

	private:
		entt::meta_handle GetHandle() override { return *this; }

	private:
		game_module::Animator* mAnimator;
		game_module::CharacterController* mController;

	};


}