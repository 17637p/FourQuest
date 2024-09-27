#pragma once

namespace fq::client
{
	/// <summary>
	/// ���� Ÿ��
	/// �̸��� �̰Թ���!
	/// </summary>
	enum class EMonsterType
	{
		Melee, // ���� ����
		Explosion, // ��ź ����
		Boss, // ����
		Plant, // �Ĺ� ����
		Spawner, // ������
		All // Quest �� 
	};

	enum class EBossMonsterAttackType
	{
		Rush = 0, // ���� 
		SmashDown = 1, // �������
		Combo = 2, // �޺� ����
		Roar = 3, // �ξ� 
		Continous =4, // ���Ӱ���
		Eat = 5, // �Ա� 
	};
}
