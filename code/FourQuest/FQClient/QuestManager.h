#pragma once
#include "../FQGameModule/Component.h"
#include "../FQGameModule/GameModule.h"

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

		std::vector<Quest> GetMainQuests() const { return mMainQuests; }
		std::vector<Quest> GetSubQuests() const { return mSubQuests; }

	private:
		entt::meta_handle GetHandle() override { return *this; }
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

		void EventProcessKillMonster();

	private:
		// ���� �̺�Ʈ ó��
		game_module::EventHandler mKillMonsterHandler;

		// �ν����� ��
		StartQuests mStartQuests;

		std::vector<Quest> mMainQuests;
		std::vector<Quest> mSubQuests;

		// ��ũ��Ʈ ��
		Quest mCurMainQuest;

		std::vector<Quest> mCurSubQuest;
		friend void RegisterMetaData();
	};
}

