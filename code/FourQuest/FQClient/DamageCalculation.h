#pragma once

/// <summary>
/// 대미지 산정 방식 정의 
/// </summary>
namespace fq::client::dc
{
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
}