#pragma once
#include "../FQGameModule/Component.h"
#include "Quest.h"

namespace fq::client
{
	class Quest;

	class QuestManager : public game_module::Component
	{
	public:
		QuestManager();
		~QuestManager();

		std::vector<Quest> GetMainQuests() const { return mMainQuests; }
		std::vector<Quest> GetSubQuests() const { return mSubQuests; }

	private:
		entt::meta_handle GetHandle() override { return *this; }
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

	private:
		Quest mCurMainQuest;
		std::vector<Quest> mCurSubQuest;

		std::vector<Quest> mMainQuests;
		std::vector<Quest> mSubQuests;

		friend void RegisterMetaData();
	};
}

