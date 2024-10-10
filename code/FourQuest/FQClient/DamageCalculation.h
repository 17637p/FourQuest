#pragma once

#include "PlayerSoulVariable.h"
#include "DamageVariable.h"

/// <summary>
/// 대미지 산정 방식 정의 
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

	inline float GetArcherCA_1_Damage(float attackPower)
	{
		return attackPower * DamageVariable::ArcherCA_1_Coefficient;
	}

	inline float GetArcherCA_2_Damage(float attackPower)
	{
		return attackPower * DamageVariable::ArcherCA_2_Coefficient;
	}

	inline float GetArcherCA_3_Damage(float attackPower)
	{
		return attackPower * DamageVariable::ArcherCA_3_Coefficient;
	}

	inline float GetArcherCA_4_Damage(float attackPower)
	{
		return attackPower * DamageVariable::ArcherCA_4_Coefficient;
	}

	inline float GetBluntFirstConsecutiveAttackDamage(float attackPower)
	{
		return attackPower * DamageVariable::BluntFirstConsecutiveAttack;
	}

	inline float GetBluntSecondConsecutiveAttackDamage(float attackPower)
	{
		return attackPower * DamageVariable::BluntSecondConsecutiveAttack;
	}

	inline float GetBluntThirdConsecutiveAttackDamage(float attackPower)
	{
		return attackPower * DamageVariable::BluntThirdConsecutiveAttack;
	}

	inline float GetBluntSwingAroundDamage(float attackPower)
	{
		return attackPower * DamageVariable::BluntSwingAroundCoefficient;
	}

	inline float GetBluntRsuhDamage(float attackPower)
	{
		return attackPower * DamageVariable::BluntRushCoefficient;
	}

	//////////////////////////////////////////////////////////////////////////
	//							영혼 공격									//
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
	//							보스 몬스터									//
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