#pragma once

#include "PlayerDefine.h"
#include "MonsterDefine.h"

#include "../FQGameModule/GameModule.h"
#include "Quest.h"

namespace fq::client::event
{
	/// <summary>
	/// �÷��̾� ��� �̺�Ʈ 
	/// </summary>
	struct RegisterPlayer
	{
		game_module::GameObject* player;
		EPlayerType type;
	};

	/// <summary>
	/// �÷��̾��� ���¸� ������Ʈ ���ִ� �̺�Ʈ
	/// </summary>
	struct UpdatePlayerState
	{
		int playerID;
		EPlayerType type;
	};

	/// <summary>
	/// �÷��̾��� ������ ������ ��쿡 ��ȥ ����
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
	/// ���͸� �׿��� �� �߻��ϴ� �̺�Ʈ 
	/// </summary>
	struct KillMonster
	{
		EMonsterType monsterType;
	};

	/// <summary>
	/// �÷��̾ Ʈ���ſ� �ε��� ������ �߻��ϴ� �̺�Ʈ
	/// </summary>
	struct PlayerCollideTrigger
	{
		int playerNumber;
		std::string colliderName;
	};

	/// <summary>
	/// �÷��̾ Ʈ���ſ� �ε����� �߿� �߻��ϴ� �̺�Ʈ
	/// </summary>
	struct PlayerCollideStayTrigger
	{
		int playerNumber;
		std::string colliderName;
	};

	/// <summary>
	/// ���潺�� �������� �� �߻��ϴ� �̺�Ʈ
	/// </summary>
	struct InProgressDefence
	{
		std::string colliderName;
		int curCount;
	};

	/// <summary>
	/// ���潺 �������� ����������
	/// </summary>
	struct InProgressDefenceUp
	{
		std::string colliderName;
	};

	/// <summary>
	/// ���潺�� �Ϸ����� �� �߻��ϴ� �̺�Ʈ
	/// </summary>
	struct CompleteDefence
	{
		std::string colliderName;
	};

	/// <summary>
	/// ����Ʈ�� �Ϸ����� �� �߻��ϴ� �̺�Ʈ
	/// </summary>
	struct ClearQuestEvent
	{
		Quest clearQuest;
		int index; // �� ��° ��������Ʈ����
	};

	/// <summary>
	/// ��� �÷��̾ ����Ʈ�� �ʿ��� �ݶ��̴��� �浹���� ��
	/// </summary>
	struct AllCollideTrigger
	{
		std::string colliderName;
	};

	/// <summary>
	/// �ݶ��̴� �浹 �ο��� ��ȭ�� ���� �� 
	/// </summary>
	struct ChangePlayerNumCollideTrigger
	{
		std::string colliderName;
		int maxPlayer;
		int curPlayer;
	};

	/// <summary>
	/// �÷��̾ ������Ʈ(����, ���Ż�)�� ��ȣ�ۿ��� �� �߻��ϴ� �̺�Ʈ
	/// </summary>
	struct ObjectInteractionEvent
	{
		game_module::ETag tag;
	};

	/// <summary>
	/// �÷��̾ ������Ʈ(����, ���Ż�)�� ��ȣ�ۿ��� �� �߻��ϴ� �̺�Ʈ
	/// </summary>
	struct ClearGoddessStatue
	{
		std::string goddessStatueName;
	};

	/// <summary>
	/// ���� ���� ����Ʈ�� �˸��� �̺�Ʈ
	/// </summary>
	struct CurrentQuest
	{
		bool isMain;
		int questIndex;
	};

	/// <summary>
	/// Repause �˾� â Off �̺�Ʈ
	/// </summary>
	struct OffPopupRepause
	{
	};

	/// <summary>
	/// Setting �˾� â Off �̺�Ʈ
	/// </summary>
	struct OffPopupSetting
	{
	};

	/// <summary>
	/// Pause �˾� â Off �̺�Ʈ
	/// </summary>
	struct OffPopupPause
	{
	};

	/// <summary>
	/// Reset �˾� â Off �̺�Ʈ
	/// </summary>
	struct OffPopupReset
	{
		bool isReset;
	};

	/// <summary>
	/// Save �˾� â Off �̺�Ʈ
	/// </summary>
	struct OffPopupSave
	{
		bool isSave;
	};

	/// <summary>
	/// Player HUD ü�� ���� ������ ���� DecreaseHPRatio �� ���� 
	/// </summary>
	struct DecreaseHPRatio
	{
		int playerID;
		float ratio;
	};

	/// <summary>
	/// CoolTime �� ���� �� �˸�
	/// </summary>
	struct InitCoolTime
	{
		int playerID;
		EArmourType mArmourType;
		char skillKey; // 'X' 'A' 'R'
	};
}