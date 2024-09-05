#pragma once
#include "IGameVariable.h"

namespace fq::client
{
	class SettingVariable : public IGameVariable
	{
	public:
		//////////////////////////////////////////////////////////////////////////
		//							설정 값 저장 								//
		//////////////////////////////////////////////////////////////////////////
		//프로그램 설정 데이터(음량, 진동, 조준 보정)
		static inline float MasterVolume = 1.f;
		static inline float BGMVolume = 1.f;
		static inline float SFXVolume = 1.f;

		static inline bool IsVibe = true;
		static inline bool IsUsedAimAssist = true;

		//////////////////////////////////////////////////////////////////////////
		//							갑옷 스폰 반경								//
		//////////////////////////////////////////////////////////////////////////
		static inline float ArmourSpawnDistance = 2.5f;
	};
}

