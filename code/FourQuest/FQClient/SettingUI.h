#pragma once
#include "../FQGameModule/Component.h"
#include "../FQGameModule/PrefabResource.h"
#include "../FQGameModule/EventHandler.h"

namespace fq::game_module
{
	class ScreenManager;
	class TimeManager;
	class TextUI;
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

		void setScaleScreen();
		void setSelectBoxPosition(float dt);
		void clickButton();

		void processInput();

		std::string wstringToString(std::wstring wStr);

	private:
		game_module::ScreenManager* mScreenManager;
		game_module::TimeManager* mTimeManager;

		// 선택한 버튼 위치 이동
		int mSelectButtonID; // 0: 게임, 1: 비디오, 2: 오디오, 3: 돌아가기
		game_module::GameObject* mSelectBackground;
		std::vector<game_module::GameObject*> mButtons;

		// Button Animation
		float mStickDelay;
		float mCurStickDelay;
		float mUIAnimSpeed;

		// 설명 Text
		std::vector<std::string> mExplanationTexts;
		game_module::TextUI* mExplanationTextUI;

	private:
		friend void RegisterMetaData();
	};
}

