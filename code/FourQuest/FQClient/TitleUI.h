#pragma once
#include "../FQGameModule/Component.h"

namespace fq::game_module
{
	class ScreenManager;
}

namespace fq::client
{
	class TitleUI : public game_module::Component
	{
	public:
		TitleUI();
		~TitleUI();

		virtual void OnStart() override;
		virtual void OnUpdate(float dt) override;

	private:
		entt::meta_handle GetHandle() override { return *this; }
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

	private:
		int mSelectButtonID; // 0: 계속하기, 1: 환경설정, 2: 미션중단, 3: 게임 종료

		game_module::GameObject* mSelectBackground;
		std::vector<game_module::GameObject*> mButtons;

		game_module::ScreenManager* mScreenManager;
	};
}

