#pragma once
#include "../FQGameModule/Component.h"
#include "../FQGameModule/GameModule.h"
#include "../FQGameModule/PrefabResource.h"

#include "Quest.h"

#include <list>

namespace fq::game_module
{
	class TextUI;
	class ImageUI;
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

		// ���� �����ڸ� �ڵ����� ����� ���� �����Ƿ� �߰� (EventHandler �����ε� ������ �𸣰���)
		QuestManager(const QuestManager& other);
		QuestManager& operator=(const QuestManager& other);

		virtual void OnStart() override;
		virtual void OnUpdate(float dt) override;
		virtual void OnDestroy() override;

		std::vector<Quest> GetMainQuests() const { return mMainQuests; }
		std::vector<Quest> GetSubQuests() const { return mSubQuests; }

		void ViewQuestInformation(Quest quest, game_module::TextUI* textUI);
		void RenderOnSubQuest(int i, bool isOn);
		void SpawnArmour(fq::game_module::PrefabResource armour);

	private:
		entt::meta_handle GetHandle() override { return *this; }
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

		void setScaleAndPositionScreen();

		void eventProcessKillMonster();
		void eventProcessPlayerCollideTrigger();
		void eventProcessCompleteDefence();
		void eventProcessClearQuest();
		void eventProcessAllColliderTrigger();
		void eventProcessObjectInteraction();
		void eventProcessClearGoddessStatue();

		void playNew(float dt);
		void playComplete(float dt);

	private:
		// �ν����� ��
		StartQuests mStartQuests;

		std::vector<Quest> mMainQuests;
		std::vector<Quest> mSubQuests;

		float mDistance;

		// ��ũ��Ʈ ��
		Quest mCurMainQuest;
		Quest mViewMainQuest; // ����� UI ������ ���� ����Ʈ 
		std::vector<Quest> mCurSubQuest;
		std::vector<Quest> mViewSubQuest; // ����� UI ������ ���� ����Ʈ ��� 
		std::list<Quest> mNextSubQuests;

		fq::game_module::PrefabResource mPortalPrefab;

		/// OnStart �� ä���ִ� �͵� (���� ���ص� �Ǵ� ��)
		// ���� �̺�Ʈ ó��
		game_module::EventHandler mKillMonsterHandler;
		game_module::EventHandler mPlayerCollideTriggerHandler;
		game_module::EventHandler mCompleteDefenceHandler;
		game_module::EventHandler mClearQuestHandler;
		game_module::EventHandler mAllCollideTriggerHandler;
		game_module::EventHandler mObjectInteractionHandler;
		game_module::EventHandler mClearGoddessStatueHandler;

		// TextUI
		game_module::TextUI* mMainQuestText;
		std::vector<game_module::TextUI*> mSubQuestTexts;

		// �ű� ����Ʈ ����
		std::vector<game_module::ImageUI*> mNewImages;
		std::vector<float> mNewImageCounts;

		std::vector<game_module::ImageUI*> mCompleteImages;
		std::vector<float> mCompleteImageCounts;
		std::vector<game_module::ImageUI*> mLeftChecks;
		std::vector<game_module::ImageUI*> mRightChecks;
		std::vector<bool> mIsFinishedCompleteAnimation;
		std::vector<game_module::ImageUI*> mQuestBoxes;

		// Gauge Width
		int mGaugeMaxWidth;
		int mFontSize;
		game_module::ScreenManager* mScreenManager;

		// 
		int mDebug;

		std::vector<std::shared_ptr<game_module::GameObject>> mAddedArmourObjects;

		friend void RegisterMetaData();
	};
}

