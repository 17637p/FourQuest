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
		static inline float MasterVolume = 0.7f;
		static inline float BGMVolume = 0.7f;
		static inline float SFXVolume = 0.7f;
		static inline float VoiceVolume = 0.7f;

		static inline bool MuteMasterVolume = false;
		static inline bool MuteBGMVolume = false;
		static inline bool MuteSFXVolume = false;
		static inline bool MuteVoiceVolume = false;

		static inline bool IsVibe = true;
		static inline bool IsUsedAimAssist = true;
		static inline bool IsAllowOtherPlayerAttack = true;

		//////////////////////////////////////////////////////////////////////////
		//							갑옷 스폰 반경								//
		//////////////////////////////////////////////////////////////////////////
		static inline float ArmourSpawnDistance = 2.5f;

		//////////////////////////////////////////////////////////////////////////
		//							난이도 관련									//
		//////////////////////////////////////////////////////////////////////////
		static inline int SelectLevel = 1;  // 0: Easy, 1: Normal, 2: Hard

		//////////////////////////////////////////////////////////////////////////
		//							에디터 설정									//
		//////////////////////////////////////////////////////////////////////////
		static inline bool IsUseCameraInit = false;


	};
}

