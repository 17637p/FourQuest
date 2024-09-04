#pragma once
#include <string>
#include <vector>
#include <directxtk/SimpleMath.h>

#include "MonsterDefine.h"
#include "../FQGameModule/GameModuleEnum.h"

namespace fq::client
{
	struct PreQuest
	{
		int preIndex = 0;
		bool preIsMain = true; // QuestType
	};

	struct QuestColliderTrigger
	{
		std::string colliderName = "";
		bool isAll;
	};

	struct MonsterKill
	{
		EMonsterType monsterType;
		int requestsNumber = 0;

		int curNumber = 0;
		bool isClear;
	};

	struct MonsterGroupKill
	{
		std::string monsterGroupName = "";

		int curNumber = 0;
		int groupMonsterNumber = 0;
	};

	struct Defence
	{
		std::string colliderName = "";
		float count = 0;

		float curCount = 0;
	};

	// UI ¾øÀ½ 
	struct ClearQuest
	{
		int clearIndex = 0;
		bool clearIsMain = true; // QuestType
	};
	
	struct QuestJoinCondition
	{
		std::vector<PreQuest> preQuestList;
		std::vector<QuestColliderTrigger> colliderTriggerList;
	};

	struct ObjectInteraction
	{
		game_module::ETag tag = game_module::ETag::Armour;
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

	struct RewardPortal
	{
		DirectX::SimpleMath::Vector3 position;
		std::string nextSceneName;
	};

	struct ArmourSpawn
	{

	};

	struct QuestReward
	{
		std::vector<RewardPortal> RewardPortalList;
	};

	class Quest
	{
	public:
		Quest();
		~Quest();

		int mIndex;
		std::string mName;
		bool mIsMain; // QuestType

		QuestJoinCondition mJoinConditionList;
		QuestClearCondition mclearConditionList;
		QuestReward mRewardList;
	};
}

