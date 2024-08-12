#pragma once


namespace fq::client
{
	/// <summary>
	/// 영혼 타입
	/// 영혼 타입은 갑옷에 상관없이 강공격을 결정합니다
	/// </summary>
	enum class ESoulType
	{
		Sword, // 검 영혼   
		Axe, // 도끼 영혼
		Bow,  // 활 영혼
		Staff // 마법 영혼
	};

	/// <summary>
	/// 플레이어 생성시에 필요한 정보
	/// </summary>
	struct PlayerInfo
	{
		unsigned int ControllerID; // 컨트롤러 아이디
		ESoulType SoulType; // 영혼타입
	};

	enum class EPlayerType
	{
		Soul,
		LivingArmour
	};

	/// <summary>
	/// 갑옷 타입 
	/// </summary>
	enum class EArmourType
	{
		Knight, // 검 방패
		Magic, // 마법 
		Warrior, // 도끼 ?
		Archer // 궁수 
	};

}
