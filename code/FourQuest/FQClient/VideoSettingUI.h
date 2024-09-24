#pragma once
#include "../FQGameModule/Component.h"
#include "../FQGameModule/PrefabResource.h"
#include "../FQGameModule/EventHandler.h"

namespace fq::game_module
{
	class ScreenManager;
	class TimeManager;
	class TextUI;
	class ImageUI;
	class SpriteAnimationUI;
}

namespace fq::client
{
	class VideoSettingUI : public game_module::Component
	{
	public:
		VideoSettingUI();
		~VideoSettingUI();

		VideoSettingUI(const VideoSettingUI& other);
		VideoSettingUI& operator=(const VideoSettingUI& other);

		virtual void OnStart() override;
		virtual void OnUpdate(float dt) override;
		virtual void OnDestroy() override;

	private:
		entt::meta_handle GetHandle() override { return *this; }
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

		void setScaleScreen();
		void setSelectBoxPosition(float dt);
		void setRadio();
		void setResolutionTextUI(int width, int height);

		void processInput(float dt);

		void initGameSetting();
		void spawnUIObject(fq::game_module::PrefabResource prefab);
		bool isChangeSettingData();
		void saveSettingData();

		void eventProcessOffPopupReset();
		void eventProcessOffPopupSave();

		std::string wstringToString(std::wstring wStr);

	private:
		// �� ���� �ִ��� (�Է��� ������)
		bool mIsActive;
		bool mHaveToDestroyed;

		fq::game_module::PrefabResource mResetMessagePrefab;
		fq::game_module::PrefabResource mSaveMessagePrefab;
		fq::game_module::PrefabResource mSettingUIPrefab;
		game_module::EventHandler mOffPopupResetHandler;
		game_module::EventHandler mOffPopupSaveHandler;

		// ���� ���� �Ŵ���
		game_module::ScreenManager* mScreenManager;
		game_module::TimeManager* mTimeManager;

		// ������ ��ư ��ġ �̵�
		int mSelectButtonID; // 0: ��Ʈ�ѷ� ����, 1: ���� ����, 2: �Ʊ� ���� ���
		game_module::GameObject* mSelectBackground;
		std::vector<game_module::GameObject*> mButtons;

		game_module::ImageUI* mSelectButtonBackground;
		game_module::SpriteAnimationUI* mSelectButtonSoulIcon;

		// Button Animation
		float mStickDelay;
		float mCurStickDelay;
		float mUIAnimSpeed;

		// ���� Text
		std::vector<std::string> mExplanationTexts;
		game_module::TextUI* mExplanationTextUI;

		std::vector<game_module::ImageUI*> mRadioOnImage;
		bool mIsWindow;
		game_module::TextUI* mResolutionTextUI;
		std::vector<DirectX::SimpleMath::Vector2> mResolutions;

		// �񱳸� ���� �����
		bool mSaveIsWindow;
		DirectX::SimpleMath::Vector2 mSaveResolution;

	private:
		friend void RegisterMetaData();
	};
}

