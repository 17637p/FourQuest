#pragma once
#include "IGameVariable.h"


namespace fq::client
{
	class DamageVariable : public IGameVariable
	{
	public:
		//////////////////////////////////////////////////////////////////////////
		//							영혼 공격 배율								//
		//////////////////////////////////////////////////////////////////////////
		static inline float SwordSoulCoefficient;
		static inline float StaffSoulCoefficient;
		static inline float AxeSoulCoefficient;
		static inline float BowSoulCoefficient;

	};

}
