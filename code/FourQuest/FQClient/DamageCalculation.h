#pragma once

/// <summary>
/// 대미지 산정 방식 정의 
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

	// 보스 몬스터 내려찍기 공격 
	inline float GetMonsterSmashDownDamage(float attackPower)
	{
		return attackPower ;
	}


}