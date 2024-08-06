#pragma once
#include <string>
#include <vector>

#include "MonsterDefine.h"
#include "../FQGameModule/GameModuleEnum.h"

namespace fq::client
{
	struct PreQuest
	{
		int preIndex;
		bool preIsMain = true; // QuestType
	};

	struct QuestColliderTrigger
	{
		std::string colliderName;
		bool isAll;
	};

	struct MonsterKill
	{
		EMonsterType monsterType;
		int requestsNumber;

		int curNumber = 0;
		bool isClear = false;
	};

	struct MonsterGroupKill
	{
		int groupIndex;

		int curNumber = 0;
		int groupMonsterNumber = 0;
	};

	struct Defence
	{
		std::string colliderName;
		float seconds;

		float curSeconds = 0;
	};

	// UI ¾øÀ½ 
	struct ClearQuest
	{
		int clearIndex;
		bool clearIsMain = true; // QuestType
	};
	
	struct QuestJoinCondition
	{
		std::vector<PreQuest> preQuestList;
		std::vector<QuestColliderTrigger> colliderTriggerList;
	};

	struct ObjectInteraction
	{
		game_module::ETag tag;
	};
	
	struct QuestClearCondition
	{
		std::vector<MonsterKill> monsterKillList;
		std::vector<MonsterGroupKill> monsterGroupKillList;
		std::vector<Defence> defenceList;
		std::vector<ClearQuest> clearQuestList;
		std::vector<QuestColliderTrigger> colliderTriggerList;
		std::vector<ObjectInteraction> objectInteration;
	};

	class Quest
	{
	public:
		Quest();
		~Quest();

	private:
		int mIndex;
		std::string mName;
		bool mIsMain; // QuestType

		QuestJoinCondition mJoinConditionList;
		QuestClearCondition mclearConditionList;
		
		friend void RegisterMetaData();
		friend class QuestManager;
	};
}

