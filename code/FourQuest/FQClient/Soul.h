#pragma once

#include "../FQGameModule/GameModule.h"


namespace fq::client
{
	class DeadArmour;

	class Soul :public game_module::Component
	{
	public:
		Soul();
		~Soul();
		
		/// <summary>
		/// 복사본을 반환합니다 
		/// </summary>
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;
		void DestorySoul();

	private:
		void OnStart()override;
		void OnTriggerEnter(const fq::game_module::Collision& collision) override;
		void OnTriggerExit(const fq::game_module::Collision& collision) override;
		void OnUpdate(float dt) override;

		entt::meta_handle GetHandle() override { return *this; }

	private:
		game_module::CharacterController* mController;
		std::vector<DeadArmour*> mSelectArmours;
	};

}
