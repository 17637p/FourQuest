#pragma once
#include "../FQGameModule/Component.h"

namespace fq::game_module
{
	class ScreenManager;
	class TimeManager;
	class TextUI;
	class ImageUI;
}

namespace fq::client
{
	class AudioSettingUI : public game_module::Component
	{
	public:
		AudioSettingUI();
		~AudioSettingUI();

		virtual void OnStart() override;
		virtual void OnUpdate(float dt) override;

	private:
		entt::meta_handle GetHandle() override { return *this; }
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

		void setScaleScreen();
		void setSelectBoxPosition(float dt);
		void setMute(int index, bool isMute);
		void setSoundBar(int index);
		void initAudioSetting();

		void processInput(float dt);

		void setSound(int index, bool isUp);

		std::string wstringToString(std::wstring wStr);

	private:
		game_module::ScreenManager* mScreenManager;
		game_module::TimeManager* mTimeManager;

		// 선택한 버튼 위치 이동
		int mSelectButtonID; // 0: 전체 음량, 1: 배경음, 2: 효과음, 3: 음성
		game_module::GameObject* mSelectBackground;
		std::vector<game_module::GameObject*> mButtons;

		// Button Animation
		float mStickDelay;
		float mCurStickDelay;
		float mUIAnimSpeed;

		// 설명 Text
		std::vector<std::string> mExplanationTexts;
		game_module::TextUI* mExplanationTextUI;

		// Mute
		std::vector<bool> mIsMute;
		std::vector<float> mSoundRatio;
		float mSoundBarWidth;
		std::vector<float> mSoundLeftInputTimes; // i
		std::vector<float> mSoundRightInputTimes;
		std::vector<game_module::ImageUI*> mSoundBars;
		std::vector<game_module::Transform*> mSoundBarIcons;
		std::vector<game_module::ImageUI*> mMuteCheckImages;
		std::vector<game_module::TextUI*> mSoundPercentTexts;

	private:
		friend void RegisterMetaData();
	};
}

