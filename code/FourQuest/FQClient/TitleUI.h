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

		void SetScaleScreen();
		void SetSelectBoxPosition(float dt);
		void ClickButton();

	private:
		game_module::ScreenManager* mScreenManager;
		
		int mSelectButtonID; // 0: 게임 시작, 1: 설정, 2: 제작진, 3: 종료
		float mUIAnimSpeed;
		game_module::GameObject* mSelectBackground;
		std::vector<game_module::GameObject*> mButtons;

		std::string mNextSceneName;

	private:
		friend void RegisterMetaData();
	};
}

