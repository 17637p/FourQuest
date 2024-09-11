#pragma once
#include "IGameVariable.h"


namespace fq::client
{
	class DamageVariable : public IGameVariable
	{
	public:
		//////////////////////////////////////////////////////////////////////////
		//							�� ���� ���� ����								//
		//////////////////////////////////////////////////////////////////////////

		static inline float SwordAttackCoefficient = 1.f;
		static inline float ShieldAttackCoefficient = 1.f;
		static inline float ShieldDashCoefficient = 1.f;

		//////////////////////////////////////////////////////////////////////////
		//						���� ���� ���� ����								//
		//////////////////////////////////////////////////////////////////////////
		static inline float MagicBallCoefficient = 1.f;
		static inline float MagicAOECoefficient = 1.f;
		static inline float MagicLaserCoefficient = 1.f;

		//////////////////////////////////////////////////////////////////////////
		//							��ȥ ���� ����								//
		//////////////////////////////////////////////////////////////////////////
		static inline float SwordSoulCoefficient = 1.f;
		static inline float StaffSoulCoefficient = 1.f;
		static inline float AxeSoulCoefficient = 1.f;
		static inline float BowSoulCoefficient = 1.f;

		//////////////////////////////////////////////////////////////////////////
		//							���� ���� ����								//
		//////////////////////////////////////////////////////////////////////////
		static inline float BossSmashDownCoefficient = 1.f;
		static inline float BossComboAttackCoefficient = 1.f;
		static inline float BossRushCoefficient = 1.f;

		//////////////////////////////////////////////////////////////////////////
		//							������ ���� ���� ����						//
		//////////////////////////////////////////////////////////////////////////
		static inline float BluntFirstConsecutiveAttack = 1.f; // ���Ӱ��� ù ��°(x��ư)
		static inline float BluntSecondConsecutiveAttack = 1.f; // ���Ӱ��� �� ��°(x��ư)
		static inline float BluntThirdConsecutiveAttack = 1.f; // ���Ӱ��� �� ��°(x��ư)
		static inline float BluntSwingAroundCoefficient = 1.f; // �۾���(a��ư)
		static inline float BluntRushCoefficient = 1.f; // �뽬 ����(r��ƽ)
	};


}
