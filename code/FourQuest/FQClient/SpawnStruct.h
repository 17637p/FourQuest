#pragma once
#include <string>
#include <vector>
#include <directxtk/SimpleMath.h>

#include "MonsterDefine.h"

namespace fq::client
{
	/// <summary>
	/// SpawnCondition
	/// </summary>
	struct SpawnColliderTrigger
	{
		std::string ColliderName;
		bool isClear = false;
	};

	enum class EComparisonOperators
	{
		Equals,
		Greater,
		Less
	};

	struct MaxEnemy
	{
		EComparisonOperators ComparisonOperator;
		int MonsterNum = 0;

		bool isClear = false;
	};

	// 오브젝트가 생성되거나, 이벤트가 실행되었을 때
	struct Timer
	{
		float Second = 0;

		float curSecond = 0;
		bool isClear = false;
	};

	struct ObjectLive
	{
		bool isLive = true;
		std::string ObjectOrPrefabName = ""; // 전체 오브젝트 중에 이 이름을 가진 오브젝트를 탐색

		bool isClear = false;
	};

	struct InProgressQuest
	{
		bool isMain;
		int QuestIndex;

		bool isClear = false;
	};

	struct InProgressDefence
	{
		std::string ColliderName;
		int Count;

		bool isClear = false;
	};

	struct BossSpawnCondition
	{
		bool isClear = false;
	};

	struct SpawnCondition
	{
		std::vector<SpawnColliderTrigger> SpawnColliderTriggerList;
		std::vector<MaxEnemy> MaxEnemyList;
		std::vector<Timer> TimerList;
		std::vector<ObjectLive> ObjectLiveList;
		std::vector<InProgressQuest> InProgressQuestList;
		std::vector<InProgressDefence> InProgressDefenceList;
		std::vector<BossSpawnCondition> BossSpwanList;
	};

	/// <summary>
	/// SpawnRule
	/// </summary>
	struct SpawnData
	{
		EMonsterType MonsterType = EMonsterType::Melee;
		std::string Name = "Monster_Group_0_";
		int SpawnerNum = 0;
		int SpawnMonsterNum = 0;
	};

	struct SpawnRule
	{
		std::vector<SpawnData> spawnData;
	};

	struct SpawnRules
	{
		bool isLoof;
		std::vector<SpawnRule> SpawnRuleList;
	};
}

