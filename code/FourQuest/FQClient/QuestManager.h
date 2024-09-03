#pragma once
#include "../FQGameModule/Component.h"
#include "../FQGameModule/GameModule.h"
#include "../FQGameModule/PrefabResource.h"

#include "Quest.h"

namespace fq::game_module
{
	class TextUI;
}

namespace fq::client
{
	struct StartSubQuestIndex
	{
		int index = 0;
	};

	struct StartQuests
	{
		int startMainQuestIndex = 0;
		std::vector<StartSubQuestIndex> startSubQuestIndex;
	};

	class QuestManager : public game_module::Component
	{
	public:
		QuestManager();
		~QuestManager();

		// 대입 연산자를 자동으로 만들어 주지 않으므로 추가 (EventHandler 때문인데 이유는 모르겠음)
		QuestManager(const QuestManager& other);
		QuestManager& operator=(const QuestManager& other);

		virtual void OnStart() override;
		virtual void OnUpdate(float dt) override;
		virtual void OnDestroy() override;

		std::vector<Quest> GetMainQuests() const { return mMainQuests; }
		std::vector<Quest> GetSubQuests() const { return mSubQuests; }

		void ViewQuestInformation(Quest quest, game_module::TextUI* textUI);
		void RenderOffQuest();

	private:
		entt::meta_handle GetHandle() override { return *this; }
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

		void SetScaleAndPositionScreen();

		void EventProcessKillMonster();
		void EventProcessPlayerCollideTrigger();
		void EventProcessCompleteDefence();
		void EventProcessClearQuest();
		void EventProcessAllColliderTrigger();
		void EventProcessObjectInteraction();

	private:
		// 인스펙터 용
		StartQuests mStartQuests;

		std::vector<Quest> mMainQuests;
		std::vector<Quest> mSubQuests;

		// 스크립트 용
		Quest mCurMainQuest;
		std::vector<Quest> mCurSubQuest;

		fq::game_module::PrefabResource mPortalPrefab;

		/// OnStart 시 채워주는 것들 (복사 안해도 되는 거)
		// 각종 이벤트 처리
		game_module::EventHandler mKillMonsterHandler;
		game_module::EventHandler mPlayerCollideTriggerHandler;
		game_module::EventHandler mCompleteDefenceHandler;
		game_module::EventHandler mClearQuestHandler;
		game_module::EventHandler mAllCollideTriggerHandler;
		game_module::EventHandler mObjectInteractionHandler;

		// TextUI
		game_module::TextUI* mMainQuestText;
		std::vector<game_module::TextUI*> mSubQuestTexts;

		// Gauge Width
		int mGaugeMaxWidth;
		int mFontSize;
		game_module::ScreenManager* mScreenManager;

		friend void RegisterMetaData();
	};
}

