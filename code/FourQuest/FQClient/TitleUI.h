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

		// ��ũ�� ���� ���缭 Scale Set
		void setScaleScreen();
		// ������ ��ư�� ���� Select Box Pos Set
		void setSelectBoxPosition(float dt);
		// ��ư ���� �� ����
		void clickButton();

		// Prefab ��ȯ
		void spawnUIObject(fq::game_module::PrefabResource prefab);

		// Setting Popup Off �� ó���� ��
		void eventProcessOffPopupSetting();

		// �Է� ó�� 
		void processInput();

	private:
		game_module::ScreenManager* mScreenManager;
		
		int mSelectButtonID; // 0: ���� ����, 1: ����, 2: ������, 3: ����
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

