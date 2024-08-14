#pragma once
#include "../FQGameModule/Component.h"
#include "../FQGameModule/GameModule.h"
#include "../FQGameModule/TextUI.h"

#include "Quest.h"

namespace fq::client
{
	struct StartSubQuestIndex
	{
		int index;
	};

	struct StartQuests
	{
		int startMainQuestIndex;
		std::vector<StartSubQuestIndex> startSubQuestIndex;
	};

	class QuestManager : public game_module::Component
	{
	public:
		QuestManager();
		~QuestManager();

		// ���� �����ڸ� �ڵ����� ����� ���� �����Ƿ� �߰� (EventHandler �����ε� ������ �𸣰���)
		QuestManager(const QuestManager& other);
		QuestManager& operator=(const QuestManager& other);

		virtual void OnStart() override;
		virtual void OnUpdate(float dt) override;
		virtual void OnDestroy() override;

		std::vector<Quest> GetMainQuests() const { return mMainQuests; }
		std::vector<Quest> GetSubQuests() const { return mSubQuests; }

		void ViewQuestInformation(Quest quest, game_module::TextUI* textUI);

	private:
		entt::meta_handle GetHandle() override { return *this; }
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

		void EventProcessKillMonster();
		void EventProcessPlayerCollideTrigger();
		void EventProcessCompleteDefence();
		void EventProcessClearQuest();
		void EventProcessAllColliderTrigger();
		void EventProcessObjectInteraction();

	private:
		// ���� �̺�Ʈ ó��
		game_module::EventHandler mKillMonsterHandler;
		game_module::EventHandler mPlayerCollideTriggerHandler;
		game_module::EventHandler mCompleteDefenceHandler;
		game_module::EventHandler mClearQuestHandler;
		game_module::EventHandler mAllCollideTriggerHandler;
		game_module::EventHandler mObjectInteractionHandler;

		// �ν����� ��
		StartQuests mStartQuests;

		std::vector<Quest> mMainQuests;
		std::vector<Quest> mSubQuests;

		// ��ũ��Ʈ ��
		Quest mCurMainQuest;
		std::vector<Quest> mCurSubQuest;

		game_module::TextUI* mMainQuestText;
		std::vector<game_module::TextUI*> mSubQuestTexts;

		// Gauge Width
		int mGaugeMaxWidth;
		int mFontSize;
		game_module::ScreenManager* mScreenManager;

		friend void RegisterMetaData();
	};
}
