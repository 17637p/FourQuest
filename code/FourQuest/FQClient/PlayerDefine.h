#pragma once


namespace fq::client
{
	/// <summary>
	/// ��ȥ Ÿ��
	/// ��ȥ Ÿ���� ���ʿ� ������� �������� �����մϴ�
	/// </summary>
	enum class ESoulType
	{
		Sword, // �� ��ȥ   
		Axe, // ���� ��ȥ
		Bow,  // Ȱ ��ȥ
		Staff // ���� ��ȥ
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
		Soul,
		LivingArmour
	};

	/// <summary>
	/// ���� Ÿ�� 
	/// </summary>
	enum class EArmourType
	{
		Knight, // �� ����
		Magic, // ���� 
		Warrior, // ���� ?
		Archer // �ü� 
	};

}
