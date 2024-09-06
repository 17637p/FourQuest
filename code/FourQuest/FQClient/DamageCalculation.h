#pragma once

#include "PlayerSoulVariable.h"
#include "DamageVariable.h"

/// <summary>
/// ����� ���� ��� ���� 
/// </summary>
namespace fq::client::dc
{
	inline float GetSwordDamage(float attackPower)
	{
		return attackPower * DamageVariable::SwordAttackCoefficient;
	}

	inline float GetShieldDamage(float attackPower)
	{
		return attackPower * DamageVariable::ShieldAttackCoefficient;
	}

	inline float GetShieldDashDamage(float attackPower)
	{
		return attackPower * DamageVariable::ShieldDashCoefficient;
	}

	inline float GetMagicBallDamage(float attackPower)
	{
		return attackPower * DamageVariable::MagicBallCoefficient;
	}

	inline float GetAOEDamage(float attackPower)
	{
		return attackPower * DamageVariable::MagicAOECoefficient;
	}

	inline float GetLaserDamage(float attackPower)
	{
		return attackPower * DamageVariable::MagicLaserCoefficient;
	}

	inline float GetArcherWADamage(float attackPower)
	{
		return attackPower * DamageVariable::ArcherWACoefficient;
	}

	inline float GetArcherSADamage(float attackPower)
	{
		return attackPower * DamageVariable::ArcherSACoefficient;
	}

	//////////////////////////////////////////////////////////////////////////
	//							��ȥ ����									//
	//////////////////////////////////////////////////////////////////////////

	inline float GetStaffSoulDamage(float attackPower)
	{
		return attackPower * DamageVariable::StaffSoulCoefficient;
	}

	inline float GetSwordSoulDamage(float attackPower)
	{
		return attackPower * DamageVariable::SwordSoulCoefficient;
	}

	inline float GetAxeSoulDamage(float attackPower)
	{
		return attackPower * DamageVariable::AxeSoulCoefficient;
	}

	inline float GetBowSoulDamage(float attackPower)
	{
		return attackPower * DamageVariable::BowSoulCoefficient;
	}

	//////////////////////////////////////////////////////////////////////////
	//							���� ����									//
	//////////////////////////////////////////////////////////////////////////

	inline float GetMonsterSmashDownDamage(float attackPower)
	{
		return attackPower * DamageVariable::BossSmashDownCoefficient;
	}

	inline float GetMonsterComboAttackDamage(float attackPower)
	{
		return attackPower * DamageVariable::BossComboAttackCoefficient;
	}

	inline float GetMonsterRushDamage(float attackPower)
	{
		return attackPower * DamageVariable::BossRushCoefficient;
	}

}