#pragma once
#include "IGameVariable.h"

namespace fq::client
{
	class SettingVariable : public IGameVariable
	{
	public:
		//////////////////////////////////////////////////////////////////////////
		//							���� �� ���� 								//
		//////////////////////////////////////////////////////////////////////////
		//���α׷� ���� ������(����, ����, ���� ����)
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
		//							���� ���� �ݰ�								//
		//////////////////////////////////////////////////////////////////////////
		static inline float ArmourSpawnDistance = 2.5f;

		//////////////////////////////////////////////////////////////////////////
		//							���̵� ����									//
		//////////////////////////////////////////////////////////////////////////
		static inline int SelectLevel = 1;  // 0: Easy, 1: Normal, 2: Hard

		//////////////////////////////////////////////////////////////////////////
		//							������ ����									//
		//////////////////////////////////////////////////////////////////////////
		static inline bool IsUseCameraInit = false;


	};
}

