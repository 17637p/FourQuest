#pragma once
#include "IGameVariable.h"

namespace fq::client
{
	class MonsterVariable : public IGameVariable
	{
	public:
		static inline bool OnRagdoll = true;						// 레그돌을 생성할 지 여부
		static inline int MaxOneFrameCreateRagdollCount = 3;		// 한 프레임에 레그돌 최대 생성 갯수
		static inline int MaxRagdollsPerScene = 15;					// 한 씬에 레그돌 최대 생성 갯수
		static inline int MinFrameCountForRagdoll = 144;			// 레그돌을 생성할 최소 프레임 제한
	}; 
}
