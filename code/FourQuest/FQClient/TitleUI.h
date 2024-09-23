#pragma once
#include "../FQGameModule/Component.h"
#include "../FQGameModule/PrefabResource.h"
#include "../FQGameModule/EventHandler.h"

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

		TitleUI(const TitleUI& other);
		TitleUI& operator=(const TitleUI& other);

		virtual void OnStart() override;
		virtual void OnUpdate(float dt) override;

	private:
		entt::meta_handle GetHandle() override { return *this; }
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

		// 스크린 비율 맞춰서 Scale Set
		void setScaleScreen();
		// 선택한 버튼에 따라 Select Box Pos Set
		void setSelectBoxPosition(float dt);
		// 버튼 선택 시 로직
		void clickButton();

		// Prefab 소환
		void spawnUIObject(fq::game_module::PrefabResource prefab);

		// Setting Popup Off 시 처리할 것
		void eventProcessOffPopupSetting();

		// 입력 처리 
		void processInput();

	private:
		game_module::ScreenManager* mScreenManager;
		
		int mSelectButtonID; // 0: 게임 시작, 1: 설정, 2: 제작진, 3: 종료
		float mUIAnimSpeed;
		game_module::GameObject* mSelectBackground;
		std::vector<game_module::GameObject*> mButtons;

		std::string mNextSceneName;

		bool mIsActive;
		fq::game_module::PrefabResource mSettingUIPrefab;
		game_module::EventHandler mOffPopupSettingHandler;

		// Button Animation
		float mStickDelay;
		float mCurStickDelay;

	private:
		friend void RegisterMetaData();
	};
}

