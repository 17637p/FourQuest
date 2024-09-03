#pragma once
#include "../FQGameModule/Component.h"

namespace fq::game_module
{
	class ScreenManager;
}

namespace fq::client
{
	class RepauseUI : public game_module::Component
	{
	public:
		RepauseUI();
		~RepauseUI();

		virtual void OnStart() override;
		virtual void OnUpdate(float dt) override;

	private:
		void SetScaleScreen();

		entt::meta_handle GetHandle() override { return *this; }
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

	private:
		game_module::ScreenManager* mScreenManager;
	};
}
