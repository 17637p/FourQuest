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
		static inline float MasterVolume = 1.f;
		static inline float BGMVolume = 1.f;
		static inline float SFXVolume = 1.f;

		static inline bool IsVibe = true;
		static inline bool IsUsedAimAssist = true;

		//////////////////////////////////////////////////////////////////////////
		//							���� ���� �ݰ�								//
		//////////////////////////////////////////////////////////////////////////
		static inline float ArmourSpawnDistance = 2.5f;
	};
}

