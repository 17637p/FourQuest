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

		// 대입 연산자를 자동으로 만들어 주지 않으므로 추가 (EventHandler 때문인데 이유는 모르겠음)
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
		// 각종 이벤트 처리
		game_module::EventHandler mKillMonsterHandler;

		// 인스펙터 용
		StartQuests mStartQuests;

		std::vector<Quest> mMainQuests;
		std::vector<Quest> mSubQuests;

		// 스크립트 용
		Quest mCurMainQuest;

		std::vector<Quest> mCurSubQuest;
		friend void RegisterMetaData();
	};
}

