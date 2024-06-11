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
		/// 복사본을 반환합니다 
		/// </summary>
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

	private:
		entt::meta_handle GetHandle() override { return *this; }

	private:
		

	};

}
