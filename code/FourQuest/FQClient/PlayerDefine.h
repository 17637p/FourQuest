#pragma once


namespace fq::client
{
	/// <summary>
	/// ��ȥ Ÿ��
	/// ��ȥ Ÿ���� ���ʿ� ������� �������� �����մϴ�
	/// </summary>
	enum class ESoulType
	{
		Sword, // �� ��ȥ 0
		Staff, // ���� ��ȥ 1
		Axe, // ���� ��ȥ 2
		Bow,  // Ȱ ��ȥ 3
		End
	};

	/// <summary>
	/// �÷��̾� �����ÿ� �ʿ��� ����
	/// </summary>
	struct PlayerInfo
	{
		unsigned int ControllerID; // ��Ʈ�ѷ� ���̵�
		ESoulType SoulType; // ��ȥŸ��
	};

	enum class EPlayerType
	{
		NONE,
		Soul,
		SoulDestoryed,
		LivingArmour,
		ArmourDestroyed,
	};

	/// <summary>
	/// ���� Ÿ�� 
	/// </summary>
	enum class EArmourType
	{
		Knight, // �� ����
		Magic, // ���� 
		Warrior, // ���� ?
		Archer, // �ü� 
		End
	};

	enum class EWeaponeMesh
	{
		Sword, // ��   
		Staff, // ���� 
		Axe,   // ���� 
		Bow,   // Ȱ 
		Shield, // ����
		End
	};

}
