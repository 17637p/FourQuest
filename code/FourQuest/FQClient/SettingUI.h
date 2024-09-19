#pragma once
#include "../FQGameModule/Component.h"

namespace fq::game_module
{
	class ScreenManager;
}

namespace fq::client
{
	class SettingUI : public game_module::Component
	{
	public:
		SettingUI();
		~SettingUI();

		virtual void OnStart() override;
		virtual void OnUpdate(float dt) override;

	private:
		entt::meta_handle GetHandle() override { return *this; }
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

		void SetScaleScreen();

	private:
		game_module::ScreenManager* mScreenManager;

	private:
		friend void RegisterMetaData();
	};
}

