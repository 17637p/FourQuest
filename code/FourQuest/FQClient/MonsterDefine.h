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
		Rush,
		SmashDown,
		Combo,
		Grawl,
		ContContinuousAttack,
	};
}
