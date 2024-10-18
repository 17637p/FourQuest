#pragma once

#include "PlayerDefine.h"
#include "MonsterDefine.h"

#include "../FQGameModule/GameModule.h"
#include "Quest.h"

namespace fq::client::event
{
	/// <summary>
	/// 플레이어 등록 이벤트 
	/// </summary>
	struct RegisterPlayer
	{
		game_module::GameObject* player;
		EPlayerType type;
	};

	/// <summary>
	/// 플레이어의 상태를 업데이트 해주는 이벤트
	/// </summary>
	struct UpdatePlayerState
	{
		int playerID;
		EPlayerType type;
	};

	/// <summary>
	/// 플레이어의 갑옷이 깨졌을 경우에 영혼 생성
	/// </summary>
	struct SummonSoul
	{
		int id;
		ESoulType soulType;
		DirectX::SimpleMath::Matrix worldTransform;
		game_module::PrefabResource soulPrefab;
		bool isDestroyArmour;
		float SoulGauge;
		float MaxSoulGauge;
	};

	/// <summary>
	/// 몬스터를 죽였을 때 발생하는 이벤트 
	/// </summary>
	struct KillMonster
	{
		EMonsterType monsterType;
	};

	/// <summary>
	/// 플레이어가 트리거에 부딪힐 때마다 발생하는 이벤트
	/// </summary>
	struct PlayerCollideTrigger
	{
		int playerNumber;
		std::string colliderName;
	};

	/// <summary>
	/// 플레이어가 트리거에 부딪히는 중에 발생하는 이벤트
	/// </summary>
	struct PlayerCollideStayTrigger
	{
		int playerNumber;
		std::string colliderName;
	};

	/// <summary>
	/// 디펜스를 진행중일 때 발생하는 이벤트
	/// </summary>
	struct InProgressDefence
	{
		std::string colliderName;
		int curCount;
	};

	/// <summary>
	/// 디펜스 게이지가 차고있을때
	/// </summary>
	struct InProgressDefenceUp
	{
		std::string colliderName;
	};

	/// <summary>
	/// 디펜스를 완료했을 때 발생하는 이벤트
	/// </summary>
	struct CompleteDefence
	{
		std::string colliderName;
	};

	/// <summary>
	/// 퀘스트를 완료했을 때 발생하는 이벤트
	/// </summary>
	struct ClearQuestEvent
	{
		Quest clearQuest;
		int index; // 몇 번째 서브퀘스트인지
	};

	/// <summary>
	/// 모든 플레이어가 퀘스트에 필요한 콜라이더에 충돌중일 때
	/// </summary>
	struct AllCollideTrigger
	{
		std::string colliderName;
	};

	/// <summary>
	/// 콜라이더 충돌 인원에 변화가 있을 때 
	/// </summary>
	struct ChangePlayerNumCollideTrigger
	{
		std::string colliderName;
		int maxPlayer;
		int curPlayer;
	};

	/// <summary>
	/// 플레이어가 오브젝트(갑옷, 여신상)와 상호작용할 때 발생하는 이벤트
	/// </summary>
	struct ObjectInteractionEvent
	{
		game_module::ETag tag;
	};

	/// <summary>
	/// 플레이어가 오브젝트(갑옷, 여신상)와 상호작용할 때 발생하는 이벤트
	/// </summary>
	struct ClearGoddessStatue
	{
		std::string goddessStatueName;
	};

	/// <summary>
	/// 진행 중인 퀘스트를 알리는 이벤트
	/// </summary>
	struct CurrentQuest
	{
		bool isMain;
		int questIndex;
	};

	/// <summary>
	/// Repause 팝업 창 Off 이벤트
	/// </summary>
	struct OffPopupRepause
	{
	};

	/// <summary>
	/// Setting 팝업 창 Off 이벤트
	/// </summary>
	struct OffPopupSetting
	{
	};

	/// <summary>
	/// Pause 팝업 창 Off 이벤트
	/// </summary>
	struct OffPopupPause
	{
	};

	/// <summary>
	/// Reset 팝업 창 Off 이벤트
	/// </summary>
	struct OffPopupReset
	{
		bool isReset;
	};

	/// <summary>
	/// Save 팝업 창 Off 이벤트
	/// </summary>
	struct OffPopupSave
	{
		bool isSave;
	};

	/// <summary>
	/// Player HUD 체력 감소 연출을 위한 DecreaseHPRatio 값 전달 
	/// </summary>
	struct DecreaseHPRatio
	{
		int playerID;
		float ratio;
	};

	/// <summary>
	/// CoolTime 다 됐을 때 알림
	/// </summary>
	struct InitCoolTime
	{
		int playerID;
		EArmourType mArmourType;
		char skillKey; // 'X' 'A' 'R'
	};
}