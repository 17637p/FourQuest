#pragma once

#include "../FQGameModule/GameModule.h"

namespace fq::client
{
	class Player :	public fq::game_module::Component
	{
	public:
		Player();
		~Player();
		
		void OnStart() override;

		void OnUpdate(float dt) override;

		/// <summary>
		/// 복사본을 반환합니다 
		/// </summary>
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

		float GetHp() const { return mHp; }
		void SetHp(float hp) { mHp = hp; }

	private:
		void processDash();

		entt::meta_handle GetHandle() override { return *this; }

	private:
		game_module::Animator* mAnimator;
		game_module::CharacterController* mController;

		float mHp;
	};


}