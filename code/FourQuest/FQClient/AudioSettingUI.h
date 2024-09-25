#pragma once
#include "../FQGameModule/Component.h"
#include "../FQGameModule/PrefabResource.h"
#include "../FQGameModule/EventHandler.h"

namespace fq::game_module
{
	class ScreenManager;
	class TimeManager;
	class SoundManager;
	class TextUI;
	class ImageUI;
	class SpriteAnimationUI;
}

namespace fq::client
{
	class AudioSettingUI : public game_module::Component
	{
	public:
		AudioSettingUI();
		~AudioSettingUI();

		AudioSettingUI(const AudioSettingUI& other);
		AudioSettingUI& operator=(const AudioSettingUI& other);

		virtual void OnStart() override;
		virtual void OnUpdate(float dt) override;
		virtual void OnDestroy() override;

	private:
		entt::meta_handle GetHandle() override { return *this; }
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

		void setScaleScreen();
		void setSelectBoxPosition(float dt);
		void setMute(int index);
		void setSoundBar(int index);
		void setSound(int index, bool isUp);

		void processInput(float dt);

		void initAudioSetting();
		void spawnUIObject(fq::game_module::PrefabResource prefab);
		bool isChangeSettingData();
		void saveSettingData();

		void eventProcessOffPopupReset();
		void eventProcessOffPopupSave();

		std::string wstringToString(std::wstring wStr);

	private:
		// 맨 위에 있는지 (입력을 받을지)
		bool mIsActive;
		bool mHaveToDestroyed;

		fq::game_module::PrefabResource mResetMessagePrefab;
		fq::game_module::PrefabResource mSaveMessagePrefab;
		fq::game_module::PrefabResource mSettingUIPrefab;
		game_module::EventHandler mOffPopupResetHandler;
		game_module::EventHandler mOffPopupSaveHandler;

		// 자주 쓰는 매니저
		game_module::ScreenManager* mScreenManager;
		game_module::TimeManager* mTimeManager;
		game_module::SoundManager* mSoundManager;

		// 선택한 버튼 위치 이동
		int mSelectButtonID; // 0: 전체 음량, 1: 배경음, 2: 효과음, 3: 음성
		game_module::GameObject* mSelectBackground;
		std::vector<game_module::GameObject*> mButtons;

		game_module::ImageUI* mSelectButtonBackground;
		game_module::SpriteAnimationUI* mSelectButtonSoulIcon;

		// Button Animation
		float mStickDelay;
		float mCurStickDelay;
		float mUIAnimSpeed;

		// 설명 Text
		std::vector<std::string> mExplanationTexts;
		game_module::TextUI* mExplanationTextUI;

		// Mute
		std::vector<bool> mIsMutes;
		std::vector<game_module::ImageUI*> mMuteCheckImages;
		// sound
		std::vector<float> mSoundRatio;
		float mSoundBarWidth;
		std::vector<float> mSoundLeftInputTimes; // i
		std::vector<float> mSoundRightInputTimes;
		std::vector<game_module::ImageUI*> mSoundBars;
		std::vector<game_module::Transform*> mSoundBarIcons;
		std::vector<game_module::TextUI*> mSoundPercentTexts;

		// 비교를 위한 저장용
		std::vector<bool> mSaveISMutes;
		std::vector<float> mSaveSoundRatio;

	private:
		friend void RegisterMetaData();
	};
}

