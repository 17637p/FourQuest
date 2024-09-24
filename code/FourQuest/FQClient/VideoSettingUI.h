#pragma once
#include "../FQGameModule/Component.h"

namespace fq::game_module
{
	class ScreenManager;
	class TimeManager;
	class TextUI;
}

namespace fq::client
{
	class VideoSettingUI : public game_module::Component
	{
	public:
		VideoSettingUI();
		~VideoSettingUI();

		virtual void OnStart() override;
		virtual void OnUpdate(float dt) override;

	private:
		entt::meta_handle GetHandle() override { return *this; }
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

		void setScaleScreen();

	private:
		game_module::ScreenManager* mScreenManager;
		game_module::TimeManager* mTimeManager;

		// Ό³Έν Text
		std::vector<std::string> mExplanationTexts;
		game_module::TextUI* mExplanationTextUI;

	private:
		friend void RegisterMetaData();
	};
}

