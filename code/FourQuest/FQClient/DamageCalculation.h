#pragma once

/// <summary>
/// ����� ���� ��� ���� 
/// </summary>
namespace fq::client::dc
{
	inline float GetSwordDamage(float attackPower)
	{
		return attackPower;
	}

	inline float GetShieldDamage(float attackPower)
	{
		return attackPower;
	}

	inline float GetShieldDashDamage(float attackPower)
	{
		return attackPower;
	}

	inline float GetMagicBallDamage(float attackPower)
	{
		return attackPower;
	}

	inline float GetAOEDamage(float attackPower)
	{
		return attackPower;
	}

	inline float GetRazerDamage(float attackPower)
	{
		return attackPower / 4.f;
	}

	//////////////////////////////////////////////////////////////////////////
	//							��ȥ ����									//
	//////////////////////////////////////////////////////////////////////////

	inline float GetStaffSoulDamage(float attackPower)
	{
		return attackPower / 4.f;
	}

	inline float GetSwordSoulDamage(float attackPower)
	{
		return attackPower / 4.f;
	}

	inline float GetAxeSoulDamage(float attackPower)
	{
		return attackPower / 4.f;
	}

	inline float GetBowSoulDamage(float attackPower)
	{
		return attackPower / 4.f;
	}


	//////////////////////////////////////////////////////////////////////////
	//							���� ����									//
	//////////////////////////////////////////////////////////////////////////

	inline float GetMonsterSmashDownDamage(float attackPower)
	{
		return attackPower ;
	}

	inline float GetMonsterComboAttackDamage(float attackPower)
	{
		return attackPower;
	}

	inline float GetMonsterRushDamage(float attackPower)
	{
		return attackPower;
	}




}