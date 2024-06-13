#pragma once

#include "../FQGameModule/GameModule.h"


namespace fq::client
{
	class Soul :public game_module::Component
	{
	public:
		Soul();
		~Soul();
		
		/// <summary>
		/// ���纻�� ��ȯ�մϴ� 
		/// </summary>
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

		void OnStart()override;

		void OnTriggerStay(const fq::game_module::Collision& collision) override;
		
		void DestorySoul();

	private:
		entt::meta_handle GetHandle() override { return *this; }


	private:
		game_module::CharacterController* mController;

	};

}
