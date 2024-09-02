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
	class PauseUI : public game_module::Component
	{
	public:
		PauseUI();
		~PauseUI();

		PauseUI(const PauseUI& other);
		PauseUI& operator=(const PauseUI& other);

		virtual void OnStart() override;
		virtual void OnUpdate(float dt) override;
		virtual void OnDestroy() override;

	private:
		void SetScaleScreen();
		void SetSelectBoxPosition(float dt);
		void ClickButton();
		void SpawnUIObject(fq::game_module::PrefabResource prefab);

		void EventProcessOffPopupRepause();

		entt::meta_handle GetHandle() override { return *this; }
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

	private:
		int mSelectButtonID; // 0: 계속하기, 1: 환경설정, 2: 미션중단, 3: 게임 종료

		game_module::GameObject* mSelectBackground;
		std::vector<game_module::GameObject*> mButtons; 
		float mUIAnimSpeed;

		bool mIsActive;
		fq::game_module::PrefabResource mRepauseUIPrefab;
		game_module::EventHandler mOffPopupRepauseHandler;

		fq::game_module::PrefabResource mSettingUIPrefab;

		game_module::ScreenManager* mScreenManager;

	private:
		friend void RegisterMetaData();
	};
}

