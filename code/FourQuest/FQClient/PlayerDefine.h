#pragma once

namespace fq::client
{
	/// <summary>
	/// ��ȥ Ÿ��
	/// ��ȥ Ÿ���� ���ʿ� ������� �������� �����մϴ�
	/// </summary>
	enum class ESoulType
	{
		Sword, // �����ݿ� ������⸦ �����մϴ�.   
		Shield, // ������ ��������� �����մϴ�
		Bow,  // ������ Ȱ������ �����մϴ�
		Staff // ������ ���������� �����մϴ�
	};

	/// <summary>
	/// �÷��̾� �����ÿ� �ʿ��� ����
	/// </summary>
	struct PlayerInfo
	{
		unsigned int ControllerID; // ��Ʈ�ѷ� ���̵�
		ESoulType SoulType; // ��ȥŸ��
	};
}
