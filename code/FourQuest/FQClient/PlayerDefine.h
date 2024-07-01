#pragma once

namespace fq::client
{
	/// <summary>
	/// 영혼 타입
	/// 영혼 타입은 갑옷에 상관없이 강공격을 결정합니다
	/// </summary>
	enum class ESoulType
	{
		Sword, // 강공격에 내려찍기를 시전합니다.   
		Shield, // 강력한 쉴드공격을 시전합니다
		Bow,  // 강력한 활공격을 시전합니다
		Staff // 강력한 마법공격을 시전합니다
	};

	/// <summary>
	/// 플레이어 생성시에 필요한 정보
	/// </summary>
	struct PlayerInfo
	{
		unsigned int ControllerID; // 컨트롤러 아이디
		ESoulType SoulType; // 영혼타입
	};
}
