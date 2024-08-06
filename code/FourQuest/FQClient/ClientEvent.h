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
	/// �÷��̾ ������Ʈ(����, ���Ż�)�� ��ȣ�ۿ��� �� �߻��ϴ� �̺�Ʈ
	/// </summary>
	struct ObjectInteractionEvent
	{
		game_module::ETag tag;
	};
}