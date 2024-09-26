#include "LevelHepler.h"

#include "LevelVariable.h"
#include "SettingVariable.h"
#include "PlayerInfoVariable.h"

float fq::client::LevelHepler::GetDamageRatio()
{
	if (!LevelVariable::OnLevelSystem)
		return 1.f;

	float level = 0.f;

	if (SettingVariable::SelectLevel == 0)
	{
		level = LevelVariable::EasyDamage;
	}
	else if (SettingVariable::SelectLevel == 1)
	{
		level = LevelVariable::NormalDamage;
	}
	else if (SettingVariable::SelectLevel == 2)
	{
		level = LevelVariable::HardDamage;
	}

	int playerNumber = 0;

	if (PlayerInfoVariable::Player1SoulType != -1)
		++playerNumber;
	if (PlayerInfoVariable::Player2SoulType != -1)
		++playerNumber;
	if (PlayerInfoVariable::Player3SoulType != -1)
		++playerNumber;
	if (PlayerInfoVariable::Player3SoulType != -1)
		++playerNumber;

	float playerRatio = 0.f;

	if (playerNumber == 2)
		playerRatio = LevelVariable::Player2Damage;
	else if (playerNumber == 3)
		playerRatio = LevelVariable::Player3Damage;
	else if (playerNumber == 4)
		playerRatio = LevelVariable::Player4Damage;

	float result = 1.f + level + playerRatio;

	return result;
}

float fq::client::LevelHepler::GetHpRatio()
{
	if (!LevelVariable::OnLevelSystem)
		return 1.f;

	float level = 0.f;

	if (SettingVariable::SelectLevel == 0)
	{
		level = LevelVariable::EasyHp;
	}
	else if (SettingVariable::SelectLevel == 1)
	{
		level = LevelVariable::NormalHp;
	}
	else if (SettingVariable::SelectLevel == 2)
	{
		level = LevelVariable::HardHp;
	}

	int playerNumber = 0;

	if (PlayerInfoVariable::Player1SoulType != -1)
		++playerNumber;
	if (PlayerInfoVariable::Player2SoulType != -1)
		++playerNumber;
	if (PlayerInfoVariable::Player3SoulType != -1)
		++playerNumber;
	if (PlayerInfoVariable::Player3SoulType != -1)
		++playerNumber;

	float playerRatio = 0.f;

	if (playerNumber == 2)
		playerRatio = LevelVariable::Player2Hp;
	else if (playerNumber == 3)
		playerRatio = LevelVariable::Player3Hp;
	else if (playerNumber == 4)
		playerRatio = LevelVariable::Player4Hp;

	float result = 1.f + level + playerRatio;

	return result;
}

float fq::client::LevelHepler::GetSpawnRatio()
{
	if (!LevelVariable::OnLevelSystem)
		return 1.f;

	int playerNumber = 0;

	if (PlayerInfoVariable::Player1SoulType != -1)
		++playerNumber;
	if (PlayerInfoVariable::Player2SoulType != -1)
		++playerNumber;
	if (PlayerInfoVariable::Player3SoulType != -1)
		++playerNumber;
	if (PlayerInfoVariable::Player3SoulType != -1)
		++playerNumber;

	float playerRatio = 0.f;

	if (playerNumber == 2)
		playerRatio = LevelVariable::Player2Spawn;
	else if (playerNumber == 3)
		playerRatio = LevelVariable::Player3Spawn;
	else if (playerNumber == 4)
		playerRatio = LevelVariable::Player4Spawn;

	float result = 1.f + playerRatio;

	return result;
}
