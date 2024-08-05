#pragma once
#include <string>
#include <vector>

#include "MonsterDefine.h"

namespace fq::client
{
	struct PreQuest
	{
		int preIndex;
		bool preIsMain; // QuestType
	};

	struct QuestColliderTrigger
	{
		std::string colliderName;
	};

	struct MonsterKill
	{
		EMonsterType monsterType;
		int requestsNumber;
	};

	struct MonsterGroupKill
	{
		int groupIndex;
	};

	struct Defence
	{
		std::string colliderName;
		float seconds;
	};

	// UI ¾øÀ½ 
	struct ClearQuest
	{
		int clearIndex;
		bool clearIsMain; // QuestType
	};
	
	struct QuestJoinCondition
	{
		std::vector<PreQuest> preQuestList;
		std::vector<QuestColliderTrigger> colliderTriggerList;
	};
	
	struct QuestClearCondition
	{
		std::vector<MonsterKill> monsterKillList;
		std::vector<MonsterGroupKill> monsterGroupKillList;
		std::vector<Defence> defenceList;
		std::vector<ClearQuest> clearQuestList;
	};

	class Quest
	{
	public:
		Quest();
		~Quest();

	private:
		int mIndex;
		bool mIsMain; // QuestType
		
		std::vector<PreQuest> preQuestList;
		std::vector<QuestColliderTrigger> colliderTriggerList;

		std::vector<MonsterKill> monsterKillList;
		std::vector<MonsterGroupKill> monsterGroupKillList;
		std::vector<Defence> defenceList;
		std::vector<ClearQuest> clearQuestList;

		std::vector<QuestJoinCondition> mJoinConditionList;
		std::vector<QuestClearCondition> mclearConditionList;
		
		friend void RegisterMetaData();
	};
}

